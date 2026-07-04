#!/usr/bin/env bash
# ----------------------------------------------------------------------------
# Stack-usage analysis for micro_core (ISO 26262-6 evidence: worst-case stack
# depth per function).
#
# Configures and builds with GCC -fstack-usage, then summarizes the resulting
# .su files into a Markdown report. Flags any dynamic/unbounded frame (VLA or
# alloca) as a review item - those cannot be statically bounded.
#
# Usage:
#   tools/stack_usage/run_stack_usage.sh [BUILD_DIR] [THRESHOLD_BYTES]
#
# BUILD_DIR       default: build-stack-usage
# THRESHOLD_BYTES default: 512 (frames above this are called out)
#
# Exit codes: 0 = no dynamic/unbounded frames, 1 = dynamic frame found, 2 = setup error.
# ----------------------------------------------------------------------------
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$ROOT"

BUILD_DIR="${1:-build-stack-usage}"
THRESHOLD="${2:-512}"
OUT_DIR="${STACK_USAGE_RESULTS_DIR:-$BUILD_DIR/stack-usage}"
mkdir -p "$OUT_DIR"

if ! command -v gcc >/dev/null 2>&1; then
    echo "ERROR: gcc not found on PATH." >&2
    exit 2
fi

echo "== Configuring $BUILD_DIR with -DMC_STACK_USAGE=ON =="
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release -DMC_STACK_USAGE=ON >/dev/null

echo "== Building =="
cmake --build "$BUILD_DIR" -j"$(nproc)" >/dev/null

python3 "$ROOT/tools/stack_usage/summarize.py" \
    --build-dir "$BUILD_DIR" \
    --root "$ROOT" \
    --threshold "$THRESHOLD" \
    --path-filter "src/" \
    --csv "$OUT_DIR/stack_usage.csv" \
    --summary "$OUT_DIR/summary.md"
rc=$?

echo
echo "Summary : $OUT_DIR/summary.md"
echo "CSV     : $OUT_DIR/stack_usage.csv"
exit $rc
