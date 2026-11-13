#pragma once

#include <vector>
#include <algorithm>
#include "Item.hpp"
#include "Common.hpp"

inline constexpr int TAMAREA = 10;

class PivotBuffer {
private:
    std::vector<Item> items_;
public:
    void Insert(const Item& item){  
        items_.push_back(item);
        std::sort(items_.begin(), items_.end(), [](const Item& a, const Item& b) {
            Metrics::RecordKeyComparison();
            return a.grade < b.grade;
        });
    }
    Item MinRemove(){
        Item min = items_.front();
        items_.erase(items_.begin());
        return min;
    }

    Item MaxRemove(){
        Item max = items_.back();
        items_.pop_back();
        return max;
    }
    
    int OccupiedCOunt()const {
        return static_cast<int>(items_.size());
    }

    void ClearVector(){
        items_.clear();
    }

};