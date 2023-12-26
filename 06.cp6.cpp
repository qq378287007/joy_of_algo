#include <deque>
#include <algorithm>
#include <functional>
#include <iostream>
#include <cassert>
using namespace std;

// river_def
enum BOAT_LOCATION
{
    LOCAL = 0,
    REMOTE
};
enum ACTION_NAME
{
    ONE_MONSTER_GO = 0,
    TWO_MONSTER_GO,
    ONE_MONK_GO,
    TWO_MONK_GO,
    ONE_MONSTER_ONE_MONK_GO,
    ONE_MONSTER_BACK,
    TWO_MONSTER_BACK,
    ONE_MONK_BACK,
    TWO_MONK_BACK,
    ONE_MONSTER_ONE_MONK_BACK,
    INVALID_ACTION_NAME
};
struct ACTION_EFFECTION
{
    ACTION_NAME act;
    BOAT_LOCATION boat_to; // 船前往的方向
    int move_monster;      // 移动的妖怪数量
    int move_monk;         // 移动的和尚数量
};

// action_description
struct ACTION_DESCRIPTION
{
    ACTION_NAME act;
    const char *description; // 相应动作的人类语言描述
};
const ACTION_DESCRIPTION actDesc[] = {
    {ONE_MONSTER_GO, "One monster go over river"},
    {TWO_MONSTER_GO, "Two monster go over river"},
    {ONE_MONK_GO, "One monk go over river"},
    {TWO_MONK_GO, "Two monk go over river"},
    {ONE_MONSTER_ONE_MONK_GO, "One monster and one monk go over river"},
    {ONE_MONSTER_BACK, "One monster go back"},
    {TWO_MONSTER_BACK, "Two monster go back"},
    {ONE_MONK_BACK, "One monk go back"},
    {TWO_MONK_BACK, "Two monk go back"},
    {ONE_MONSTER_ONE_MONK_BACK, "One monster and one monk go back"}};
const char *GetActionDescription(ACTION_NAME act)
{
    for (int i = 0; i < sizeof(actDesc) / sizeof(actDesc[0]); ++i)
        if (actDesc[i].act == act)
            return actDesc[i].description;
    return "Unknown action";
}

// item_state
const int monster_count = 3;
const int monk_count = 3;
struct ItemState
{
    int local_monster{monster_count};
    int local_monk{monk_count};
    int remote_monster{0};
    int remote_monk{0};
    BOAT_LOCATION boat{LOCAL}; // LOCAL or REMOTE
    ACTION_NAME curAct{INVALID_ACTION_NAME};

    /*
        ItemState()
        {
            SetState(monster_count, monk_count, 0, 0, LOCAL);
            curAct = INVALID_ACTION_NAME;
        }
        ItemState(const ItemState &state)
        {
            SetState(state.local_monster, state.local_monk, state.remote_monster, state.remote_monk, state.boat);
            curAct = state.curAct;
        }

        ItemState &operator=(const ItemState &state)
        {
            SetState(state.local_monster, state.local_monk, state.remote_monster, state.remote_monk, state.boat);
            curAct = state.curAct;
            return *this;
        }
        void SetState(int lmonster, int lmonk, int rmonster, int rmonk, BOAT_LOCATION bl)
        {
            local_monster = lmonster;
            local_monk = lmonk;
            remote_monster = rmonster;
            remote_monk = rmonk;
            boat = bl;
        }
    */
    inline bool IsSameState(const ItemState &state) const
    {
        return (local_monster == state.local_monster &&
                local_monk == state.local_monk &&
                remote_monster == state.remote_monster &&
                remote_monk == state.remote_monk &&
                boat == state.boat);
    }

    void PrintStates() const
    {
        cout << GetActionDescription(curAct) << endl;
    }

    inline bool IsFinalState() const
    {
        return (local_monster == 0 &&
                local_monk == 0 &&
                remote_monster == monster_count &&
                remote_monk == monk_count &&
                boat == REMOTE);
    }

    bool CanTakeAction(const ACTION_EFFECTION &ae) const
    {
        if (boat == ae.boat_to)
            return false;

        int tmp_local_monster = local_monster + ae.move_monster;
        if (tmp_local_monster < 0 || tmp_local_monster > monster_count)
            return false;

        int tmp_local_monk = local_monk + ae.move_monk;
        if (tmp_local_monk < 0 || tmp_local_monk > monk_count)
            return false;

        return true;
    }

    bool IsValidState() const
    {
        if (local_monk > 0 && local_monster > local_monk)
            return false;

        if (remote_monk > 0 && remote_monster > remote_monk)
            return false;

        return true;
    }
};

const ACTION_EFFECTION actEffect[] = {
    {ONE_MONSTER_GO, REMOTE, -1, 0},
    {TWO_MONSTER_GO, REMOTE, -2, 0},
    {ONE_MONK_GO, REMOTE, 0, -1},
    {TWO_MONK_GO, REMOTE, 0, -2},
    {ONE_MONSTER_ONE_MONK_GO, REMOTE, -1, -1},
    {ONE_MONSTER_BACK, LOCAL, 1, 0},
    {TWO_MONSTER_BACK, LOCAL, 2, 0},
    {ONE_MONK_BACK, LOCAL, 0, 1},
    {TWO_MONK_BACK, LOCAL, 0, 2},
    {ONE_MONSTER_ONE_MONK_BACK, LOCAL, 1, 1}};

inline bool IsSameItemState(const ItemState &state1, const ItemState &state2)
{
    return state1.IsSameState(state2);
}

inline bool IsProcessedState(const deque<ItemState> &states, const ItemState &newState)
{
    return find_if(states.cbegin(), states.cend(), bind(&IsSameItemState, newState, placeholders::_1)) != states.cend();
}

void PrintResult(const deque<ItemState> &states)
{
    cout << "Find Result : " << endl;
    for_each(states.cbegin(), states.cend(), mem_fn(&ItemState::PrintStates));
    cout << endl;
}

ItemState MakeActionNewState(const ItemState &curState, const ACTION_EFFECTION &ae)
{
    ItemState newState = curState;
    newState.local_monster += ae.move_monster;
    newState.local_monk += ae.move_monk;
    newState.remote_monster -= ae.move_monster;
    newState.remote_monk -= ae.move_monk;
    newState.boat = ae.boat_to;
    newState.curAct = ae.act;
    return newState;
}

void SearchState(deque<ItemState> &states);
void SearchStateOnNewAction(deque<ItemState> &states, const ItemState &current, const ACTION_EFFECTION &ae)
{
    if (current.CanTakeAction(ae))
    {
        ItemState next = MakeActionNewState(current, ae);
        if (next.IsValidState() && !IsProcessedState(states, next))
        {
            states.push_back(next);
            SearchState(states);
            states.pop_back();
        }
    }
}

void SearchState(deque<ItemState> &states)
{
    const ItemState &current = states.back(); // 每次都从当前状态开始
    if (current.IsFinalState())
    {
        PrintResult(states);
        return;
    }

    // 尝试用10种动作分别与当前状态组合
    for (int i = 0; i < sizeof(actEffect) / sizeof(actEffect[0]); i++)
        SearchStateOnNewAction(states, current, actEffect[i]);
}

int main()
{
    ItemState init;
    deque<ItemState> states;
    states.push_back(init);
    SearchState(states);

    assert(states.size() == 1); // 穷举结束后states应该还是只有一个初始状态

    return 0;
}