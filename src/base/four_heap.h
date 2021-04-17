#pragma once
#ifndef FOUR_HEAP_H
#define FOUR_HEAP_H
#include <ctime>
#include <vector>
#include <queue>
#include <map>
#include <cassert>
#include <algorithm>
#include <iostream>

#include "timernode.h"

class FourHeap
{
public:
    FourHeap();
    // FourHeap(vector<T> heap);
    ~FourHeap();
    void push(TimerNode &val);
    TimerNode front() const;
    void pop();
    bool find(TimerNode &node) const;
    bool find(TimerId) const;
    void extend(float, TimerId, bool);
    bool remove(int id);
    bool remove(TimerNode &node);
    void reset(int id, TimerStamp &timer);
    void reset(TimerNode &, TimerStamp &);
    std::vector<TimerNode> get_k(TimerStamp &);
    bool empty()
    {
        return heap_.empty();
    }
    void clear();

private:
    int shif_up_(size_t pos);
    int shif_down_(size_t pos);
    void swap_(size_t l_pos, size_t r_pos);
    void destory_(size_t pos);
    std::vector<TimerNode> heap_;
    std::unordered_map<int, int> index_;
};
#endif