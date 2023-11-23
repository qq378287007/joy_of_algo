#include <string>
#include <iostream>
using namespace std;

const int CHN_CHAR_LENGTH = 3; // utf-8编码，一个汉字占3个字节

const string chnNumChar[] = {"零", "一", "二", "三", "四", "五", "六", "七", "八", "九"};
const string chnUnitChar[] = {"", "十", "百", "千"};
const string chnUnitSection[] = {"", "万", "亿", "万亿"};

struct
{
    string name;  // 中文权位名称
    int value;    // 10的倍数值
    bool secUnit; // 是否是节权位
} chnValuePair[] = {{"十", 10, false}, {"百", 100, false}, {"千", 1000, false}, {"万", 10000, true}, {"亿", 100000000, true}};

struct
{
    unsigned num;
    string chnNumStr;
} testPair[] = {
    {0, "零"},
    {1, "一"},
    {2, "二"},
    {3, "三"},
    {4, "四"},
    {5, "五"},
    {6, "六"},
    {7, "七"},
    {8, "八"},
    {9, "九"},
    {10, "一十"},
    {11, "一十一"},
    {110, "一百一十"},
    {111, "一百一十一"},
    {100, "一百"},
    {102, "一百零二"},
    {1020, "一千零二十"},
    {1001, "一千零一"},
    {1015, "一千零一十五"},
    {1000, "一千"},
    {10000, "一万"},
    {20010, "二万零一十"},
    {20001, "二万零一"},
    {100000, "一十万"},
    {1000000, "一百万"},
    {10000000, "一千万"},
    {100000000, "一亿"},
    {1000000000, "一十亿"},
    {1000001000, "一十亿一千"},
    {1000000100, "一十亿零一百"},
    {200010, "二十万零一十"},
    {2000105, "二百万零一百零五"},
    {20001007, "二千万一千零七"},
    {2000100190, "二十亿零一十万零一百九十"},
    {1040010000, "一十亿四千零一万"},
    {200012301, "二亿零一万二千三百零一"},
    {2005010010, "二十亿零五百零一万零一十"},
    {4009060200, "四十亿零九百零六万零二百"},
    {4294967295, "四十二亿九千四百九十六万七千二百九十五"},
};

string SectionToChinese(unsigned int section)
{
    string chnStr;
    bool zero = true;
    for (int unitPos = 0; section > 0; unitPos++, section /= 10)
    {
        int v = section % 10;
        if (v == 0)
        {
            if (!zero)
            {
                zero = true; // 需要补0，zero的作用是确保对连续的多个0，只补一个中文零
                chnStr.insert(0, chnNumChar[v]);
            }
        }
        else
        {
            zero = false; // 至少有一个数字不是0
            chnStr.insert(0, chnNumChar[v] + chnUnitChar[unitPos]);
        }
    }
    return chnStr;
}
string NumberToChinese(unsigned int num)
{
    if (num == 0)
        return chnNumChar[0];

    string chnStr;
    for (int unitPos = 0; num > 0; unitPos++)
    {
        unsigned int section = num % 10000;
        num /= 10000;

        if (section > 0)
        {
            chnStr.insert(0, SectionToChinese(section) + chnUnitSection[unitPos]);

            if (section < 1000 && num != 0)
                chnStr.insert(0, chnNumChar[0]);
        }
    }
    return chnStr;
}

int ChineseToValue(const string &chnStr)
{
    for (int val = 0; val < sizeof(chnNumChar) / sizeof(chnNumChar[0]); val++)
        if (chnStr.compare(chnNumChar[val]) == 0)
        //if (chnStr == chnNumChar[val])
            return val;
    return -1;
}
int ChineseToUnit(const string &chnStr, bool &secUnit)
{
    for (int unit = 0; unit < sizeof(chnValuePair) / sizeof(chnValuePair[0]); unit++)
    {
        if (chnStr.compare(chnValuePair[unit].name) == 0)
        //if (chnStr == chnValuePair[unit].name)
        {
            secUnit = chnValuePair[unit].secUnit;
            return chnValuePair[unit].value;
        }
    }
    return 1;
}
unsigned int ChineseToNumber(const string &chnString)
{
    const string::size_type len = chnString.length();
    unsigned int section = 0;
    int number = 0;
    string::size_type pos = 0;

    unsigned int rtn = 0;
    while (pos < len)
    {
        int num = ChineseToValue(chnString.substr(pos, CHN_CHAR_LENGTH));
        if (num >= 0) // 数字还是单位？
        {
            number = num;
            pos += CHN_CHAR_LENGTH;
            if (pos >= len) // 如果是最后一位数字，直接结束
            {
                section += number;
                rtn += section;
                break;
            }
        }
        else
        {
            bool secUnit = false;
            int unit = ChineseToUnit(chnString.substr(pos, CHN_CHAR_LENGTH), secUnit);
            if (secUnit) // 是节权位说明一个节已经结束
            {
                section = (section + number) * unit;
                rtn += section;
                section = 0;
            }
            else
            {
                section += (number * unit);
            }
            number = 0;
            pos += CHN_CHAR_LENGTH;
            if (pos >= len)
            {
                rtn += section;
                break;
            }
        }
    }

    return rtn;
}

int main()
{
    for (int i = 0; i < sizeof(testPair) / sizeof(testPair[0]); i++)
        cout << "{" << testPair[i].num << ", " << testPair[i].chnNumStr << "}, {" << NumberToChinese(testPair[i].num) << ", " << ChineseToNumber(testPair[i].chnNumStr) << "}" << endl;
    return 0;
}
