#include <string>
#include <stack>
using namespace std;

#if 1
int max(int *values, int size)
{
    int mval = *values;
    for (int i = 1; i < size; i++)
    {
        if (values[i] > mval)
            mval = values[i];
    }
    return mval;
}

int Factorial(int n)
{
    int value = 1;
    for (int i = 1; i < n; i++)
        value *= i;
    return value;
}

int Factorial2(int n)
{
    if (n == 0)
        return 1;
    return n * Factorial2(n - 1);
}

struct TNODE
{
    int key;
    TNODE *left;
    TNODE *right;
};
bool FindTNode(TNODE *tr, int key)
{
    if (tr == NULL)
        return false;
    if (tr->key == key)
        return true;

    if (key < tr->key)
        return FindTNode(tr->left, key);
    else
        return FindTNode(tr->right, key);
}

bool FindTNode2(TNODE *tr, int key)
{
    for (TNODE *curNode = tr; curNode != NULL;)
    {
        if (curNode->key == key)
            return true;

        if (key < curNode->key)
            curNode = curNode->left;
        else
            curNode = curNode->right;
    }

    return false;
}

int day_of_year(int year)
{
    static int days_per_year = 365;
    if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))
        return days_per_year + 1;
    return days_per_year;
}

/*
typedef bool(*ProcessNextFuncPtr)(const ItemState& current, ItemState& next);
struct ActionProcess
{
    Action act;
    ProcessNextFuncPtr processFunct;
};
ActionProcess actMap[] = {
    {FARMER_GO, proceeFarmerGo},
    {FARMER_BACK, proceeFarmerBack},
    ...
};


ItemState next;
for (int i = 0; i < sizeof(actMap) / sizeof(actMap[0]); i++)
{
    if (actMap[i].act == action)
    {
        actMap[i].processFunc(current, next);
        break;
    }
}
*/

#define MAX_NUMBER_RANGE 100
void CountNumbers(int *values, int count)
{
    int numCount[MAX_NUMBER_RANGE] = {0};
    for (int i = 0; i < count; i++)
        numCount[values[i] - 1]++;
}

/*
//单循环遍历3*3网格
for (int i = 0; i < 9; i++)
{
    int row = i / 3;
    int col = i % 3;
    game->cells[row][col].fixed = false;
}
*/

/*
// 从第v个顶点出发递归地深度优先遍历图G
void DFS(Graph G, int v)
{
    VisitFunction(v);            // 访问v点;
    for_each(vi : v的所有邻接点) // 遍历v的所有邻接点
    {
        if (vi没有被访问过)
            DFS(G, vi);
    }
}
*/

/*
void GraphTravel(Graph G)
{
    for_each(v : G的所有顶点) // 遍历G的所有顶点
    {
        DFS(G, v);
    }
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
#endif

bool IsLeftBrackets(string::value_type ch)
{
    return ch == '(';
}

bool IsRightBrackets(string::value_type ch)
{
    return ch == ')';
}

bool IsMatchBrackets(const string &express)
{
    stack<string::value_type> epStk;
    for (string::size_type i = 0; i < express.length(); i++)
    {
        if (IsLeftBrackets(express[i]))
        {
            epStk.push(express[i]);
        }
        else if (IsRightBrackets(express[i]))
        {
            if (epStk.empty())
                return false;
            epStk.pop();
        }
    }

    return epStk.empty();
}

/*
void someFunction
{
double kg = gScale * 102.1 + 55.3;
NotifyModule1(kk);
double kl1 = kg / l_mask;
NotifyModule2(kl1);
double kl2 = kg * 1.25 / l_mask;
NotifyModule2(kl2);

double globalKerp = GetGlobalKerp();
NotifyGlobalModule(globalKerp);
double localKrep = globalKerp / localMask;
NotifyLocalModule(localKrep);
double localKrepBoost = globalKerp * 1.25 / localMask;
NotifyLocalModule(localKrepBoost);
}
*/

int main()
{
    bool rtn = IsMatchBrackets(string("((()))"));
    rtn = IsMatchBrackets(string("(44(55)fdg( )  df)"));
    rtn = IsMatchBrackets(string("(ss(ddf))ff)"));
    return 0;
}
