#include <cstdio>
#include <cstring>
#include <vector>
#include <cassert>
#include <cmath>
#include <ctime>
#include <string>
using namespace std;

/*
#include "support.h"
#include "calendar_func.h"

#include "calendar_const.h"
#include "DayInfo.h"
#include "MonthInfo.h"
#include "ChnMonthInfo.h"
#include "ChineseCalendar.h"
#include "WzCalendarCtrl.h"
*/

// calendar_const
const int MONTHES_FOR_YEAR = 12;
const int DAYS_FOR_WEEK = 7;

const int DAYS_OF_LEAP_YEAR = 366;
const int DAYS_OF_NORMAL_YEAR = 365;

const int SOLAR_TERMS_COUNT = 24;

const int HEAVENLY_STEMS = 10;
const int EARTHLY_BRANCHES = 12;
const int CHINESE_SHENGXIAO = 12;
const int MAX_GREGORIAN_MONTH_DAYS = 31;

const int MAX_CHINESE_MONTH_DAYS = 30;
const int CHINESE_L_MONTH_DAYS = 30;
const int CHINESE_S_MONTH_DAYS = 29;
const int MAX_CHINESE_MONTH_FOR_YEAR = 13;

const double JD2000 = 2451545.0;

const double PI = 3.1415926535897932384626433832795; // 圆周率

const double ARC_SEC_PER_RADIAN = 180.0 * 3600.0 / PI; // 每弧度的角秒数

const double RADIAN_PER_ANGLE = PI / 180.0; // 一度代表的弧度

const double SUN_EL_V = 360.0 / 365.2422;

const double MOON_EL_V = 360.0 / 27.32; // 360.0 / 29.53058886;

typedef int SOLAR_TERMS;

// 节气定义
const int VERNAL_EQUINOX = 0;	  // 春分
const int CLEAR_AND_BRIGHT = 1;	  // 清明
const int GRAIN_RAIN = 2;		  // 谷雨
const int SUMMER_BEGINS = 3;	  // 立夏
const int GRAIN_BUDS = 4;		  // 小满
const int GRAIN_IN_EAR = 5;		  // 芒种
const int SUMMER_SOLSTICE = 6;	  // 夏至
const int SLIGHT_HEAT = 7;		  // 小暑
const int GREAT_HEAT = 8;		  // 大暑
const int AUTUMN_BEGINS = 9;	  // 立秋
const int STOPPING_THE_HEAT = 10; // 处暑
const int WHITE_DEWS = 11;		  // 白露
const int AUTUMN_EQUINOX = 12;	  // 秋分
const int COLD_DEWS = 13;		  // 寒露
const int HOAR_FROST_FALLS = 14;  // 霜降
const int WINTER_BEGINS = 15;	  // 立冬
const int LIGHT_SNOW = 16;		  // 小雪
const int HEAVY_SNOW = 17;		  // 大雪
const int WINTER_SOLSTICE = 18;	  // 冬至
const int SLIGHT_COLD = 19;		  // 小寒
const int GREAT_COLD = 20;		  // 大寒
const int SPRING_BEGINS = 21;	  // 立春
const int THE_RAINS = 22;		  // 雨水
const int INSECTS_AWAKEN = 23;	  // 惊蛰

#define COUNT_ITEM(x) (sizeof(x) / sizeof(x[0]))

// vsop87_data
struct VSOP87_COEFFICIENT
{
	double A;
	double B;
	double C;
};

// 数据来源是：《天文计算》 附录II
//  计算太阳黄经周期项
extern const VSOP87_COEFFICIENT Earth_L0[64];
extern const VSOP87_COEFFICIENT Earth_L1[34];
extern const VSOP87_COEFFICIENT Earth_L2[20];
extern const VSOP87_COEFFICIENT Earth_L3[7];
extern const VSOP87_COEFFICIENT Earth_L4[3];
extern const VSOP87_COEFFICIENT Earth_L5[1];

// 计算太阳黄纬周期项
extern const VSOP87_COEFFICIENT Earth_B0[5];
extern const VSOP87_COEFFICIENT Earth_B1[2];
extern const VSOP87_COEFFICIENT Earth_B2[4];
extern const VSOP87_COEFFICIENT Earth_B3[4];
extern const VSOP87_COEFFICIENT Earth_B4[1];

// 计算日地经距离周期项
extern const VSOP87_COEFFICIENT Earth_R0[40];
extern const VSOP87_COEFFICIENT Earth_R1[10];
extern const VSOP87_COEFFICIENT Earth_R2[6];
extern const VSOP87_COEFFICIENT Earth_R3[2];
extern const VSOP87_COEFFICIENT Earth_R4[1];

// 计算太阳黄经周期项
const VSOP87_COEFFICIENT Earth_L0[64] = {
	{175347046.0, 0.0000000, 000000.0000000},
	{3341656.0, 4.6692568, 6283.0758500},
	{34894.0, 4.6261000, 12566.1517000},
	{3497.0, 2.7441000, 5753.3849000},
	{3418.0, 2.8289000, 3.5231000},
	{3136.0, 3.6277000, 77713.7715000},
	{2676.0, 4.4181000, 7860.4194000},
	{2343.0, 6.1352000, 3930.2097000},
	{1324.0, 0.7425000, 11506.7698000},
	{1273.0, 2.0371000, 529.6910000},
	{1799.0, 1.1096000, 1577.3435000},
	{990.0, 5.2330000, 5884.9270000},
	{902.0, 2.0450000, 26.2980000},
	{857.0, 3.5080000, 398.1490000},
	{780.0, 1.1790000, 5223.6940000},
	{753.0, 2.5330000, 5507.5530000},
	{505.0, 4.5830000, 18849.2280000},
	{492.0, 4.2050000, 775.5230000},
	{357.0, 2.9200000, 000000.0670000},
	{317.0, 5.8490000, 11790.6290000},
	{284.0, 1.8990000, 796.2980000},
	{271.0, 0.3150000, 10977.0790000},
	{243.0, 0.3450000, 5486.7780000},
	{206.0, 4.8060000, 2544.3140000},
	{205.0, 1.8690000, 5573.1430000},
	{202.0, 2.4580000, 6069.7770000},
	{156.0, 0.8330000, 213.2990000},
	{132.0, 3.4110000, 2942.4630000},
	{126.0, 1.0830000, 20.7750000},
	{119.0, 0.6450000, 000000.9800000},
	{107.0, 0.6360000, 4694.0030000},
	{102.0, 0.9760000, 15720.8390000},
	{102.0, 4.2670000, 7.1140000},
	{99.0, 6.2100000, 2146.1700000},
	{98.0, 0.6800000, 155.4200000},
	{86.0, 5.9800000, 161000.6900000},
	{85.0, 1.3000000, 6275.9600000},
	{85.0, 3.6700000, 71430.7000000},
	{80.0, 1.8100000, 17260.1500000},
	{79.0, 3.0400000, 12036.4600000},
	{75.0, 1.7600000, 5088.6300000},
	{74.0, 3.5000000, 3154.6900000},
	{74.0, 4.6800000, 801.8200000},
	{70.0, 0.8300000, 9437.7600000},
	{62.0, 3.9800000, 8827.3900000},
	{61.0, 1.8200000, 7084.9000000},
	{57.0, 2.7800000, 6286.6000000},
	{56.0, 4.3900000, 14143.5000000},
	{56.0, 3.4700000, 6279.5500000},
	{52.0, 0.1900000, 12139.5500000},
	{52.0, 1.3300000, 1748.0200000},
	{51.0, 0.2800000, 5856.4800000},
	{49.0, 0.4900000, 1194.4500000},
	{41.0, 5.3700000, 8429.2400000},
	{41.0, 2.4000000, 19651.0500000},
	{39.0, 6.1700000, 10447.3900000},
	{37.0, 6.0400000, 10213.2900000},
	{37.0, 2.5700000, 1059.3800000},
	{36.0, 1.7100000, 2352.8700000},
	{36.0, 1.7800000, 6812.7700000},
	{33.0, 0.5900000, 17789.8500000},
	{30.0, 0.4400000, 83996.8500000},
	{30.0, 2.7400000, 1349.8700000},
	{25.0, 3.1600000, 4690.4800000}};
const VSOP87_COEFFICIENT Earth_L1[34] = {
	{628331966747.0, 0.000000, 00000.0000000},
	{206059.0, 2.678235, 6283.0758500},
	{4303.0, 2.635100, 12566.1517000},
	{425.0, 1.590000, 3.5230000},
	{119.0, 5.796000, 26.2980000},
	{109.0, 2.966000, 1577.3440000},
	{93.0, 2.590000, 18849.2300000},
	{72.0, 1.140000, 529.6900000},
	{68.0, 1.870000, 398.1500000},
	{67.0, 4.410000, 5507.5500000},
	{59.0, 2.890000, 5223.6900000},
	{56.0, 2.170000, 155.4200000},
	{45.0, 0.400000, 796.3000000},
	{36.0, 0.470000, 775.5200000},
	{29.0, 2.650000, 7.1100000},
	{21.0, 5.340000, 00000.9800000},
	{19.0, 1.850000, 5486.7800000},
	{19.0, 4.970000, 213.3000000},
	{17.0, 2.990000, 6275.9600000},
	{16.0, 0.030000, 2544.3100000},
	{16.0, 1.430000, 2146.1700000},
	{15.0, 1.210000, 10977.0800000},
	{12.0, 2.830000, 1748.0200000},
	{12.0, 3.260000, 5088.6300000},
	{12.0, 5.270000, 1194.4500000},
	{12.0, 2.080000, 4694.0000000},
	{11.0, 0.770000, 553.5700000},
	{10.0, 1.300000, 6286.6000000},
	{10.0, 4.240000, 1349.8700000},
	{9.0, 2.700000, 242.7300000},
	{9.0, 5.640000, 951.7200000},
	{8.0, 5.300000, 2352.8700000},
	{6.0, 2.650000, 9437.7600000},
	{6.0, 4.670000, 4690.4800000}};
const VSOP87_COEFFICIENT Earth_L2[20] = {
	{52919.0, 0.0000, 00000.0000},
	{8720.0, 1.0721, 6283.0758},
	{309.0, 0.8670, 12566.1520},
	{27.0, 0.0500, 3.5200},
	{16.0, 5.1900, 26.3000},
	{16.0, 3.6800, 155.4200},
	{10.0, 0.7600, 18849.2300},
	{9.0, 2.0600, 77713.7700},
	{7.0, 0.8300, 775.5200},
	{5.0, 4.6600, 1577.3400},
	{4.0, 1.0300, 7.1100},
	{4.0, 3.4400, 5573.1400},
	{3.0, 5.1400, 796.3000},
	{3.0, 6.0500, 5507.5500},
	{3.0, 1.1900, 242.7300},
	{3.0, 6.1200, 529.6900},
	{3.0, 0.3100, 398.1500},
	{3.0, 2.2800, 553.5700},
	{2.0, 4.3800, 5223.6900},
	{2.0, 3.7500, 00000.9800}};
