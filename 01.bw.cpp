#include <conio.h>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

#undef UNICODE

#include "graphics.h"
#include "FreeImage.h"

class EasyXEnv
{
public:
	EasyXEnv(int width, int height, int flags = 0)
	{
		m_hWnd = initgraph(width, height, flags);
	}
	virtual ~EasyXEnv()
	{
		closegraph();
	}
	HWND GetGraphHwnd()
	{
		return m_hWnd;
	}

protected:
	HWND m_hWnd;
};

bool InitBwBinPalette(FIBITMAP *bmp)
{
	if (FreeImage_GetBPP(bmp) != 1)
		return false;

	RGBQUAD *pal = FreeImage_GetPalette(bmp);
	pal[0].rgbRed = 0;
	pal[0].rgbGreen = 0;
	pal[0].rgbBlue = 0;
	pal[1].rgbRed = 255;
	pal[1].rgbGreen = 255;
	pal[1].rgbBlue = 255;

	return true;
}

bool ConvertGrayScaleToBW(FIBITMAP *gray_bmp, FIBITMAP *bw_bmp, BYTE lvalue)
{
	int gray_width = FreeImage_GetWidth(gray_bmp);
	int gray_height = FreeImage_GetHeight(gray_bmp);
	int bw_width = FreeImage_GetWidth(bw_bmp);
	int bw_height = FreeImage_GetHeight(bw_bmp);

	if ((bw_width != gray_width) || (bw_height != gray_height))
		return false;

	if (FreeImage_GetBPP(bw_bmp) != 1)
		return false;

	BYTE cidx;
	BYTE didx;
	for (int y = 0; y < gray_height; y++)
	{
		for (int x = 0; x < gray_width; x++)
		{
			FreeImage_GetPixelIndex(gray_bmp, x, y, &cidx);
			didx = (cidx >= lvalue) ? 1 : 0;
			FreeImage_SetPixelIndex(bw_bmp, x, y, &didx);
		}
	}
	return true;
}
void SaveBwBinFile(FIBITMAP *graybmp, const char *filename, BYTE lvalue)
{
	int width = FreeImage_GetWidth(graybmp);
	int height = FreeImage_GetHeight(graybmp);

	FIBITMAP *bwbmp = FreeImage_Allocate(width, height, 1);
	InitBwBinPalette(bwbmp);
	ConvertGrayScaleToBW(graybmp, bwbmp, lvalue);
	FreeImage_Save(FIF_BMP, bwbmp, filename, 0);
	FreeImage_Unload(bwbmp);
}

bool ConvertGrayScaleToBWWellner(FIBITMAP *gray_bmp, FIBITMAP *bw_bmp, double t)
{
	int gray_width = FreeImage_GetWidth(gray_bmp);
	int gray_height = FreeImage_GetHeight(gray_bmp);
	double factor = (100.0 - t) / 100.0;
	int s = gray_width / 8; // gray_width >> 3
	double gn = 127.0 * s;

	vector<double> prev_gn(gray_width);
	fill_n(prev_gn.begin(), gray_width, 127.0 * s); // 上一行的 gn，初始化为平均值 127

	BYTE pn;
	BYTE tn;
	for (int y = 0; y < gray_height; y++)
	{
		for (int x = 0; x < gray_width; x++) // 处理奇数行，从左向右
		{
			FreeImage_GetPixelIndex(gray_bmp, x, y, &pn);
			gn = pn + (1.0 - 1.0 / s) * gn; // 利用g(n) = pn + (1- 1/s) * g(n-1) 计算 g(n)
			tn = ((double)pn >= factor * (gn + prev_gn[x]) / 2.0 / s) ? 1 : 0;
			FreeImage_SetPixelIndex(bw_bmp, x, y, &tn);
			prev_gn[x] = gn;
		}

		y++;				  // 转到下一行，处理偶数行
		if (y == gray_height) // 没有下一行了，结束
			break;

		for (int x = gray_width - 1; x >= 0; x--)
		{
			FreeImage_GetPixelIndex(gray_bmp, x, y, &pn);
			gn = pn + (1.0 - 1.0 / s) * gn; // 利用g(n) = pn + (1- 1/s) * g(n-1) 计算 g(n)
			tn = ((double)pn >= factor * (gn + prev_gn[x]) / 2.0 / s) ? 1 : 0;
			FreeImage_SetPixelIndex(bw_bmp, x, y, &tn);
			prev_gn[x] = gn;
		}
	}

	return true;
}
void SaveBwBinFileWellner(FIBITMAP *graybmp, const char *filename)
{
	int width = FreeImage_GetWidth(graybmp);
	int height = FreeImage_GetHeight(graybmp);

	FIBITMAP *bwbmp = FreeImage_Allocate(width, height, 1);
	InitBwBinPalette(bwbmp);
	ConvertGrayScaleToBWWellner(graybmp, bwbmp, 15);
	FreeImage_Save(FIF_BMP, bwbmp, filename, 0);
	FreeImage_Unload(bwbmp);
}

