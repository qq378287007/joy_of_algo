#include <locale>
#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>
using namespace std;

typedef struct
{
    int max_len;
    map<int, vector<wstring>> words;
} WORD_DICT;

void InitDict(WORD_DICT &dict)
{
    // ���Ѿ��������
    vector<wstring> w1 = {L"��", L"��", L"��", L"��", L"��", L"��", L"��"};
    vector<wstring> w2 = {L"�Ͼ�", L"����", L"�г�", L"��˼", L"���", L"����", L"����"};
    vector<wstring> w3 = {L"�Ͼ���", L"������", L"�����"};
    vector<wstring> w4 = {L"����㳡", L"ϲ������", L"�߿���·"};
    vector<wstring> w5 = {L"��������", L"��������̨"};

    dict.max_len = 5;
    dict.words[1] = w1;
    dict.words[2] = w2;
    dict.words[3] = w3;
    dict.words[4] = w4;
    dict.words[5] = w5;
}

// �������Ϻ����˴����

bool LookupDict(const WORD_DICT &dict, wstring &word)
{
    int length = word.length(); // ���ݴʳ�ѡ���Ӧ�Ĵʼ�
    const vector<wstring> &wds = dict.words.at(length);

    return binary_search(wds.begin(), wds.end(), word);
}

bool MatchWord(const WORD_DICT &dict, wstring &s2)
{
    while (!s2.empty())
    {
        if (LookupDict(dict, s2)) // ��ʵ�
        {
            return true;
        }
        else
        {
            s2.pop_back(); // ��s2��ɾ�����һ����
        }
    }

    // s2������Ҳûƥ�䵽�ʣ������Ǵʵ�������
    return false;
}

bool MaxMatching(const wstring &sentence, const WORD_DICT &dict, vector<wstring> &words)
{
    wstring s1 = sentence;

    while (!s1.empty())
    {
        int s2_len = (s1.length() > dict.max_len) ? dict.max_len : s1.length();
        wstring s2 = s1.substr(0, s2_len);
        if (!MatchWord(dict, s2))
        {
            return false;
        }

        words.push_back(s2);
        s1 = s1.substr(s2.length());
    }

    return true;
}

bool ReverseMatchWord(const WORD_DICT &dict, wstring &s2)
{
    while (!s2.empty())
    {
        if (LookupDict(dict, s2)) // ��ʵ�
        {
            return true;
        }
        else
        {
            s2 = s2.substr(1); // ��s2��ɾ����һ����
        }
    }

    // s2������Ҳûƥ�䵽�ʣ������Ǵʵ�������
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
        {
            return false;
        }

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

    wstring sentence = L"������������˼";
    vector<wstring> words;
    if (MaxMatching(sentence, dict, words))
    {
        for (auto &str : words)
        {
            wcout << str << L"/";
        }

        wcout << endl;
    }

    words.clear();
    if (ReverseMaxMatching(sentence, dict, words))
    {
        for (auto &str : words)
        {
            wcout << str << L"/";
        }

        wcout << endl;
    }

    wstring sentence2 = L"�Ͼ��г�������";
    words.clear();
    if (MaxMatching(sentence2, dict, words))
    {
        for (auto &str : words)
        {
            wcout << str << L"/";
        }

        wcout << endl;
    }

    words.clear();
    if (ReverseMaxMatching(sentence2, dict, words))
    {
        for (auto &str : words)
        {
            wcout << str << L"/";
        }

        wcout << endl;
    }

    return 0;
}
