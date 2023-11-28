#include <cmath>
using namespace std;

#include "Common.h"

static const double PRECISION = 1E-10;
inline bool IsPrecisionZero(double value)
{
    return fabs(value) < PRECISION;
}


