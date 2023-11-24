#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <numeric>
#include <cmath>
#include <cfloat>
using namespace std;

typedef struct
{
    int count;
    int totalWords;
    vector<int> wordsNum;
    double pci;
    vector<double> pfci;
} TRAINING_T;

typedef pair<vector<string>, string> EXAMPLE_T;
typedef map<string, TRAINING_T> TRAINING_RESULT;

// 人工分类好的样本数据
vector<EXAMPLE_T> examples =
    {
        {{"周六", "公司", "庆祝", "聚餐", "时间", "订餐"}, "普通邮件"},
        {{"喜欢", "概率论", "考试", "研究", "及格", "补考", "失败"}, "普通邮件"},
        {{"贝叶斯", "理论", "算法", "公式", "困难"}, "普通邮件"},
        {{"上海", "晴朗", "郊游", "青草", "蓝天", "帐篷", "停车场", "拥堵"}, "普通邮件"},
        {{"代码", "走查", "错误", "反馈", "修改", "入库", "编译"}, "普通邮件"},
        {{"公司", "单元测试", "覆盖率", "时间", "用例", "失败", "成功"}, "普通邮件"},
        {{"优惠", "打折", "促销", "返利", "金融", "理财"}, "垃圾邮件"},
        {{"公司", "发票", "税点", "优惠", "增值税", "返利"}, "垃圾邮件"},
        {{"抽奖", "中奖", "点击", "恭喜", "申请", "资格"}, "垃圾邮件"},
        {{"爆款", "秒杀", "打折", "抵用券", "特惠"}, "垃圾邮件"},
        {{"招聘", "兼职", "日薪", "信用", "合作"}, "垃圾邮件"},
        {{"贷款", "资金", "担保", "抵押", "小额", "利息"}, "垃圾邮件"},
        {{"正规", "发票", "税务局", "验证", "咨询", "打折"}, "垃圾邮件"},
        {{"诚意", "合作", "特价", "机票", "欢迎", "咨询"}, "垃圾邮件"}};

vector<int> MakeWordsVec(const vector<string> &allWords, const vector<string> &words)
{
    vector<int> wordVec(allWords.size(), 0);
    for (auto &word : words)
    {
        auto it = find(allWords.begin(), allWords.end(), word);
        if (it != allWords.end())
            wordVec[it - allWords.begin()] += 1;
    }
    return move(wordVec);
}

vector<string> MakeAllWordsList(const vector<EXAMPLE_T> &examples)
{
    set<string> wordsSet;
    for (auto &e : examples)
        wordsSet.insert(e.first.begin(), e.first.end());

    vector<string> wordsList;
    copy(wordsSet.begin(), wordsSet.end(), back_inserter(wordsList));
    return move(wordsList);
}

TRAINING_T &GetTrainClassificationDate(TRAINING_RESULT &tr, const string &classification, size_t vecLen)
{
    auto it = tr.find(classification);
    if (it == tr.end())
    {
        tr[classification].count = 1;
        tr[classification].totalWords = 2;                    // 初始化为2，避免除0
        tr[classification].wordsNum = vector<int>(vecLen, 1); // 初始化为1
        tr[classification].pfci = vector<double>(vecLen);
    }
    else
    {
        tr[classification].count++;
    }

    return tr[classification];
}

TRAINING_RESULT TrainingExample(const vector<string> &allWords, const vector<EXAMPLE_T> &examples)
{
    TRAINING_RESULT tr;

    for (auto &e : examples)
    {
        TRAINING_T &tt = GetTrainClassificationDate(tr, e.second, allWords.size());
        vector<int> wordNum = MakeWordsVec(allWords, e.first);
        tt.totalWords += accumulate(wordNum.begin(), wordNum.end(), 0);
        transform(wordNum.begin(), wordNum.end(), tt.wordsNum.begin(), tt.wordsNum.begin(), plus<int>());
    }

    for (auto &cr : tr)
    {
        cr.second.pci = double(cr.second.count) / examples.size();
        for (size_t i = 0; i < allWords.size(); i++)
            cr.second.pfci[i] = log(double(cr.second.wordsNum[i]) / cr.second.totalWords);
    }

    return move(tr);
}

string ClassifyResult(const TRAINING_RESULT &tr, const vector<string> &allWords,
                      const vector<string> &wordsList)
{
    double pm = -DBL_MAX;
    string classification;

    vector<int> numVec = MakeWordsVec(allWords, wordsList);
    for (auto &cr : tr)
    {
        double p = 0.0;
        for (size_t i = 0; i < allWords.size(); i++)
            p += numVec[i] * cr.second.pfci[i];
        p += log(cr.second.pci);
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
    vector<string> allWords = MakeAllWordsList(examples);
    TRAINING_RESULT tr = TrainingExample(allWords, examples);

    // vector<string> testWords = { "公司", "讨论", "单元测试", "覆盖率", "错误", "成功" };
    vector<string> testWords = {"公司", "保险", "讨论", "喜欢", "周六", "郊游", "蓝天"};
    string classification = ClassifyResult(tr, allWords, testWords);
    cout << "[";
    copy(testWords.begin(), testWords.end(), ostream_iterator<string>(cout, " "));
    cout << "] 被分类为： " << classification << endl
         << endl;

    vector<string> testWords2 = {"公司", "优惠", "打折", "秒杀", "喜欢", "合作"};
    string classification2 = ClassifyResult(tr, allWords, testWords2);
    cout << "[";
    copy(testWords2.begin(), testWords2.end(), ostream_iterator<string>(cout, " "));
    cout << "] 被分类为： " << classification2 << endl
         << endl;

    return 0;
}
