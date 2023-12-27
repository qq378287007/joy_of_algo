#include <vector>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <stack>
#include <list>
#include <functional>
using namespace std;

// common
#define COUNT_OF(x) (sizeof(x) / sizeof(x[0]))
#define ROUND_INT(x) ((x > 0) ? (int)(x + 0.5) : (int)(x - 0.5))

// point
struct POINT
{
    int x;
    int y;
};
struct Point
{
    int x;
    int y;
    Point(int x_ = 0, int y_ = 0) : x(x_), y(y_) {}
};

// edge_def
struct EDGE
{
    double xi;
    double dx;
    int ymax;
#ifdef _DEBUG
    POINT ps;
    POINT pe;
#endif
};
struct EDGE2
{
    double xi;
    double dx;
    int ymax;
    int dy;
#ifdef _DEBUG
    POINT ps;
    POINT pe;
#endif
};
struct EDGE3
{
    double xi;
    double dx;
    int ymax;
    int ymin;
#ifdef _DEBUG
    POINT ps;
    POINT pe;
#endif
};

struct Polygon
{
    vector<Point> pts;
    Polygon(POINT *p, int count)
    {
        for (int i = 0; i < count; i++)
            pts.push_back(Point(p[i].x, p[i].y));
    }
    // Polygon(const Polygon &py) { copy(py.pts.begin(), py.pts.end(), back_inserter(pts)); }

    inline bool IsValid() const { return pts.size() >= 3; }
    inline int GetPolyCount() const { return static_cast<int>(pts.size()); }
};

// dev_adp
static const int MAX_MAP_WIDTH = 16;
static const int MAX_MAP_HEIGHT = 16;
static int color_pixel_map[MAX_MAP_WIDTH][MAX_MAP_HEIGHT] = {{0}};
static int map_width = 0;
static int map_height = 0;
int GetPixelColor(int x, int y)
{
    assert((x >= 0) && (x < MAX_MAP_WIDTH));
    assert((y >= 0) && (y < MAX_MAP_HEIGHT));
    return color_pixel_map[y][x];
}

void SetPixelColor(int x, int y, int color)
{
    assert((x >= 0) && (x < MAX_MAP_WIDTH));
    assert((y >= 0) && (y < MAX_MAP_HEIGHT));
    color_pixel_map[y][x] = color;
}

void DrawHorizontalLine(int x1, int x2, int y, int color)
{
    y = (MAX_MAP_HEIGHT - 1) - y;
    for (int i = x1; i <= x2; i++)
        SetPixelColor(i, y, color);
}

void FillBackground(int color)
{
    for (int i = 0; i < map_width; i++)
        for (int j = 0; j < map_height; j++)
            color_pixel_map[i][j] = color;
}

void SetPixelMap(int map[MAX_MAP_WIDTH][MAX_MAP_HEIGHT], int width, int height)
{
    assert((width > 0) && (width <= MAX_MAP_WIDTH));
    assert((height > 0) && (height <= MAX_MAP_HEIGHT));

    map_width = width;
    map_height = height;
    for (int i = 0; i < map_width; i++)
        for (int j = 0; j < map_height; j++)
            color_pixel_map[i][j] = map[i][j];
}

void PrintCurrentPixelMap()
{
    for (int i = 0; i < map_width; i++)
    {
        for (int j = 0; j < map_height; j++)
            cout << color_pixel_map[i][j] << ' ';
        cout << endl;
    }
    cout << endl;
}

// function
void GetPolygonMinMax(const Polygon &py, int &ymin, int &ymax)
{
    ymin = py.pts[0].y;
    ymax = py.pts[0].y;
    for (int i = 1; i < py.GetPolyCount(); i++)
    {
        if (ymin > py.pts[i].y)
            ymin = py.pts[i].y;
        if (ymax < py.pts[i].y)
            ymax = py.pts[i].y;
    }
}

void HorizonEdgeFill(const Polygon &py, int color)
{
    for (int i = 0; i < py.GetPolyCount(); i++)
    {
        const Point &ps = py.pts[i];
        const Point &pe = py.pts[(i + 1) % py.GetPolyCount()];

        if (pe.y == ps.y)
        {
            if (ps.x <= pe.x)
                DrawHorizontalLine(ps.x, pe.x, ps.y, color);
            else
                DrawHorizontalLine(pe.x, ps.x, ps.y, color);
        }
    }
}

// recursion_seed_fill
struct DIRECTION
{
    int x_offset;
    int y_offset;
};
static const DIRECTION direction_8[] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};
static const DIRECTION direction_4[] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

