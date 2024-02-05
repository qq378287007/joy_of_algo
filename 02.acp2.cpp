#include <string>
#include <stack>
#include <vector>
#include <climits>
#include <memory>
#include <iostream>
using namespace std;

int Factorial(int n)
{
    if (n <= 1)
        return 1;

    int value = 1;
    for (int i = 2; i <= n; i++)
        value *= i;
    return value;
}
int Factorial2(int n)
{
    if (n <= 1)
        return 1;
    return n * Factorial2(n - 1);
}

int max(const vector<int> &values)
{
    int mval = INT_MIN;
    for (int value : values)
        if (mval < value)
            mval = value;
    return mval;
}

struct Cell
{
    bool fixed;
};
struct Game
{
    Cell cells[3][3];
};
void func(Game *game)
{
    for (int i = 0; i < 9; i++) // 单循环遍历二维数组
    {
        int row = i / 3;
        int col = i % 3;
        game->cells[row][col].fixed = false;
    }
}

struct TNODE
{
    int key;
    TNODE *left{nullptr};
    TNODE *right{nullptr};
};
bool FindTNode(TNODE *tr, int key) // 递归实现二叉树查找，尾递归
{
    if (tr == nullptr)
        return false;

    if (key == tr->key)
        return true;
    else if (key < tr->key)
        return FindTNode(tr->left, key);
    else
        return FindTNode(tr->right, key);
}
bool FindTNode2(TNODE *tr, int key) // 尾递归转循环形式
{
    TNODE *curNode = tr;
    while (curNode != nullptr)
    {
        if (key == curNode->key)
            return true;
        else if (key < curNode->key)
            curNode = curNode->left;
        else
            curNode = curNode->right;
    }
    return false;
}

int day_of_year(int year)
{
    if (year % 400 == 0)
        return 366;
    else if (year % 100 == 0)
        return 365;
    else if (year % 4 == 0)
        return 366;
    else
        return 365;
}

enum Action
{
    FARMER_GO,
    FARMER_BACK,
};
struct ItemState
{
};
bool proceeFarmerGo(const ItemState &current, ItemState &next) { return true; }
bool proceeFarmerBack(const ItemState &current, ItemState &next) { return true; }
using ProcessNextFuncPtr = bool (*)(const ItemState &current, ItemState &next);
struct ActionProcess
{
    Action act;
    ProcessNextFuncPtr processFunc;
};
vector<ActionProcess> actMap{
    {FARMER_GO, proceeFarmerGo},
    {FARMER_BACK, proceeFarmerBack},
};
ItemState func(Action action, const ItemState &current)
{
    ItemState next;
    for (ActionProcess cur_ap : actMap)
    {
        if (cur_ap.act == action)
        {
            cur_ap.processFunc(current, next);
            break;
        }
    }
    return next;
}

// values取值范围[1, 100]
// 统计次数
#define MAX_NUMBER_RANGE 100
void CountNumbers(const vector<int> &values)
{
    int numCount[MAX_NUMBER_RANGE] = {0};
    for (int value : values)
        numCount[value - 1]++;
}

bool IsMatchBrackets(const string &express)
{
    stack<char> epStk;
    for (size_t i = 0; i < express.length(); i++)
    {
        if ('(' == express[i])
        {
            epStk.push(express[i]);
        }
        else if (')' == express[i])
        {
            if (epStk.empty())
                return false;
            epStk.pop();
        }
    }
    return epStk.empty();
}

struct Point
{
    int ind;
    bool visited{false};
    vector<shared_ptr<Point>> next;
    Point(int i) : ind(i) {}
};

struct Graph
{
    vector<shared_ptr<Point>> points;

    Graph()
    {
        for (int i = 0; i < 6; i++)
            points.emplace_back(make_shared<Point>(i));

        points[0]->next.emplace_back(points[1]);
        points[0]->next.emplace_back(points[2]);
        points[0]->next.emplace_back(points[3]);
        points[0]->next.emplace_back(points[4]);

        points[1]->next.emplace_back(points[0]);
        points[1]->next.emplace_back(points[2]);
        points[1]->next.emplace_back(points[5]);

        points[2]->next.emplace_back(points[0]);
        points[2]->next.emplace_back(points[1]);
        points[2]->next.emplace_back(points[3]);
        points[2]->next.emplace_back(points[5]);

        points[3]->next.emplace_back(points[0]);
        points[3]->next.emplace_back(points[2]);
        points[3]->next.emplace_back(points[4]);
        points[3]->next.emplace_back(points[5]);

        points[4]->next.emplace_back(points[0]);
        points[4]->next.emplace_back(points[3]);
        points[4]->next.emplace_back(points[5]);

        points[5]->next.emplace_back(points[1]);
        points[5]->next.emplace_back(points[2]);
        points[5]->next.emplace_back(points[3]);
        points[5]->next.emplace_back(points[4]);
    }
};

void VisitFunction(const shared_ptr<Point> &v)
{
    cout << v->ind << endl;
}

void DFS(const shared_ptr<Point> &v)
{
    if (v->visited)
        return;

    VisitFunction(v); // 访问v点;
    v->visited = true;

    for (const shared_ptr<Point> &vi : v->next) // 遍历v的所有邻接点
        DFS(vi);
}
void GraphTravel(Graph g)
{
    for (const shared_ptr<Point> &v : g.points) // 遍历G的所有顶点
        DFS(v);
}
/*
// 从第v个顶点出发递归地深度优先遍历图G
void DFS(Graph G, int v)
{
    VisitFunction(v);            // 访问v点;
    for_each(vi : v的所有邻接点) // 遍历v的所有邻接点
        if (vi没有被访问过)
            DFS(G, vi);
}
void GraphTravel(Graph G)
{
    for_each(v : G的所有顶点) // 遍历G的所有顶点
        DFS(G, v);
}
*/
/*
void BFS(Graph G, int v)
{
    for_each(vi : v的所有邻接点) // 遍历v的所有邻接点
    {
        if (vi没有被访问过)
        {
            VisitFunction(vi); // 访问v点;
            EnQueue(Q, vi);    // vi入队列
        }
    }
    while (!QueueEmpty(Q))
    {
        DeQueue(Q, u); // 队头元素出队并置为u
        BFS(G, u);
    }
}
*/

int main()
{
    bool rtn = IsMatchBrackets(string("((()))"));
    rtn = IsMatchBrackets(string("(44(55)fdg( )  df)"));
    rtn = IsMatchBrackets(string("(ss(ddf))ff)"));

    Graph g;
    GraphTravel(g);
    return 0;
}
