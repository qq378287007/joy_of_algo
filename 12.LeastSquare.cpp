#include <cassert>
#include <numeric>
#include <vector>
#include <iostream>
#include <cmath>
#include <bits/stdc++.h>
using namespace std;

static const double PRECISION = 1E-10;
inline static bool IsPrecisionZero(double value) { return fabs(value) < PRECISION; }

class GuassEquation
{
protected:
    int m_DIM;
    double *m_matrixA{nullptr};
    double *m_bVal{nullptr};

protected:
    // M是维度，column取值范围从0到M-1
    int SelectPivotalElement(int column)
    {
        assert(column < m_DIM);

        int row = column;
        double maxU = fabs(m_matrixA[column * m_DIM + column]);

        for (int i = column + 1; i < m_DIM; i++)
        {
            if (fabs(m_matrixA[i * m_DIM + column]) > maxU)
            {
                maxU = fabs(m_matrixA[i * m_DIM + column]);
                row = i;
            }
        }

        return row;
    }

    void SwapRow(int row1, int row2)
    {
        assert(row1 < m_DIM && row2 < m_DIM);

        double temp;
        for (int i = 0; i < m_DIM; i++)
        {
            temp = m_matrixA[row1 * m_DIM + i];
            m_matrixA[row1 * m_DIM + i] = m_matrixA[row2 * m_DIM + i];
            m_matrixA[row2 * m_DIM + i] = temp;
        }
        temp = m_bVal[row1];
        m_bVal[row1] = m_bVal[row2];
        m_bVal[row2] = temp;
    }

    void SimplePivotalRow(int row, int startColumn)
    {
        assert(row < m_DIM && startColumn < m_DIM);

        double simple = m_matrixA[row * m_DIM + startColumn];

        for (int i = startColumn; i < m_DIM; i++)
            m_matrixA[row * m_DIM + i] /= simple;

        m_bVal[row] /= simple;
    }

    void RowElimination(int rowS, int rowE, int startColumn)
    {
        assert(rowS < m_DIM && rowE < m_DIM && startColumn < m_DIM);

        double simple = m_matrixA[rowE * m_DIM + startColumn];

        for (int i = startColumn; i < m_DIM; i++)
            m_matrixA[rowE * m_DIM + i] -= m_matrixA[rowS * m_DIM + i] * simple;
        m_bVal[rowE] -= m_bVal[rowS] * simple;
    }

public:
    GuassEquation(int M, double *A = nullptr, double *b = nullptr)
    {
        m_DIM = M;
        m_matrixA = new double[m_DIM * m_DIM];
        if (m_matrixA)
        {
            m_bVal = new double[m_DIM];
            if (m_bVal)
            {
                if (A)
                    memcpy(m_matrixA, A, sizeof(double) * m_DIM * m_DIM);

                if (b)
                    memcpy(m_bVal, b, sizeof(double) * m_DIM);
            }
            else
            {
                delete[] m_matrixA;
                m_matrixA = nullptr;
            }
        }
    }
    virtual ~GuassEquation()
    {
        if (m_matrixA)
        {
            delete[] m_matrixA;
            m_matrixA = nullptr;
        }
        if (m_bVal)
        {
            delete[] m_bVal;
            m_bVal = nullptr;
        }
    }

    double &operator()(int row, int col)
    {
        assert(row < m_DIM && col < m_DIM);
        // double(*rowPtr)[] = (double(*)[])m_matrixA;
        return *(m_matrixA + row * m_DIM + col);
    }

    const double &operator()(int row, int col) const
    {
        assert(row < m_DIM && col < m_DIM);
        return *(m_matrixA + row * m_DIM + col);
    }

    // 带列主元的高斯消去法解方程组，最后的解在matrixA的对角线上
    bool Resolve(vector<double> &xValue)
    {
        assert(xValue.size() == m_DIM);

        // 消元，得到上三角阵
        for (int i = 0; i < m_DIM - 1; i++)
        {
            // 按列选主元
            int pivotRow = SelectPivotalElement(i);
            if (pivotRow != i) // 如果有必要，交换行
                SwapRow(i, pivotRow);

            if (IsPrecisionZero(m_matrixA[i * m_DIM + i])) // 主元是0? 不存在唯一解
                return false;

            // 对系数归一化处理，使行第一个系数是1.0
            SimplePivotalRow(i, i);

            // 逐行进行消元
            for (int j = i + 1; j < m_DIM; j++)
                RowElimination(i, j, i);
        }
        // 回代求解
        m_matrixA[(m_DIM - 1) * m_DIM + m_DIM - 1] = m_bVal[m_DIM - 1] / m_matrixA[(m_DIM - 1) * m_DIM + m_DIM - 1];
        for (int i = m_DIM - 2; i >= 0; i--)
        {
            double totalCof = 0.0;
            for (int j = i + 1; j < m_DIM; j++)
                totalCof += m_matrixA[i * m_DIM + j] * m_matrixA[j * m_DIM + j];

            m_matrixA[i * m_DIM + i] = (m_bVal[i] - totalCof) / m_matrixA[i * m_DIM + i];
        }

        // 将对角线元素的解逐个存入解向量
        for (int i = 0; i < m_DIM; i++)
            xValue[i] = m_matrixA[i * m_DIM + i];

        return true;
    }

#if 1
    void DebugDump()
    {
        cout.setf(ios::fixed, ios::floatfield); // 不用科学计数法表示浮点数
        cout.precision(6);
        for (int i = 0; i < m_DIM; i++)
        {
            for (int j = 0; j < m_DIM; j++)
                cout << setw(9) << m_matrixA[i * m_DIM + j] << " ";
            cout << "| ";
            cout << setw(8) << m_bVal[i] << endl;
        }
    }
#endif
};