void FloodSeedFill(int x, int y, int old_color, int new_color)
{
    if (GetPixelColor(x, y) == old_color)
    {
        SetPixelColor(x, y, new_color);
        for (int i = 0; i < COUNT_OF(direction_8); i++)
            FloodSeedFill(x + direction_8[i].x_offset, y + direction_8[i].y_offset, old_color, new_color);
    }
}

void BoundarySeedFill(int x, int y, int new_color, int boundary_color)
{
    int curColor = GetPixelColor(x, y);
    if ((curColor != boundary_color) && (curColor != new_color))
    {
        SetPixelColor(x, y, new_color);
        for (int i = 0; i < COUNT_OF(direction_8); i++)
            BoundarySeedFill(x + direction_8[i].x_offset, y + direction_8[i].y_offset, new_color, boundary_color);
    }
}

// scanline_seed_fill
static int FillLineRight(int x, int y, int new_color, int boundary_color)
{
    int xt = x;
    for (; GetPixelColor(xt, y) != boundary_color; xt++)
        SetPixelColor(xt, y, new_color);
    return xt - x;
}

static int FillLineLeft(int x, int y, int new_color, int boundary_color)
{
    int xt = x;
    for (; GetPixelColor(xt, y) != boundary_color; xt--)
        SetPixelColor(xt, y, new_color);
    return x - xt;
}

// 点不是边界，而且没有填充过
inline static bool IsPixelValid(int x, int y, int new_color, int boundary_color)
{
    return ((GetPixelColor(x, y) != boundary_color) && (GetPixelColor(x, y) != new_color));
}

static int SkipInvalidInLine(int x, int y, int xRight, int new_color, int boundary_color)
{
    int xt = x;
    while (!IsPixelValid(xt, y, new_color, boundary_color) && (xt < xRight))
        xt++;
    return xt - x;
}

static void SearchLineNewSeed(stack<Point> &stk, int xLeft, int xRight, int y, int new_color, int boundary_color)
{
    int xt = xLeft;
    while (xt <= xRight)
    {
        bool findNewSeed = false;
        while (IsPixelValid(xt, y, new_color, boundary_color) && (xt < xRight))
        {
            findNewSeed = true;
            xt++;
        }
        if (findNewSeed)
        {
            if (IsPixelValid(xt, y, new_color, boundary_color) && (xt == xRight))
                stk.push(Point(xt, y));
            else
                stk.push(Point(xt - 1, y));
        }

        // 向右跳过内部的无效点（处理区间右端有障碍点的情况）
        int xspan = SkipInvalidInLine(xt, y, xRight, new_color, boundary_color);
        xt += (xspan == 0) ? 1 : xspan;
        // 处理特殊情况,以退出while(x <= xright)循环
    }
}

void ScanLineSeedFill(int x, int y, int new_color, int boundary_color)
{
    stack<Point> stk;

    stk.push(Point(x, y)); // 第1步，种子点入站
    while (!stk.empty())
    {
        Point seed = stk.top(); // 第2步，取当前种子点
        stk.pop();

        // 第3步，向左右填充
        int count = FillLineRight(seed.x, seed.y, new_color, boundary_color); // 向右填充
        int xRight = seed.x + count - 1;
        count = FillLineLeft(seed.x - 1, seed.y, new_color, boundary_color); // 向左填充
        int xLeft = seed.x - count;

        // 第4步，处理相邻两条扫描线
        SearchLineNewSeed(stk, xLeft, xRight, seed.y - 1, new_color, boundary_color);
        SearchLineNewSeed(stk, xLeft, xRight, seed.y + 1, new_color, boundary_color);
    }
}

// scanline_fill
static void InitScanLineNewEdgeTable(vector<list<EDGE>> &slNet, const Polygon &py, int ymin, int ymax)
{
    EDGE e;
    for (int i = 0; i < py.GetPolyCount(); i++)
    {
        const Point &ps = py.pts[i];
        const Point &pe = py.pts[(i + 1) % py.GetPolyCount()];
        const Point &pss = py.pts[(i - 1 + py.GetPolyCount()) % py.GetPolyCount()];
        const Point &pee = py.pts[(i + 2) % py.GetPolyCount()];

#ifdef _DEBUG
        e.ps.x = ps.x;
        e.ps.y = ps.y;
        e.pe.x = pe.x;
        e.pe.y = pe.y;
#endif
        if (pe.y != ps.y) // 不处理水平线
        {
            e.dx = double(pe.x - ps.x) / double(pe.y - ps.y);
            if (pe.y > ps.y)
            {
                e.xi = ps.x;
                if (pee.y >= pe.y)
                    e.ymax = pe.y - 1;
                else
                    e.ymax = pe.y;

                slNet[ps.y - ymin].push_front(e);
            }
            else
            {
                e.xi = pe.x;
                if (pss.y >= ps.y)
                    e.ymax = ps.y - 1;
                else
                    e.ymax = ps.y;
                slNet[pe.y - ymin].push_front(e);
            }
        }
    }
}