unsigned long long *CalcIntegralImages(FIBITMAP *graybmp)
{
	int width = FreeImage_GetWidth(graybmp);
	int height = FreeImage_GetHeight(graybmp);

	unsigned long long *ivals = new (nothrow) unsigned long long[width * height];
	if (ivals == nullptr)
		return nullptr;
	memset(ivals, 0, width * height * sizeof(unsigned long long));

	BYTE cidx;
	for (int x = 0; x < width; x++)
	{
		unsigned long long sum = 0;
		for (int y = 0; y < height; y++)
		{
			FreeImage_GetPixelIndex(graybmp, x, y, &cidx);
			sum += cidx;
			if (x == 0)
				ivals[x + y * width] = sum;
			else
				ivals[x + y * width] = ivals[x - 1 + y * width] + sum;
		}
	}

	return ivals;
}
bool ConvertGrayScaleToBWBradleyRoth(FIBITMAP *gray_bmp, FIBITMAP *bw_bmp, double t)
{
	int width = FreeImage_GetWidth(gray_bmp);
	int height = FreeImage_GetHeight(gray_bmp);
	double factor = (100.0 - t) / 100.0;
	int s = width / 8; // width >> 3

	unsigned long long *ivals = CalcIntegralImages(gray_bmp); // 计算图像积分矩阵
	if (ivals == nullptr)
		return false;

	BYTE pn;
	BYTE tn;
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			// 计算(x,y) 的包围范围 (x1,y1]-(x2,y2]
			int x1 = x - s / 2;
			int y1 = y - s / 2;
			int x2 = x + s / 2;
			int y2 = y + s / 2;
			x1 = (x1 < 0) ? 0 : x1; // 修正边界
			y1 = (y1 < 0) ? 0 : y1;
			x2 = (x2 >= width) ? (width - 1) : x2;
			y2 = (y2 >= height) ? (height - 1) : y2;
			int count = (x2 - x1) * (y2 - y1); // 计算包围范围内实际像素点的个数，不包括 x1 和 y1 所在的行和列
			// 公式翻译：I(x2,y2)-I(x2,y1-1)-I(x1-1,y2)+I(x1-1,y1-1)
			unsigned long long sum = ivals[x2 + y2 * width] - ivals[x2 + y1 * width] - ivals[x1 + y2 * width] + ivals[x1 + y1 * width];

			FreeImage_GetPixelIndex(gray_bmp, x, y, &pn);
			tn = ((double)pn >= (factor * sum / count)) ? 1 : 0;
			FreeImage_SetPixelIndex(bw_bmp, x, y, &tn);
		}
	}

	delete[] ivals;
	return true;
}
void SaveBwBinFileBradleyRoth(FIBITMAP *graybmp, const char *filename)
{
	int width = FreeImage_GetWidth(graybmp);
	int height = FreeImage_GetHeight(graybmp);

	FIBITMAP *bwbmp = FreeImage_Allocate(width, height, 1);
	InitBwBinPalette(bwbmp);
	ConvertGrayScaleToBWBradleyRoth(graybmp, bwbmp, 15);
	FreeImage_Save(FIF_BMP, bwbmp, filename, 0);
	FreeImage_Unload(bwbmp);
}

