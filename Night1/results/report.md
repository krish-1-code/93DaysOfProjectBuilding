# Search Benchmark Observatory: measured results

Each value summarizes nine repeated runs on one seeded dataset per size. Lookup times use a warmed, fixed 512-query batch with 50% membership. These runs describe this machine and workload, not a universal performance ranking.

| Items | Method | Setup (ms) | Lookup median (ns/query) | Lookup IQR (ns/query) | Setup + batch median (ms) | Estimated break-even queries |
|---|---|---|---|---|---|---|
| 1,000 | linear | 0.0000 | 215.6 | 191.2–222.1 | 0.1104 | — |
| 1,000 | binary | 0.0270 | 48.2 | 38.9–49.0 | 0.0490 | 162 |
| 1,000 | hash | 0.0265 | 9.0 | 6.6–10.5 | 0.0310 | 129 |
| 10,000 | linear | 0.0000 | 1789.3 | 1741.0–2021.5 | 0.9161 | — |
| 10,000 | binary | 0.3775 | 55.7 | 54.1–62.5 | 0.4053 | 218 |
| 10,000 | hash | 0.2957 | 11.1 | 10.5–11.7 | 0.3016 | 167 |
| 50,000 | linear | 0.0000 | 8881.4 | 8667.0–10202.9 | 4.5473 | — |
| 50,000 | binary | 2.2125 | 66.6 | 66.4–70.9 | 2.2516 | 251 |
| 50,000 | hash | 1.5195 | 11.1 | 10.5–11.3 | 1.5252 | 172 |

## Interpretation

Binary search pays to copy and sort the input; the hash set pays to build an index. Linear search uses the original vector and has zero additional index setup cost.

The break-even estimate divides median setup time by the median per-query savings relative to linear search, rounded up. It assumes the same data and query distribution, a reusable index, and approximately constant per-query cost. It is an estimate, not a measured crossing.

Comparison counts were collected outside timed lookup batches. Binary counts both equality and ordering comparisons; linear counts equality comparisons. Hash-set internal comparison counts are left blank because they were not instrumented.

Limitations: small fixed workloads, integer keys, exact 50% hit rate, warm-cache reuse, nine repetitions of each single dataset, allocator and OS scheduling effects, and one compiler/machine. Index destruction and memory usage are not measured. No confidence interval or statistical significance claim is made. Change one workload variable at a time before generalizing.

## Explain it aloud

What problem did you solve? Why do lookup costs differ? How much does index construction matter? What would you change to test whether the result holds on another workload?
