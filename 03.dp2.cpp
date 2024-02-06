#include <string>
#include <cmath>
#include <vector>
using namespace std;

// 贪婪法
// 局部最优解，不一定是全局最优解

// 1, 5, 10, 25类型硬币，凑出n元最少硬币数量
// vector<int> coins = {25, 20, 5, 1};
// int amount = 41;
int coinNumber(const vector<int> &coins, int amount)
{
    if (amount < 0)
        return -1;

    int num = 0;
    while (amount > 0)
    {
        bool found = false;
        for (int coin : coins)
        {
            if (coin <= amount)
            {
                amount -= coin;
                num++;
                found = true;
                break;
            }
        }
        if (!found)
            return -1;
    }

    return num;
}

int EditDistance1(const string &src, const string &dest)
{
    const int src_len = src.length();
    const int dest_len = dest.length();
    if (src_len == 0 || dest_len == 0)
        return abs(src_len - dest_len);

    if (src[0] == dest[0])
        return EditDistance1(src.substr(1), dest.substr(1));

    int edIns = EditDistance1(src, dest.substr(1));           // source 插入字符
    int edDel = EditDistance1(src.substr(1), dest);           // source 删除字符
    int edRep = EditDistance1(src.substr(1), dest.substr(1)); // source 替换字符

    return min(min(edIns, edDel), edRep) + 1;
}

int EditDistance2(const string &src, const string &dest)
{
    const int src_len = src.length();
    const int dest_len = dest.length();
    vector<vector<int>> d(src_len + 1, vector<int>(dest_len + 1, 0));

    for (int i = 0; i <= src_len; i++)
        d[i][0] = i;
    for (int j = 0; j <= dest_len; j++)
        d[0][j] = j;

    for (int i = 1; i <= src_len; i++)
    {
        for (int j = 1; j <= dest_len; j++)
        {
            if ((src[i - 1] == dest[j - 1]))
            {
                d[i][j] = d[i - 1][j - 1]; // 不需要编辑操作
            }
            else
            {
                int edIns = d[i][j - 1];     // source 插入字符
                int edDel = d[i - 1][j];     // source 删除字符
                int edRep = d[i - 1][j - 1]; // source 替换字符
                d[i][j] = min(min(edIns, edDel), edRep) + 1;
            }
        }
    }

    return d[src_len][dest_len];
}

struct MEMO_RECORD
{
    int refCount;
    int distance;
};
static int recC = 0;
int EditDistance(const string &src, const string &dest, int i, int j, vector<vector<MEMO_RECORD>> &memo)
{
    recC++;
    if (memo[i][j].refCount != 0) // 查表，直接返回
    {
        memo[i][j].refCount++;
        return memo[i][j].distance;
    }

    const int src_len = src.length();
    const int dest_len = dest.length();

    int distance = 0;
    if (i >= src_len)
    {
        distance = dest_len;
    }
    else if (j >= dest_len)
    {
        distance = src_len;
    }
    else
    {
        if (src[i] == dest[j])
        {
            distance = EditDistance(src, dest, i + 1, j + 1, memo);
        }
        else
        {
            int edIns = EditDistance(src, dest, i, j + 1, memo);     // source 插入字符
            int edDel = EditDistance(src, dest, i + 1, j, memo);     // source 删除字符
            int edRep = EditDistance(src, dest, i + 1, j + 1, memo); // source 替换字符
            distance = min(min(edIns, edDel), edRep) + 1;
        }
    }

    memo[i][j].refCount = 1;
    memo[i][j].distance = distance;

    return distance;
}
int EditDistance3(const string &src, const string &dest)
{
    const int src_len = src.length();
    const int dest_len = dest.length();
    vector<vector<MEMO_RECORD>> memo(src_len + 1, vector<MEMO_RECORD>(dest_len + 1, {0, 0}));
    return EditDistance(src, dest, 0, 0, memo);
}

/*
const int INVALID_VALUE = 0xFFFF;
const int MAX_STRING_LEN = 50;

// #define min(a, b)  (((a) < (b)) ? (a) : (b))



*/
int main()
{
    /*
    for (int i = 0; i < MAX_STRING_LEN; i++)
    {
        for (int j = 0; j < MAX_STRING_LEN; j++)
        {
            memo[i][j].distance = INVALID_VALUE;
            memo[i][j].refCount = 0;
        }
    }
    int fff = EditDistance2("SNOWY", "SUNNY");
    int ddd = EditDistance("SNOWY", "SUNNY", 0, 0);
    int eee = EditDistance3("SNOWY", "SUNNY");
*/
    return 0;
}
