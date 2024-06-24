#include <cstring>
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

// 数组实现固定长度缓冲区
namespace n1
{
    static const int SYNC_LOG_CNT = 1000;
    static const int SYNC_LOG_MEMOVER_CNT = 50;

    using INT32U = int;
    using EPON_SYNC_LOG_DATA = int;

    struct EPON_SYNC_LOG
    {
        INT32U logCnt;
        EPON_SYNC_LOG_DATA syncLogs[SYNC_LOG_CNT];
    };
    static EPON_SYNC_LOG s_EponSyncLog;

    static void Epon_Sync_Log_Add(EPON_SYNC_LOG_DATA *pLogData)
    {
        INT32U syncLogCnt = s_EponSyncLog.logCnt;
        if (syncLogCnt < SYNC_LOG_CNT) // 如果缓冲区有空间，则直接写入当前一条记录
        {
            memmove(s_EponSyncLog.syncLogs + syncLogCnt, pLogData, sizeof(EPON_SYNC_LOG_DATA));
            s_EponSyncLog.logCnt++;
        }
        else // 缓冲区已满，向前移动50条记录，为新纪录腾出空间
        {
            memmove(s_EponSyncLog.syncLogs, s_EponSyncLog.syncLogs + SYNC_LOG_MEMOVER_CNT, (SYNC_LOG_CNT - SYNC_LOG_MEMOVER_CNT) * sizeof(EPON_SYNC_LOG_DATA));

            // 清空新腾出来的空间
            memset(s_EponSyncLog.syncLogs + (SYNC_LOG_CNT - SYNC_LOG_MEMOVER_CNT), 0, SYNC_LOG_MEMOVER_CNT * sizeof(EPON_SYNC_LOG_DATA));

            // 写入当前一条日志
            memmove(s_EponSyncLog.syncLogs + (SYNC_LOG_CNT - SYNC_LOG_MEMOVER_CNT), pLogData, sizeof(EPON_SYNC_LOG_DATA));

            s_EponSyncLog.logCnt = SYNC_LOG_CNT - SYNC_LOG_MEMOVER_CNT + 1;
        }
    }
}

// 环形队列优化固定长度缓冲区
namespace n2
{
    static const int N = 1000 + 1;
    struct EPON_SYNC_LOG
    {
        int head{0}; // head开始
        int tail{0}; // 不包括tail
        int syncLogs[N];

        inline int number() const { return (N + tail - head) % N; }

        void push(int data)
        {
            syncLogs[tail] = data;
            tail = (tail + 1) % N;
            if (head == tail)
                head = (head + 1) % N;
        }
        void push(const vector<int> &datas)
        {
            for (int data : datas)
                push(data);
        }

        bool pop(int &data)
        {
            if (head == tail)
                return false;

            data = syncLogs[tail];
            tail = (N + tail - 1) % N;
            return true;
        }
    };
}

namespace n3
{
    static const int MAX_DIRTY_POINT = 4; // 相邻连接点个数阈值，用于判断污点(不超过5个点相连)
    static const int MAX_BMP_WIDTH = 1600;
    static const int MAX_BMP_HEIGHT = 1200;

    struct POINT
    {
        int x;
        int y;
    };

    struct RESULT
    {
        POINT pts[MAX_DIRTY_POINT]; // 记录搜索过的前4个点的位置
        int count;
    };

    static const POINT dir[] = {{-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}}; // 8邻域
    // static const POINT dir[] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};//4邻域

    static void SearchDirty(char bmp[MAX_BMP_WIDTH][MAX_BMP_HEIGHT], int x, int y, RESULT *result)
    {
        for (int i = 0; i < sizeof(dir) / sizeof(dir[0]); i++)
        {
            int nx = x + dir[i].x;
            int ny = y + dir[i].y;
            if ((nx >= 0 && nx < MAX_BMP_WIDTH) && (ny >= 0 && nx < MAX_BMP_HEIGHT) && (bmp[nx][ny] == 1))
            {
                if (result->count < MAX_DIRTY_POINT) // 记录前MAX_DIRTY_POINT个点的位置
                {
                    result->pts[result->count].x = nx;
                    result->pts[result->count].y = ny;
                }
                result->count++;
                if (result->count > MAX_DIRTY_POINT)
                    break;

                SearchDirty(bmp, nx, ny, result);
            }
        }
    }
}