static void PrintEdgeInfo(const EDGE &e)
{
    cout << "\t";
#ifdef _DEBUG
    cout << "ps = (" << e.ps.x << ", " << e.ps.y << ") pe = (";
    cout << e.pe.x << ", " << e.pe.y << ") ";
#endif
    cout << "x1 = " << e.xi << " dx = " << e.dx << " ymax = " << e.ymax << endl;
}

static void PrintNewEdgeTable(const vector<list<EDGE>> &slNet)
{
    int lCount = static_cast<int>(slNet.size());
    for (int i = 0; i < lCount; i++)
    {
        const list<EDGE> &eList = slNet[i];
        if (eList.empty())
        {
            cout << "Scan-Line " << i << " has empty NET !" << endl;
        }
        else
        {
            cout << "Scan-Line " << i << " has " << eList.size() << " NET(s) !" << endl;
            for_each(eList.cbegin(), eList.cend(), PrintEdgeInfo);
        }
    }
}

// aet是有序表，按照EDGE.x1从小到大排序，插入操作必须保证aet有序
static void InsertEdgeToAet(const EDGE &e, list<EDGE> &aet)
{
    if (!aet.empty())
    {
        for (list<EDGE>::iterator it = aet.begin(); it != aet.end(); ++it)
        {
            if (e.xi <= it->xi)
            {
                aet.insert(it, e);
                return;
            }
        }
    }
    aet.push_back(e);
}

// 将新边表中的边逐一插入到活动边表中
static void InsertNetListToAet(const list<EDGE> &net, list<EDGE> &aet)
{
    for (list<EDGE>::const_iterator it = net.cbegin(); it != net.cend(); ++it)
        InsertEdgeToAet(*it, aet);
}

static void FillAetScanLine(const list<EDGE> &aet, int y, int color)
{
    assert((aet.size() % 2) == 0);

    list<EDGE>::const_iterator it = aet.cbegin();
    while (it != aet.cend())
    {
        int x1 = ROUND_INT(it->xi);
        ++it;
        int x2 = ROUND_INT(it->xi);
        ++it;

        DrawHorizontalLine(x1, x2, y, color);
    }
}

inline static bool IsEdgeOutOfActive(const EDGE &e, int y) { return e.ymax == y; }

static void RemoveNonActiveEdgeFromAet(list<EDGE> &aet, int y)
{
    aet.remove_if(bind(IsEdgeOutOfActive, placeholders::_1, y));
}

inline static void UpdateAetEdgeInfo(EDGE &e) { e.xi += e.dx; }

inline static bool EdgeXiComparator(const EDGE &e1, const EDGE &e2) { return (e1.xi <= e2.xi); }

static void UpdateAndResortAet(list<EDGE> &aet)
{
    for_each(aet.begin(), aet.end(), UpdateAetEdgeInfo); // 更新xi
    aet.sort(EdgeXiComparator);                          // 根据xi从小到大重新排序
}

static void ProcessScanLineFill(const vector<list<EDGE>> &slNet, int ymin, int ymax, int color)
{
    list<EDGE> aet;
    for (int y = ymin; y <= ymax; y++)
    {
        InsertNetListToAet(slNet[y - ymin], aet);
        FillAetScanLine(aet, y, color);
        RemoveNonActiveEdgeFromAet(aet, y); // 删除非活动边
        UpdateAndResortAet(aet);            // 更新活动边表中每项的xi值，并根据xi重新排序
    }
}

void ScanLinePolygonFill(const Polygon &py, int color)
{
    assert(py.IsValid());

    int ymin = 0;
    int ymax = 0;
    GetPolygonMinMax(py, ymin, ymax);
    vector<list<EDGE>> slNet(ymax - ymin + 1);
    InitScanLineNewEdgeTable(slNet, py, ymin, ymax);
    // PrintNewEdgeTable(slNet);
    HorizonEdgeFill(py, color); // 水平边直接画线填充
    ProcessScanLineFill(slNet, ymin, ymax, color);
}

