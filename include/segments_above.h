#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>

#include "persistent_segment_tree.h"

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