FIBITMAP *MakeTestGrayImage()
{
	FIBITMAP *graybmp = FreeImage_Allocate(4, 4, 8);

	BYTE cidx = 4;
	FreeImage_SetPixelIndex(graybmp, 0, 0, &cidx);
	cidx = 1;
	FreeImage_SetPixelIndex(graybmp, 1, 0, &cidx);
	cidx = 2;
	FreeImage_SetPixelIndex(graybmp, 2, 0, &cidx);
	cidx = 2;
	FreeImage_SetPixelIndex(graybmp, 3, 0, &cidx);

	cidx = 0;
	FreeImage_SetPixelIndex(graybmp, 0, 1, &cidx);
	cidx = 4;
	FreeImage_SetPixelIndex(graybmp, 1, 1, &cidx);
	cidx = 1;
	FreeImage_SetPixelIndex(graybmp, 2, 1, &cidx);
	cidx = 3;
	FreeImage_SetPixelIndex(graybmp, 3, 1, &cidx);

	cidx = 3;
	FreeImage_SetPixelIndex(graybmp, 0, 2, &cidx);
	cidx = 1;
	FreeImage_SetPixelIndex(graybmp, 1, 2, &cidx);
	cidx = 0;
	FreeImage_SetPixelIndex(graybmp, 2, 2, &cidx);
	cidx = 4;
	FreeImage_SetPixelIndex(graybmp, 3, 2, &cidx);

	cidx = 2;
	FreeImage_SetPixelIndex(graybmp, 0, 3, &cidx);
	cidx = 1;
	FreeImage_SetPixelIndex(graybmp, 1, 3, &cidx);
	cidx = 3;
	FreeImage_SetPixelIndex(graybmp, 2, 3, &cidx);
	cidx = 2;
	FreeImage_SetPixelIndex(graybmp, 3, 3, &cidx);

	return graybmp;
}
void TestIntegralImages()
{
	FIBITMAP *graybmp = MakeTestGrayImage();
	unsigned long long *intv = CalcIntegralImages(graybmp);
	delete[] intv;
	FreeImage_Unload(graybmp);
}

int main()
{
	const char *src_gray_file = "lena_gray3.jpg";
	const char *src_bw_file = "lena_bw.bmp";
	const char *src_bw_file2 = "lena_bw2.bmp";
	const char *src_bw_file3 = "lena_bw3.bmp";

	EasyXEnv init(1536, 512);

	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(src_gray_file);
	if (fif != FIF_UNKNOWN)
	{
		FIBITMAP *graybmp = FreeImage_Load(fif, src_gray_file, 0);
		SaveBwBinFile(graybmp, src_bw_file, 120);
		SaveBwBinFileWellner(graybmp, src_bw_file2);
		SaveBwBinFileBradleyRoth(graybmp, src_bw_file3);
		FreeImage_Unload(graybmp);

		settextcolor(RGB(0, 192, 255));
		settextstyle(48, 0, "Consolas");

		IMAGE exImgAvg;
		loadimage(&exImgAvg, src_bw_file);
		putimage(0, 0, &exImgAvg);
		RECT rc1 = {0, 0, 512, 48};
		drawtext("Global Thread: 120", &rc1, DT_CENTER | DT_VCENTER);

		IMAGE exImgWeller;
		loadimage(&exImgWeller, src_bw_file2);
		putimage(512, 0, &exImgWeller);
		RECT rc2 = {512, 0, 1024, 48};
		drawtext("Weller 1993", &rc2, DT_CENTER | DT_VCENTER);

		IMAGE exImgBR;
		loadimage(&exImgBR, src_bw_file3);
		putimage(1024, 0, &exImgBR);
		RECT rc3 = {1024, 0, 1536, 48};
		drawtext("Bradley && Roth ", &rc3, DT_CENTER | DT_VCENTER);

		Sleep(10000);
	}
	return 0;
}

// set name=01.bw&&g++ %name%.cpp libeasyx.a -o %name%.exe -I. -L. -lFreeImage&&%name%.exe
