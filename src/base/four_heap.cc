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
    int maxi_index = -1;
    while (j < n)
    {
        // 赋值那边做了深拷贝。
        auto maxi = heap_[i].get_timer();
        for (int k = j; k < n && k < j + 4; k++)
        {
            if (maxi < heap_[k].get_timer())
            {
                maxi = heap_[k].get_timer();
                maxi_index = k;
            }
        }
        if (maxi_index == -1)
        {
            break;
        }
        swap_(i, maxi_index);
        i = maxi_index;
        j = i << 2 + 1;
        cnt++;
        maxi_index = -1;
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
        if (shif_down_(pos) == 0)
        {
            shif_up_(pos);
        }
    }
    index_.erase(heap_.back().get_node_seq());
    heap_.pop_back();
}

void FourHeap::push(int seq, TimerNode &node)
{
    if (index_.count(seq) == 0)
    {
        index_[seq] = heap_.size();
        heap_.push_back(node);
        if (heap_.size() == 1)
        {
            return;
        }
        shif_up_(seq);
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

TimerNode FourHeap::front()
{
    return heap_.front();
}

void FourHeap::pop()
{
    assert(heap_.size() > 0);
    swap_(0, heap_.size() - 1);
    destory_(0);
}

void FourHeap::remove(int id)
{
    assert(index_.count(id) == 1);
    destory_(index_[id]);
}

void FourHeap::reset(int id, TimerStamp &timer)
{

    heap_[index_[id]].set_timer_stamp(timer);
    shif_down_(index_[id]);
}
