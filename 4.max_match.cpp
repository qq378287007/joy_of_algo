// max_match.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <locale>
#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>

typedef struct
{
    int max_len;
    std::map<int, std::vector<std::wstring>> words;
}WORD_DICT;

void InitDict(WORD_DICT& dict)
{
    //���Ѿ��������
    std::vector<std::wstring> w1 = { L"��", L"��", L"��", L"��", L"��", L"��", L"��" };
    std::vector<std::wstring> w2 = { L"�Ͼ�", L"����", L"�г�", L"��˼", L"���", L"����", L"����" };
    std::vector<std::wstring> w3 = { L"�Ͼ���", L"������", L"�����" };
    std::vector<std::wstring> w4 = { L"����㳡", L"ϲ������", L"�߿���·" };
    std::vector<std::wstring> w5 = { L"��������", L"��������̨" };

    dict.max_len = 5;
    dict.words[1] = w1;
    dict.words[2] = w2;
    dict.words[3] = w3;
    dict.words[4] = w4;
    dict.words[5] = w5;
}

//�������Ϻ����˴����

bool LookupDict(const WORD_DICT& dict, std::wstring& word)
{
    int length = word.length(); //���ݴʳ�ѡ���Ӧ�Ĵʼ�
    const std::vector<std::wstring>& wds = dict.words.at(length);
    
    return std::binary_search(wds.begin(), wds.end(), word);
}

bool MatchWord(const WORD_DICT& dict, std::wstring& s2)
{
    while (!s2.empty())
    {
        if (LookupDict(dict, s2)) //��ʵ�
        {
            return true;
        }
        else
        {
            s2.pop_back(); //��s2��ɾ�����һ����
        }
    }

    //s2������Ҳûƥ�䵽�ʣ������Ǵʵ�������
    return false;
}

bool MaxMatching(const std::wstring& sentence, const WORD_DICT& dict, std::vector<std::wstring>& words)
{
    std::wstring s1 = sentence;

    while (!s1.empty())
    {
        int s2_len = (s1.length() > dict.max_len) ? dict.max_len : s1.length();
        std::wstring s2 = s1.substr(0, s2_len);
        if (!MatchWord(dict, s2))
        {
            return false;
        }

        words.push_back(s2);
        s1 = s1.substr(s2.length());
    }

    return true;
}

bool ReverseMatchWord(const WORD_DICT& dict, std::wstring& s2)
{
    while (!s2.empty())
    {
        if (LookupDict(dict, s2)) //��ʵ�
        {
            return true;
        }
        else
        {
            s2 = s2.substr(1); //��s2��ɾ����һ����
        }
    }

    //s2������Ҳûƥ�䵽�ʣ������Ǵʵ�������
    return false;
}

bool ReverseMaxMatching(const std::wstring& sentence, const WORD_DICT& dict, std::vector<std::wstring>& words)
{
    std::wstring s1 = sentence;

    while (!s1.empty())
    {
        int s2_pos = (s1.length() > dict.max_len) ? s1.length() - dict.max_len : 0;
        std::wstring s2 = s1.substr(s2_pos);
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
    //std::locale::global(std::locale("zh_CN"));
    std::wcout.imbue(std::locale("chs"));

    WORD_DICT dict;

    InitDict(dict);

    std::wstring sentence = L"������������˼";
    std::vector<std::wstring> words;
    if (MaxMatching(sentence, dict, words))
    {
        for (auto& str : words)
        {
            std::wcout << str << L"/";
        }

        std::wcout << std::endl;
    }

    words.clear();
    if (ReverseMaxMatching(sentence, dict, words))
    {
        for (auto& str : words)
        {
            std::wcout << str << L"/";
        }

        std::wcout << std::endl;
    }

    std::wstring sentence2 = L"�Ͼ��г�������";
    words.clear();
    if (MaxMatching(sentence2, dict, words))
    {
        for (auto& str : words)
        {
            std::wcout << str << L"/";
        }

        std::wcout << std::endl;
    }

    words.clear();
    if (ReverseMaxMatching(sentence2, dict, words))
    {
        for (auto& str : words)
        {
            std::wcout << str << L"/";
        }

        std::wcout << std::endl;
    }

    return 0;
}
