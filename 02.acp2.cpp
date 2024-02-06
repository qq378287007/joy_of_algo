#include <string>
#include <stack>
#include <vector>
#include <queue>
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

    inline void addNext(const shared_ptr<Point> &p)
    {
        next.emplace_back(p);
    }
    bool VisitFunction()
    {
        if (visited)
            return true;

        cout << ind << endl;
        visited = true;

        return false;
    }
};

struct Graph
{
    vector<shared_ptr<Point>> vp;

    Graph()
    {
        shared_ptr<Point> p0(new Point(0));
        shared_ptr<Point> p1(new Point(1));
        shared_ptr<Point> p2(new Point(2));
        shared_ptr<Point> p3(new Point(3));
        shared_ptr<Point> p4(new Point(4));
        shared_ptr<Point> p5(new Point(5));

        p0->addNext(p1);
        p0->addNext(p2);
        p0->addNext(p3);

        p1->addNext(p4);

        p2->addNext(p4);
        p2->addNext(p5);

        p3->addNext(p5);

        vp.emplace_back(p0);
    }
};

void DFS(shared_ptr<Point> &v) // 深度优先遍历
{
    if (v->VisitFunction()) // 访问v点
        return;
    for (shared_ptr<Point> &vi : v->next) // 遍历v的所有邻接点
        DFS(vi);
}
void GraphTravel(Graph &g)
{
    for (shared_ptr<Point> &v : g.vp) // 遍历G的所有根顶点
        DFS(v);
}

void BFS(const shared_ptr<Point> &v) // 广度优先遍历
{
    queue<shared_ptr<Point>> q;

    q.push(v);
    while (!q.empty())
    {
        shared_ptr<Point> vi = q.front();
        q.pop();

        vi->VisitFunction(); // 访问v点

        for (const shared_ptr<Point> &vvi : vi->next) // 遍历v的所有邻接点
            if (vvi->visited == false)
                q.push(vvi);
    }
}
void GraphTravel2(Graph &g)
{
    for (shared_ptr<Point> &v : g.vp) // 遍历G的所有根顶点
        BFS(v);
}

int main()
{
    bool rtn = IsMatchBrackets(string("((()))"));
    rtn = IsMatchBrackets(string("(44(55)fdg( )  df)"));
    rtn = IsMatchBrackets(string("(ss(ddf))ff)"));

    Graph g;
    GraphTravel(g);

    Graph g2;
    GraphTravel2(g2);
    return 0;
}
