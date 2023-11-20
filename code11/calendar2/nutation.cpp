#include "stdafx.h"
#include <math.h>
#include "nutation.h"
#include "support.h"


//天体章动系数类型变量
typedef struct
{
    double   D;
    double   M;
    double   Mp;
    double   F;
    double   omega;
    double   sine1;
    double   sine2;
    double   cosine1;
    double   cosine2;
}NUTATION_COEFFICIENT;

/*
天体黄经章动和黄赤交角章动的周期项，来源《天体计算》第21章，表21-A
单位是0".0001.
*/
const NUTATION_COEFFICIENT nutation[63] =
{
    { 0, 0, 0, 0, 1, -171996,  -174.2,  92025,     8.9    },
    {-2, 0, 0, 2, 2,  -13187,    -1.6,   5736,    -3.1    },
    { 0, 0, 0, 2, 2,   -2274,    -0.2,    977,    -0.5    },
    { 0, 0, 0, 0, 2,    2062,     0.2,   -895,     0.5    },
    { 0, 1, 0, 0, 0,    1426,    -3.4,     54,    -0.1    },
    { 0, 0, 1, 0, 0,     712,     0.1,     -7,       0    },
    {-2, 1, 0, 2, 2,    -517,     1.2,    224,    -0.6    },
    { 0, 0, 0, 2, 1,    -386,    -0.4,    200,       0    },
    { 0, 0, 1, 2, 2,    -301,       0,    129,    -0.1    },
    {-2,-1, 0, 2, 2,     217,    -0.5,    -95,     0.3    },
    {-2, 0, 1, 0, 0,    -158,       0,      0,       0    },
    {-2, 0, 0, 2, 1,     129,     0.1,    -70,       0    },
    { 0, 0,-1, 2, 2,     123,       0,    -53,       0    },
    { 2, 0, 0, 0, 0,      63,       0,      0,       0    },
    { 0, 0, 1, 0, 1,      63,     0.1,    -33,       0    },
    { 2, 0,-1, 2, 2,     -59,       0,     26,       0    },
    { 0, 0,-1, 0, 1,     -58,    -0.1,     32,       0    },
    { 0, 0, 1, 2, 1,     -51,       0,     27,       0    },
    {-2, 0, 2, 0, 0,      48,       0,      0,       0    },
    { 0, 0,-2, 2, 1,      46,       0,    -24,       0    },
    { 2, 0, 0, 2, 2,     -38,       0,     16,       0    },
    { 0, 0, 2, 2, 2,     -31,       0,     13,       0    },
    { 0, 0, 2, 0, 0,      29,       0,      0,       0    },
    {-2, 0, 1, 2, 2,      29,       0,    -12,       0    },
    { 0, 0, 0, 2, 0,      26,       0,      0,       0    },
    {-2, 0, 0, 2, 0,     -22,       0,      0,       0    },
    { 0, 0,-1, 2, 1,      21,       0,    -10,       0    },
    { 0, 2, 0, 0, 0,      17,    -0.1,      0,       0    },
    { 2, 0,-1, 0, 1,      16,       0,     -8,       0    },
    {-2, 2, 0, 2, 2,     -16,     0.1,      7,       0    },
    { 0, 1, 0, 0, 1,     -15,       0,      9,       0    },
    {-2, 0, 1, 0, 1,     -13,       0,      7,       0    },
    { 0,-1, 0, 0, 1,     -12,       0,      6,       0    },
    { 0, 0, 2,-2, 0,      11,       0,      0,       0    },
    { 2, 0,-1, 2, 1,     -10,       0,      5,       0    },
    { 2, 0, 1, 2, 2,      -8,       0,      3,       0    },
    { 0, 1, 0, 2, 2,       7,       0,     -3,       0    },
    {-2, 1, 1, 0, 0,      -7,       0,      0,       0    },
    { 0,-1, 0, 2, 2,      -7,       0,      3,       0    },
    { 2, 0, 0, 2, 1,      -7,       0,      3,       0    },
    { 2, 0, 1, 0, 0,       6,       0,      0,       0    },
    {-2, 0, 2, 2, 2,       6,       0,     -3,       0    },
    {-2, 0, 1, 2, 1,       6,       0,     -3,       0    },
    { 2, 0,-2, 0, 1,      -6,       0,      3,       0    },
    { 2, 0, 0, 0, 1,      -6,       0,      3,       0    },
    { 0,-1, 1, 0, 0,       5,       0,      0,       0    },
    {-2,-1, 0, 2, 1,      -5,       0,      3,       0    },
    {-2, 0, 0, 0, 1,      -5,       0,      3,       0    },
    { 0, 0, 2, 2, 1,      -5,       0,      3,       0    },
    {-2, 0, 2, 0, 1,       4,       0,      0,       0    },
    {-2, 1, 0, 2, 1,       4,       0,      0,       0    },
    { 0, 0, 1,-2, 0,       4,       0,      0,       0    },
    {-1, 0, 1, 0, 0,      -4,       0,      0,       0    },
    {-2, 1, 0, 0, 0,      -4,       0,      0,       0    },
    { 1, 0, 0, 0, 0,      -4,       0,      0,       0    },
    { 0, 0, 1, 2, 0,       3,       0,      0,       0    },
    { 0, 0,-2, 2, 2,      -3,       0,      0,       0    },
    {-1,-1, 1, 0, 0,      -3,       0,      0,       0    },
    { 0, 1, 1, 0, 0,      -3,       0,      0,       0    },
    { 0,-1, 1, 2, 2,      -3,       0,      0,       0    },
    { 2,-1,-1, 2, 2,      -3,       0,      0,       0    },
    { 0, 0, 3, 2, 2,      -3,       0,      0,       0    },
    { 2,-1, 0, 2, 2,      -3,       0,      0,       0    }
};

