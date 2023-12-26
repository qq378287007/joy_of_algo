#include <deque>
#include <iostream>
#include <algorithm>
#include <functional>
#include <cassert>
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

    void SetBuckets(const int *buckets)
    {
        for (int i = 0; i < BUCKETS_COUNT; ++i)
            bucket_s[i] = buckets[i];
    }

    BucketState(const int *buckets = bucket_init_state)
    {
        SetBuckets(buckets);
    }
    /*
        BucketState(const BucketState &state)
        {
            SetBuckets((const int *)state.bucket_s);
            curAction = state.curAction;
        }

        BucketState &operator=(const BucketState &state)
        {
            SetBuckets((const int *)state.bucket_s);
            curAction = state.curAction;
            return *this;
        }
    */
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

    /*
        bool operator==(const BucketState &state) const
        {
            for (int i = 0; i < BUCKETS_COUNT; ++i)
                if (bucket_s[i] != state.bucket_s[i])
                    return false;
            return true;
        }
    */
    bool IsSameState(const BucketState &state) const
    {
        for (int i = 0; i < BUCKETS_COUNT; ++i)
            if (bucket_s[i] != state.bucket_s[i])
                return false;
        return true;
    }

    bool IsFinalState() const
    {
        return IsSameState(BucketState(bucket_final_state));
    }

    bool CanTakeDumpAction(int from, int to) const
    {
        assert(from >= 0 && from < BUCKETS_COUNT);
        assert(to >= 0 && to < BUCKETS_COUNT);

        // 不是同一个桶，且from桶中有水，且to桶中不满
        return from != to && !IsBucketEmpty(from) && !IsBucketFull(to);
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
    return find_if(states.cbegin(), states.cend(), bind(&IsSameBucketState, newState, placeholders::_1)) != states.cend();
}

void PrintResult(deque<BucketState> &states)
{
    cout << "Find Result : " << endl;
    for_each(states.cbegin(), states.cend(), mem_fn(&BucketState::PrintStates));
    cout << endl;
}

void SearchState(deque<BucketState> &states);
void SearchStateOnAction(deque<BucketState> &states, const BucketState &current, int from, int to)
{
    if (current.CanTakeDumpAction(from, to)) // 判断是否能倒水
    {
        BucketState next = current.DumpWater(from, to); // 从from到to倒水，返回倒水后的状态
        if (!IsProcessedState(states, next))            // 判断状态是否已存在
        {
            states.push_back(next);
            SearchState(states);
            states.pop_back();
        }
    }
}

void SearchState(deque<BucketState> &states)
{
    const BucketState &current = states.back(); // 每次都从当前状态开始
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