const VSOP87_COEFFICIENT Earth_L3[7] = {
	{289.0, 5.844, 6283.076},
	{35.0, 0.000, 00000.000},
	{17.0, 5.490, 12566.150},
	{3.0, 5.200, 155.420},
	{1.0, 4.720, 3.520},
	{1.0, 5.300, 18849.230},
	{1.0, 5.970, 242.730}};
const VSOP87_COEFFICIENT Earth_L4[3] = {
	{114.0, 3.142, 00000.00},
	{8.0, 4.130, 6283.08},
	{1.0, 3.840, 12566.15}};
const VSOP87_COEFFICIENT Earth_L5[1] = {{1.0, 3.14, 0.0}};

// 计算太阳黄纬周期项
const VSOP87_COEFFICIENT Earth_B0[5] = {
	{280.0, 3.199, 84334.662},
	{102.0, 5.422, 5507.553},
	{80.0, 3.880, 5223.690},
	{44.0, 3.700, 2352.870},
	{32.0, 4.000, 1577.340}};
const VSOP87_COEFFICIENT Earth_B1[2] = {
	{9.0, 3.90, 5507.55},
	{6.0, 1.73, 5223.69}};
const VSOP87_COEFFICIENT Earth_B2[4] = {
	{22378.0, 3.38509, 10213.28555},
	{282.0, 0.00000, 00000.00000},
	{173.0, 5.25600, 20426.57100},
	{27.0, 3.87000, 30639.86000}};
const VSOP87_COEFFICIENT Earth_B3[4] = {
	{647.0, 4.992, 10213.286},
	{20.0, 3.140, 00000.000},
	{6.0, 0.770, 20426.570},
	{3.0, 5.440, 30639.860}};

const VSOP87_COEFFICIENT Earth_B4[1] = {{14.0, 0.32, 10213.29}};

// 计算日地经距离周期项
const VSOP87_COEFFICIENT Earth_R0[40] = {
	{100013989, 0, 0},
	{1670700, 3.0984635, 6283.0758500},
	{13956, 3.05525, 12566.15170},
	{3084, 5.1985, 77713.7715},
	{1628, 1.1739, 5753.3849},
	{1576, 2.8469, 7860.4194},
	{925, 5.453, 11506.770},
	{542, 4.564, 3930.210},
	{472, 3.661, 5884.927},
	{346, 0.964, 5507.553},
	{329, 5.900, 5223.694},
	{307, 0.299, 5573.143},
	{243, 4.273, 11790.629},
	{212, 5.847, 1577.344},
	{186, 5.022, 10977.079},
	{175, 3.012, 18849.228},
	{110, 5.055, 5486.778},
	{98, 0.89, 6069.78},
	{86, 5.69, 15720.84},
	{86, 1.27, 161000.69},
	{65, 0.27, 17260.15},
	{63, 0.92, 529.69},
	{57, 2.01, 83996.85},
	{56, 5.24, 71430.70},
	{49, 3.25, 2544.31},
	{47, 2.58, 775.52},
	{45, 5.54, 9437.76},
	{43, 6.01, 6275.96},
	{39, 5.36, 4694.00},
	{38, 2.39, 8827.39},
	{37, 0.83, 19651.05},
	{37, 4.90, 12139.55},
	{36, 1.67, 12036.46},
	{35, 1.84, 2942.46},
	{33, 0.24, 7084.90},
	{32, 0.18, 5088.63},
	{32, 1.78, 398.15},
	{28, 1.21, 6286.60},
	{28, 1.90, 6279.55},
	{26, 4.59, 10447.39}};
const VSOP87_COEFFICIENT Earth_R1[10] = {
	{103019, 1.107490, 6283.075850},
	{1721, 1.0644, 12566.1517},
	{702, 3.142, 0},
	{32, 1.02, 18849.23},
	{31, 2.84, 5507.55},
	{25, 1.32, 5223.69},
	{18, 1.42, 1577.34},
	{10, 5.91, 10977.08},
	{9, 1.42, 6275.96},
	{9, 0.27, 5486.78}};
const VSOP87_COEFFICIENT Earth_R2[6] = {
	{4359, 5.7846, 6283.0758},
	{124, 5.579, 12566.152},
	{12, 3.14, 0},
	{9, 3.63, 77713.77},
	{6, 1.87, 5573.14},
	{3, 5.47, 18849.23}};
const VSOP87_COEFFICIENT Earth_R3[2] = {
	{145, 4.273, 6283.076},
	{7, 3.92, 12566.15}};
const VSOP87_COEFFICIENT Earth_R4[1] = {{4, 2.56, 6283.08}};

// td_utc
// 本文件的内容来自寿星万年历，如有商业应用，请咨询刘建伟先生
struct TD_UTC_DELTA
{
	int year;
	double d1, d2, d3, d4;
};

// TD - UT1 计算表
const TD_UTC_DELTA deltaTbl[] = {
	{-4000, 108371.7, -13036.80, 392.000, 0.0000},
	{-500, 17201.0, -627.82, 16.170, -0.3413},
	{-150, 12200.6, -346.41, 5.403, -0.1593},
	{150, 9113.8, -328.13, -1.647, 0.0377},
	{500, 5707.5, -391.41, 0.915, 0.3145},
	{900, 2203.4, -283.45, 13.034, -0.1778},
	{1300, 490.1, -57.35, 2.085, -0.0072},
	{1600, 120.0, -9.81, -1.532, 0.1403},
	{1700, 10.2, -0.91, 0.510, -0.0370},
	{1800, 13.4, -0.72, 0.202, -0.0193},
	{1830, 7.8, -1.81, 0.416, -0.0247},
	{1860, 8.3, -0.13, -0.406, 0.0292},
	{1880, -5.4, 0.32, -0.183, 0.0173},
	{1900, -2.3, 2.06, 0.169, -0.0135},
	{1920, 21.2, 1.69, -0.304, 0.0167},
	{1940, 24.2, 1.22, -0.064, 0.0031},
	{1960, 33.2, 0.51, 0.231, -0.0109},
	{1980, 51.0, 1.29, -0.026, 0.0032},
	{2000, 63.87, 0.1, 0.0, 0.0},
	{2005, 0.0, 0.0, 0.0, 0.0}};

// 二次曲线外推
static double deltatExt(double y, int jsd)
{
	double dy = (y - 1820.0) / 100.0;
	return -20.0 + jsd * dy * dy;
}

static double TdUtcDeltatT(double y)
{
	// 计算世界时与原子时之差,传入年
	if (y >= 2005)
	{
		// sd是2005年之后几年（一值到y1年）的速度估计。
		// sjd是y1年之后的加速度估计。瑞士星历表jsd=31,NASA网站jsd=32,skmap的jsd=29
		int y1 = 2014;
		double sd = 0.4;
		int jsd = 31;
		if (y <= y1)
			return 64.7 + (y - 2005) * sd;							// 直线外推
		double v = deltatExt(y, jsd);								// 二次曲线外推
		double dv = deltatExt(y1, jsd) - (64.7 + (y1 - 2005) * sd); // y1年的二次外推与直线外推的差
		if (y < y1 + 100)
			v -= dv * (y1 + 100 - y) / 100;
		return v;
	}
	else
	{
		int i;
		for (i = 0; i < sizeof(deltaTbl) / sizeof(deltaTbl[0]); i++)
		{
			if (y < deltaTbl[i + 1].year)
				break;
		}
		double t1 = double(y - deltaTbl[i].year) / double(deltaTbl[i + 1].year - deltaTbl[i].year) * 10.0;
		double t2 = t1 * t1;
		double t3 = t2 * t1;

		return deltaTbl[i].d1 + deltaTbl[i].d2 * t1 + deltaTbl[i].d3 * t2 + deltaTbl[i].d4 * t3;
	}
}

double TdUtcDeltatT2(double jd2k)
{
	// 传入儒略日(J2000起算),计算TD-UT(单位:日)
	return TdUtcDeltatT(jd2k / 365.2425 + 2000) / 86400.0;
}

// Names
extern char *nameOfMonth[MONTHES_FOR_YEAR];
extern char *nameOfWeek[DAYS_FOR_WEEK];

extern char *nameOfStems[HEAVENLY_STEMS];
extern char *nameOfBranches[EARTHLY_BRANCHES];
extern char *nameOfShengXiao[CHINESE_SHENGXIAO];
extern char *nameOfChnDay[MAX_CHINESE_MONTH_DAYS];
extern char *nameOfChnMonth[MONTHES_FOR_YEAR];
extern char *nameOfJieQi[SOLAR_TERMS_COUNT];

char *nameOfMonth[MONTHES_FOR_YEAR] = {
	"January", "February", "March", "April", "May",
	"June", "July", "August", "September", "October",
	"November", "December"};