// scanline_fill_o
struct SP_EDGES_TABLE
{
    vector<EDGE2> slEdges;
    int first;
    int last;
};

// slEdges是按照maxy从大到小有序的表，插入操作要满足这个条件
static void InsertEdgeToEdgesTable(const EDGE2 &e, vector<EDGE2> &slEdges)
{
    if (!slEdges.empty())
    {
        for (vector<EDGE2>::iterator it = slEdges.begin(); it != slEdges.end(); ++it)
        {
            if (e.ymax >= it->ymax)
            {
                slEdges.insert(it, e);
                return;
            }
        }
    }
    slEdges.push_back(e);
}

static void InitScanLineEdgesTable(SP_EDGES_TABLE &spET, const Polygon &py)
{
    EDGE2 e;
    for (int i = 0; i < py.GetPolyCount(); i++)
    {
        const Point &ps = py.pts[i];
        const Point &pe = py.pts[(i + 1) % py.GetPolyCount()];
        const Point &pee = py.pts[(i + 2) % py.GetPolyCount()];

#ifdef _DEBUG
        e.ps.x = ps.x;
        e.ps.y = ps.y;
        e.pe.x = pe.x;
        e.pe.y = pe.y;
#endif
        if (pe.y != ps.y) // 不处理水平线
        {
            e.dx = double(pe.x - ps.x) / double(pe.y - ps.y);
            if (pe.y > ps.y)
            {
                if (pe.y < pee.y) // 左顶点
                {
                    e.xi = pe.x - e.dx;
                    e.ymax = pe.y - 1;
                    e.dy = e.ymax - ps.y + 1;
                }
                else
                {
                    e.xi = pe.x;
                    e.ymax = pe.y;
                    e.dy = pe.y - ps.y + 1;
                }
            }
            else //(pe.y < ps.y)
            {
                if (pe.y > pee.y) // 右顶点
                {
                    e.xi = ps.x;
                    e.ymax = ps.y;
                    e.dy = ps.y - (pe.y + 1) + 1;
                }
                else
                {
                    e.xi = ps.x;
                    e.ymax = ps.y;
                    e.dy = ps.y - pe.y + 1;
                }
            }

            InsertEdgeToEdgesTable(e, spET.slEdges);
        }
    }
    spET.first = spET.last = 0;
}

static void UpdateEdgesTableActiveRange(SP_EDGES_TABLE &spET, int yScan)
{
    vector<EDGE2> &slET = spET.slEdges;
    int edgesCount = static_cast<int>(slET.size());
    while ((spET.last < (edgesCount - 1)) && (slET[spET.last + 1].ymax >= yScan))
        spET.last++;
    while (slET[spET.first].dy == 0)
        spET.first++;
}

static int GetIntersectionInActiveRange(SP_EDGES_TABLE &spET, int pos)
{
    vector<EDGE2> &slET = spET.slEdges;
    assert((pos >= 0) && (pos < static_cast<int>(slET.size())));

    int isPos = pos;
    while ((isPos <= spET.last) && (slET[isPos].dy == 0))
        isPos++;

    if (isPos <= spET.last)
        return isPos;

    return -1;
}

static void FillActiveRangeScanLine(SP_EDGES_TABLE &spET, int yScan, int color)
{
    vector<EDGE2> &slET = spET.slEdges;
    int pos = spET.first;

    do
    {
        pos = GetIntersectionInActiveRange(spET, pos);
        if (pos != -1)
        {
            int x1 = ROUND_INT(slET[pos].xi);
            pos = GetIntersectionInActiveRange(spET, pos + 1);
            if (pos != -1)
            {
                int x2 = ROUND_INT(slET[pos].xi);
                pos++;
                DrawHorizontalLine(x1, x2, yScan, color);
            }
            else
            {
                assert(false);
            }
        }
    } while (pos != -1);
}

inline static bool EdgeXiComparator2(const EDGE2 &e1, const EDGE2 &e2) { return e1.xi < e2.xi; }

static void SortActiveRangeByX(SP_EDGES_TABLE &spET)
{
    vector<EDGE2> &slET = spET.slEdges;
    sort(slET.begin() + spET.first, slET.begin() + spET.last + 1, EdgeXiComparator2);
}

