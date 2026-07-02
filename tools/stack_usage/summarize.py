#!/usr/bin/env python3
"""Summarize GCC .su (stack-usage) files into a Markdown report.

Each .su line has the form:
    <file>:<line>:<col>:<function>\t<bytes>\t<qualifier>

qualifier is one of: static, dynamic, dynamic,bounded. Only 'static' frames
give a fixed byte count; dynamic frames (VLA/alloca) are flagged separately
since they cannot be bounded at compile time.
"""
import argparse
import csv
import glob
import os
import sys


def parse_su_file(path, root):
    rows = []
    with open(path, "r", encoding="utf-8") as f:
        for line in f:
            line = line.rstrip("\n")
            if not line:
                continue
            try:
                loc, bytes_str, qualifier = line.split("\t")
                file_part, lineno, _col, func = loc.rsplit(":", 3)
            except ValueError:
                continue
            src = os.path.relpath(file_part, root) if os.path.isabs(file_part) else file_part
            rows.append({
                "file": src,
                "line": lineno,
                "function": func,
                "bytes": int(bytes_str),
                "qualifier": qualifier,
            })
    return rows


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--build-dir", required=True, help="Build directory to scan for .su files")
    ap.add_argument("--root", default=".", help="Project root, used to relativize paths")
    ap.add_argument("--threshold", type=int, default=512, help="Bytes above which a frame is flagged")
    ap.add_argument("--path-filter", default=None,
                     help="Only include rows whose relative path starts with this prefix (e.g. 'src/')")
    ap.add_argument("--csv", help="Optional path to write the full per-function CSV")
    ap.add_argument("--summary", required=True, help="Path to write the Markdown summary")
    args = ap.parse_args()

    su_files = glob.glob(os.path.join(args.build_dir, "**", "*.su"), recursive=True)
    all_rows = []
    for su in su_files:
        all_rows.extend(parse_su_file(su, args.root))

    if args.path_filter:
        all_rows = [r for r in all_rows if r["file"].startswith(args.path_filter)]

    if not all_rows:
        with open(args.summary, "w", encoding="utf-8") as f:
            f.write("# Stack Usage Summary\n\nNo .su files found under `%s`. "
                    "Rebuild with `-DMC_STACK_USAGE=ON`.\n" % args.build_dir)
        print("No .su files found under %s" % args.build_dir)
        return 1

    all_rows.sort(key=lambda r: r["bytes"], reverse=True)
    dynamic = [r for r in all_rows if r["qualifier"] != "static"]
    over_threshold = [r for r in all_rows if r["bytes"] > args.threshold]
    total_functions = len(all_rows)
    max_frame = all_rows[0]

    if args.csv:
        with open(args.csv, "w", newline="", encoding="utf-8") as f:
            w = csv.DictWriter(f, fieldnames=["file", "line", "function", "bytes", "qualifier"])
            w.writeheader()
            w.writerows(all_rows)

    lines = []
    lines.append("# Stack Usage Summary")
    lines.append("")
    lines.append("Generated from `.su` files under `%s` (GCC `-fstack-usage`)." % args.build_dir)
    lines.append("")
    lines.append("| Metric | Value |")
    lines.append("|:--|--:|")
    lines.append("| Functions analysed | %d |" % total_functions)
    lines.append("| Largest frame | `%s` - %d bytes (%s:%s) |" % (
        max_frame["function"], max_frame["bytes"], max_frame["file"], max_frame["line"]))
    lines.append("| Dynamic/unbounded frames | %d |" % len(dynamic))
    lines.append("| Frames over %d bytes | %d |" % (args.threshold, len(over_threshold)))
    lines.append("")

    if dynamic:
        lines.append("## Dynamic-stack functions (unbounded - review required)")
        lines.append("")
        lines.append("| Function | Bytes | Qualifier | Location |")
        lines.append("|:--|--:|:--|:--|")
        for r in dynamic:
            lines.append("| `%s` | %d | %s | %s:%s |" % (
                r["function"], r["bytes"], r["qualifier"], r["file"], r["line"]))
        lines.append("")

    lines.append("## Top %d frames by size" % min(20, total_functions))
    lines.append("")
    lines.append("| Function | Bytes | Location |")
    lines.append("|:--|--:|:--|")
    for r in all_rows[:20]:
        lines.append("| `%s` | %d | %s:%s |" % (r["function"], r["bytes"], r["file"], r["line"]))
    lines.append("")

    with open(args.summary, "w", encoding="utf-8") as f:
        f.write("\n".join(lines) + "\n")

    print("Functions analysed: %d" % total_functions)
    print("Largest frame: %s - %d bytes" % (max_frame["function"], max_frame["bytes"]))
    print("Dynamic/unbounded frames: %d" % len(dynamic))
    print("Frames over %d bytes: %d" % (args.threshold, len(over_threshold)))
    print("Summary written to %s" % args.summary)

    return 1 if dynamic else 0


if __name__ == "__main__":
    sys.exit(main())
