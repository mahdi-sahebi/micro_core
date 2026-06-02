#!/usr/bin/env bash
# ----------------------------------------------------------------------------
# MISRA C:2012 static-analysis gate for micro_core (ISO 26262-6, ASIL-C).
#
# Runs cppcheck with the MISRA addon over the whole src/ tree. ASIL-C policy:
#   * Zero Mandatory or Required violations are allowed -> exit 1 on any.
#   * Advisory violations are reported but do NOT fail the build.
#   * Documented deviations live in tools/misra/suppressions.txt and are
#     cross-referenced in doc/safety/deviation_records.md.
#
# Usage:
#   tools/misra/run_misra.sh [BUILD_DIR]
#
# BUILD_DIR (default: build) must contain a compile_commands.json. If absent,
# the script falls back to analysing src/ with include/ on the include path.
#
# Exit codes: 0 = compliant, 1 = Required/Mandatory violation, 2 = setup error.
# ----------------------------------------------------------------------------
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$ROOT"

BUILD_DIR="${1:-build}"
MISRA_DIR="tools/misra"
OUT_DIR="${MISRA_RESULTS_DIR:-$BUILD_DIR/misra}"
mkdir -p "$OUT_DIR"

if ! command -v cppcheck >/dev/null 2>&1; then
    echo "ERROR: cppcheck not found on PATH." >&2
    echo "  Install it (e.g. 'sudo apt-get install -y cppcheck') and re-run." >&2
    exit 2
fi

echo "== cppcheck $(cppcheck --version) =="

# The MISRA addon takes an optional rule-texts file. The official MISRA rule
# text is copyrighted, so it is not committed. If a licensed copy is present
# we pass it through (richer report); otherwise we run with rule numbers only.
ADDON_ARG="--addon=misra"
if [ -f "$MISRA_DIR/misra-rule-texts.txt" ]; then
    ADDON_ARG="--addon=$MISRA_DIR/misra.json"
    echo "Using local MISRA rule texts."
else
    echo "NOTE: $MISRA_DIR/misra-rule-texts.txt not present; reporting rule numbers only."
fi

COMMON_ARGS=(
    --enable=style,warning
    --addon-python=python3
    "$ADDON_ARG"
    --suppressions-list="$MISRA_DIR/suppressions.txt"
    --inline-suppr
    --std=c99
    --platform=unix64
    --error-exitcode=0          # we classify severity ourselves below
    --template='{file}:{line}: [{id}] {message}'
    -DNULL=0
)

XML_OUT="$OUT_DIR/misra.xml"
TXT_OUT="$OUT_DIR/misra.txt"

if [ -f "$BUILD_DIR/compile_commands.json" ]; then
    echo "Analysing via $BUILD_DIR/compile_commands.json (project flags honoured)."
    # Restrict to project sources; exclude tests and build trees.
    cppcheck "${COMMON_ARGS[@]}" \
        --project="$BUILD_DIR/compile_commands.json" \
        --file-filter='*/src/*' \
        --xml --output-file="$XML_OUT" 2>"$TXT_OUT" || true
else
    echo "No compile_commands.json in $BUILD_DIR; analysing src/ directly."
    cppcheck "${COMMON_ARGS[@]}" \
        -I include \
        src \
        --xml --output-file="$XML_OUT" 2>"$TXT_OUT" || true
fi

# ---------------------------------------------------------------------------
# Classify findings. cppcheck emits MISRA ids as 'misra-c2012-<rule>'. We map
# each rule number to its ISO/MISRA category using the committed CSV so we can
# enforce the ASIL-C policy (Mandatory/Required block, Advisory warn).
# ---------------------------------------------------------------------------
python3 "$MISRA_DIR/classify.py" \
    --xml "$XML_OUT" \
    --categories "$MISRA_DIR/rule_categories.csv" \
    --summary "$OUT_DIR/summary.md"
rc=$?

echo
echo "Full report : $TXT_OUT"
echo "XML report  : $XML_OUT"
echo "Summary     : $OUT_DIR/summary.md"
exit $rc
