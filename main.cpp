#include <iostream>
#include <vector>

#include "segments_above.h"

int main() {
    std::ios::sync_with_stdio(false);

    int n = 0;
    int m = 0;
    if (!(std::cin >> n >> m)) {
        return 0;
    }

    std::vector<HorizontalSegment> segments(n);
    for (auto& s : segments) {
        std::cin >> s.l >> s.r >> s.h;
    }

    SegmentsAbove index(segments);

    for (int i = 0; i < m; ++i) {
        int64_t x = 0;
        int64_t y = 0;
        std::cin >> x >> y;
        std::cout << index.Query(x, y) << std::endl;
    }
    return 0;
}