void GetEarthNutationParameter(double dt, double *D, double *M, double *Mp, double *F, double *Omega)
{
    double T = dt * 10; /*T是从J2000起算的儒略世纪数*/
    double T2 = T * T;
    double T3 = T2 * T;

    /*平距角（如月对地心的角距离）*/
    *D = 297.85036 + 445267.111480 * T - 0.0019142 * T2 + T3 / 189474.0;

    /*太阳（地球）平近点角*/
    *M = 357.52772 + 35999.050340 * T - 0.0001603 * T2 - T3 / 300000.0;

    /*月亮平近点角*/
    *Mp = 134.96298 + 477198.867398 * T + 0.0086972 * T2 + T3 / 56250.0;

    /*月亮纬度参数*/
    *F = 93.27191 + 483202.017538 * T - 0.0036825 * T2 + T3 / 327270.0;

    /*黄道与月亮平轨道升交点黄经*/
    *Omega = 125.04452 - 1934.136261 * T + 0.0020708 * T2 + T3 / 450000.0;
}

/*计算某时刻的黄经章动干扰量，dt是儒略千年数，返回值单位是度*/
double CalcEarthLongitudeNutation(double dt)
{
    double T = dt * 10;
    double D,M,Mp,F,Omega;

	GetEarthNutationParameter(dt, &D, &M, &Mp, &F, &Omega);

    double resulte = 0.0 ;
    for(int i = 0; i < sizeof(nutation) / sizeof(nutation[0]); i++)
    {
        double sita = nutation[i].D * D + nutation[i].M * M + nutation[i].Mp * Mp + nutation[i].F * F + nutation[i].omega * Omega;
		sita = DegreeToRadian(sita);

        resulte += (nutation[i].sine1 + nutation[i].sine2 * T ) * sin(sita);
    }

    /*先乘以章动表的系数 0.0001，然后换算成度的单位*/
    return resulte * 0.0001 / 3600.0; 
}

/*计算某时刻的黄赤交角章动干扰量，dt是儒略千年数，返回值单位是度*/
double CalcEarthObliquityNutation(double dt)
{
    double T = dt * 10; /*T是从J2000起算的儒略世纪数*/
    double D,M,Mp,F,Omega;

	GetEarthNutationParameter(dt, &D, &M, &Mp, &F, &Omega);

    double resulte = 0.0 ;
    for(int i = 0; i < sizeof(nutation) / sizeof(nutation[0]); i++)
    {
        double sita = nutation[i].D * D + nutation[i].M * M + nutation[i].Mp * Mp + nutation[i].F * F + nutation[i].omega * Omega;
		sita = DegreeToRadian(sita);

        resulte += (nutation[i].cosine1 + nutation[i].cosine2 * T ) * cos(sita);
    }

    /*先乘以章动表的系数 0.001，然后换算成度的单位*/
    return resulte * 0.0001 / 3600.0; 
}