char *nameOfWeek[DAYS_FOR_WEEK] = {"星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};

char *nameOfStems[HEAVENLY_STEMS] = {"甲", "乙", "丙", "丁", "戊", "己", "庚", "辛", "壬", "癸"};
char *nameOfBranches[EARTHLY_BRANCHES] = {"子", "丑", "寅", "卯", "辰", "巳", "午", "未", "申", "酉", "戌", "亥"};
char *nameOfShengXiao[CHINESE_SHENGXIAO] = {"鼠", "牛", "虎", "兔", "龙", "蛇", "马", "羊", "猴", "鸡", "狗", "猪"};
char *nameOfChnDay[MAX_CHINESE_MONTH_DAYS] = {
	"初一", "初二", "初三", "初四", "初五", "初六", "初七", "初八", "初九", "初十",
	"十一", "十二", "十三", "十四", "十五", "十六", "十七", "十八", "十九", "二十",
	"廿一", "廿二", "廿三", "廿四", "廿五", "廿六", "廿七", "廿八", "廿九", "三十"};
char *nameOfChnMonth[MONTHES_FOR_YEAR] = {"正", "二", "三", "四", "五", "六", "七", "八", "九", "十", "十一", "腊"};
char *nameOfJieQi[SOLAR_TERMS_COUNT] = {
	"春分", "清明", "谷雨", "立夏", "小满", "芒种", "夏至", "小暑",
	"大暑", "立秋", "处暑", "白露", "秋分", "寒露", "霜降", "立冬",
	"小雪", "大雪", "冬至", "小寒", "大寒", "立春", "雨水", "惊蛰"};

// elp2000_data
struct MOON_ECLIPTIC_LONGITUDE_COEFF
{
	double D;
	double M;
	double Mp;
	double F;
	double eiA;
	double erA;
};
struct MOON_ECLIPTIC_LATITUDE_COEFF
{
	double D;
	double M;
	double Mp;
	double F;
	double eiA;
};
extern const MOON_ECLIPTIC_LONGITUDE_COEFF Moon_longitude[60];
extern const MOON_ECLIPTIC_LATITUDE_COEFF moon_Latitude[60];

/*
	月球黄经周期项(ΣI)及距离(Σr).
	黄经单位:0.000001度,距离单位:0.001千米.
--------------------------------------------------
  角度的组合系数  ΣI的各项振幅A  Σr的各项振幅A
  D  M  M' F        (正弦振幅)       (余弦振幅)
--------------------------------------------------
*/
const MOON_ECLIPTIC_LONGITUDE_COEFF Moon_longitude[60] = {
	{0, 0, 1, 0, 6288744, -20905355},
	{2, 0, -1, 0, 1274027, -3699111},
	{2, 0, 0, 0, 658314, -2955968},
	{0, 0, 2, 0, 213618, -569925},
	{0, 1, 0, 0, -185116, 48888},
	{0, 0, 0, 2, -114332, -3149},
	{2, 0, -2, 0, 58793, 246158},
	{2, -1, -1, 0, 57066, -152138},
	{2, 0, 1, 0, 53322, -170733},
	{2, -1, 0, 0, 45758, -204586},
	{0, 1, -1, 0, -40923, -129620},
	{1, 0, 0, 0, -34720, 108743},
	{0, 1, 1, 0, -30383, 104755},
	{2, 0, 0, -2, 15327, 10321},
	{0, 0, 1, 2, -12528, 0},
	{0, 0, 1, -2, 10980, 79661},
	{4, 0, -1, 0, 10675, -34782},
	{0, 0, 3, 0, 10034, -23210},
	{4, 0, -2, 0, 8548, -21636},
	{2, 1, -1, 0, -7888, 24208},
	{2, 1, 0, 0, -6766, 30824},
	{1, 0, -1, 0, -5163, -8379},
	{1, 1, 0, 0, 4987, -16675},
	{2, -1, 1, 0, 4036, -12831},
	{2, 0, 2, 0, 3994, -10445},
	{4, 0, 0, 0, 3861, -11650},
	{2, 0, -3, 0, 3665, 14403},
	{0, 1, -2, 0, -2689, -7003},
	{2, 0, -1, 2, -2602, 0},
	{2, -1, -2, 0, 2390, 10056},
	{1, 0, 1, 0, -2348, 6322},
	{2, -2, 0, 0, 2236, -9884},
	{0, 1, 2, 0, -2120, 5751},
	{0, 2, 0, 0, -2069, 0},
	{2, -2, -1, 0, 2048, -4950},
	{2, 0, 1, -2, -1773, 4130},
	{2, 0, 0, 2, -1595, 0},
	{4, -1, -1, 0, 1215, -3958},
	{0, 0, 2, 2, -1110, 0},
	{3, 0, -1, 0, -892, 3258},
	{2, 1, 1, 0, -810, 2616},
	{4, -1, -2, 0, 759, -1897},
	{0, 2, -1, 0, -713, -2117},
	{2, 2, -1, 0, -700, 2354},
	{2, 1, -2, 0, 691, 0},
	{2, -1, 0, -2, 596, 0},
	{4, 0, 1, 0, 549, -1423},
	{0, 0, 4, 0, 537, -1117},
	{4, -1, 0, 0, 520, -1571},
	{1, 0, -2, 0, -487, -1739},
	{2, 1, 0, -2, -399, 0},
	{0, 0, 2, -2, -381, -4421},
	{1, 1, 1, 0, 351, 0},
	{3, 0, -2, 0, -340, 0},
	{4, 0, -3, 0, 330, 0},
	{2, -1, 2, 0, 327, 0},
	{0, 2, 1, 0, -323, 1165},
	{1, 1, -1, 0, 299, 0},
	{2, 0, 3, 0, 294, 0},
	{2, 0, -1, -2, 0, 8752}};

/*
  月球黄纬周期项(ΣI).单位:0.000001度.
-------------------------------------
  角度的组合系数 ΣI的各项振幅A
  D  M  M' F       (正弦振幅)
-------------------------------------
*/
const MOON_ECLIPTIC_LATITUDE_COEFF moon_Latitude[60] = {
	{0, 0, 0, 1, 5128122},
	{0, 0, 1, 1, 280602},
	{0, 0, 1, -1, 277693},
	{2, 0, 0, -1, 173237},
	{2, 0, -1, 1, 55413},
	{2, 0, -1, -1, 46271},
	{2, 0, 0, 1, 32573},
	{0, 0, 2, 1, 17198},
	{2, 0, 1, -1, 9266},
	{0, 0, 2, -1, 8822},
	{2, -1, 0, -1, 8216},
	{2, 0, -2, -1, 4324},
	{2, 0, 1, 1, 4200},
	{2, 1, 0, -1, -3359},
	{2, -1, -1, 1, 2463},
	{2, -1, 0, 1, 2211},
	{2, -1, -1, -1, 2065},
	{0, 1, -1, -1, -1870},
	{4, 0, -1, -1, 1828},
	{0, 1, 0, 1, -1794},
	{0, 0, 0, 3, -1749},
	{0, 1, -1, 1, -1565},
	{1, 0, 0, 1, -1491},
	{0, 1, 1, 1, -1475},
	{0, 1, 1, -1, -1410},
	{0, 1, 0, -1, -1344},
	{1, 0, 0, -1, -1335},
	{0, 0, 3, 1, 1107},
	{4, 0, 0, -1, 1021},
	{4, 0, -1, 1, 833},
	{0, 0, 1, -3, 777},
	{4, 0, -2, 1, 671},
	{2, 0, 0, -3, 607},
	{2, 0, 2, -1, 596},
	{2, -1, 1, -1, 491},
	{2, 0, -2, 1, -451},
	{0, 0, 3, -1, 439},
	{2, 0, 2, 1, 422},
	{2, 0, -3, -1, 421},
	{2, 1, -1, 1, -366},
	{2, 1, 0, 1, -351},
	{4, 0, 0, 1, 331},
	{2, -1, 1, 1, 315},
	{2, -2, 0, -1, 302},
	{0, 0, 1, 3, -283},
	{2, 1, 1, -1, -229},
	{1, 1, 0, -1, 223},
	{1, 1, 0, 1, 223},
	{0, 1, -2, -1, -220},
	{2, 1, -1, -1, -220},
	{1, 0, 1, 1, -185},
	{2, -1, -2, -1, 181},
	{0, 1, 2, 1, -177},
	{4, 0, -2, -1, 176},
	{4, -1, -1, -1, 166},
	{1, 0, 1, -1, -164},
	{4, 0, 1, -1, 132},
	{1, 0, -1, -1, -119},
	{4, -1, 0, -1, 115},
	{2, -2, 0, 1, 107}};

// DayInfo
struct DAY_INFO
{
	int dayNo;		// 本月内的日序号
	int week;		// 本日的星期
	int mdayNo;		// 本日对应的农历月内日序号
	int mmonth;		// 本日所在的农历月序号
	SOLAR_TERMS st; // 本日对应的节气，-1 表示不是节气
};

class CDayInfo
{

protected:
	DAY_INFO m_Info;

public:
	CDayInfo(const DAY_INFO *info)
	{
		SetDayInfo(info);
	}
	void SetDayInfo(const DAY_INFO *info)
	{
		if (info != NULL)
			memmove(&m_Info, info, sizeof(DAY_INFO));
	}
	void GetDayInfo(DAY_INFO *info)
	{
		if (info != NULL)
			memmove(info, &m_Info, sizeof(DAY_INFO));
	}
	int GetDayDate() const { return m_Info.dayNo; };
	int GetDayChnDate() const { return m_Info.mdayNo; };
	int GetDayChnMonth() const { return m_Info.mmonth; };
	SOLAR_TERMS GetDaySolarTerm() const { return m_Info.st; };
	int GetDayWeek() const { return m_Info.week; };
};

// MonthInfo
struct MONTH_INFO
{
	int month;			 // 公历月序号
	int days;			 // 公历月天数
	int first_week;		 // 本月1号的星期
	double jieqiJD;		 // 本月节气的JD，本地时间
	SOLAR_TERMS jieqi;	 // 本月节气序号
	double zhongqiJD;	 // 本月中气的JD，本地时间
	SOLAR_TERMS zhongqi; // 本月中气的序号
};

// class CChineseCalendar;
class CMonthInfo
{
	friend class CChineseCalendar;

protected:
	MONTH_INFO m_Info;
	vector<CDayInfo> m_DayInfo;

protected:
	CMonthInfo(const MONTH_INFO *info)
	{
		SetMonthInfo(info);
	}

public:
	void SetMonthInfo(const MONTH_INFO *info)
	{
		if (info != NULL)
		{
			memmove(&m_Info, info, sizeof(MONTH_INFO));
		}
	}
	void GetMonthInfo(MONTH_INFO *info)
	{
		if (info != NULL)
		{
			memmove(info, &m_Info, sizeof(MONTH_INFO));
		}
	}
	const CDayInfo &GetDayInfo(int day) const
	{
		assert((m_DayInfo.size() > 0) && (m_DayInfo.size() <= MAX_GREGORIAN_MONTH_DAYS));

		if ((day < 1) || (day > (int)m_DayInfo.size()))
		{
			return m_DayInfo[0]; // 出错总是返回第一天的数据
		}

		return m_DayInfo[day - 1];
	}
	void AddSingleDay(CDayInfo &info)
	{
		m_DayInfo.push_back(info);
	}

	int GetMonthIndex() const { return m_Info.month; };
	int GetDaysCount() const { return m_Info.days; };
	int GetFirstDayWeek() const { return m_Info.first_week; };
	bool CheckValidDayCount()
	{
		return (m_Info.days == m_DayInfo.size());
	}

	void ClearInfo()
	{
		m_DayInfo.erase(m_DayInfo.begin(), m_DayInfo.end());
	}
};

// ChnMonthInfo
struct CHN_MONTH_INFO
{
	int mmonth;	   // 农历月序号
	int mname;	   // 农历月名称
	int mdays;	   // 本月天数
	double shuoJD; // 本月朔日的JD，本地时间
	double nextJD; // 下月朔日的JD，本地时间
	int leap;	   // 0 是正常月，1 是闰月
};

class CChnMonthInfo
{
protected:
	CHN_MONTH_INFO m_Info;

public:
	CChnMonthInfo(const CHN_MONTH_INFO *info)
	{
		SetChnmonthInfo(info);
	}

	void SetChnmonthInfo(const CHN_MONTH_INFO *info)
	{
		if (info != NULL)
		{
			memmove(&m_Info, info, sizeof(CHN_MONTH_INFO));
		}
	}
	void GetChnmonthInfo(CHN_MONTH_INFO *info)
	{
		if (info != NULL)
		{
			memmove(info, &m_Info, sizeof(CHN_MONTH_INFO));
		}
	}

	void SetLeapMonth(bool leap) { m_Info.leap = leap ? 1 : 0; };
	void ReIndexMonthName() { m_Info.mname--; };
	double GetNewMoonDayJD() { return m_Info.shuoJD; };
	double GetNextNewMoonDayJD() { return m_Info.nextJD; };
	int GetMonthIndex() { return m_Info.mmonth; };
	bool IsLeapMonth() { return (m_Info.leap == 1); };
	int GetMonthDays() { return m_Info.mdays; };
	int GetMonthName() { return m_Info.mname; };
};

// support
#define COUNT_OF(element) (sizeof(element) / sizeof(element[0]))
struct WZDAYTIME
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	double second;
};
const int daysOfMonth[MONTHES_FOR_YEAR] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
inline bool IsLeapYear(int year) { return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0); }

