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

## Run on Windows (PowerShell)
Run these commands from this project directory. You need a C++17 compiler and Python 3. C++ compiler selection and runtime DLL availability depend on your installation.

    New-Item -ItemType Directory -Force -Path build, results | Out-Null
    g++ -std=c++17 -O2 -Wall -Wextra -pedantic benchmark.cpp -o build/search_bench.exe
    ./build/search_bench.exe --self-test
    ./build/search_bench.exe results/trials.csv
    python analyze.py results/trials.csv --output results

GCC needs its runtime DLL directory on PATH. The validated local run used MSYS2 UCRT64 GCC 15.2.0 and the bundled Python runtime. See results/environment.json.

## Read the evidence
- results/trials.csv: 81 measured trial rows.
- results/summary.csv: nine algorithm/size summaries.
- results/report.md: measured table and interpretation.
- results/environment.json: compiler, Python, build flags, and experiment configuration.
- results/validation.txt: correctness and analysis validation output.

Generation is outside all timings. Binary setup includes vector copy plus sorting. Hash setup includes reserve and insertion. Linear has no additional index setup. Setup measurements are not randomized; query-method order is randomized per trial. Correctness/counting work and warm-up happen before query timing. Destruction is excluded.

These are synthetic microbenchmarks. Observed runtime is not a proof of asymptotic complexity. A hash set uses extra memory, and its expected constant-time lookup does not imply guaranteed constant time. The break-even estimate applies only to the measured query distribution. Comparing lookup-only results and comparing full setup-plus-lookup costs answers two different questions.

## Our 2–3 hour session
1. 20 minutes: explain the search functions and loop invariant; reproduce correctness checks.
2. 40–60 minutes: add one chosen variable — dataset size, hit ratio, duplicates, or number of queries — as a command-line parameter. Keep the other variables fixed.
3. 25–40 minutes: rerun, compare with the baseline, and explain any unexpected observation.
4. 20–30 minutes: optionally add a Matplotlib chart of median lookup time and IQR using the saved CSV.
5. 15 minutes: write the design tradeoff and record a 90-second demo.

The assistant implemented the initial baseline. Make the next experiment yours: define a hypothesis, change one workload factor, measure the effect, and explain the result.

## Speaking prompt
“You need to look up many items in a collection. When would you scan, sort once, or build a hash index? What did your measurements establish, and what still needs testing?”

## Resume candidate after your extension and explanation
Built a reproducible C++/Python search benchmark comparing three lookup strategies across [verified workload sizes], with separate index setup measurements, correctness validation, and repeated-trial analysis.

Replace placeholders with verified facts. A percentage improvement needs a clearly specified baseline and workload. This baseline has no deployed users or production performance evidence.

