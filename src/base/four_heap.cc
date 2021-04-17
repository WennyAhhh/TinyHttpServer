#include "four_heap.h"

FourHeap::FourHeap()
{
    heap_.reserve(1024);
}

FourHeap::~FourHeap()
{
    clear();
}

void FourHeap::clear()
{
    heap_.clear();
    index_.clear();
}

void FourHeap::swap_(size_t l_pos, size_t r_pos)
{
    using std::swap;
    swap(heap_[l_pos], heap_[r_pos]);
    index_[heap_[l_pos].get_node_seq()] = l_pos;
    index_[heap_[r_pos].get_node_seq()] = r_pos;
}

int FourHeap::shif_down_(size_t pos)
{
    int cnt = 0;
    int n = heap_.size();
    int j = pos << 2 + 1;
    int i = pos;
    int mini_index = -1;
    while (j < n)
    {
        // 赋值那边做了深拷贝。
        auto mini = heap_[i].get_timer();
        for (int k = j; k < n && k < j + 4; k++)
        {
            if (mini > heap_[k].get_timer())
            {
                mini = heap_[k].get_timer();
                mini_index = k;
            }
        }
        if (mini_index == -1)
        {
            break;
        }
        swap_(i, mini_index);
        i = mini_index;
        j = i << 2 + 1;
        cnt++;
        mini_index = -1;
    }
    return cnt;
}

int FourHeap::shif_up_(size_t pos)
{
    int cnt = 0;
    int n = heap_.size();
    int j = (pos - 1) >> 2;
    int i = pos;
    while (j >= 0)
    {
        if (heap_[j] <= heap_[i])
        {
            break;
        }
        swap_(i, j);
        i = j;
        j = (pos - 1) >> 2;
        cnt++;
    }
    return cnt;
}

void FourHeap::destory_(size_t pos)
{
    size_t n = heap_.size() - 1;
    assert(pos <= n);
    if (pos < n)
    {
        swap_(pos, n);
    }
    int seq = heap_.back().get_node_seq();
    index_.erase(seq);
    heap_.pop_back();
    if (pos < heap_.size() && shif_down_(pos) == 0)
    {
        shif_up_(pos);
    }
}

void FourHeap::push(TimerNode &node)
{
    int seq = node.get_node_seq();
    if (index_.count(seq) == 0)
    {
        int pos = heap_.size();
        index_[seq] = pos;
        heap_.push_back(node);
        if (heap_.size() == 1)
        {
            return;
        }
        shif_up_(pos);
    }
    else
    {
        int pos = index_[seq];
        heap_[pos] = node;
        if (shif_down_(pos) == 0)
        {
            shif_up_(pos);
        }
    }
}

TimerNode FourHeap::front() const
{
    return heap_.front();
}

bool FourHeap::find(TimerId id) const
{
    return index_.count(id) > 0;
}

bool FourHeap::find(TimerNode &node) const
{
    return find(node.get_node_seq());
}

void FourHeap::pop()
{
    assert(heap_.size() > 0);
    swap_(0, heap_.size() - 1);
    destory_(heap_.size() - 1);
}

bool FourHeap::remove(int id)
{
    if (index_.count(id) == 0)
    {
        return false;
    }
    destory_(index_[id]);
    return true;
}

bool FourHeap::remove(TimerNode &node)
{
    return remove(node.get_node_seq());
}

void FourHeap::reset(int id, TimerStamp &timer)
{

    heap_[index_[id]].set_timer_stamp(timer);
    shif_down_(index_[id]);
}

void FourHeap::reset(TimerNode &node, TimerStamp &timer)
{
    reset(node.get_node_seq(), timer);
}

std::vector<TimerNode> FourHeap::get_k(TimerStamp &timer)
{
    std::vector<TimerNode> res;
    if (heap_.size() == 0)
    {
        return res;
    }
    int pre = 0;
    std::priority_queue<TimerNode> q;
    int top = 0;
    int n = heap_.size();
    q.push(heap_[top]);
    while (!q.empty())
    {
        TimerNode curr_node = q.top();
        q.pop();
        if (curr_node.get_timer() > timer)
        {
            break;
        }
        res.emplace_back(curr_node);
        top = index_[curr_node.get_node_seq()] >> 2;
        for (int i = 1; top + i < n && i <= 4; ++i)
        {
            q.push(heap_[top + i]);
        }
    }
    return res;
}

void FourHeap::extend(float interval, TimerId id, bool repeat)
{
    int pos = index_[id];
    TimerNode &node = heap_[pos];
    node.set_repeat(repeat);
    interval = interval > 0 ? interval : node.get_interval();
    node.add(interval);
    shif_down_(pos);
}