// 判断是否是启用格里历以后的日期
// 1582年，罗马教皇将1582年10月4日指定为1582年10月15日，从此废止了凯撒大帝制定的儒略历，改用格里历
bool IsGregorianDays(int year, int month, int day)
{
	if (year < 1582)
		return false;

	if (year == 1582)
	{
		if ((month < 10) || ((month == 10) && (day < 15)))
			return false;
	}

	return true;
}

int GetDaysOfMonth(int year, int month)
{
	if ((month < 1) || (month > 12))
		return 0;

	int days = daysOfMonth[month - 1];
	if ((month == 2) && IsLeapYear(year))
	{
		days++;
	}

	return days;
}

// 计算一年中过去的天数，包括指定的这一天
int CalcYearPassedDays(int year, int month, int day)
{
	int passedDays = 0;

	int i;
	for (i = 0; i < month - 1; i++)
	{
		passedDays += daysOfMonth[i];
	}

	passedDays += day;
	if ((month > 2) && IsLeapYear(year))
		passedDays++;

	return passedDays;
}

// 计算一年中还剩下的天数，不包括指定的这一天
int CalcYearRestDays(int year, int month, int day)
{
	int leftDays = daysOfMonth[month - 1] - day;

	int i;
	for (i = month; i < MONTHES_FOR_YEAR; i++)
	{
		leftDays += daysOfMonth[i];
	}

	if ((month <= 2) && IsLeapYear(year))
		leftDays++;

	return leftDays;
}

// 计算years年1月1日和yeare年1月1日之间的天数，
// 包括years年1月1日，但是不包括yeare年1月1日
int CalcYearsDays(int years, int yeare)
{
	int days = 0;

	int i;
	for (i = years; i < yeare; i++)
	{
		if (IsLeapYear(i))
			days += DAYS_OF_LEAP_YEAR;
		else
			days += DAYS_OF_NORMAL_YEAR;
	}

	return days;
}

int CalculateDays(int ys, int ms, int ds, int ye, int me, int de)
{
	int days = CalcYearRestDays(ys, ms, ds);

	if (ys != ye) // 不是同一年的日期
	{
		if ((ye - ys) >= 2) // 间隔超过一年，要计算间隔的整年时间
		{
			days += CalcYearsDays(ys + 1, ye);
		}
		days += CalcYearPassedDays(ye, me, de);
	}
	else
	{
		days = days - CalcYearRestDays(ye, me, de);
	}

	return days;
}

int TotalWeek(int year, int month, int day)
{
	int d = CalcYearPassedDays(year, month, day);
	int y = year - 1;
	int w = y * DAYS_OF_NORMAL_YEAR + y / 4 - y / 100 + y / 400 + d;

	return w % 7;
}

int ZellerWeek(int year, int month, int day)
{
	int m = month;
	int d = day;

	if (month <= 2) // 对小于2的月份进行修正
	{
		year--;
		m = month + 12;
	}

	int y = year % 100;
	int c = year / 100;

	int w = (y + y / 4 + c / 4 - 2 * c + (13 * (m + 1) / 5) + d - 1) % 7;
	if (w < 0) // 修正计算结果是负数的情况
		w += 7;

	return w;
}

double CalculateJulianDay(int year, int month, int day, int hour, int minute, double second)
{
	if (month <= 2)
	{
		month += 12;
		year -= 1;
	}
	int B = -2;
	if (IsGregorianDays(year, month, day))
	{
		B = year / 400 - year / 100;
	}

	double a = 365.25 * year;
	double b = 30.6001 * (month + 1);
	return int(a) + int(b) + B + 1720996.5 + day + hour / 24.0 + minute / 1440.0 + second / 86400.0;
}

double CalculateJulianDay2(WZDAYTIME *pDT)
{
	return CalculateJulianDay(pDT->year, pDT->month, pDT->day, pDT->hour, pDT->minute, pDT->second);
}

void GetDayTimeFromJulianDay(double jd, WZDAYTIME *pDT)
{
	int cna, cnd;
	double cnf;

	double jdf = jd + 0.5;
	cna = int(jdf);
	cnf = jdf - cna;
	if (cna > 2299161)
	{
		cnd = int((cna - 1867216.25) / 36524.25);
		cna = cna + 1 + cnd - int(cnd / 4);
	}
	cna = cna + 1524;
	int year = int((cna - 122.1) / 365.25);
	cnd = cna - int(365.25 * year);
	int month = int(cnd / 30.6001);
	int day = cnd - int(month * 30.6001);
	year = year - 4716;
	month = month - 1;
	if (month > 12)
		month = month - 12;
	if (month <= 2)
		year = year + 1;
	if (year < 1)
		year = year - 1;
	cnf = cnf * 24.0;
	pDT->hour = int(cnf);
	cnf = cnf - pDT->hour;
	cnf = cnf * 60.0;
	pDT->minute = int(cnf);
	cnf = cnf - pDT->minute;
	// cnf = cnf * 60.0;
	pDT->second = cnf * 60.0;
	pDT->year = year;
	pDT->month = month;
	pDT->day = day;
}

// 格林威治时间转本地时间（以格里历表示本地时间）
void UTCToLocalTime(int &year, int &month, int &day, int &hour, int &minute, double &second)
{
	_tzset();

	long _tz = 0;
	_get_timezone(&_tz);
	// 计算本地时间和标准时间的时差（单位：秒）
	int nDifference_hour = static_cast<int>(_tz / 3600);
	int nDifference_minute = static_cast<int>((_tz - nDifference_hour * 3600) / 60);
	int nDifference_second = static_cast<int>((_tz - nDifference_hour * 3600) - nDifference_minute * 60);

	// 格林威治时间 + 时差 = 本地时间

	// 秒
	second = second - nDifference_second;
	if (second >= 60 || second < 0)
	{
		minute = second > 0 ? minute + 1 : minute - 1;
		second = abs(abs(second) - 60);
	}

	// 分
	minute = minute - nDifference_minute;
	if (minute >= 60 || minute < 0)
	{
		hour = minute > 0 ? hour + 1 : hour - 1;
		minute = abs(abs(minute) - 60);
	}

	// 时
	hour = hour - nDifference_hour;
	if (hour >= 24 || hour < 0)
	{
		day = (hour >= 24 || hour == 0) ? day + 1 : day - 1;
		hour = abs(abs(hour) - 24);
	}

	// 日
	int nDaysOfMonth = GetDaysOfMonth(year, month);
	if (day > nDaysOfMonth || day <= 0)
	{
		if (day > nDaysOfMonth)
			month++;
		if (day < nDaysOfMonth || day <= 0)
			month--;

		day = abs(abs(day) - nDaysOfMonth);
	}
	// 月
	if (month > 12 || month <= 0)
	{
		year = month > 0 ? year + 1 : year - 1;
		month = month > 0 ? abs(month - 12) : abs(12 + month);
	}
}

// 本地时间转格林威治时间（以格里历表示）
void LocalTimetoUTC(int &year, int &month, int &day, int &hour, int &minute, double &second)
{
	_tzset();

	long _tz = 0;
	_get_timezone(&_tz);
	// 计算本地时间和标准时间的时差（单位：秒）
	int nDifference_hour = static_cast<int>(_tz / 3600);
	int nDifference_minute = static_cast<int>((_tz - nDifference_hour * 3600) / 60);
	int nDifference_second = static_cast<int>((_tz - nDifference_hour * 3600) - nDifference_minute * 60);

	// 本地时间 - 时差 = 格林威治时间

	// 秒
	second = second + nDifference_second;
	if (second >= 60 || second < 0)
	{
		minute = second > 0 ? minute + 1 : minute - 1;
		second = abs(abs(second) - 60);
	}

	// 分
	minute = minute + nDifference_minute;
	if (minute >= 60 || minute < 0)
	{
		hour = minute > 0 ? hour + 1 : hour - 1;
		minute = abs(abs(minute) - 60);
	}

	// 时
	hour = hour + nDifference_hour;
	if (hour >= 24 || hour < 0)
	{
		day = (hour >= 24 || hour == 0) ? day + 1 : day - 1;
		hour = abs(abs(hour) - 24);
	}

	// 日
	int nDaysOfMonth = GetDaysOfMonth(year, month);
	if (day > nDaysOfMonth || day <= 0)
	{
		if (day > nDaysOfMonth)
			month++;
		if (day < nDaysOfMonth || day <= 0)
			month--;

		day = abs(abs(day) - nDaysOfMonth);
	}
	// 月
	if (month > 12 || month <= 0)
	{
		year = month > 0 ? year + 1 : year - 1;
		month = month > 0 ? abs(month - 12) : abs(12 + month);
	}
}

double JDUTCToLocalTime(double utcJD)
{
	_tzset();

	long seconds = 0;
	_get_timezone(&seconds);

	return utcJD - (double)seconds / 86400.0;
}

double JDLocalTimetoUTC(double localJD)
{
	_tzset();

	long seconds = 0;
	_get_timezone(&seconds);

	return localJD + (double)seconds / 86400.0;
}

/*
double JDUTCToLocalTime(double utcJD)
{
	WZDAYTIME dt;
	GetDayTimeFromJulianDay(utcJD, &dt);

	UTCToLocalTime(dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);

	return CalculateJulianDay(dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
}

double JDLocalTimetoUTC(double localJD)
{
	WZDAYTIME dt;
	GetDayTimeFromJulianDay(localJD, &dt);
	LocalTimetoUTC(dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);

	return CalculateJulianDay(dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
}
*/
double JDUTCtoTD(double utcJD)
{
	double jd2K = utcJD - JD2000;
	double tian = TdUtcDeltatT2(jd2K);
	utcJD += tian;

	return utcJD;
}