static void UpdateActiveRangeIntersection(SP_EDGES_TABLE &spET)
{
    for (int pos = spET.first; pos <= spET.last; pos++)
    {
        if (spET.slEdges[pos].dy > 0)
        {
            spET.slEdges[pos].dy--;
            spET.slEdges[pos].xi -= spET.slEdges[pos].dx;
        }
    }
}

static void ProcessScanLineFill2(SP_EDGES_TABLE &spET, int ymin, int ymax, int color)
{
    for (int yScan = ymax; yScan >= ymin; yScan--)
    {
        UpdateEdgesTableActiveRange(spET, yScan);
        SortActiveRangeByX(spET);
        FillActiveRangeScanLine(spET, yScan, color);
        UpdateActiveRangeIntersection(spET);
    }
}

void ScanLinePolygonFill2(const Polygon &py, int color)
{
    assert(py.IsValid());

    int ymin = 0;
    int ymax = 0;
    GetPolygonMinMax(py, ymin, ymax);
    SP_EDGES_TABLE spET;
    InitScanLineEdgesTable(spET, py);
    HorizonEdgeFill(py, color); // 水平边直接画线填充
    ProcessScanLineFill2(spET, ymin, ymax, color);
}

// edge_center_fill
static const int MAX_X_CORD = MAX_MAP_WIDTH - 1;
static const int A = 9;

static void InitScanLineEdgesTable(vector<EDGE3> &et, const Polygon &py)
{
    EDGE3 e;
    for (int i = 0; i < py.GetPolyCount(); i++)
    {
        const Point &ps = py.pts[i];
        const Point &pe = py.pts[(i + 1) % py.GetPolyCount()];
        const Point &pee = py.pts[(i + 2) % py.GetPolyCount()];

#ifdef _DEBUG
        e.ps.x = ps.x;
        e.ps.y = ps.y;
        e.pe.x = pe.x;
        e.pe.y = pe.y;
#endif
        if (pe.y != ps.y) // 不处理水平线
        {
            e.dx = double(pe.x - ps.x) / double(pe.y - ps.y);
            if (pe.y > ps.y)
            {
                if (pe.y < pee.y) // 左顶点
                {
                    e.xi = pe.x - e.dx;
                    e.ymax = pe.y - 1;
                }
                else
                {
                    e.xi = pe.x;
                    e.ymax = pe.y;
                }
                e.ymin = ps.y;
            }
            else //(pe.y < ps.y)
            {
                if (pe.y > pee.y) // 右顶点
                {
                    e.xi = ps.x;
                    e.ymax = ps.y;
                }
                else
                {
                    e.xi = ps.x;
                    e.ymax = ps.y;
                }
                e.ymin = pe.y;
            }
            et.push_back(e);
        }
    }
}

static void ComplementScanLineColor(int x1, int x2, int y)
{
    y = (MAX_MAP_HEIGHT - 1) - y;
    for (int x = x1; x <= x2; x++)
    {
        int color = GetPixelColor(x, y);
        SetPixelColor(x, y, A - color);
    }
}

static void EdgeScanMarkColor(EDGE3 &e)
{
    for (int y = e.ymax; y >= e.ymin; y--)
    {
        int x = ROUND_INT(e.xi);
        ComplementScanLineColor(x, MAX_X_CORD, y);
        e.xi -= e.dx;
    }
}

static void FenceScanMarkColor(EDGE3 e, int fence)
{
    for (int y = e.ymax; y >= e.ymin; y--)
    {
        int x = ROUND_INT(e.xi);
        if (x > fence)
            ComplementScanLineColor(fence, x, y);
        else
            ComplementScanLineColor(x, fence - 1, y);
        e.xi -= e.dx;
    }
}

void EdgeCenterMarkFill(const Polygon &py, int color)
{
    vector<EDGE3> et;
    InitScanLineEdgesTable(et, py);                    // 初始化边表
    FillBackground(A - color);                         // 对整个填充区域背景颜色取补
    for_each(et.begin(), et.end(), EdgeScanMarkColor); // 依次处理每一条边
}

void EdgeFenceMarkFill(const Polygon &py, int fence, int color)
{
    vector<EDGE3> et;
    InitScanLineEdgesTable(et, py);                                                    // 初始化边表
    FillBackground(A - color);                                                         // 对整个填充区域背景颜色取补
    for_each(et.begin(), et.end(), bind(FenceScanMarkColor, placeholders::_1, fence)); // 依次处理每一条边
}

