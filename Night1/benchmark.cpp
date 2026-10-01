#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

using Clock = std::chrono::steady_clock;
using Data = std::vector<int>;
volatile std::uint64_t sink = 0;

bool linear(const Data& data, int target) {
    for (int value : data) if (value == target) return true;
    return false;
}

bool binary(const Data& sorted, int target) {
    std::size_t lo = 0, hi = sorted.size();
    while (lo < hi) {
        const auto mid = lo + (hi - lo) / 2;
        if (sorted[mid] == target) return true;
        if (sorted[mid] < target) lo = mid + 1;
        else hi = mid;
    }
    return false;
}

std::uint64_t comparisons(const Data& data, const Data& queries, bool sorted) {
    std::uint64_t count = 0;
    for (int target : queries) {
        if (!sorted) {
            for (int value : data) {
                ++count;
                if (value == target) break;
            }
        } else {
            std::size_t lo = 0, hi = data.size();
            while (lo < hi) {
                const auto mid = lo + (hi - lo) / 2;
                ++count; // Equality comparison.
                if (data[mid] == target) break;
                ++count; // Ordering comparison.
                if (data[mid] < target) lo = mid + 1;
                else hi = mid;
            }
        }
    }
    return count;
}

template<class F>
std::int64_t elapsed_ns(F&& fn) {
    const auto start = Clock::now();
    fn();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - start).count();
}

void check_case(const Data& data, const Data& queries) {
    Data sorted = data;
    std::sort(sorted.begin(), sorted.end());
    std::unordered_set<int> hashed(data.begin(), data.end());
    for (int target : queries) {
        const bool expected = std::find(data.begin(), data.end(), target) != data.end();
        if (linear(data, target) != expected || binary(sorted, target) != expected ||
            (hashed.find(target) != hashed.end()) != expected)
            throw std::runtime_error("Search disagreement for target " + std::to_string(target));
    }
}

void self_test() {
    check_case({}, {-1, 0, 1});
    check_case({4}, {3, 4, 5});
    check_case({9, 1, 1, 5, -3, 9}, {-4, -3, 0, 1, 5, 9, 10});
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> value(-25, 25);
    for (int length = 0; length <= 100; ++length) {
        Data data(length), queries;
        for (int& x : data) x = value(rng);
        for (int x = -30; x <= 30; ++x) queries.push_back(x);
        check_case(data, queries);
    }
    if (comparisons({1, 2, 3}, {3, 4}, false) != 6 ||
        comparisons({}, {1}, true) != 0 ||
        comparisons({1}, {1, 2}, true) != 3)
        throw std::runtime_error("Comparison-count check failed");
    std::cout << "PASS: empty/singleton/duplicate/boundary/missing cases, "
                 "101 seeded datasets, and comparison-count fixtures.\n";
}

void benchmark(const std::string& path) {
    constexpr int query_count = 512;
    constexpr int trials = 9;
    std::ofstream out(path);
    if (!out) throw std::runtime_error("Cannot open output: " + path);
    out << "n,queries,trial,algorithm,setup_ns,query_ns,hits,key_comparisons\n";
    std::mt19937 rng(20260930);
    for (int n : {1000, 10000, 50000}) {
        Data data(n), queries;
        for (int i = 0; i < n; ++i) data[i] = i * 2;
        std::shuffle(data.begin(), data.end(), rng);
        std::uniform_int_distribution<int> index(0, n - 1);
        for (int i = 0; i < query_count; ++i) {
            const int x = index(rng) * 2;
            queries.push_back(i % 2 == 0 ? x : x + 1); // Exact 50% membership.
        }
        std::shuffle(queries.begin(), queries.end(), rng);
        check_case(data, queries);
        for (int trial = 1; trial <= trials; ++trial) {
            Data sorted;
            std::unordered_set<int> hashed;
            // Binary index includes copy + sort; data generation is shared and excluded.
            const auto binary_setup = elapsed_ns([&] {
                sorted = data;
                std::sort(sorted.begin(), sorted.end());
            });
            const auto hash_setup = elapsed_ns([&] {
                hashed.reserve(data.size());
                hashed.insert(data.begin(), data.end());
            });
            const auto linear_comparisons = comparisons(data, queries, false);
            const auto binary_comparisons = comparisons(sorted, queries, true);
            std::vector<int> order{0, 1, 2};
            std::shuffle(order.begin(), order.end(), rng);
            for (int algorithm : order) {
                const auto search = [&](int target) {
                    if (algorithm == 0) return linear(data, target);
                    if (algorithm == 1) return binary(sorted, target);
                    return hashed.find(target) != hashed.end();
                };
                std::uint64_t warm_hits = 0;
                for (int target : queries) warm_hits += search(target);
                sink = warm_hits;
                std::uint64_t hits = 0;
                const auto query_ns = elapsed_ns([&] {
                    for (int target : queries) hits += search(target);
                });
                sink = hits;
                if (hits != query_count / 2) throw std::runtime_error("Unexpected hit count");
                const auto name = algorithm == 0 ? "linear" : algorithm == 1 ? "binary" : "hash";
                const auto setup = algorithm == 0 ? 0 : algorithm == 1 ? binary_setup : hash_setup;
                out << n << ',' << query_count << ',' << trial << ',' << name << ','
                    << setup << ',' << query_ns << ',' << hits << ',';
                if (algorithm == 0) out << linear_comparisons;
                else if (algorithm == 1) out << binary_comparisons;
                // std::unordered_set's internal comparisons are not instrumented.
                out << '\n';
            }
        }
    }
    if (!out) throw std::runtime_error("Failed while writing results");
    std::cout << "Saved 81 trial rows to " << path << "\n";
}

int main(int argc, char** argv) {
    try {
        if (argc > 2) throw std::runtime_error("Usage: search_bench [--self-test | output.csv]");
        if (argc == 2 && std::string(argv[1]) == "--self-test") self_test();
        else {
            self_test();
            benchmark(argc == 2 ? argv[1] : "trials.csv");
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}