double JDTDtoUTC(double tdJD)
{
	double jd2K = tdJD - JD2000;
	double tian = TdUtcDeltatT2(jd2K);
	tdJD -= tian;

	return tdJD;
}

double JDLocalTimetoTD(double localJD)
{
	double tmp = JDLocalTimetoUTC(localJD);

	return JDUTCtoTD(tmp);
}

double JDTDtoLocalTime(double tdJD)
{
	double tmp = JDTDtoUTC(tdJD);

	return JDUTCToLocalTime(tmp);
}

double RadianToDegree(double radian)
{
	return radian * 180.0 / PI;
}

double DegreeToRadian(double degree)
{
	return degree * PI / 180.0;
}

// 调整角度到 0-360 之间
double Mod360Degree(double degrees)
{
	double dbValue = degrees;

	while (dbValue < 0.0)
		dbValue += 360.0;

	while (dbValue > 360.0)
		dbValue -= 360.0;

	return dbValue;
}

int JDtoString(double jd, char *strBuf, int bufSize)
{
	WZDAYTIME dt;

	GetDayTimeFromJulianDay(jd, &dt);

	return sprintf_s(strBuf, bufSize, "%04d-%02d-%02d, %02d:%02d:%4.2f", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
}

// other_func
static const double s_stAccInfo[] = {
	0.00, 1272494.40, 2548020.60, 3830143.80, 5120226.60, 6420865.80,
	7732018.80, 9055272.60, 10388958.00, 11733065.40, 13084292.40, 14441592.00,
	15800560.80, 17159347.20, 18513766.20, 19862002.20, 21201005.40, 22529659.80,
	23846845.20, 25152606.00, 26447687.40, 27733451.40, 29011921.20, 30285477.60};

// 已知1900年小寒时刻为1月6日2:05:00，
const double base1900_SlightColdJD = 2415025.5868055555;

// 利用已知节气推算其它节气的函数，st的值以小寒节气为0，大寒为1，其它节气类推
double CalculateSolarTermsByExp(int year, int st)
{
	if ((st < 0) || (st > 24))
		return 0.0;

	double stJd = 365.24219878 * (year - 1900) + s_stAccInfo[st] / 86400.0;

	return base1900_SlightColdJD + stJd;
}

/*
计算节气和朔日的经验公式

当天到1900年1月0日（星期日）的差称为积日，那么第y年（1900算0年）第x个节气的积日是：
F = 365.242 * y + 6.2 + 15.22 *x - 1.9 * sin(0.262 * x)

从1900年开始的第m个朔日的公式是：
M = 1.6 + 29.5306 * m + 0.4 * sin(1 - 0.45058 * m)
*/
double CalculateSolarTermsByFm(int year, int st)
{
	double baseJD = CalculateJulianDay(1900, 1, 1, 0, 0, 0.0) - 1;
	int y = year - 1900;

	double accDay = 365.2422 * y + 6.2 + 15.22 * st - 1.0 * sin(0.262 * st);

	return baseJD + accDay;
}

double CalculateNewMoonByFm(int m)
{
	double baseJD = CalculateJulianDay(1900, 1, 1, 0, 0, 0.0) - 1;

	double accDay = 1.6 + 29.5306 * m + 0.4 * sin(1 - 0.45058 * m);

	return baseJD + accDay;
}

// nutation
// 天体章动系数类型变量
struct NUTATION_COEFFICIENT
{
	double D;
	double M;
	double Mp;
	double F;
	double omega;
	double sine1;
	double sine2;
	double cosine1;
	double cosine2;
};
// 天体黄经章动和黄赤交角章动的周期项，来源《天体计算》第21章，表21-A
// 单位是0".0001.
const NUTATION_COEFFICIENT nutation[63] = {
	{0, 0, 0, 0, 1, -171996, -174.2, 92025, 8.9},
	{-2, 0, 0, 2, 2, -13187, -1.6, 5736, -3.1},
	{0, 0, 0, 2, 2, -2274, -0.2, 977, -0.5},
	{0, 0, 0, 0, 2, 2062, 0.2, -895, 0.5},
	{0, 1, 0, 0, 0, 1426, -3.4, 54, -0.1},
	{0, 0, 1, 0, 0, 712, 0.1, -7, 0},
	{-2, 1, 0, 2, 2, -517, 1.2, 224, -0.6},
	{0, 0, 0, 2, 1, -386, -0.4, 200, 0},
	{0, 0, 1, 2, 2, -301, 0, 129, -0.1},
	{-2, -1, 0, 2, 2, 217, -0.5, -95, 0.3},
	{-2, 0, 1, 0, 0, -158, 0, 0, 0},
	{-2, 0, 0, 2, 1, 129, 0.1, -70, 0},
	{0, 0, -1, 2, 2, 123, 0, -53, 0},
	{2, 0, 0, 0, 0, 63, 0, 0, 0},
	{0, 0, 1, 0, 1, 63, 0.1, -33, 0},
	{2, 0, -1, 2, 2, -59, 0, 26, 0},
	{0, 0, -1, 0, 1, -58, -0.1, 32, 0},
	{0, 0, 1, 2, 1, -51, 0, 27, 0},
	{-2, 0, 2, 0, 0, 48, 0, 0, 0},
	{0, 0, -2, 2, 1, 46, 0, -24, 0},
	{2, 0, 0, 2, 2, -38, 0, 16, 0},
	{0, 0, 2, 2, 2, -31, 0, 13, 0},
	{0, 0, 2, 0, 0, 29, 0, 0, 0},
	{-2, 0, 1, 2, 2, 29, 0, -12, 0},
	{0, 0, 0, 2, 0, 26, 0, 0, 0},
	{-2, 0, 0, 2, 0, -22, 0, 0, 0},
	{0, 0, -1, 2, 1, 21, 0, -10, 0},
	{0, 2, 0, 0, 0, 17, -0.1, 0, 0},
	{2, 0, -1, 0, 1, 16, 0, -8, 0},
	{-2, 2, 0, 2, 2, -16, 0.1, 7, 0},
	{0, 1, 0, 0, 1, -15, 0, 9, 0},
	{-2, 0, 1, 0, 1, -13, 0, 7, 0},
	{0, -1, 0, 0, 1, -12, 0, 6, 0},
	{0, 0, 2, -2, 0, 11, 0, 0, 0},
	{2, 0, -1, 2, 1, -10, 0, 5, 0},
	{2, 0, 1, 2, 2, -8, 0, 3, 0},
	{0, 1, 0, 2, 2, 7, 0, -3, 0},
	{-2, 1, 1, 0, 0, -7, 0, 0, 0},
	{0, -1, 0, 2, 2, -7, 0, 3, 0},
	{2, 0, 0, 2, 1, -7, 0, 3, 0},
	{2, 0, 1, 0, 0, 6, 0, 0, 0},
	{-2, 0, 2, 2, 2, 6, 0, -3, 0},
	{-2, 0, 1, 2, 1, 6, 0, -3, 0},
	{2, 0, -2, 0, 1, -6, 0, 3, 0},
	{2, 0, 0, 0, 1, -6, 0, 3, 0},
	{0, -1, 1, 0, 0, 5, 0, 0, 0},
	{-2, -1, 0, 2, 1, -5, 0, 3, 0},
	{-2, 0, 0, 0, 1, -5, 0, 3, 0},
	{0, 0, 2, 2, 1, -5, 0, 3, 0},
	{-2, 0, 2, 0, 1, 4, 0, 0, 0},
	{-2, 1, 0, 2, 1, 4, 0, 0, 0},
	{0, 0, 1, -2, 0, 4, 0, 0, 0},
	{-1, 0, 1, 0, 0, -4, 0, 0, 0},
	{-2, 1, 0, 0, 0, -4, 0, 0, 0},
	{1, 0, 0, 0, 0, -4, 0, 0, 0},
	{0, 0, 1, 2, 0, 3, 0, 0, 0},
	{0, 0, -2, 2, 2, -3, 0, 0, 0},
	{-1, -1, 1, 0, 0, -3, 0, 0, 0},
	{0, 1, 1, 0, 0, -3, 0, 0, 0},
	{0, -1, 1, 2, 2, -3, 0, 0, 0},
	{2, -1, -1, 2, 2, -3, 0, 0, 0},
	{0, 0, 3, 2, 2, -3, 0, 0, 0},
	{2, -1, 0, 2, 2, -3, 0, 0, 0}};

static void GetEarthNutationParameter(double dt, double *D, double *M, double *Mp, double *F, double *Omega)
{
	double T = dt * 10; // T是从J2000起算的儒略世纪数
	double T2 = T * T;
	double T3 = T2 * T;

	// 平距角（如月对地心的角距离）
	*D = 297.85036 + 445267.111480 * T - 0.0019142 * T2 + T3 / 189474.0;

	// 太阳（地球）平近点角
	*M = 357.52772 + 35999.050340 * T - 0.0001603 * T2 - T3 / 300000.0;

	// 月亮平近点角
	*Mp = 134.96298 + 477198.867398 * T + 0.0086972 * T2 + T3 / 56250.0;

	// 月亮纬度参数
	*F = 93.27191 + 483202.017538 * T - 0.0036825 * T2 + T3 / 327270.0;

	// 黄道与月亮平轨道升交点黄经
	*Omega = 125.04452 - 1934.136261 * T + 0.0020708 * T2 + T3 / 450000.0;
}

// 计算某时刻的黄经章动干扰量，dt是儒略千年数，返回值单位是度
double CalcEarthLongitudeNutation(double dt)
{
	double T = dt * 10;
	double D, M, Mp, F, Omega;

	GetEarthNutationParameter(dt, &D, &M, &Mp, &F, &Omega);

	double resulte = 0.0;
	for (int i = 0; i < sizeof(nutation) / sizeof(nutation[0]); i++)
	{
		double sita = nutation[i].D * D + nutation[i].M * M + nutation[i].Mp * Mp + nutation[i].F * F + nutation[i].omega * Omega;
		sita = DegreeToRadian(sita);

		resulte += (nutation[i].sine1 + nutation[i].sine2 * T) * sin(sita);
	}

	// 先乘以章动表的系数 0.0001，然后换算成度的单位
	return resulte * 0.0001 / 3600.0;
}

// 计算某时刻的黄赤交角章动干扰量，dt是儒略千年数，返回值单位是度
double CalcEarthObliquityNutation(double dt)
{
	double T = dt * 10; // T是从J2000起算的儒略世纪数
	double D, M, Mp, F, Omega;

	GetEarthNutationParameter(dt, &D, &M, &Mp, &F, &Omega);

	double resulte = 0.0;
	for (int i = 0; i < sizeof(nutation) / sizeof(nutation[0]); i++)
	{
		double sita = nutation[i].D * D + nutation[i].M * M + nutation[i].Mp * Mp + nutation[i].F * F + nutation[i].omega * Omega;
		sita = DegreeToRadian(sita);

		resulte += (nutation[i].cosine1 + nutation[i].cosine2 * T) * cos(sita);
	}

	// 先乘以章动表的系数 0.001，然后换算成度的单位
	return resulte * 0.0001 / 3600.0;
}

// moon
static void GetMoonEclipticParameter(double dt, double *Lp, double *D, double *M, double *Mp, double *F, double *E)
{
	double T = dt; // T是从J2000起算的儒略世纪数
	double T2 = T * T;
	double T3 = T2 * T;
	double T4 = T3 * T;

	// 月球平黄经
	*Lp = 218.3164591 + 481267.88134236 * T - 0.0013268 * T2 + T3 / 538841.0 - T4 / 65194000.0;
	*Lp = Mod360Degree(*Lp);

	// 月日距角
	*D = 297.8502042 + 445267.1115168 * T - 0.0016300 * T2 + T3 / 545868.0 - T4 / 113065000.0;
	*D = Mod360Degree(*D);

	// 太阳平近点角
	*M = 357.5291092 + 35999.0502909 * T - 0.0001536 * T2 + T3 / 24490000.0;
	*M = Mod360Degree(*M);

	// 月亮平近点角
	*Mp = 134.9634114 + 477198.8676313 * T + 0.0089970 * T2 + T3 / 69699.0 - T4 / 14712000.0;
	*Mp = Mod360Degree(*Mp);

	// 月球经度参数(到升交点的平角距离)
	*F = 93.2720993 + 483202.0175273 * T - 0.0034029 * T2 - T3 / 3526000.0 + T4 / 863310000.0;
	*F = Mod360Degree(*F);

	*E = 1 - 0.002516 * T - 0.0000074 * T2;
}

// 计算月球地心黄经周期项的和
static double CalcMoonECLongitudePeriodic(double D, double M, double Mp, double F, double E)
{
	double EI = 0.0;

	for (int i = 0; i < COUNT_OF(Moon_longitude); i++)
	{
		double sita = Moon_longitude[i].D * D + Moon_longitude[i].M * M + Moon_longitude[i].Mp * Mp + Moon_longitude[i].F * F;
		sita = DegreeToRadian(sita);
		EI += (Moon_longitude[i].eiA * sin(sita) * pow(E, fabs(Moon_longitude[i].M)));
	}

	return EI;
}

// 计算月球地心黄纬周期项的和
static double CalcMoonECLatitudePeriodicTbl(double D, double M, double Mp, double F, double E)
{
	double EB = 0.0;

	for (int i = 0; i < COUNT_OF(moon_Latitude); i++)
	{
		double sita = moon_Latitude[i].D * D + moon_Latitude[i].M * M + moon_Latitude[i].Mp * Mp + moon_Latitude[i].F * F;
		sita = DegreeToRadian(sita);
		EB += (moon_Latitude[i].eiA * sin(sita) * pow(E, fabs(Moon_longitude[i].M)));
	}

	return EB;
}

// 计算月球地心距离周期项的和
static double CalcMoonECDistancePeriodicTbl(double D, double M, double Mp, double F, double E)
{
	double ER = 0.0;

	for (int i = 0; i < COUNT_OF(Moon_longitude); i++)
	{
		double sita = Moon_longitude[i].D * D + Moon_longitude[i].M * M + Moon_longitude[i].Mp * Mp + Moon_longitude[i].F * F;
		sita = DegreeToRadian(sita);
		ER += (Moon_longitude[i].erA * cos(sita) * pow(E, fabs(Moon_longitude[i].M)));
	}

	return ER;
}

// 计算金星摄动,木星摄动以及地球扁率摄动对月球地心黄经的影响,dt 是儒略世纪数，Lp和F单位是度
static double CalcMoonLongitudePerturbation(double dt, double Lp, double F)
{
	double T = dt; // T是从J2000起算的儒略世纪数
	double A1 = 119.75 + 131.849 * T;
	double A2 = 53.09 + 479264.290 * T;

	A1 = Mod360Degree(A1);
	A2 = Mod360Degree(A2);

	double result = 3958.0 * sin(DegreeToRadian(A1));
	result += (1962.0 * sin(DegreeToRadian(Lp - F)));
	result += (318.0 * sin(DegreeToRadian(A2)));

	return result;
}

// 计算金星摄动,木星摄动以及地球扁率摄动对月球地心黄纬的影响,dt 是儒略世纪数，Lp、Mp和F单位是度
static double CalcMoonLatitudePerturbation(double dt, double Lp, double F, double Mp)
{
	double T = dt; // T是从J2000起算的儒略世纪数
	double A1 = 119.75 + 131.849 * T;
	double A3 = 313.45 + 481266.484 * T;

	A1 = Mod360Degree(A1);
	A3 = Mod360Degree(A3);

	double result = -2235.0 * sin(DegreeToRadian(Lp));
	result += (382.0 * sin(DegreeToRadian(A3)));
	result += (175.0 * sin(DegreeToRadian(A1 - F)));
	result += (175.0 * sin(DegreeToRadian(A1 + F)));
	result += (127.0 * sin(DegreeToRadian(Lp - Mp)));
	result += (115.0 * sin(DegreeToRadian(Lp + Mp)));

	return result;
}

// 获取月亮地心黄经，单位度
double GetMoonEclipticLongitudeEC(double dbJD)
{
	double Lp, D, M, Mp, F, E;
	double dt = (dbJD - JD2000) / 36525.0; // 儒略世纪数

	GetMoonEclipticParameter(dt, &Lp, &D, &M, &Mp, &F, &E);

	// 计算月球地心黄经周期项
	double EI = CalcMoonECLongitudePeriodic(D, M, Mp, F, E);

	// 修正金星,木星以及地球扁率摄动
	EI += CalcMoonLongitudePerturbation(dt, Lp, F);

	// 计算月球地心黄经
	double longitude = Lp + EI / 1000000.0;

	// 计算天体章动干扰
	longitude += CalcEarthLongitudeNutation(dt / 10.0);

	longitude = Mod360Degree(longitude); // 映射到0-360范围内
	return longitude;
}

double GetMoonEclipticLatitudeEC(double dbJD)
{
	double dt = (dbJD - JD2000) / 36525.0; // 儒略世纪数

	double Lp, D, M, Mp, F, E;
	GetMoonEclipticParameter(dt, &Lp, &D, &M, &Mp, &F, &E);

	// 计算月球地心黄纬周期项
	double EB = CalcMoonECLatitudePeriodicTbl(D, M, Mp, F, E);

	// 修正金星,木星以及地球扁率摄动
	EB += CalcMoonLatitudePerturbation(dt, Lp, F, Mp);

	// 计算月球地心黄纬
	double latitude = EB / 1000000.0;

	return latitude;
}

double GetMoonEarthDistance(double dbJD)
{
	double dt = (dbJD - JD2000) / 36525.0; // 儒略世纪数

	double Lp, D, M, Mp, F, E;
	GetMoonEclipticParameter(dt, &Lp, &D, &M, &Mp, &F, &E);

	// 计算月球地心距离周期项
	double ER = CalcMoonECDistancePeriodicTbl(D, M, Mp, F, E);

	// 计算月球地心黄纬
	double distance = 385000.56 + ER / 1000.0;

	return distance;
}

// sun

static double CalcPeriodicTerm(const VSOP87_COEFFICIENT *coff, int count, double dt)
{
	double val = 0.0;

	for (int i = 0; i < count; i++)
		val += (coff[i].A * cos((coff[i].B + coff[i].C * dt)));

	return val;
}

/*计算太阳的地心黄经(度)，dt是儒略千年数*/

double CalcSunEclipticLongitudeEC(double dt)
{

	double L0 = CalcPeriodicTerm(Earth_L0, COUNT_OF(Earth_L0), dt);
	double L1 = CalcPeriodicTerm(Earth_L1, COUNT_OF(Earth_L1), dt);
	double L2 = CalcPeriodicTerm(Earth_L2, COUNT_OF(Earth_L2), dt);
	double L3 = CalcPeriodicTerm(Earth_L3, COUNT_OF(Earth_L3), dt);
	double L4 = CalcPeriodicTerm(Earth_L4, COUNT_OF(Earth_L4), dt);
	double L5 = CalcPeriodicTerm(Earth_L5, COUNT_OF(Earth_L5), dt);

	double L = (((((L5 * dt + L4) * dt + L3) * dt + L2) * dt + L1) * dt + L0) / 100000000.0;

	/*地心黄经 = 日心黄经 + 180度*/
	return (Mod360Degree(Mod360Degree(L / RADIAN_PER_ANGLE) + 180.0));
}

/*计算太阳的地心黄纬(度)，dt是儒略千年数*/
double CalcSunEclipticLatitudeEC(double dt)
{
	double B0 = CalcPeriodicTerm(Earth_B0, COUNT_OF(Earth_B0), dt);
	double B1 = CalcPeriodicTerm(Earth_B1, COUNT_OF(Earth_B1), dt);
	double B2 = CalcPeriodicTerm(Earth_B2, COUNT_OF(Earth_B2), dt);
	double B3 = CalcPeriodicTerm(Earth_B3, COUNT_OF(Earth_B3), dt);
	double B4 = CalcPeriodicTerm(Earth_B4, COUNT_OF(Earth_B4), dt);

	double B = (((((B4 * dt) + B3) * dt + B2) * dt + B1) * dt + B0) / 100000000.0;

	/*地心黄纬 = －日心黄纬*/
	return -(B / RADIAN_PER_ANGLE);
}

/*修正太阳的地心黄经，longitude, latitude 是修正前的太阳地心黄经和地心黄纬(度)，dt是儒略千年数，返回值单位度*/
double AdjustSunEclipticLongitudeEC(double dt, double longitude, double latitude)
{
	double T = dt * 10; // T是儒略世纪数

	double dbLdash = longitude - 1.397 * T - 0.00031 * T * T;

	// 转换为弧度
	dbLdash *= RADIAN_PER_ANGLE;

	return (-0.09033 + 0.03916 * (cos(dbLdash) + sin(dbLdash)) * tan(latitude * RADIAN_PER_ANGLE)) / 3600.0;
}

/*修正太阳的地心黄纬，longitude是修正前的太阳地心黄经(度)，dt是儒略千年数，返回值单位度*/
double AdjustSunEclipticLatitudeEC(double dt, double longitude)
{
	double T = dt * 10; // T是儒略世纪数
	double dLdash = longitude - 1.397 * T - 0.00031 * T * T;

	// 转换为弧度
	dLdash *= RADIAN_PER_ANGLE;

	return (0.03916 * (cos(dLdash) - sin(dLdash))) / 3600.0;
}

double CalcSunEarthRadius(double dt)
{
	double R0 = CalcPeriodicTerm(Earth_R0, COUNT_OF(Earth_R0), dt);
	double R1 = CalcPeriodicTerm(Earth_R1, COUNT_OF(Earth_R1), dt);
	double R2 = CalcPeriodicTerm(Earth_R2, COUNT_OF(Earth_R2), dt);
	double R3 = CalcPeriodicTerm(Earth_R3, COUNT_OF(Earth_R3), dt);
	double R4 = CalcPeriodicTerm(Earth_R4, COUNT_OF(Earth_R4), dt);

	double R = (((((R4 * dt) + R3) * dt + R2) * dt + R1) * dt + R0) / 100000000.0;

	return R;
}

/*得到某个儒略日的太阳地心黄经(视黄经)，单位度*/
double GetSunEclipticLongitudeEC(double jde)
{
	double dt = (jde - JD2000) / 365250.0; /*儒略千年数*/

	// 计算太阳的地心黄经
	double longitude = CalcSunEclipticLongitudeEC(dt);

	// 计算太阳的地心黄纬
	double latitude = CalcSunEclipticLatitudeEC(dt) * 3600.0;

	// 修正精度
	longitude += AdjustSunEclipticLongitudeEC(dt, longitude, latitude);

	// 修正天体章动
	longitude += CalcEarthLongitudeNutation(dt);

	// 修正光行差
	/*太阳地心黄经光行差修正项是: -20".4898/R*/
	longitude -= (20.4898 / CalcSunEarthRadius(dt)) / 3600.0;

	return longitude;
}

/*得到某个儒略日的太阳地心黄纬(视黄纬)，单位度*/
double GetSunEclipticLatitudeEC(double jde)
{
	double dt = (jde - JD2000) / 365250.0; /*儒略千年数*/

	// 计算太阳的地心黄经
	double longitude = CalcSunEclipticLongitudeEC(dt);
	// 计算太阳的地心黄纬
	double latitude = CalcSunEclipticLatitudeEC(dt) * 3600.0;

	// 修正精度
	double delta = AdjustSunEclipticLatitudeEC(dt, longitude);
	latitude += delta * 3600.0;

	return latitude;
}

/*得到某个时间的太阳地心黄经(视黄经)，单位度，dt是儒略世纪数*/
// double GetSunEclipticLongitudeEC2(double dt);

// calendar_func
static double GetInitialEstimateSolarTerms(int year, int angle)
{
	int STMonth = int(ceil(double((angle + 90.0) / 30.0)));
	STMonth = STMonth > 12 ? STMonth - 12 : STMonth;

	/* 每月第一个节气发生日期基本都4-9日之间，第二个节气的发生日期都在16－24日之间*/
	if ((angle % 15 == 0) && (angle % 30 != 0))
	{
		return CalculateJulianDay(year, STMonth, 6, 12, 0, 0.00);
	}
	else
	{
		return CalculateJulianDay(year, STMonth, 20, 12, 0, 0.00);
	}
}

// 计算指定年份的任意节气，angle是节气在黄道上的读数
// 返回指定节气的儒略日时间(力学时)
double CalculateSolarTerms(int year, int angle)
{
	double JD0, JD1, stDegree, stDegreep;

	JD1 = GetInitialEstimateSolarTerms(year, angle);
	do
	{
		JD0 = JD1;
		stDegree = GetSunEclipticLongitudeEC(JD0);
		/*
			对黄经0度迭代逼近时，由于角度360度圆周性，估算黄经值可能在(345,360]和[0,15)两个区间，
			如果值落入前一个区间，需要进行修正
		*/
		stDegree = ((angle == 0) && (stDegree > 345.0)) ? stDegree - 360.0 : stDegree;
		stDegreep = (GetSunEclipticLongitudeEC(JD0 + 0.000005) - GetSunEclipticLongitudeEC(JD0 - 0.000005)) / 0.00001;
		JD1 = JD0 - (stDegree - angle) / stDegreep;
	} while ((fabs(JD1 - JD0) > 0.0000001));

	return JD1;
}

/*
得到给定的时间后面第一个日月合朔的时间，平均误差小于3秒
输入参数是指定时间的力学时儒略日数
返回值是日月合朔的力学时儒略日数
*/
double CalculateMoonShuoJD(double tdJD)
{
	double JD0, JD1, stDegree, stDegreep;

	JD1 = tdJD;
	do
	{
		JD0 = JD1;
		double moonLongitude = GetMoonEclipticLongitudeEC(JD0);
		double sunLongitude = GetSunEclipticLongitudeEC(JD0);
		if ((moonLongitude > 330.0) && (sunLongitude < 30.0))
		{
			sunLongitude = 360.0 + sunLongitude;
		}
		if ((sunLongitude > 330.0) && (moonLongitude < 30.0))
		{
			moonLongitude = 60.0 + moonLongitude;
		}

		stDegree = moonLongitude - sunLongitude;
		if (stDegree >= 360.0)
			stDegree -= 360.0;

		if (stDegree < -360.0)
			stDegree += 360.0;

		stDegreep = (GetMoonEclipticLongitudeEC(JD0 + 0.000005) - GetSunEclipticLongitudeEC(JD0 + 0.000005) - GetMoonEclipticLongitudeEC(JD0 - 0.000005) + GetSunEclipticLongitudeEC(JD0 - 0.000005)) / 0.00001;
		JD1 = JD0 - stDegree / stDegreep;
	} while ((fabs(JD1 - JD0) > 0.00000001));

	return JD1;
}

void CalculateStemsBranches(int year, int *stems, int *branches)
{
	int sc = year - 2000;
	*stems = (7 + sc) % 10;
	*branches = (5 + sc) % 12;

	if (*stems <= 0)
		*stems += 10;
	if (*branches <= 0)
		*branches += 12;
}

// ChineseCalendar
const int SPRING_BEGINS_INDEX = 3;
const int SOLAR_TERMS_CALC_COUNT = 25;
const int NEW_MOON_CALC_COUNT = 15;

class CChineseCalendar
{
protected:
	bool m_bInitial;
	int m_year;
	double m_SolarTermsJD[SOLAR_TERMS_CALC_COUNT];
	double m_NewMoonJD[NEW_MOON_CALC_COUNT];

	vector<CMonthInfo> m_MonthInfo;
	vector<CChnMonthInfo> m_ChnMonthInfo;

public:
	CChineseCalendar()
	{
		memset(m_SolarTermsJD, 0, sizeof(double) * SOLAR_TERMS_CALC_COUNT);
		memset(m_NewMoonJD, 0, sizeof(double) * NEW_MOON_CALC_COUNT);
		m_bInitial = false;
	}
	CMonthInfo &GetMonthInfo(int month)
	{
		assert(m_MonthInfo.size() == MONTHES_FOR_YEAR);

		if ((month < 1) || (month > MONTHES_FOR_YEAR))
			return m_MonthInfo[0]; // 出错总是返回第一个月的数据

		return m_MonthInfo[month - 1];
	}

	CChnMonthInfo &GetChnMonthInfo(int mmonth)
	{
		assert(m_ChnMonthInfo.size() > 0);

		if ((mmonth < 0) || (mmonth >= (int)m_ChnMonthInfo.size()))
			return m_ChnMonthInfo[0]; // 出错总是返回第一个月的数据

		return m_ChnMonthInfo[mmonth];
	}

	bool SetGeriYear(int year)
	{
		ClearCalendar();
		m_year = year;
		CalculateProcData();
		if (!BuildAllChnMonthInfo())
			return false;
		CalcLeapChnMonth();
		m_bInitial = BuildAllMonthInfo();

		return m_bInitial;
	}

	void ClearCalendar()
	{
		for (vector<CMonthInfo>::iterator mi = m_MonthInfo.begin(); mi != m_MonthInfo.end(); ++mi)
			mi->ClearInfo();

		m_MonthInfo.erase(m_MonthInfo.begin(), m_MonthInfo.end());
		m_ChnMonthInfo.erase(m_ChnMonthInfo.begin(), m_ChnMonthInfo.end());

		memset(m_SolarTermsJD, 0, sizeof(double) * SOLAR_TERMS_CALC_COUNT);
		memset(m_NewMoonJD, 0, sizeof(double) * NEW_MOON_CALC_COUNT);
		m_bInitial = false;
	}

	void GetSpringBeginsDay(int *month, int *day)
	{
		if (!m_bInitial)
			return;

		if ((month != NULL) && (day != NULL))
		{
			WZDAYTIME dt;
			GetDayTimeFromJulianDay(m_SolarTermsJD[SPRING_BEGINS_INDEX], &dt);
			*month = dt.month;
			*day = dt.day;
		}
	}

	int GetCurrentYear() { return m_year; };
	bool IsCalendarReady() { return m_bInitial; };

protected:
	void CalculateProcData()
	{
		// 计算从上一年冬至开始到今天冬至全部25个节气
		GetAllSolarTermsJD(m_year - 1, WINTER_SOLSTICE, m_SolarTermsJD);

		double lastDongZhi = JDLocalTimetoTD(m_SolarTermsJD[0]);
		double tmpShuo = CalculateMoonShuoJD(lastDongZhi); // 求与冬至最近的一个朔日
		tmpShuo = JDTDtoLocalTime(tmpShuo);
		if (tmpShuo > m_SolarTermsJD[0]) // 这里原来的代码是if(tmpShuo > lastDongZhi)，有问题
			tmpShuo -= 29.53;

		GetNewMoonJDs(tmpShuo, m_NewMoonJD);

		// char buf[64];
		// char bufAll[128];
		// for(int i = 0; i < NEW_MOON_CALC_COUNT; i++)
		//{
		//	JDtoString(m_NewMoonJD[i], buf, 64);
		//	sprintf(bufAll, "%f, %s\n", m_NewMoonJD[i], buf);
		//	TRACE(bufAll);
		// }
	}

	// 计算从某个时间之后的连续13个朔日，jd是开始时间的儒略日，本地时间
	// NewMoon中存放jd之后的13个朔日时间，单位是儒略日，本地时间
	void GetNewMoonJDs(double jd, double *NewMoon)
	{
		double tdjd = JDLocalTimetoTD(jd);

		for (int i = 0; i < NEW_MOON_CALC_COUNT; i++)
		{
			double shuoJD = CalculateMoonShuoJD(tdjd);
			shuoJD = JDTDtoLocalTime(shuoJD);
			NewMoon[i] = shuoJD;

			tdjd += 29.5; // 转到下一个最接近朔日的时间
		}
	}

	// 从某一年的某个节气开始，连续计算25个节气，返回各节气的儒略日，本地时间
	void GetAllSolarTermsJD(int year, int start, double *SolarTerms)
	{
		int i = 0;
		int st = start;
		while (i < 25)
		{
			double jd = CalculateSolarTerms(year, st * 15);
			SolarTerms[i++] = JDTDtoLocalTime(jd);
			if (st == WINTER_SOLSTICE)
				year++;

			st = (st + 1) % SOLAR_TERMS_COUNT;
		}
	}

	SOLAR_TERMS GetSolarTermsName(double *solarTerms, int count, double today)
	{
		int i = 0;
		while (i < 25)
		{
			if (int(solarTerms[i] + 0.5) == int(today + 0.5))
				return (i + 18) % SOLAR_TERMS_COUNT;

			i++;
		}

		return -1;
	}

	bool BuildAllChnMonthInfo()
	{
		CHN_MONTH_INFO info; // 一年最多可13个农历月
		int i;
		int yuejian = 11; // 采用夏历建寅，冬至所在月份为农历11月
		for (i = 0; i < (NEW_MOON_CALC_COUNT - 1); i++)
		{
			info.mmonth = i;
			info.mname = (yuejian <= 12) ? yuejian : yuejian - 12;
			info.shuoJD = m_NewMoonJD[i];
			info.nextJD = m_NewMoonJD[i + 1];
			info.mdays = int(info.nextJD + 0.5) - int(info.shuoJD + 0.5);
			info.leap = 0;

			CChnMonthInfo cm(&info);
			m_ChnMonthInfo.push_back(cm);

			yuejian++;
		}

		return (m_ChnMonthInfo.size() == (NEW_MOON_CALC_COUNT - 1));
	}

	// 根据节气计算是否有闰月，如果有闰月，根据农历月命名规则，调整月名称
	void CalcLeapChnMonth()
	{
		assert(m_ChnMonthInfo.size() > 0); // 阴历月的初始化必须在这个之前

		int i;
		// 这里原来的代码是if(int(m_NewMoonJD[13] + 0.5) < int(m_SolarTermsJD[24] + 0.5))，有问题
		if (int(m_NewMoonJD[13] + 0.5) <= int(m_SolarTermsJD[24] + 0.5)) // 第13月的月末没有超过冬至，说明今年需要闰一个月
		{
			// 找到第一个没有中气的月
			i = 1;
			while (i < (NEW_MOON_CALC_COUNT - 1))
			{
				// 这块原来的代码是if(m_NewMoonJD[i + 1] < m_SolarTermsJD[2 * i])，有问题
				// m_NewMoonJD[i + 1]是第i个农历月的下一个月的月首，本该属于第i个月的中气如果比下一个月
				//   的月首还晚，或者与下个月的月首是同一天（民间历法），则说明第i个月没有中气
				if (int(m_NewMoonJD[i + 1] + 0.5) <= int(m_SolarTermsJD[2 * i] + 0.5))
					break;
				i++;
			}
			if (i < (NEW_MOON_CALC_COUNT - 1)) // 找到闰月，对后面的农历月调整月名
			{
				m_ChnMonthInfo[i].SetLeapMonth(true);
				while (i < (NEW_MOON_CALC_COUNT - 1))
					m_ChnMonthInfo[i++].ReIndexMonthName();
			}
		}
	}

	bool BuildMonthAllDaysInfo(CMonthInfo &mi)
	{
		DAY_INFO info;
		int j = 0;
		for (j = 0; j < mi.GetDaysCount(); j++)
		{
			double today = CalculateJulianDay(m_year, mi.GetMonthIndex(), j + 1, 0, 0, 1);
			info.dayNo = j + 1;
			info.week = (mi.GetFirstDayWeek() + j) % DAYS_FOR_WEEK;
			CChnMonthInfo &cm = FindChnMonthInfo(today);
			info.mmonth = cm.GetMonthIndex();
			info.mdayNo = int(today + 0.5) - int(cm.GetNewMoonDayJD() + 0.5);
			info.st = GetSolarTermsName(m_SolarTermsJD, 25, today);

			CDayInfo di(&info);
			mi.AddSingleDay(di);
		}

		return mi.CheckValidDayCount();
	}

	bool BuildMonthInfo(int month)
	{
		MONTH_INFO info;
		info.month = month;
		info.first_week = ZellerWeek(m_year, info.month, 1);
		info.days = GetDaysOfMonth(m_year, info.month);
		CMonthInfo mi(&info);

		if (BuildMonthAllDaysInfo(mi))
		{
			m_MonthInfo.push_back(mi);

			return true;
		}

		return false;
	}

	// 找到today所在的阴历月
	CChnMonthInfo &FindChnMonthInfo(double todayJD)
	{
		assert(m_ChnMonthInfo.size() > 0);

		int this_day = int(todayJD + 0.5);

		int k = 0;
		while (k < (int)m_ChnMonthInfo.size())
		{
			int last_day = int(m_ChnMonthInfo[k].GetNewMoonDayJD() + 0.5);
			int next_day = int(m_ChnMonthInfo[k].GetNextNewMoonDayJD() + 0.5);

			if ((this_day >= last_day) && (this_day < next_day))
				return m_ChnMonthInfo[k];

			k++;
		}

		return m_ChnMonthInfo[0]; // 异常出现时，总是返回第一个月信息
	}

	bool BuildAllMonthInfo()
	{
		assert(m_ChnMonthInfo.size() > 0); // 阴历月的初始化必须在这个之前

		for (int i = 0; i < MONTHES_FOR_YEAR; i++)
			if (!BuildMonthInfo(i + 1))
				return false;
		return true;
	}
};

// WzCalendarCtrl
const int CALENDAR_WEEKBANNER_HEIGHT = 18;
const int CALENDAR_YEARBANNER_HEIGHT = 22;
const int CALENDAR_HEADER_HEIGHT = CALENDAR_WEEKBANNER_HEIGHT + CALENDAR_YEARBANNER_HEIGHT;
const int CALENDAR_NUMBER_HEIGHT = 30;
class CWzCalendarCtrl
{
protected:
	int m_curMonth;
	CChineseCalendar m_calendar;

public:
	void SetGeriYear(int year)
	{
		if (m_calendar.SetGeriYear(year))
			SetCurrentMonth(1);
	}

	void SetCurrentMonth(int month)
	{
		if (!m_calendar.IsCalendarReady())
			return;

		if ((month >= 1) && (month <= MONTHES_FOR_YEAR))
			m_curMonth = month;
	}

	void NextMonth()
	{
		if (!m_calendar.IsCalendarReady())
			return;

		if (m_curMonth < 12)
			m_curMonth++;
	}

	void LastMonth()
	{
		if (!m_calendar.IsCalendarReady())
			return;

		if (m_curMonth > 1)
			m_curMonth--;
	}

protected:
	void DrawCellDateInfo(const CDayInfo &di)
	{
		string strText;
		// strText.Format(_T("%d"), di.GetDayDate());
		bool weekend = ((di.GetDayWeek() % DAYS_FOR_WEEK) == 0) || ((di.GetDayWeek() % DAYS_FOR_WEEK) == 6);
	}
	void DrawCellChnInfo(const CDayInfo &di)
	{
		string strText;

		CChnMonthInfo &cmi = m_calendar.GetChnMonthInfo(di.GetDayChnMonth());
		if (di.GetDayChnDate() == 0) // 每月的第一天？
		{
			if (cmi.GetMonthName() == 1)
			{
				strText += "春节";
			}
			else
			{
				if (cmi.IsLeapMonth())
					strText += "闰";

				strText += nameOfChnMonth[cmi.GetMonthName() - 1];
				strText += (cmi.GetMonthDays() == CHINESE_L_MONTH_DAYS) ? "月大" : "月小";
			}
		}
		else
		{
			if (di.GetDaySolarTerm() != -1)
			{
				strText += nameOfJieQi[di.GetDaySolarTerm()];
			}
			else
			{
				strText += nameOfChnDay[di.GetDayChnDate()];
			}
		}
	}
	void DrawAllBodyCells(const CMonthInfo &mi)
	{
		CalculateBodyRow(mi);

		for (int i = 1; i <= mi.GetDaysCount(); i++)
		{
			const CDayInfo &dayInfo = mi.GetDayInfo(i);
			dayInfo.GetDayWeek();
			dayInfo.GetDayWeek() == 6;
		}
	}
	void DrawCalendarBody()

	{
		if (m_calendar.IsCalendarReady())
		{
			CMonthInfo &mi = m_calendar.GetMonthInfo(m_curMonth);
			DrawAllBodyCells(mi);
		}
	}

	int CalculateBodyRow(const CMonthInfo &mi)
	{
		int days = mi.GetDaysCount();
		int firstRowDays = DAYS_FOR_WEEK - mi.GetFirstDayWeek();

		int rows = 1 + (days - firstRowDays) / DAYS_FOR_WEEK;
		if (((days - firstRowDays) % DAYS_FOR_WEEK) != 0)
			rows++;

		return rows;
	}

	void MakeYearBannerText(string &text)
	{
		int stems, branchs, sbMonth, sbDay;

		int year = m_calendar.GetCurrentYear();
		m_calendar.GetSpringBeginsDay(&sbMonth, &sbDay);
		// 过了立春，才是新的农历年开始
		CalculateStemsBranches((m_curMonth >= sbMonth) ? year : year - 1, &stems, &branchs);

		// text.Format(_T("%4d年%02d月, 农历【%s%s】%s年"),year, m_curMonth, nameOfStems[stems - 1], nameOfBranches[branchs - 1], nameOfShengXiao[branchs - 1]);
	}
};

// calendar2Dlg
//  从某一年的某个节气开始，连续计算25个节气，返回各节气的儒略日，本地时间
void GetAllSolarTermsJD(int year, double *SolarTerms, int start)
{
	int i = 0;
	int st = start;
	while (i < 25)
	{
		double jd = CalculateSolarTerms(year, st * 15);
		SolarTerms[i++] = JDTDtoLocalTime(jd);
		if (st == WINTER_SOLSTICE)
			year++;
		st = (st + 1) % SOLAR_TERMS_COUNT;
	}
}
void PrintJDTimes(double *jds, int count)
{
	char timeStr[128];
	printf("\n");
	for (int i = 0; i < count; i++)
	{
		JDtoString(jds[i], timeStr, 128);
		printf("%f, %s\n", jds[i], timeStr);
	}
	printf("\n");
}
void Test()
{
	int this_year = 2011;
	double solarTerms[25];
	GetAllSolarTermsJD(this_year - 1, solarTerms, WINTER_SOLSTICE);
	PrintJDTimes(solarTerms, 25);
}

int main()
{
	/*
	CWzCalendarCtrl m_CalendarCtrl;
	m_CalendarCtrl.SetGeriYear(2015);
	m_CalendarCtrl.LastMonth();
	m_CalendarCtrl.NextMonth();
	*/

	return 0;
}