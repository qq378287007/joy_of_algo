#include <iostream>
#include <vector>
using namespace std;

struct OBJECT
{
    int weight;
    int price;
    int status; // 0:未选中；1:已选中；2:已经不可选
};

void PrintResult(const vector<OBJECT> &objs)
{
    int totalW = 0;
    int totalP = 0;
    for (int i = 0; i < static_cast<int>(objs.size()); i++)
    {
        if (objs[i].status == 1)
        {
            totalW += objs[i].weight;
            totalP += objs[i].price;
            cout << "object " << i + 1 << ": weight=" << objs[i].weight << ", price=" << objs[i].price << endl;
        }
    }
    cout << "total weight : " << totalW << ", total price : " << totalP << endl;
}

struct KNAPSACK_PROBLEM
{
    vector<OBJECT> objs;
    int totalC;
};
using SELECT_POLICY = int (*)(const vector<OBJECT> &objs, int c);
// 最大价值
int Choosefunc1(const vector<OBJECT> &objs, int c)
{
    if (c <= 0)
        return -1;

    int mp = 0;
    int index = -1;
    for (int i = 0; i < static_cast<int>(objs.size()); i++)
    {
        if (objs[i].status == 0 && mp < objs[i].price)
        {
            mp = objs[i].price;
            index = i;
        }
    }
    return index;
}
// 最小重量
int Choosefunc2(const vector<OBJECT> &objs, int c)
{
    if (c <= 0)
        return -1;

    int mw = 10000;
    int index = -1;
    for (int i = 0; i < static_cast<int>(objs.size()); i++)
    {
        if (objs[i].status == 0 && mw > objs[i].weight)
        {
            mw = objs[i].weight;
            index = i;
        }
    }
    return index;
}
// 最大价值/重量比
int Choosefunc3(const vector<OBJECT> &objs, int c)
{
    if (c <= 0)
        return -1;

    double ms = 0.0;
    int index = -1;
    for (int i = 0; i < static_cast<int>(objs.size()); i++)
    {
        if (objs[i].status == 0)
        {
            double si = objs[i].price / objs[i].weight;
            if (ms < si)
            {
                ms = si;
                index = i;
            }
        }
    }
    return index;
}

// 贪婪算法
void GreedyAlgo(KNAPSACK_PROBLEM &problem, SELECT_POLICY spFunc)
{
    int idx;
    int ntc = 0;

    // spFunc 每次选最符合策略的那个物品，选后再检查
    while ((idx = spFunc(problem.objs, problem.totalC - ntc)) != -1)
    {
        // 所选物品是否满足背包承重要求？
        if ((ntc + problem.objs[idx].weight) <= problem.totalC)
        {
            problem.objs[idx].status = 1;
            ntc += problem.objs[idx].weight;
        }
        else
        {
            // 不能选这个物品了，做个标记后重新选
            problem.objs[idx].status = 2;
        }
    }

    PrintResult(problem.objs);
}

// 动态规划
const int N = 7;           // 物品数量
const int V = 150;         // 背包容量
int f[N + 1][V + 1] = {0}; // 背包容量为V+1，装载N+1个物品的最大价值
int Package(int *W, int *C, int N, int V)
{
    for (int i = 1; i <= N; i++)
    {
        for (int j = C[i]; j <= V; j++)
        {
            f[i][j] = max(f[i - 1][j], f[i - 1][j - C[i]] + W[i]);
            cout << "f[" << i << "][" << j << "]=" << f[i][j] << endl;
        }
        cout << endl;
    }
    return f[N][V];
}
void DPAlgo()
{
    int W[8] = {0, 10, 40, 30, 50, 35, 40, 30}; // 物品权重
    int C[8] = {0, 35, 30, 60, 50, 40, 10, 25}; // 物品大小
    int result = Package(W, C, N, V);
    if (result > 0)
    {
        cout << "the opt value: " << result << endl;
        int j = V;
        for (int i = N; i > 0; i--)
        {
            if (f[i][j] == (f[i - 1][j - C[i]] + W[i]))
            {
                cout << i << ": "
                     << "w=" << W[i] << ", c=" << C[i] << endl;
                j -= C[i];
            }
        }
    }
    else
        cout << "can not find the opt value" << endl;
}

int main()
{
#if 0
    OBJECT objects[] = {{35, 10, 0}, {30, 40, 0}, {60, 30, 0}, {50, 50, 0}, {40, 35, 0}, {10, 40, 0}, {25, 30, 0}};
    KNAPSACK_PROBLEM problem;
    problem.objs.assign(objects, objects + 7);
    problem.totalC = 150;

    GreedyAlgo(problem, Choosefunc1);
    // GreedyAlgo(problem, Choosefunc2);
    // GreedyAlgo(problem, Choosefunc3);
#endif
    DPAlgo();

    return 0;
}
