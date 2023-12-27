#include <cmath>
#include <iostream>
#include <cassert>
#include <vector>
#include <cstring>
using namespace std;

#define ARR_INDEX(row, col, dim) ((row) * (dim) + (col))

static const double PRECISION = 1.0E-10;
inline static bool IsPrecisionZero(double value) { return fabs(value) < PRECISION; }

inline static double FuncY(double dbX) { return 3.0 / (1.0 + dbX * dbX); }

class ThomasEquation
{
protected:
	double *m_matrixA{nullptr};
	double *m_bVal{nullptr};
	int m_DIM;

public:
	ThomasEquation(int M, double *A = nullptr, double *b = nullptr)
	{
		assert(m_matrixA == NULL && m_bVal == NULL);

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
				m_matrixA = NULL;
			}
		}
	}
	virtual ~ThomasEquation()
	{
		if (m_matrixA)
		{
			delete[] m_matrixA;
			m_matrixA = NULL;
		}
		if (m_bVal)
		{
			delete[] m_bVal;
			m_bVal = NULL;
		}
	}

	double &operator()(int row, int col)
	{
		assert(row < m_DIM && col < m_DIM);
		return *(m_matrixA + row * m_DIM + col);
	}

	const double &operator()(int row, int col) const
	{
		assert(row < m_DIM && col < m_DIM);
		return *(m_matrixA + row * m_DIM + col);
	}

	// 追赶法求对三角矩阵方程组的解
	bool Resolve(vector<double> &xValue)
	{
		assert(xValue.size() == m_DIM);

		vector<double> L(m_DIM);
		vector<double> M(m_DIM);
		vector<double> U(m_DIM);
		vector<double> Y(m_DIM);

		// 消元，追的过程
		L[0] = m_matrixA[ARR_INDEX(0, 0, m_DIM)];
		U[0] = m_matrixA[ARR_INDEX(0, 1, m_DIM)] / L[0];
		Y[0] = m_bVal[0] / L[0];
		for (int i = 1; i < m_DIM; i++)
		{
			if (IsPrecisionZero(m_matrixA[ARR_INDEX(i, i, m_DIM)]))
				return false;

			M[i] = m_matrixA[ARR_INDEX(i, i - 1, m_DIM)];
			L[i] = m_matrixA[ARR_INDEX(i, i, m_DIM)] - M[i] * U[i - 1];
			Y[i] = (m_bVal[i] - M[i] * Y[i - 1]) / L[i];
			if ((i + 1) < m_DIM)
				U[i] = m_matrixA[ARR_INDEX(i, i + 1, m_DIM)] / L[i];
		}
		// 回代求解，赶的过程
		xValue[m_DIM - 1] = Y[m_DIM - 1];
		for (int i = m_DIM - 2; i >= 0; i--)
			xValue[i] = Y[i] - U[i] * xValue[i + 1];

		return true;
	}

#if 1
	void DebugDump()
	{
		for (int i = 0; i < m_DIM; i++)
		{
			for (int j = 0; j < m_DIM; j++)
				cout << m_matrixA[i * m_DIM + j] << " ";
			cout << "| " << m_bVal[i] << endl;
		}
	}
#endif
};
#if 0
    double matrixA[3][3] = { {2.5, 2.3, -5.1}, {5.3, 9.6, 1.5}, {8.1, 1.7, -4.3} };
    double b[3] = {3.7, 3.8, 5.5 };
//    double matrixA[3][3] = { {1, 2, 3}, {0, 1, 2}, {2, 4, 1} };
//    double b[3] = {14, 8, 13 };

    CDoubleArray R(3);
    GuassEquation equation(3, (double *)matrixA, b);

    equation.Resolve(R);
#endif

class SplineFitting
{
protected:
	vector<double> m_valXi;
	vector<double> m_valYi;
	vector<double> m_valMi;
	int m_valN{0};
	bool m_bCalcCompleted{false};

public:
	virtual ~SplineFitting() {}

