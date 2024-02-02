#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <functional>
using namespace std;

#include <experimental/filesystem>
namespace std_fs = experimental::filesystem;

const int BMP_WIDTH = 32;
const int BMP_HEIGHT = 32;
const int NUM_COUNT = 10; // 0-9
const int K = 9;

const string FILE_PATH = ".\\";

struct SampleVec
{
    int cat;
    char vec[BMP_WIDTH * BMP_HEIGHT];
};

struct CatResult
{
    double distance;
    int cat;
};

pair<bool, int> GetCategoryFromFileName(const string fileName)
{
    size_t pos = fileName.find('_');
    if (pos == string::npos)
        return {false, 0};

    int cat = fileName[pos - 1] - '0';
    return {true, cat};
}

bool AppendToVec(SampleVec &vec, int row, string &sline)
{
    if (sline.length() != BMP_WIDTH)
        return false;

    char *pvs = vec.vec + row * BMP_WIDTH;
    for (size_t i = 0; i < sline.length(); i++)
        *pvs++ = sline[i] - '0';
    return true;
}

bool LoadFileToVec(const string &fileName, SampleVec &vec)
{
    ifstream file(fileName, ios::in);
    if (!file)
        return false;

    int row = 0;
    string sline;
    while (getline(file, sline))
    {
        if (!AppendToVec(vec, row, sline))
            break;
        row++;
    }

    return (row == BMP_HEIGHT);
}

double ManhattanDustance(const SampleVec &vec1, const SampleVec &vec2)
{
    double total = 0.0;
    for (int i = 0; i < BMP_WIDTH * BMP_HEIGHT; i++)
        total += abs(vec1.vec[i] - vec2.vec[i]);
    return total;
}

bool LoadDataSet(const string filePath, vector<SampleVec> &dataSet)
{
    for (auto &p : std_fs::directory_iterator(filePath))
    {
        string fileName = p.path().generic_string();
        pair<bool, int> catrtn = GetCategoryFromFileName(fileName);
        if (!catrtn.first)
            return false;

        SampleVec vec = {catrtn.second};
        if (!LoadFileToVec(fileName, vec))
            return false;

        dataSet.emplace_back(vec); // better than push_back()
    }

    return true;
}

int GetMaxCountCategory(const vector<int> &count)
{
    int mj = 0;
    for (int j = 1; j < NUM_COUNT; j++)
        if (count[j] > count[mj])
            mj = j;
    return mj;
}

int Classify(const vector<SampleVec> &dataTrain, const SampleVec &vec)
{
    int idx = 0;
    vector<CatResult> cr(dataTrain.size());
    for (auto &vt : dataTrain)
    {
        cr[idx].cat = vt.cat;
        cr[idx++].distance = ManhattanDustance(vt, vec);
    }

    auto lessCrPred = [](const CatResult &cr1, const CatResult &cr2) -> bool
    { return (cr1.distance < cr2.distance); };
    sort(cr.begin(), cr.end(), lessCrPred);

    vector<int> count(NUM_COUNT, 0);
    for (int i = 0; i < K; i++)
        count[cr[i].cat]++;

    return GetMaxCountCategory(count);
}

int main()
{
    const string trainFile = FILE_PATH + "traindata";
    vector<SampleVec> dataTrain;
    if (!LoadDataSet(trainFile, dataTrain))
    {
        cout << "Fail to load trainning data file!" << endl;
        return 1;
    }

    const string testFile = FILE_PATH + "testdata";
    vector<SampleVec> dataTest;
    if (!LoadDataSet(testFile, dataTest))
    {
        cout << "Fail to load test data file!" << endl;
        return 1;
    }

    vector<pair<int, int>> result(NUM_COUNT);
    for (const SampleVec &v : dataTest)
    {
        result[v.cat].first++;
        int cat = Classify(dataTrain, v);
        if (cat == v.cat)
            result[v.cat].second++;
    }

    for (size_t i = 0; i < result.size(); i++)
    {
        double rato = 100.0 * result[i].second / result[i].first;
        cout << "category : " << i << "  success : " << setprecision(4) << rato << endl;
    }

    return 0;
}

// g++ 24.knn-digital.cpp -o 24.knn-digital -std=c++17 -L-lstdc++fs
// g++ -std=c++11 24.knn-digital.cpp -o 24.knn-digital -lstdc++fs