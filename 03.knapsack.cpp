#include <iostream>
#include <vector>
using namespace std;

struct OBJECT
{
    int weight;
    int price;
    int status; // 0:未选中；1:已选中；2:已经不可选
};

struct KNAPSACK_PROBLEM
{
    vector<OBJECT> objs;
    int totalC;
};

typedef int (*SELECT_POLICY)(const vector<OBJECT> &objs, int c);

int Choosefunc1(const vector<OBJECT> &objs, int c)
{
    if (c <= 0)
        return -1;

    int index = -1;
    int mp = 0;
    for (int i = 0; i < static_cast<int>(objs.size()); i++)
    {
        if ((objs[i].status == 0) && (objs[i].price > mp))
        {
            mp = objs[i].price;
            index = i;
        }
    }
    return index;
}

int Choosefunc2(const vector<OBJECT> &objs, int c)
{
    if (c <= 0)
        return -1;

    int index = -1;
    int mw = 10000;
    for (int i = 0; i < static_cast<int>(objs.size()); i++)
    {
        if ((objs[i].status == 0) && (objs[i].weight < mw))
        {
            mw = objs[i].weight;
            index = i;
        }
    }
    return index;
}

int Choosefunc3(const vector<OBJECT> &objs, int c)
{
    if (c <= 0)
        return -1;

    int index = -1;
    double ms = 0.0;
    for (int i = 0; i < static_cast<int>(objs.size()); i++)
    {
        if (objs[i].status == 0)
        {
            double si = objs[i].price / objs[i].weight;
            if (si > ms)
            {
                ms = si;
                index = i;
            }
        }
    }
    return index;
}

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

void GreedyAlgo(KNAPSACK_PROBLEM *problem, SELECT_POLICY spFunc)
{
    int idx;
    int ntc = 0;

    // spFunc 每次选最符合策略的那个物品，选后再检查
    while ((idx = spFunc(problem->objs, problem->totalC - ntc)) != -1)
    {
        // 所选物品是否满足背包承重要求？
        if ((ntc + problem->objs[idx].weight) <= problem->totalC)
        {
            problem->objs[idx].status = 1;
            ntc += problem->objs[idx].weight;
        }
        else
        {
            // 不能选这个物品了，做个标记后重新选
            problem->objs[idx].status = 2;
        }
    }

    PrintResult(problem->objs);
}

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
    // OBJECT objects[] = {{35, 10, 0}, {30, 40, 0}, {60, 30, 0}, {50, 50, 0}, {40, 35, 0}, {10, 40, 0}, {25, 30, 0}};
    // KNAPSACK_PROBLEM problem;
    // problem.objs.assign(objects, objects + 7);
    // problem.totalC = 150;
    // GreedyAlgo(&problem, Choosefunc1);
    // GreedyAlgo(&problem, Choosefunc2);
    // GreedyAlgo(&problem, Choosefunc3);

    DPAlgo();

    return 0;
}