struct Point
{
    int x, y;
};
static const vector<Point> dir4{{-1, 0}, {0, -1}, {1, 0}, {0, 1}};                                     // 4邻域
static const vector<Point> dir8{{-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}}; // 8邻域
static const int height = 400;
static const int width = 600;

// 递归深度过深，栈溢出风险
struct Image
{
    vector<vector<int>> values{height, vector<int>(width, 1)};
    vector<vector<bool>> notVisited{height, vector<bool>(width, true)};

    void search(const vector<Point> &dir, int x0, int y0, vector<Point> &points)
    {
        notVisited[x0][y0] = false;
        points.push_back({x0, y0});

        for (Point p : dir)
        {
            int x = x0 + p.x;
            int y = y0 + p.y;
            if (x < 0 || x >= height || y < 0 || y >= width || !notVisited[x][y] || values[x][y] == 0)
                continue;
            search(dir, x, y, points);
        }
    }
    vector<vector<Point>> search(const vector<Point> &dir)
    {
        vector<vector<Point>> vp;
        for (int x = 0; x < height; x++)
        {
            for (int y = 0; y < width; y++)
            {
                if (notVisited[x][y] && values[x][y] == 1)
                {
                    vector<Point> points;
                    search(dir, x, y, points);
                    vp.emplace_back(points);
                    cout << points.size() << endl;
                }
            }
        }
        return vp;
    };
};

struct Image2
{
    vector<vector<int>> values{height, vector<int>(width, 1)};
    vector<vector<bool>> notVisited{height, vector<bool>(width, true)};

    vector<Point> search(const vector<Point> &dir, int x0, int y0)
    {
        notVisited[x0][y0] = false;

        vector<Point> points;
        queue<Point> q;
        q.push({x0, y0});
        while (!q.empty())
        {
            Point p = q.front();
            for (Point curP : dir)
            {
                int x = p.x + curP.x;
                int y = p.y + curP.y;
                if (x >= 0 && x < height && y >= 0 && y < width && notVisited[x][y] && values[x][y] == 1)
                {
                    q.push({x, y});
                    notVisited[x][y] = false;
                }
            }
            points.emplace_back(p);
            q.pop();
        }
        return points;
    }
    vector<vector<Point>> search(const vector<Point> &dir)
    {
        vector<vector<Point>> vp;
        for (int x = 0; x < height; x++)
            for (int y = 0; y < width; y++)
                if (notVisited[x][y] && values[x][y] == 1)
                    vp.emplace_back(search(dir, x, y));
        return vp;
    };
};

void NotifyModule1(double) {}
void NotifyModule2(double) {}
static const double gScale = 1.0;
static const double kk = 1.0;
static const double l_mask = 1.0;
void func1()
{
    double kg = gScale * 102.1 + 55.3;
    NotifyModule1(kk);
    double kl1 = kg / l_mask;
    NotifyModule2(kl1);
    double kl2 = kg * 1.25 / l_mask;
    NotifyModule2(kl2);
}

double GetGlobalKerp()
{
    return 1.0;
}
void NotifyGlobalModule(double) {}
void NotifyLocalModule(double) {}
static const double localMask = 1.0;
void func2()
{
    double globalKerp = GetGlobalKerp();
    NotifyGlobalModule(globalKerp);
    double localKrep = globalKerp / localMask;
    NotifyLocalModule(localKrep);
    double localKrepBoost = globalKerp * 1.25 / localMask;
    NotifyLocalModule(localKrepBoost);
}

int main()
{
    Image2 img;
    vector<vector<Point>> vp = img.search(dir8);
    cout << vp.size() << endl;
    cout << vp[0].size() << endl;
    return 0;
}
