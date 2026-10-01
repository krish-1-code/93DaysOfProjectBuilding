# Night 1 — Search Benchmark Observatory

A reproducible C++/Python experiment comparing linear search, binary search, and a hash-set index. This is a starter research and engineering project; the user has not yet independently extended or presented it.

## What works
- Hand-written linear and binary search; a C++ STL hash-set baseline.
- Seeded, shuffled integer datasets at 1,000, 10,000, and 50,000 items.
- Nine repeated trials and 512 queries per size, with an exact 50% hit rate.
- Index setup and lookup timings measured separately.
- Randomized lookup-method order, warm-up, and checksums to retain measured work.
- Correctness checks against std::find, including empty arrays, duplicates, boundaries, missing values, and 101 seeded small datasets.
- Python CSV summary: medians, interquartile ranges, total batch cost, and estimated index break-even.
- No third-party Python package or paid service required for this version.