#define ARR_INDEX(row, col, dim) ((row) * (dim) + (col))

// 最小二乘法曲线拟合
bool CalculateCurveParameter(vector<double> *X, vector<double> *Y, long M, long N, vector<double> *A)
{
    // X,Y --  X,Y两轴的坐标
    // M   --  结果变量组数
    // N   --  采样数目
    // A   --  结果参数

    long i, j, k;
    double Z, D1, D2, C, P, G, Q;
    vector<double> B, T, S;
    B.resize(N);
    T.resize(N);
    S.resize(N);

    if (M > N)
        M = N;
    for (i = 0; i < M; i++)
        (*A)[i] = 0;
    Z = 0;
    B[0] = 1;
    D1 = N;
    P = 0;
    C = 0;
    for (i = 0; i < N; i++)
    {
        P = P + (*X)[i] - Z;
        C = C + (*Y)[i];
    }
    C = C / D1;
    P = P / D1;
    (*A)[0] = C * B[0];
    if (M > 1)
    {
        T[1] = 1;
        T[0] = -P;
        D2 = 0;
        C = 0;
        G = 0;
        for (i = 0; i < N; i++)
        {
            Q = (*X)[i] - Z - P;
            D2 = D2 + Q * Q;
            C = (*Y)[i] * Q + C;
            G = ((*X)[i] - Z) * Q * Q + G;
        }
        C = C / D2;
        P = G / D2;
        Q = D2 / D1;
        D1 = D2;
        (*A)[1] = C * T[1];
        (*A)[0] = C * T[0] + (*A)[0];
    }
    for (j = 2; j < M; j++)
    {
        S[j] = T[j - 1];
        S[j - 1] = -P * T[j - 1] + T[j - 2];
        if (j >= 3)
        {
            for (k = j - 2; k >= 1; k--)
                S[k] = -P * T[k] + T[k - 1] - Q * B[k];
        }
        S[0] = -P * T[0] - Q * B[0];
        D2 = 0;
        C = 0;
        G = 0;
        for (i = 0; i < N; i++)
        {
            Q = S[j];
            for (k = j - 1; k >= 0; k--)
                Q = Q * ((*X)[i] - Z) + S[k];
            D2 = D2 + Q * Q;
            C = (*Y)[i] * Q + C;
            G = ((*X)[i] - Z) * Q * Q + G;
        }
        C = C / D2;
        P = G / D2;
        Q = D2 / D1;
        D1 = D2;
        (*A)[j] = C * S[j];
        T[j] = S[j];
        for (k = j - 1; k >= 0; k--)
        {
            (*A)[k] = C * S[k] + (*A)[k];
            B[k] = T[k];
            T[k] = S[k];
        }
    }
    return true;
}

bool LeastSquare(const vector<double> &x_value, const vector<double> &y_value, int M, vector<double> &a_value)
{
    assert(x_value.size() == y_value.size());
    assert(a_value.size() == M);

    double *matrix = new double[M * M];
    double *b = new double[M];

    vector<double> x_m(x_value.size(), 1.0);
    vector<double> y_i(y_value.size(), 0.0);
    for (int i = 0; i < M; i++)
    {
        matrix[ARR_INDEX(0, i, M)] = accumulate(x_m.begin(), x_m.end(), 0.0);
        for (int j = 0; j < static_cast<int>(y_value.size()); j++)
            y_i[j] = x_m[j] * y_value[j];

        b[i] = accumulate(y_i.begin(), y_i.end(), 0.0);
        for (int k = 0; k < static_cast<int>(x_m.size()); k++)
            x_m[k] *= x_value[k];
    }
    for (int row = 1; row < M; row++)
    {
        for (int i = 0; i < M - 1; i++)
            matrix[ARR_INDEX(row, i, M)] = matrix[ARR_INDEX(row - 1, i + 1, M)];

        matrix[ARR_INDEX(row, M - 1, M)] = accumulate(x_m.begin(), x_m.end(), 0.0);
        for (int k = 0; k < static_cast<int>(x_m.size()); k++)
            x_m[k] *= x_value[k];
    }

    GuassEquation equation(M, matrix, b);
    delete[] matrix;
    delete[] b;

    return equation.Resolve(a_value);
}

int main()
{
#if 0
    //double matrixA[3][3] = {{2.5, 2.3, -5.1}, {5.3, 9.6, 1.5}, {8.1, 1.7, -4.3}};
    //double b[3] = {3.7, 3.8, 5.5};
     double matrixA[3][3] = { {1, 2, 3}, {0, 1, 2}, {2, 4, 1} };
     double b[3] = {14, 8, 13 };

    vector<double> R(3);
    GuassEquation equation(3, (double *)matrixA, b);
    equation.Resolve(R);
    equation.DebugDump();

#endif

#if 1
    const int N = 12;
    double x_value[N] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
    double y_value[N] = {8.41, 9.94, 11.58, 13.02, 14.33, 15.92, 17.54, 19.22, 20.49, 22.01, 23.53, 24.47};
    // const int N = 6;
    // double x_value[N] = { 0.5, 1.0, 1.5, 2.0, 2.5, 3.0 };
    // double y_value[N] = { 1.75, 2.45, 3.81, 4.80, 7.00, 8.60 };

    vector<double> X, Y;
    vector<double> A(2);

    X.assign(x_value, x_value + N);
    Y.assign(y_value, y_value + N);

    CalculateCurveParameter(&X, &Y, 2, N, &A);

    vector<double> B(2);
    LeastSquare(X, Y, 2, B);
#endif

    cout << "Over!\n";
    return 0;
}
