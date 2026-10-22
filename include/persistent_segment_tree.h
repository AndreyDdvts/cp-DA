#pragma once

#include <cstdint>
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