	void CalcSpline(double *Xi, double *Yi, int n, int boundType, double b1, double b2)
	{
		assert((boundType == 1) || (boundType == 2));

		double *matrixA = new double[n * n];
		if (matrixA == NULL)
		{
			return;
		}
		double *d = new double[n];
		if (d == NULL)
		{
			delete[] matrixA;
			return;
		}

		m_valN = n;
		m_valXi.assign(Xi, Xi + m_valN);
		m_valYi.assign(Yi, Yi + m_valN);
		m_valMi.resize(m_valN);
		memset(matrixA, 0, sizeof(double) * n * n);

		matrixA[ARR_INDEX(0, 0, m_valN)] = 2.0;
		matrixA[ARR_INDEX(m_valN - 1, m_valN - 1, m_valN)] = 2.0;
		if (boundType == 1) /*第一类边界条件*/
		{
			matrixA[ARR_INDEX(0, 1, m_valN)] = 1.0;					  // v0
			matrixA[ARR_INDEX(m_valN - 1, m_valN - 2, m_valN)] = 1.0; // un
			double h0 = Xi[1] - Xi[0];
			d[0] = 6 * ((Yi[1] - Yi[0]) / h0 - b1) / h0; // d0
			double hn_1 = Xi[m_valN - 1] - Xi[m_valN - 2];
			d[m_valN - 1] = 6 * (b2 - (Yi[m_valN - 1] - Yi[m_valN - 2]) / hn_1) / hn_1; // dn
		}
		else /*第二类边界条件*/
		{
			matrixA[ARR_INDEX(0, 1, m_valN)] = 0.0;					  // v0
			matrixA[ARR_INDEX(m_valN - 1, m_valN - 2, m_valN)] = 0.0; // un
			d[0] = 2 * b1;											  // d0
			d[m_valN - 1] = 2 * b2;									  // dn
		}
		/*计算ui,vi,di，i = 2,3,...,n-1*/
		for (int i = 1; i < (m_valN - 1); i++)
		{
			double hi_1 = Xi[i] - Xi[i - 1];
			double hi = Xi[i + 1] - Xi[i];
			matrixA[ARR_INDEX(i, i - 1, m_valN)] = hi_1 / (hi_1 + hi); // ui
			matrixA[ARR_INDEX(i, i, m_valN)] = 2.0;
			matrixA[ARR_INDEX(i, i + 1, m_valN)] = 1 - matrixA[ARR_INDEX(i, i - 1, m_valN)];  // vi = 1 - ui
			d[i] = 6 * ((Yi[i + 1] - Yi[i]) / hi - (Yi[i] - Yi[i - 1]) / hi_1) / (hi_1 + hi); // di
		}

		ThomasEquation equation(m_valN, matrixA, d);
		equation.Resolve(m_valMi);
		m_bCalcCompleted = true;

		delete[] matrixA;
		delete[] d;
	}

	double GetValue(double x)
	{
		if (!m_bCalcCompleted)
		{
			return 0.0;
		}
		if ((x < m_valXi[0]) || (x > m_valXi[m_valN - 1]))
		{
			return 0.0;
		}
		int i = 0;
		for (i = 0; i < (m_valN - 1); i++)
		{
			if ((x >= m_valXi[i]) && (x < m_valXi[i + 1]))
				break;
		}
		double hi = m_valXi[i + 1] - m_valXi[i];
		double xi_1 = m_valXi[i + 1] - x;
		double xi = x - m_valXi[i];

		double y = xi_1 * xi_1 * xi_1 * m_valMi[i] / (6 * hi);
		y += (xi * xi * xi * m_valMi[i + 1] / (6 * hi));

#if 0
    y += ((m_valYi[i] - m_valMi[i] * hi * hi / 6) * xi_1 / hi);
    y += ((m_valYi[i + 1] - m_valMi[i + 1] * hi * hi / 6) * xi / hi);
#endif

		double Ai = (m_valYi[i + 1] - m_valYi[i]) / hi - (m_valMi[i + 1] - m_valMi[i]) * hi / 6.0;
		y += Ai * x;
		double Bi = m_valYi[i + 1] - m_valMi[i + 1] * hi * hi / 6.0 - Ai * m_valXi[i + 1];
		y += Bi;
		return y;
	}
};

// LogicDevice
int wndRgLeft = 0;
int wndRgRight = 100;
int wndRgTop = 0;
int wndRgBottom = 100;
int centerX = 50;
int centerY = 50;

double luX = 1.0;
double luY = 1.0;

double x_min = 0.0;
double x_max = 100.0;
double y_min = 0.0;
double y_max = 100.0;
double x_center = 50.0;
double y_center = 50.0;

void SetXLogicRange(double min, double max)
{
	x_min = min;
	x_max = max;
	luX = (wndRgRight - wndRgLeft) / fabs(x_max - x_min);
	if ((x_min >= 0) && (x_max >= 0))
	{
		centerX = wndRgLeft;
		x_center = x_min;
	}
	else if ((x_min <= 0) && (x_max <= 0))
	{
		centerX = wndRgRight;
		x_center = x_max;
	}
	else
	{
		double scale = fabs(x_min) / fabs(x_max - x_min);
		centerX = wndRgLeft + int((wndRgRight - wndRgLeft) * scale);
		x_center = 0.0;
	}
}

void SetYLogicRange(double min, double max)
{
	y_min = min;
	y_max = max;
	luY = (wndRgBottom - wndRgTop) / fabs(y_max - y_min);
	if ((y_min >= 0) && (y_max >= 0))
	{
		centerY = wndRgBottom;
		y_center = y_min;
	}
	else if ((y_min <= 0) && (y_max <= 0))
	{
		centerY = wndRgTop;
		y_center = y_max;
	}
	else
	{
		double scale = fabs(y_min) / fabs(y_max - y_min);
		centerY = wndRgBottom - int((wndRgBottom - wndRgTop) * scale);
		y_center = 0.0;
	}
}

