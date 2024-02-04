#include <deque>
#include <iostream>
#include <algorithm>
#include <functional>
#include <cassert>
#include <vector>
using namespace std;

const int BUCKETS_COUNT = 3;
const int bucket_capicity[BUCKETS_COUNT] = {8, 5, 3};
const int bucket_init_state[BUCKETS_COUNT] = {8, 0, 0};
const int bucket_final_state[BUCKETS_COUNT] = {4, 4, 0};

struct ACTION
{
    int from;
    int to;
    int water;
};

struct BucketState
{
    ACTION curAction{-1, 0, 8};
    int bucket_s[BUCKETS_COUNT];

    BucketState(const int *const buckets = bucket_init_state)
    {
        for (int i = 0; i < BUCKETS_COUNT; ++i)
            bucket_s[i] = buckets[i];
    }

    bool IsBucketEmpty(int bucket) const
    {
        assert(bucket >= 0 && bucket < BUCKETS_COUNT);
        return bucket_s[bucket] == 0;
    }

    bool IsBucketFull(int bucket) const
    {
        assert(bucket >= 0 && bucket < BUCKETS_COUNT);
        return bucket_s[bucket] >= bucket_capicity[bucket];
    }

    bool IsSameState(const BucketState &state) const
    {
        for (int i = 0; i < BUCKETS_COUNT; ++i)
            if (bucket_s[i] != state.bucket_s[i])
                return false;
        return true;
    }

    bool IsFinalState() const
    {
        for (int i = 0; i < BUCKETS_COUNT; ++i)
            if (bucket_s[i] != bucket_final_state[i])
                return false;
        return true;
    }

    bool CanTakeDumpAction(int from, int to) const
    {
        assert(from >= 0 && from < BUCKETS_COUNT);
        assert(to >= 0 && to < BUCKETS_COUNT);
        return from != to && !IsBucketEmpty(from) && !IsBucketFull(to); // 不是同一个桶，且from桶中有水，且to桶中不满
    }

    // 从from到to倒水，返回倒水后状态
    BucketState DumpWater(int from, int to) const // CanTakeDumpAction判断后调用，肯定能倒水
    {
        BucketState next = *this;
        int dump_water = min(next.bucket_s[from], bucket_capicity[to] - next.bucket_s[to]);
        next.bucket_s[from] -= dump_water;
        next.bucket_s[to] += dump_water;
        next.curAction = {from, to, dump_water};
        return next;
    }

    void PrintStates() const
    {
        cout << "Dump " << curAction.water << " water from " << curAction.from + 1 << " to " << curAction.to + 1 << ", ";
        cout << "buckets water states is : ";
        for (int i = 0; i < BUCKETS_COUNT; ++i)
            cout << bucket_s[i] << " ";
        cout << endl;
    }
};

inline bool IsSameBucketState(const BucketState &state1, const BucketState &state2)
{
    return state1.IsSameState(state2);
}

inline bool IsProcessedState(const deque<BucketState> &states, const BucketState &newState)
{
    return find_if(states.cbegin(), states.cend(), bind(IsSameBucketState, newState, placeholders::_1)) != states.cend();
}

void PrintResult(deque<BucketState> &states)
{
    cout << "Find Result : " << endl;
    for_each(states.cbegin(), states.cend(), mem_fn(BucketState::PrintStates)); // mem_fn绑定类成员函数
    // for_each(states.cbegin(), states.cend(), bind(BucketState::PrintStates, placeholders::_1));
    cout << endl;
}

void SearchState(deque<BucketState> &states, vector<deque<BucketState>> &vs);
void SearchStateOnAction(deque<BucketState> &states, const BucketState &current, int from, int to, vector<deque<BucketState>> &vs)
{
    if (current.CanTakeDumpAction(from, to)) // 判断是否能倒水
    {
        BucketState next = current.DumpWater(from, to); // 从from到to倒水，返回倒水后的状态
        if (!IsProcessedState(states, next))            // 判断状态是否已存在
        {
            states.push_back(next);
            SearchState(states, vs);
            states.pop_back();
        }
    }
}

void SearchState(deque<BucketState> &states, vector<deque<BucketState>> &vs)
{
    const BucketState &current = states.back(); // 每次都从当前状态开始
    if (current.IsFinalState())
    {
        // PrintResult(states);
        vs.emplace_back(states);
        return;
    }

    // 使用两重循环排列组合6种倒水状态
    for (int j = 0; j < BUCKETS_COUNT; ++j)
        for (int i = 0; i < BUCKETS_COUNT; ++i)
            SearchStateOnAction(states, current, i, j, vs);
}

int main()
{
    BucketState init;
    deque<BucketState> states;
    states.push_back(init);

    vector<deque<BucketState>> vs;
    SearchState(states, vs);

    int min_num = 999;
    for (deque<BucketState> &v : vs)
    {
        PrintResult(v);
        if (min_num > v.size())
            min_num = v.size();
    }
    cout << vs.size() << endl; // 倒水可能情况
    cout << min_num << endl;   // 最少倒水次数

    assert(states.size() == 1); // 穷举结束后states应该还是只有一个初始状态

    return 0;
}
