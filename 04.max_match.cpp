#include <locale>
#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>
using namespace std;

struct WORD_DICT
{
    int max_len;
    map<int, vector<wstring>> words;
};

void InitDict(WORD_DICT &dict)
{
    // 词已经排序过了
    vector<wstring> w1 = {L"大", L"市", L"我", L"是", L"有", L"桥", L"江"};
    vector<wstring> w2 = {L"南京", L"大桥", L"市长", L"意思", L"编程", L"计算", L"长江"};
    vector<wstring> w3 = {L"南京市", L"江大桥", L"计算机"};
    vector<wstring> w4 = {L"人民广场", L"喜马拉雅", L"高科西路"};
    vector<wstring> w5 = {L"人民大会堂", L"沪上新闻台"};

    dict.max_len = 5;
    dict.words[1] = w1;
    dict.words[2] = w2;
    dict.words[3] = w3;
    dict.words[4] = w4;
    dict.words[5] = w5;
}

// 张三是上海市人大代表
bool LookupDict(const WORD_DICT &dict, wstring &word)
{
    int length = word.length(); // 根据词长选择对应的词集
    const vector<wstring> &wds = dict.words.at(length);

    return binary_search(wds.begin(), wds.end(), word);
}

bool MatchWord(const WORD_DICT &dict, wstring &s2)
{
    while (!s2.empty())
    {
        if (LookupDict(dict, s2)) // 查词典
            return true;
        else
            s2.pop_back(); // 从s2中删除最后一个字
    }

    // s2都空了也没匹配到词？可能是词典有问题
    return false;
}

bool MaxMatching(const wstring &sentence, const WORD_DICT &dict, vector<wstring> &words)
{
    wstring s2;
    for (wstring s1 = sentence; !s1.empty(); s1 = s1.substr(s2.length()))
    {
        int s2_len = (s1.length() > dict.max_len) ? dict.max_len : s1.length();
        s2 = s1.substr(0, s2_len);
        if (!MatchWord(dict, s2))
            return false;

        words.push_back(s2);
    }

    return true;
}

bool ReverseMatchWord(const WORD_DICT &dict, wstring &s2)
{
    while (!s2.empty())
    {
        if (LookupDict(dict, s2)) // 查词典
            return true;
        else
            s2 = s2.substr(1); // 从s2中删除第一个字
    }

    // s2都空了也没匹配到词？可能是词典有问题
    return false;
}

bool ReverseMaxMatching(const wstring &sentence, const WORD_DICT &dict, vector<wstring> &words)
{
    wstring s1 = sentence;
    while (!s1.empty())
    {
        int s2_pos = (s1.length() > dict.max_len) ? s1.length() - dict.max_len : 0;
        wstring s2 = s1.substr(s2_pos);
        if (!ReverseMatchWord(dict, s2))
            return false;

        words.push_back(s2);
        s1 = s1.substr(0, s1.length() - s2.length());
    }

    return true;
}

int main()
{
    // locale::global(locale("zh_CN"));
    wcout.imbue(locale("chs"));

    WORD_DICT dict;
    InitDict(dict);

    wstring sentence = L"计算机编程有意思";
    vector<wstring> words;
    if (MaxMatching(sentence, dict, words))
    {
        for (const wstring &str : words)
            wcout << str << L"/";
        wcout << endl;
    }

    words.clear();
    if (ReverseMaxMatching(sentence, dict, words))
    {
        for (const wstring &str : words)
            wcout << str << L"/";
        wcout << endl;
    }

    wstring sentence2 = L"南京市长江大桥";
    words.clear();
    if (MaxMatching(sentence2, dict, words))
    {
        for (const wstring &str : words)
            wcout << str << L"/";
        wcout << endl;
    }

    words.clear();
    if (ReverseMaxMatching(sentence2, dict, words))
    {
        for (const wstring &str : words)
            wcout << str << L"/";
        wcout << endl;
    }

    return 0;
}
