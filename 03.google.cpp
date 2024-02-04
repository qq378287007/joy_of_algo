#include <cassert>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

/*
Google方程式
WWWDOT-GOOGLE=DOTCOM
每个字符代表0~9的数字（不重复）
WWWDOT、GOOGLE和DOTCOM，都是合法数字，不以0开头
*/
struct CHAR_ITEM
{
    char c;       // 字符
    int value;    // 数字
    bool leading; // 是否是数字最高位（最高位不为0）
};

struct CHAR_VALUE
{
    bool used; // 是否被使用
    int value; // 数字
};

bool GetCharItem(const vector<CHAR_ITEM> &ci, char c, CHAR_ITEM &i)
{
    for (const CHAR_ITEM &item : ci)
    {
        if (item.c == c)
        {
            i = item;
            return true;
        }
    }
    return false;
}

int MakeIntegerValue(const vector<CHAR_ITEM> &ci, const string &chars)
{
    CHAR_ITEM i;
    int value = 0;
    for (char p : chars)
    {
        if (!GetCharItem(ci, p, i))
            return 0;
        value = value * 10 + i.value;
    }
    return value;
}
//static int total = 0;//总组合次数
void OnCharListReady(const vector<CHAR_ITEM> &ci)
{
    //total++;
    const string &minuend = "WWWDOT";
    const string &subtrahend = "GOOGLE";
    const string &diff = "DOTCOM";
    //  const string &minuend    = "AAB";
    //  const string &subtrahend = "BBC";
    //  const string &diff       = "CCD";

    int m = MakeIntegerValue(ci, minuend);
    int s = MakeIntegerValue(ci, subtrahend);
    int d = MakeIntegerValue(ci, diff);
    if (m - s == d)
        cout << m << " - " << s << " = " << d << endl;
}

bool IsValueValid(const CHAR_ITEM &ii, const CHAR_VALUE &vv)
{
    if (ii.leading && vv.value == 0)
        return false;

    if (vv.used)
        return false;

    return true;
}

using CharListReadyFuncPtr = void (*)(const vector<CHAR_ITEM> &ci);
void SearchingResult(vector<CHAR_ITEM> &ci, vector<CHAR_VALUE> &cv, int index, CharListReadyFuncPtr callback)
{
    if (index == ci.size())
    {
        callback(ci);
        return;
    }

    for (int i = 0; i < cv.size(); ++i)
    {
        if (IsValueValid(ci[index], cv[i]))
        {
            cv[i].used = true; // set used sign
            ci[index].value = cv[i].value;
            SearchingResult(ci, cv, index + 1, callback);
            cv[i].used = false; // clear used sign
        }
    }
}

int main(int argc, char *argv[])
{
    vector<CHAR_ITEM> char_item{{'W', -1, true}, {'D', -1, true}, {'O', -1, false}, {'T', -1, false}, {'G', -1, true}, {'L', -1, false}, {'E', -1, false}, {'C', -1, false}, {'M', -1, false}};
    // vector<CHAR_ITEM> char_item{ {'A', -1, true}, {'B', -1, true}, {'C', -1, true}, {'D', -1, false} };

    vector<CHAR_VALUE> char_val{{false, 0}, {false, 1}, {false, 2}, {false, 3}, {false, 4}, {false, 5}, {false, 6}, {false, 7}, {false, 8}, {false, 9}};

    SearchingResult(char_item, char_val, 0, OnCharListReady);

    return 0;
}
