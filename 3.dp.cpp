#include <string.h>
#include <stdlib.h>
#include <math.h>

const int INVALID_VALUE = 0xFFFF;
const int MAX_STRING_LEN = 50;

// #define min(a, b)  (((a) < (b)) ? (a) : (b))

typedef struct
{
    int distance;
    int refCount;
} MEMO_RECORD;

MEMO_RECORD memo[MAX_STRING_LEN][MAX_STRING_LEN];

static int recC = 0;
int EditDistance(char *src, char *dest, int i, int j)
{
    recC++;
    if (memo[i][j].refCount != 0) // 查表，直接返回
    {
        memo[i][j].refCount++;
        return memo[i][j].distance;
    }
    int distance = 0;
    if (strlen(src + i) == 0)
    {
        distance = strlen(dest + j);
    }
    else if (strlen(dest + j) == 0)
    {
        distance = strlen(src + i);
    }
    else
    {
        if (src[i] == dest[j])
        {
            distance = EditDistance(src, dest, i + 1, j + 1);
        }
        else
        {
            int edIns = EditDistance(src, dest, i, j + 1) + 1;     // source 插入字符
            int edDel = EditDistance(src, dest, i + 1, j) + 1;     // source 删除字符
            int edRep = EditDistance(src, dest, i + 1, j + 1) + 1; // source 替换字符
            distance = __min(__min(edIns, edDel), edRep);
        }
    }

    memo[i][j].distance = distance;
    memo[i][j].refCount = 1;

    return distance;
}

int EditDistance2(char *src, char *dest)
{
    if ((strlen(src) == 0) || (strlen(dest) == 0))
        return abs((int)strlen(src) - (int)strlen(dest));

    if (src[0] == dest[0])
        return EditDistance2(src + 1, dest + 1);

    int edIns = EditDistance2(src, dest + 1) + 1;     // source 插入字符
    int edDel = EditDistance2(src + 1, dest) + 1;     // source 删除字符
    int edRep = EditDistance2(src + 1, dest + 1) + 1; // source 替换字符

    return __min(__min(edIns, edDel), edRep);
}

int EditDistance3(char *src, char *dest)
{
    int i, j;
    int d[MAX_STRING_LEN][MAX_STRING_LEN] = {0xFFFF};

    for (i = 0; i <= strlen(src); i++)
        d[i][0] = i;
    for (j = 0; j <= strlen(dest); j++)
        d[0][j] = j;

    for (i = 1; i <= strlen(src); i++)
    {
        for (j = 1; j <= strlen(dest); j++)
        {
            if ((src[i - 1] == dest[j - 1]))
            {
                d[i][j] = d[i - 1][j - 1]; // 不需要编辑操作
            }
            else
            {
                int edIns = d[i][j - 1] + 1;     // source 插入字符
                int edDel = d[i - 1][j] + 1;     // source 删除字符
                int edRep = d[i - 1][j - 1] + 1; // source 替换字符

                d[i][j] = __min(__min(edIns, edDel), edRep);
            }
        }
    }

    return d[strlen(src)][strlen(dest)];
}

int main(int argc, char *argv[])
{
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

    return 0;
}