// edge_mark_fill.h
void EdgeMarkFill(int xmin, int xmax, int ymin, int ymax, int boundarycolor, int color)
{
    for (int y = ymin; y <= ymax; y++)
    {
        int flag = -1;
        for (int x = xmin; x <= xmax; x++)
        {
            if (GetPixelColor(x, y) == boundarycolor)
                flag = -flag;
            if (flag == 1)
                SetPixelColor(x, y, color);
        }
    }
}

int flood_seed_map[MAX_MAP_WIDTH][MAX_MAP_HEIGHT] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0},
    {0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0},
    {0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0},
    {0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0},
    {0, 0, 2, 2, 2, 2, 2, 2, 0, 0, 2, 2, 0, 0, 0, 0},
    {0, 0, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 0, 0, 0},
    {0, 0, 0, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0},
    {0, 0, 0, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

int boundary_seed_map[MAX_MAP_WIDTH][MAX_MAP_HEIGHT] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 0, 0, 0, 0},
    {0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0, 0},
    {0, 1, 2, 2, 3, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0, 0},
    {0, 1, 2, 2, 3, 3, 2, 2, 2, 2, 1, 1, 1, 0, 0, 0},
    {0, 1, 3, 2, 3, 2, 2, 3, 1, 1, 2, 2, 1, 1, 0, 0},
    {0, 1, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 3, 1, 1, 0},
    {0, 1, 1, 2, 2, 2, 1, 1, 2, 2, 2, 3, 2, 2, 1, 0},
    {0, 0, 1, 2, 2, 2, 1, 1, 2, 2, 3, 2, 2, 2, 1, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 3, 3, 2, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 3, 2, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

// seed (6, 7)
int scanline_seed_map[MAX_MAP_WIDTH][MAX_MAP_HEIGHT] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0},
    {0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

int scanline_poly_map[MAX_MAP_WIDTH][MAX_MAP_HEIGHT] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

int edge_center_map[MAX_MAP_WIDTH][MAX_MAP_HEIGHT] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

void TestFloodSeedFill()
{
    SetPixelMap(flood_seed_map, 16, 16);
    PrintCurrentPixelMap();
    FloodSeedFill(4, 5, 2, 4);
    PrintCurrentPixelMap();
}

void TestBoundarySeedFill()
{
    SetPixelMap(boundary_seed_map, 16, 16);
    PrintCurrentPixelMap();
    BoundarySeedFill(4, 5, 4, 1);
    PrintCurrentPixelMap();
}

void TestScanLineSeedFill()
{
    SetPixelMap(scanline_seed_map, 16, 16);
    PrintCurrentPixelMap();
    ScanLineSeedFill(6, 7, 4, 1);
    PrintCurrentPixelMap();
}

void TestScanLineFill()
{
    POINT pts[] = {{2, 8}, {1, 3}, {5, 1}, {13, 5}, {9, 10}, {7, 6}};
    Polygon py = Polygon(pts, COUNT_OF(pts));

    SetPixelMap(scanline_poly_map, 16, 16);

    ScanLinePolygonFill(py, 2);

    PrintCurrentPixelMap();
}

void TestScanLineFill2()
{
    POINT pts[] = {{2, 8}, {1, 3}, {5, 1}, {13, 5}, {9, 10}, {7, 6}};
    Polygon py = Polygon(pts, COUNT_OF(pts));

    SetPixelMap(scanline_poly_map, 16, 16);

    ScanLinePolygonFill2(py, 2);

    PrintCurrentPixelMap();
}

void TestEdgeCenterMarkFill()
{
    POINT pts[] = {{2, 1}, {12, 1}, {12, 14}, {5, 8}, {2, 11}};
    Polygon py = Polygon(pts, COUNT_OF(pts));

    SetPixelMap(edge_center_map, 16, 16);

    EdgeCenterMarkFill(py, 2);

    PrintCurrentPixelMap();
}

void TestEdgeFenceMarkFill()
{
    POINT pts[] = {{2, 1}, {12, 1}, {12, 14}, {5, 8}, {2, 11}};
    Polygon py = Polygon(pts, COUNT_OF(pts));

    SetPixelMap(edge_center_map, 16, 16);

    EdgeFenceMarkFill(py, 5, 2);

    PrintCurrentPixelMap();
}

int main()
{
    TestFloodSeedFill();
    TestBoundarySeedFill();
    TestScanLineSeedFill();

    TestScanLineFill();

    TestScanLineFill2();

    TestEdgeCenterMarkFill();

    TestEdgeFenceMarkFill();

    cout << "Over!\n";
    return 0;
}
