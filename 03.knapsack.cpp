#include <iostream>
#include <vector>
using namespace std;

// 0-1背包问题
// N件物体和1个承重为C的背包，每件物体的重量和价值分别为weight[i]和price[i]
// 选择几件物体装入背包，在满足背包承重前提下，价值总和最高
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
struct OBJECT2
{
    int weight; // 重量
    int price;  // 价值
};
void DPAlgo2(const vector<OBJECT2> &objs, const int V)
{
    const int N = objs.size(); // 物品数量
    vector<vector<int>> f(N + 1, vector<int>(V + 1, 0));
    for (int i = 1; i <= N; i++)
        for (int j = objs[i - 1].weight; j <= V; j++)
            f[i][j] = max(f[i - 1][j], f[i - 1][j - objs[i - 1].weight] + objs[i - 1].price); // 不装载第i个物品（容量不变），或者装载第i个物品（容量改变，价值改变）

    int result = f[N][V]; // 背包容量为V，装载前N个物品的最大价值
    if (result > 0)
    {
        cout << "the max price: " << result << endl;
        int j = V;
        for (int i = N; i >= 1; i--)
        {
            if (f[i][j] != f[i - 1][j]) // 装载了第N件物品
            {
                cout << i << ": "
                     << "weight=" << objs[i - 1].weight << ", price=" << objs[i - 1].price << endl;
                j -= objs[i - 1].weight;
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

#if 1
    vector<OBJECT2> objs{{35, 10}, {30, 40}, {60, 30}, {50, 50}, {40, 35}, {10, 40}, {5, 30}}; // 可选物品（重量+价值）
    const int V = 150;                                                                         // 背包容量
    DPAlgo2(objs, V);
#endif
    return 0;
}
