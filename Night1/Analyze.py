"""Summarize measured C++ trials"""
import argparse
import csv
import math
from collections import defaultdict
from pathlib import Path
from statistics import median

def quantile(values, fraction):
   
    ordered = sorted(values)
    position = (len(ordered) - 1) * fraction
    lower = math.floor(position)
    upper = math.ceil(position)
    return ordered[lower] + (ordered[upper] - ordered[lower]) * (position - lower)

def summarize(source, destination):
    groups = defaultdict(list)
    with source.open(newline="", encoding="utf-8") as handle:
        for row in csv.DictReader(handle):
            for key in ("n", "queries", "trial", "setup_ns", "query_ns", "hits"):
                row[key] = int(row[key])
            if row["queries"] <= 0 or row["setup_ns"] < 0 or row["query_ns"] <= 0:
                raise ValueError("Invalid benchmark timing or query count")
            if row["hits"] * 2 != row["queries"]:
                raise ValueError("Expected exactly 50% successful queries")
            if row["algorithm"] not in {"linear", "binary", "hash"}:
                raise ValueError("Unknown search algorithm")
            groups[row["n"], row["algorithm"]].append(row)
    if not groups:
        raise ValueError("No trial rows found")
    summary = []
    for n in sorted({key[0] for key in groups}):
        if any((n, alg) not in groups for alg in ("linear", "binary", "hash")):
            raise ValueError("Missing algorithm for array size")
        baseline = median(r["query_ns"] / r["queries"] for r in groups[n, "linear"])
        for algorithm in ("linear", "binary", "hash"):
            rows = groups[n, algorithm]
            if len({r["trial"] for r in rows}) != len(rows):
                raise ValueError("Duplicate trial IDs")
            times = [r["query_ns"] / r["queries"] for r in rows]
            setup = median(r["setup_ns"] for r in rows)
            lookup = median(times)
            savings = baseline - lookup
            comparisons = [int(r["key_comparisons"]) / r["queries"]
                           for r in rows if r["key_comparisons"]]
            summary.append({
                "n": n, "algorithm": algorithm, "trials": len(rows),
                "median_setup_ms": round(setup / 1e6, 6),
                "median_lookup_ns": round(lookup, 3),
                "lookup_q1_ns": round(quantile(times, .25), 3),
                "lookup_q3_ns": round(quantile(times, .75), 3),
                "median_total_batch_ms": round(
                    median(r["setup_ns"] + r["query_ns"] for r in rows) / 1e6, 6),
                "lookup_speedup_vs_linear": round(baseline / lookup, 3),
                "estimated_break_even_queries": (
                    math.ceil(setup / savings) if algorithm != "linear" and savings > 0 else ""),
                "mean_key_comparisons_per_query": (
                    round(median(comparisons), 3) if comparisons else ""),
            })
    destination.mkdir(parents=True, exist_ok=True)
    with (destination / "summary.csv").open("w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(handle, fieldnames=list(summary[0]))
        writer.writeheader()
        writer.writerows(summary)
    lines = [
        "# Search Benchmark Observatory: measured results",
        "",
        "Each value summarizes nine repeated runs on one seeded dataset per size. "
        "Lookup times use a warmed, fixed 512-query batch with 50% membership. "
        "These runs describe this machine and workload, not a universal performance ranking.",
        "",
        "| Items | Method | Setup (ms) | Lookup median (ns/query) | Lookup IQR (ns/query) | "
        "Setup + batch median (ms) | Estimated break-even queries |",
        "|---|---|---|---|---|---|---|",
    ]
    for row in summary:
        lines.append(
            f"| {row['n']:,} | {row['algorithm']} | {row['median_setup_ms']:.4f} | "
            f"{row['median_lookup_ns']:.1f} | {row['lookup_q1_ns']:.1f}–"
            f"{row['lookup_q3_ns']:.1f} | {row['median_total_batch_ms']:.4f} | "
            f"{row['estimated_break_even_queries'] or '—'} |")
    lines.extend([
        "", "## Interpretation", "",
        "Binary search pays to copy and sort the input; the hash set pays to build an index. "
        "Linear search uses the original vector and has zero additional index setup cost.",
        "",
        "The break-even estimate divides median setup time by the median per-query savings "
        "relative to linear search, rounded up. It assumes the same data and query distribution, "
        "a reusable index, and approximately constant per-query cost. It is an estimate, not a measured crossing.",
        "",
        "Comparison counts were collected outside timed lookup batches. Binary counts both "
        "equality and ordering comparisons; linear counts equality comparisons. Hash-set internal "
        "comparison counts are left blank because they were not instrumented.",
        "",
        "Limitations: small fixed workloads, integer keys, exact 50% hit rate, warm-cache reuse, "
        "nine repetitions of each single dataset, allocator and OS scheduling effects, and one compiler/machine. "
        "Index destruction and memory usage are not measured. No confidence interval or statistical "
        "significance claim is made. Change one workload variable at a time before generalizing.",
        "",
        "## Explain it aloud", "",
        "What problem did you solve? Why do lookup costs differ? How much does index construction "
        "matter? What would you change to test whether the result holds on another workload?",
    ])
    (destination / "report.md").write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"Saved {len(summary)} summary rows and report to {destination}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("trials", type=Path)
    parser.add_argument("--output", type=Path, default=Path("results"))
    args = parser.parse_args()
    summarize(args.trials, args.output)

