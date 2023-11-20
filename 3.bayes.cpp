// bayes.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <numeric>

typedef struct
{
    int count;
    int totalWords;
    std::vector<int> wordsNum;
    double pci;
    std::vector<double> pfci;
}TRAINING_T;

typedef std::pair<std::vector<std::string>, std::string>   EXAMPLE_T;
typedef std::map<std::string, TRAINING_T>  TRAINING_RESULT;

//�˹�����õ���������
std::vector<EXAMPLE_T>  examples =
{
    { { "����", "��˾", "��ף", "�۲�", "ʱ��", "����" }, "��ͨ�ʼ�" },
    { { "ϲ��", "������", "����", "�о�", "����", "����", "ʧ��" }, "��ͨ�ʼ�" },
    { { "��Ҷ˹", "����", "�㷨", "��ʽ", "����" }, "��ͨ�ʼ�" },
    { { "�Ϻ�", "����", "����", "���", "����", "����", "ͣ����", "ӵ��" }, "��ͨ�ʼ�" },
    { { "����", "�߲�", "����", "����", "�޸�", "���", "����" }, "��ͨ�ʼ�" },
    { { "��˾", "��Ԫ����", "������", "ʱ��", "����", "ʧ��", "�ɹ�" }, "��ͨ�ʼ�" },
    { { "�Ż�", "����", "����", "����", "����", "���" }, "�����ʼ�" },
    { { "��˾", "��Ʊ", "˰��", "�Ż�", "��ֵ˰", "����" }, "�����ʼ�" },
    { { "�齱", "�н�", "���", "��ϲ", "����", "�ʸ�" }, "�����ʼ�" },
    { { "����", "��ɱ", "����", "����ȯ", "�ػ�" }, "�����ʼ�" },
    { { "��Ƹ", "��ְ", "��н", "����", "����" }, "�����ʼ�" },
    { { "����", "�ʽ�", "����", "��Ѻ", "С��", "��Ϣ" }, "�����ʼ�" },
    { { "����", "��Ʊ", "˰���", "��֤", "��ѯ", "����" }, "�����ʼ�" },
    { { "����", "����", "�ؼ�", "��Ʊ", "��ӭ", "��ѯ" }, "�����ʼ�" }
};



std::vector<int> MakeWordsVec(const std::vector<std::string>& allWords, const std::vector<std::string>& words)
{
    std::vector<int> wordVec(allWords.size(), 0);

    for (auto& word : words)
    {
        auto it = std::find(allWords.begin(), allWords.end(), word);
        if (it != allWords.end())
        {
            wordVec[it - allWords.begin()] += 1;
        }
    }

    return std::move(wordVec);
}

std::vector<std::string> MakeAllWordsList(const std::vector<EXAMPLE_T>& examples)
{
    std::set<std::string> wordsSet;
    for (auto& e : examples)
    {
        wordsSet.insert(e.first.begin(), e.first.end());
    }

    std::vector<std::string> wordsList;
    std::copy(wordsSet.begin(), wordsSet.end(), std::back_inserter(wordsList));
    return std::move(wordsList);
}

TRAINING_T& GetTrainClassificationDate(TRAINING_RESULT& tr, const std::string& classification, std::size_t vecLen)
{
    auto it = tr.find(classification);
    if (it == tr.end())
    {
        tr[classification].count = 1;
        tr[classification].totalWords = 2; //��ʼ��Ϊ2�������0
        tr[classification].wordsNum = std::vector<int>(vecLen, 1); //��ʼ��Ϊ1
        tr[classification].pfci = std::vector<double>(vecLen);
    }
    else
    {
        tr[classification].count++;
    }

    return tr[classification];
}

TRAINING_RESULT TrainingExample(const std::vector<std::string>& allWords, const std::vector<EXAMPLE_T>& examples)
{
    TRAINING_RESULT tr;

    for (auto& e : examples)
    {
        TRAINING_T& tt = GetTrainClassificationDate(tr, e.second, allWords.size());
        std::vector<int> wordNum = MakeWordsVec(allWords, e.first);
        tt.totalWords += std::accumulate(wordNum.begin(), wordNum.end(), 0);
        std::transform(wordNum.begin(), wordNum.end(), tt.wordsNum.begin(), 
                       tt.wordsNum.begin(), std::plus<int>());
    }

    for (auto& cr : tr)
    {
        cr.second.pci = double(cr.second.count) / examples.size();
        for (std::size_t i = 0; i < allWords.size(); i++)
        {
            cr.second.pfci[i] = std::log(double(cr.second.wordsNum[i]) / cr.second.totalWords);
        }
    }
    
    return std::move(tr);
}

std::string ClassifyResult(const TRAINING_RESULT& tr, const std::vector<std::string>& allWords, 
                           const std::vector<std::string>& wordsList)
{
    double pm = -DBL_MAX;
    std::string classification;

    std::vector<int> numVec = MakeWordsVec(allWords, wordsList);
    for (auto& cr : tr)
    {
        double p = 0.0;
        for (std::size_t i = 0; i < allWords.size(); i++)
        {
            p += numVec[i] * cr.second.pfci[i];
        }
        p += std::log(cr.second.pci);
        if (p > pm)
        {
            pm = p;
            classification = cr.first;
        }
    }

    return classification;
}

int main()
{
    std::vector<std::string> allWords = MakeAllWordsList(examples);

    TRAINING_RESULT tr = TrainingExample(allWords, examples);


    //std::vector<std::string> testWords = { "��˾", "����", "��Ԫ����", "������", "����", "�ɹ�" };
    std::vector<std::string> testWords = { "��˾", "����", "����", "ϲ��", "����", "����", "����" };
    std::string classification = ClassifyResult(tr, allWords, testWords);
    std::cout << "[";
    std::copy(testWords.begin(), testWords.end(), std::ostream_iterator<std::string>(std::cout, " "));
    std::cout << "] ������Ϊ�� " << classification << std::endl << std::endl;

    std::vector<std::string> testWords2 = { "��˾", "�Ż�", "����", "��ɱ", "ϲ��", "����" };
    std::string classification2 = ClassifyResult(tr, allWords, testWords2);
    std::cout << "[";
    std::copy(testWords2.begin(), testWords2.end(), std::ostream_iterator<std::string>(std::cout, " "));
    std::cout << "] ������Ϊ�� " << classification2 << std::endl << std::endl;

    return 0;
}

