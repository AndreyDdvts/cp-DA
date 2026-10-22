#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

class PersistentSegmentTree {
public:
    using Root = int32_t;
    static constexpr Root kEmpty = 0;

    explicit PersistentSegmentTree(int size, std::size_t expectedUpdates = 0)
        : size_(size) {
        nodes_.reserve(1 + expectedUpdates * (Depth(size) + 1));
        nodes_.push_back({0, 0, 0});
    }

    Root Add(Root version, int pos, int delta) {
        return Add(version, 0, size_ - 1, pos, delta);
    }

    int64_t Sum(Root version, int l, int r) const {
        if (l > r || size_ == 0) {
            return 0;
        }
        return Sum(version, 0, size_ - 1, l, r);
    }

    int Size() const { return size_; }
    std::size_t NodeCount() const { return nodes_.size(); }

private:
    struct Node {
        int32_t left;
        int32_t right;
        int32_t sum;
    };

    static int Depth(int size) {
        int d = 0;
        while ((1 << d) < size) {
            ++d;
        }
        return d;
    }

    Root Add(Root v, int tl, int tr, int pos, int delta) {
        Node copy = nodes_[v];
        copy.sum += delta;
        if (tl != tr) {
            int tm = tl + (tr - tl) / 2;
            if (pos <= tm) {
                copy.left = Add(copy.left, tl, tm, pos, delta);
            } else {
                copy.right = Add(copy.right, tm + 1, tr, pos, delta);
            }
        }
        nodes_.push_back(copy);
        return static_cast<Root>(nodes_.size() - 1);
    }

    int64_t Sum(Root v, int tl, int tr, int l, int r) const {
        if (v == kEmpty || r < tl || tr < l) {
            return 0;
        }
        if (l <= tl && tr <= r) {
            return nodes_[v].sum;
        }
        int tm = tl + (tr - tl) / 2;
        return Sum(nodes_[v].left, tl, tm, l, r) +
               Sum(nodes_[v].right, tm + 1, tr, l, r);
    }

    int size_;
    std::vector<Node> nodes_;
};

struct HorizontalSegment {
    int64_t l;
    int64_t r;
    int64_t h;
};

class SegmentsAbove {
public:
    explicit SegmentsAbove(const std::vector<HorizontalSegment>& segments)
        : tree_(0) {
        heights_.reserve(segments.size());
        for (const auto& s : segments) {
            heights_.push_back(s.h);
        }
        std::sort(heights_.begin(), heights_.end());
        heights_.erase(std::unique(heights_.begin(), heights_.end()),
                       heights_.end());

        struct Event {
            int64_t x;
            int height;
            int delta;
        };
        std::vector<Event> events;
        events.reserve(segments.size() * 2);
        for (const auto& s : segments) {
            int k = HeightIndex(s.h);
            events.push_back({s.l, k, +1});
            events.push_back({s.r + 1, k, -1});
        }
        std::sort(events.begin(), events.end(),
                  [](const Event& a, const Event& b) { return a.x < b.x; });

        tree_ = PersistentSegmentTree(static_cast<int>(heights_.size()),
                                      events.size());
        PersistentSegmentTree::Root root = PersistentSegmentTree::kEmpty;
        for (std::size_t i = 0; i < events.size();) {
            std::size_t j = i;
            while (j < events.size() && events[j].x == events[i].x) {
                root = tree_.Add(root, events[j].height, events[j].delta);
                ++j;
            }
            versionX_.push_back(events[i].x);
            versionRoot_.push_back(root);
            i = j;
        }
    }

    int64_t Query(int64_t x, int64_t y) const {
        auto itX = std::upper_bound(versionX_.begin(), versionX_.end(), x);
        if (itX == versionX_.begin()) {
            return 0;
        }
        auto root = versionRoot_[itX - versionX_.begin() - 1];
        int from = static_cast<int>(
            std::upper_bound(heights_.begin(), heights_.end(), y) -
            heights_.begin());
        return tree_.Sum(root, from, static_cast<int>(heights_.size()) - 1);
    }

    std::size_t VersionCount() const { return versionRoot_.size(); }
    std::size_t NodeCount() const { return tree_.NodeCount(); }

private:
    int HeightIndex(int64_t h) const {
        return static_cast<int>(
            std::lower_bound(heights_.begin(), heights_.end(), h) -
            heights_.begin());
    }

    std::vector<int64_t> heights_;
    PersistentSegmentTree tree_;
    std::vector<int64_t> versionX_;
    std::vector<PersistentSegmentTree::Root> versionRoot_;
};

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
