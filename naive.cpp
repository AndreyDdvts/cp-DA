#include <cstdint>
#include <iostream>
#include <vector>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n = 0;
    int m = 0;
    std::cin >> n >> m;
    std::vector<int64_t> l(n), r(n), h(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> l[i] >> r[i] >> h[i];
    }
    for (int j = 0; j < m; ++j) {
        int64_t x = 0;
        int64_t y = 0;
        std::cin >> x >> y;
        int64_t cnt = 0;
        for (int i = 0; i < n; ++i) {
            cnt += (l[i] <= x && x <= r[i] && h[i] > y);
        }
        std::cout << cnt << '\n';
    }
    return 0;
}
