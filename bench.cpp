#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

#include "segments_above.h"

namespace {
using Clock = std::chrono::steady_clock;

double Ms(Clock::time_point a, Clock::time_point b) {
    return std::chrono::duration<double, std::milli>(b - a).count();
}

struct Test {
    std::vector<HorizontalSegment> segments;
    std::vector<std::pair<int64_t, int64_t>> points;
};

Test Generate(int n, int m, std::mt19937_64& rng) {
    const int64_t kMax = 1'000'000'000;
    std::uniform_int_distribution<int64_t> coord(-kMax, kMax);
    Test t;
    t.segments.reserve(n);
    for (int i = 0; i < n; ++i) {
        int64_t a = coord(rng);
        int64_t b = coord(rng);
        if (a == b) {
            ++b;
        }
        if (a > b) {
            std::swap(a, b);
        }
        t.segments.push_back({a, b, coord(rng)});
    }
    t.points.reserve(m);
    for (int i = 0; i < m; ++i) {
        t.points.emplace_back(coord(rng), coord(rng));
    }
    return t;
}

int64_t Naive(const Test& t) {
    int64_t checksum = 0;
    for (auto [x, y] : t.points) {
        for (const auto& s : t.segments) {
            checksum += (s.l <= x && x <= s.r && s.h > y);
        }
    }
    return checksum;
}

}

int main() {
    std::mt19937_64 rng(2026);
    const int kSizes[] = {1000, 5000, 10000, 20000, 30000, 50000, 70000, 100000};
    const int kRepeats = 5;

    std::cout << "n,build_ms,query_ms,total_ms,naive_ms,nodes\n";
    for (int n : kSizes) {
        double build = 0;
        double query = 0;
        double naive = 0;
        std::size_t nodes = 0;
        for (int rep = 0; rep < kRepeats; ++rep) {
            Test t = Generate(n, n, rng);

            auto t0 = Clock::now();
            SegmentsAbove index(t.segments);
            auto t1 = Clock::now();
            int64_t checksum = 0;
            for (auto [x, y] : t.points) {
                checksum += index.Query(x, y);
            }
            auto t2 = Clock::now();
            build += Ms(t0, t1);
            query += Ms(t1, t2);
            nodes = index.NodeCount();

            auto t3 = Clock::now();
            int64_t expected = Naive(t);
            auto t4 = Clock::now();
            naive += Ms(t3, t4);
            if (expected != checksum) {
                std::cerr << "MISMATCH at n = " << n << '\n';
                return 1;
            }
        }
        std::cout << n << ',' << build / kRepeats << ',' << query / kRepeats
                  << ',' << (build + query) / kRepeats << ','
                  << naive / kRepeats << ',' << nodes << '\n';
    }
    return 0;
}
