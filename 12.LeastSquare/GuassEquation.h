#pragma once

#include <vector>
using namespace std;

class GuassEquation
{
public:
    GuassEquation(int M, double *A=NULL, double *b=NULL);
    virtual ~GuassEquation();

    void Init(int M, double *A, double *b);
    double &operator()(int row, int col);
    const double &operator()(int row, int col) const;

    bool Resolve(vector<double> &xValue);

#ifdef _DEBUG
    void DebugDump();
#endif

protected:
    int SelectPivotalElement(int column);
    void SwapRow(int row1, int row2);
    void SimplePivotalRow(int row, int startColumn);
    void RowElimination(int rowS, int rowE, int startColumn);

protected:
    double *m_matrixA;
    int m_DIM;
    double *m_bVal;
};
