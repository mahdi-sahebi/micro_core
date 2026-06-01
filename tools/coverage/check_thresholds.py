#!/usr/bin/env python3
"""Enforce ISO 26262-6 ASIL-C structural-coverage thresholds.

Reads gcovr's --json-summary output and fails if statement (line) or
branch/decision coverage is below the configured minimum (default 100%).
Writes a Markdown summary for the safety case.
"""
import argparse
import json
import sys


def pct(covered, total):
    return 100.0 if total == 0 else (100.0 * covered / total)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--summary", required=True)
    ap.add_argument("--line-min", type=float, default=100.0)
    ap.add_argument("--branch-min", type=float, default=100.0)
    ap.add_argument("--report", required=True)
    args = ap.parse_args()

    with open(args.summary) as fh:
        data = json.load(fh)

    line_p = data.get("line_percent", pct(data.get("line_covered", 0),
                                          data.get("line_total", 0)))
    branch_p = data.get("branch_percent", pct(data.get("branch_covered", 0),
                                              data.get("branch_total", 0)))

    line_ok = line_p >= args.line_min
    branch_ok = branch_p >= args.branch_min

    lines = [
        "# Structural Coverage Summary (ASIL-C, ISO 26262-6 Table 9)\n",
        "| Metric | Coverage | Target | Status |",
        "|:--|--:|--:|:--|",
        f"| Statement (C0) | {line_p:.1f}% | {args.line_min:.0f}% | "
        f"{'PASS' if line_ok else 'FAIL'} |",
        f"| Branch/Decision (C1) | {branch_p:.1f}% | {args.branch_min:.0f}% | "
        f"{'PASS' if branch_ok else 'FAIL'} |",
        "| MC/DC | see mcdc job | 100% | (separate tool) |",
        "",
    ]
    with open(args.report, "w") as fh:
        fh.write("\n".join(lines) + "\n")

    print(f"Statement (C0): {line_p:.1f}% (target {args.line_min:.0f}%) "
          f"-> {'PASS' if line_ok else 'FAIL'}")
    print(f"Branch (C1):    {branch_p:.1f}% (target {args.branch_min:.0f}%) "
          f"-> {'PASS' if branch_ok else 'FAIL'}")

    if not (line_ok and branch_ok):
        print("COVERAGE GATE FAILED.", file=sys.stderr)
        return 1
    print("COVERAGE GATE PASSED.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
