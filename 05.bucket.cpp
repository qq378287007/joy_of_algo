#include <deque>
#include <iostream>
#include <algorithm>
#include <functional>
#include <cassert>
using namespace std;

struct ACTION
{
    int from;
    int to;
    int water;
};

const int BUCKETS_COUNT = 3;
int bucket_capicity[BUCKETS_COUNT] = {8, 5, 3};
int bucket_init_state[BUCKETS_COUNT] = {8, 0, 0};
int bucket_final_state[BUCKETS_COUNT] = {4, 4, 0};
struct BucketState
{
    ACTION curAction;
    int bucket_s[BUCKETS_COUNT];

    BucketState()
    {
        SetBuckets(bucket_init_state);
        SetAction(8, -1, 0);
    }

    BucketState(const int *buckets)
    {
        SetBuckets(buckets);
        SetAction(8, -1, 0);
    }

    BucketState(const BucketState &state)
    {
        SetBuckets((const int *)state.bucket_s);
        SetAction(state.curAction.water, state.curAction.from, state.curAction.to);
    }

    BucketState &operator=(const BucketState &state)
    {
        SetBuckets((const int *)state.bucket_s);
        SetAction(state.curAction.water, state.curAction.from, state.curAction.to);
        return *this;
    }

    bool IsSameState(const BucketState &state) const
    {
        for (int i = 0; i < BUCKETS_COUNT; ++i)
            if (bucket_s[i] != state.bucket_s[i])
                return false;
        return true;
    }

    bool operator==(const BucketState &state) const
    {
        for (int i = 0; i < BUCKETS_COUNT; ++i)
            if (bucket_s[i] != state.bucket_s[i])
                return false;
        return true;
    }

    void SetAction(int w, int f, int t)
    {
        curAction.water = w;
        curAction.from = f;
        curAction.to = t;
    }

    void SetBuckets(const int *buckets)
    {
        for (int i = 0; i < BUCKETS_COUNT; ++i)
            bucket_s[i] = buckets[i];
    }

    bool CanTakeDumpAction(int from, int to) const
    {
        assert(from >= 0 && from < BUCKETS_COUNT);
        assert(to >= 0 && to < BUCKETS_COUNT);

        // 不是同一个桶，且from桶中有水，且to桶中不满
        return from != to && !IsBucketEmpty(from) && !IsBucketFull(to);
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

    bool IsFinalState() const
    {
        return IsSameState(BucketState(bucket_final_state));
    }

    // 从from到to倒水，返回实际倒水体积
    bool DumpWater(int from, int to, BucketState &next) const
    {
        next.SetBuckets(bucket_s);
        int dump_water = bucket_capicity[to] - next.bucket_s[to];
        if (next.bucket_s[from] >= dump_water)
        {
            next.bucket_s[to] += dump_water;
            next.bucket_s[from] -= dump_water;
        }
        else
        {
            next.bucket_s[to] += next.bucket_s[from];
            dump_water = next.bucket_s[from];
            next.bucket_s[from] = 0;
        }
        if (dump_water > 0) // 是一个有效的倒水动作?
        {
            next.SetAction(dump_water, from, to);
            return true;
        }

        return false;
    }

    void PrintStates() const
    {
        cout << "Dump " << curAction.water << " water from "
             << curAction.from + 1 << " to " << curAction.to + 1 << ", ";
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
    return find_if(states.cbegin(), states.cend(), bind(&IsSameBucketState, newState, placeholders::_1)) != states.cend();
}

void PrintResult(deque<BucketState> &states)
{
    cout << "Find Result : " << endl;
    for_each(states.cbegin(), states.cend(), mem_fn(&BucketState::PrintStates));
    cout << endl
         << endl;
}

void SearchState(deque<BucketState> &states);

void SearchStateOnAction(deque<BucketState> &states, const BucketState &current, int from, int to)
{
    if (current.CanTakeDumpAction(from, to))
    {
        BucketState next;
        bool bDump = current.DumpWater(from, to, next); // 从from到to倒水，如果成功，返回倒水后的状态
        if (bDump && !IsProcessedState(states, next))
        {
            states.push_back(next);
            SearchState(states);
            states.pop_back();
        }
    }
}

void SearchState(deque<BucketState> &states)
{
    BucketState current = states.back(); // 每次都从当前状态开始
    if (current.IsFinalState())
    {
        PrintResult(states);
        return;
    }

    // 使用两重循环排列组合6种倒水状态
    for (int j = 0; j < BUCKETS_COUNT; ++j)
        for (int i = 0; i < BUCKETS_COUNT; ++i)
            SearchStateOnAction(states, current, i, j);
}

int main()
{
    BucketState init;
    deque<BucketState> states;
    states.push_back(init);
    SearchState(states);

    assert(states.size() == 1); // 穷举结束后states应该还是只有一个初始状态

    return 0;
}
