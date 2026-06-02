#!/usr/bin/env bash
# ----------------------------------------------------------------------------
# MC/DC coverage gate via clang -fcoverage-mcdc + llvm-cov (ISO 26262-6 ASIL-C).
#
# gcov cannot measure Modified Condition/Decision Coverage; clang's source-based
# coverage with -fcoverage-mcdc can. This builds an instrumented tree with
# clang, runs the unit tests, and reports MC/DC.
#
# Usage: tools/coverage/run_mcdc.sh [BUILD_DIR]
# Env:   MCDC_MIN  minimum MC/DC % required to pass (default 0 = report only,
#                  ramp toward the documented ASIL-C target of 100).
# Exit:  0 = target met, 1 = below target, 2 = setup error.
# ----------------------------------------------------------------------------
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$ROOT"

BUILD_DIR="${1:-build-mcdc}"
OUT_DIR="$BUILD_DIR/mcdc"
MCDC_MIN="${MCDC_MIN:-0}"

for tool in clang llvm-profdata llvm-cov cmake; do
    command -v "$tool" >/dev/null 2>&1 || { echo "ERROR: $tool not found." >&2; exit 2; }
done

# -fcoverage-mcdc requires a reasonably recent clang (>= 18).
CLANG_VER="$(clang --version | sed -n 's/.*version \([0-9]*\).*/\1/p' | head -1)"
MCDC_FLAG="-fcoverage-mcdc"
if [ "${CLANG_VER:-0}" -lt 18 ]; then
    echo "WARNING: clang $CLANG_VER < 18; -fcoverage-mcdc unavailable, falling back to branch coverage only." >&2
    MCDC_FLAG=""
fi

INSTR="-fprofile-instr-generate -fcoverage-mapping $MCDC_FLAG"

echo "== Configuring clang-instrumented build in $BUILD_DIR =="
cmake -S . -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_C_FLAGS="$INSTR" \
    -DCMAKE_EXE_LINKER_FLAGS="$INSTR" >/dev/null
echo "== Building =="
cmake --build "$BUILD_DIR" --parallel >/dev/null

mkdir -p "$OUT_DIR"
shopt -s nullglob
mapfile -d '' tests < <(find "$BUILD_DIR/test" -type f -name 'test_*' -executable -not -name '*-*' -print0)
[ ${#tests[@]} -gt 0 ] || { echo "ERROR: no test executables." >&2; exit 2; }

profs=()
i=0
for t in "${tests[@]}"; do
    raw="$OUT_DIR/$(basename "$t").$i.profraw"
    LLVM_PROFILE_FILE="$raw" "$t" >/dev/null 2>&1 || { echo "FAIL $(basename "$t")"; exit 1; }
    profs+=("$raw"); i=$((i+1))
done

llvm-profdata merge -sparse "${profs[@]}" -o "$OUT_DIR/merged.profdata"

# Report against the test binaries; restrict to project src/.
OBJ_ARGS=()
for t in "${tests[@]}"; do OBJ_ARGS+=(-object "$t"); done

llvm-cov report "${OBJ_ARGS[@]}" \
    -instr-profile="$OUT_DIR/merged.profdata" \
    -show-mcdc-summary 2>/dev/null \
    $(find src -name '*.c') | tee "$OUT_DIR/report.txt" || \
llvm-cov report "${OBJ_ARGS[@]}" \
    -instr-profile="$OUT_DIR/merged.profdata" \
    $(find src -name '*.c') | tee "$OUT_DIR/report.txt"

llvm-cov show "${OBJ_ARGS[@]}" \
    -instr-profile="$OUT_DIR/merged.profdata" \
    -show-mcdc -format=html -output-dir="$OUT_DIR/html" \
    $(find src -name '*.c') >/dev/null 2>&1 || true

# Extract the MC/DC percentage from the TOTAL line if present.
mcdc_pct="$(awk '/^TOTAL/ {for(i=1;i<=NF;i++) if($i ~ /%$/) last=$i} END{gsub(/%/,"",last); print last+0}' "$OUT_DIR/report.txt" 2>/dev/null || echo 0)"

echo
echo "== MC/DC report: $OUT_DIR/report.txt (target ${MCDC_MIN}%) =="
python3 - "$mcdc_pct" "$MCDC_MIN" "$OUT_DIR/mcdc_summary.md" <<'PY'
import sys
pct=float(sys.argv[1] or 0); mn=float(sys.argv[2]); out=sys.argv[3]
ok = pct >= mn
with open(out,"w") as f:
    f.write("# MC/DC Coverage Summary (ASIL-C, ISO 26262-6 Table 9)\n\n")
    f.write("| Metric | Coverage | Target | Status |\n|:--|--:|--:|:--|\n")
    f.write(f"| MC/DC | {pct:.1f}% | {mn:.0f}% | {'PASS' if ok else 'FAIL'} |\n")
print(f"MC/DC: {pct:.1f}% (gate target {mn:.0f}%) -> {'PASS' if ok else 'FAIL'}")
sys.exit(0 if ok else 1)
PY
rc=$?
exit $rc