void SetDevicePointRange(int left, int top, int right, int bottom)
{
	wndRgLeft = left;
	wndRgRight = right;
	wndRgTop = top;
	wndRgBottom = bottom;

	SetXLogicRange(x_min, x_max);
	SetYLogicRange(y_min, y_max);
}

int DeviceXFromLogicX(double x)
{
	if (x < x_min || x > x_max)
		return 0;
	int devX = centerX + int((x - x_center) * luX + 0.5);
	return devX;
}

int DeviceYFromLogicY(double y)
{
	if (y < y_min || y > y_max)
		return 0;
	int devY = centerY - int((y - y_center) * luY + 0.5);
	return devY;
}

void GetDeviceCenter(int &cx, int &cy)
{
	cx = centerX;
	cy = centerY;
}

// CDisplayWnd
class CDisplayWnd
{
private:
	void DrawAxis()
	{
		int centerX, centerY;
		GetDeviceCenter(centerX, centerY);
		//::MoveToEx(hDC, 0, centerY, (LPPOINT)NULL);
		//::LineTo(hDC, m_rcClient.Width(), centerY);
		//::MoveToEx(hDC, centerX, 0, (LPPOINT)NULL);
		//::LineTo(hDC, centerX, m_rcClient.Height());
	}

	void DrawSample(int x, int y, const string &text)
	{
		//::MoveToEx(hDC, x, y, NULL);
		//::LineTo(hDC, x + 30, y);
		//::TextOut(hDC, x + 40, y - 6, text, lstrlen(text));
	}

	void DrawSpline(const vector<double> &xArray, const vector<double> &yArray)
	{
		int x0 = DeviceXFromLogicX(xArray[0]);
		int y0 = DeviceYFromLogicY(yArray[0]);
		//::MoveToEx(hDC, x0, y0, NULL);
		for (int i = 1; i < (int)xArray.size(); i++)
		{
			int x1 = DeviceXFromLogicX(xArray[i]);
			int y1 = DeviceYFromLogicY(yArray[i]);
			//::LineTo(hDC, x1, y1);
			x0 = x1;
			y0 = y1;
		}
	}

	void DrawCurve()
	{
		// DrawBackground(pDC, RGB(255, 255, 255));
		// DrawAxis(pDC, RGB(0, 0, 0));

		DrawSample(10, 20, "原函数曲线");
		DrawSample(10, 50, "三次样条函数曲线");

		if (m_xPpoint.size() > 0 && m_yPpoint.size() > 0)
			DrawSpline(m_xPpoint, m_yPpoint);

		if (m_xSource.size() > 0 && m_ySource.size() > 0)
			DrawSpline(m_xSource, m_ySource);

		if (m_xSpline.size() > 0 && m_ySpline.size() > 0)
			DrawSpline(m_xSpline, m_ySpline);
	}

protected:
	vector<double> m_xPpoint;
	vector<double> m_yPpoint;
	vector<double> m_xSource;
	vector<double> m_ySource;
	vector<double> m_xSpline;
	vector<double> m_ySpline;

public:
	virtual ~CDisplayWnd() {}

public:
	void SetInterpolationPoint(double *x, double *y, int count)
	{
		m_xPpoint.assign(x, x + count);
		m_yPpoint.assign(y, y + count);
	}

	void SetSourceCurve(double *x, double *y, int count)
	{
		m_xSource.assign(x, x + count);
		m_ySource.assign(y, y + count);
	}
	void SetSourceSplineCurve(double *x, double *y, int count)
	{
		m_xSpline.assign(x, x + count);
		m_ySpline.assign(y, y + count);
	}
	void SetPointLogicRange(double xmin, double xmax, double ymin, double ymax)
	{
		SetXLogicRange(xmin, xmax);
		SetYLogicRange(ymin, ymax);
	}
};

int main()
{
	double sourceX[17] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 7.5, 8.0};
	double sourceY[17];
	for (int i = 0; i < 17; i++)
		sourceY[i] = FuncY(sourceX[i]);

	SplineFitting fitting;
	// fitting.CalcSpline(sourceX, sourceY, 9, 1, 0.0, -0.004608);
	fitting.CalcSpline(sourceX, sourceY, 17, 1, 0.0, -0.01136);
	double xi[1024], yi[1024], si[1024];
	int ind = 0;
	for (double i = sourceX[0]; i < sourceX[16]; i += 0.01)
	{
		xi[ind] = i;
		yi[ind] = FuncY(i);
		si[ind] = fitting.GetValue(i);
		ind++;
	}

	CDisplayWnd m_pDispWnd;
	// m_pDispWnd.SetInterpolationPoint(sourceX, sourceY, 9);
	m_pDispWnd.SetSourceCurve(xi, yi, ind);
	m_pDispWnd.SetSourceSplineCurve(xi, si, ind);
	m_pDispWnd.SetPointLogicRange(-1.0, 9.0, -1.0, 5.0);

	cout << "Over!\n";
	return 0;
}
