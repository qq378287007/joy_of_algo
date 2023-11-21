#include <conio.h>
#include <iostream>
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

const double one_third = 1.0 / 3.0;
BYTE RGB2GrayAvg(BYTE r, BYTE g, BYTE b)
{
	return BYTE(r * one_third + g * one_third + b * one_third + 0.5);
}

BYTE RGB2GrayMax(BYTE r, BYTE g, BYTE b)
{
	BYTE rg = r > g ? r : g;
	return b > rg ? b : rg;
}

BYTE RGB2GrayWeight(BYTE r, BYTE g, BYTE b)
{
	return BYTE(r * 0.3 + g * 0.59 + b * 0.11 + 0.5);
}

bool ConvertColorToGray(FIBITMAP *color_bmp, FIBITMAP *gray_bmp, BYTE (*calculator)(BYTE r, BYTE g, BYTE b))
{
	int color_width = FreeImage_GetWidth(color_bmp);
	int color_height = FreeImage_GetHeight(color_bmp);
	int gray_width = FreeImage_GetWidth(gray_bmp);
	int gray_height = FreeImage_GetHeight(gray_bmp);

	if ((color_width != gray_width) || (color_height != gray_height))
		return false;

	RGBQUAD color;
	BYTE cidx;
	for (int y = 0; y < color_height; y++)
	{
		for (int x = 0; x < color_width; x++)
		{
			FreeImage_GetPixelColor(color_bmp, x, y, &color);
			cidx = calculator(color.rgbRed, color.rgbGreen, color.rgbBlue);
			FreeImage_SetPixelIndex(gray_bmp, x, y, &cidx);
		}
	}
	return true;
}

bool InitGrayScalePalette(FIBITMAP *bmp)
{
	if (FreeImage_GetBPP(bmp) != 8)
		return false;

	RGBQUAD *pal = FreeImage_GetPalette(bmp);
	for (int i = 0; i < 256; i++)
	{
		pal[i].rgbRed = i;
		pal[i].rgbGreen = i;
		pal[i].rgbBlue = i;
	}

	return true;
}

void SaveGrayScaleFile(FIBITMAP *bmp, const char *filename, BYTE (*calculator)(BYTE r, BYTE g, BYTE b))
{
	int width = FreeImage_GetWidth(bmp);
	int height = FreeImage_GetHeight(bmp);

	FIBITMAP *graybmp = FreeImage_Allocate(width, height, 8);
	InitGrayScalePalette(graybmp);
	ConvertColorToGray(bmp, graybmp, calculator);
	FreeImage_Save(FIF_JPEG, graybmp, filename, 0);
	FreeImage_Unload(graybmp);
}

int main()
{
	const char *src_color_file = "lena.jpg";
	const char *src_gray_file = "lena_gray.jpg";
	const char *src_gray_file2 = "lena_gray2.jpg";
	const char *src_gray_file3 = "lena_gray3.jpg";
	const char *src_bin_file = "E:\\Alg 2\\resource\\lena_bin.jpg";

	EasyXEnv init(1536, 512);

	const char *pVerString = FreeImage_GetVersion();
	cout << "FreeImage Version: " << pVerString << endl;

	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(src_color_file);
	if (fif != FIF_UNKNOWN)
	{
		FIBITMAP *bmp = FreeImage_Load(fif, src_color_file, 0);
		SaveGrayScaleFile(bmp, src_gray_file, RGB2GrayAvg);
		SaveGrayScaleFile(bmp, src_gray_file2, RGB2GrayMax);
		SaveGrayScaleFile(bmp, src_gray_file3, RGB2GrayWeight);
		FreeImage_Unload(bmp);

		settextcolor(RGB(0, 192, 255));
		settextstyle(48, 0, "Consolas");

		IMAGE exImgAvg;
		loadimage(&exImgAvg, src_gray_file);
		putimage(0, 0, &exImgAvg);
		RECT rc1 = {0, 0, 512, 48};
		drawtext("mean", &rc1, DT_CENTER | DT_VCENTER);

		IMAGE exImgMax;
		loadimage(&exImgMax, src_gray_file2);
		putimage(512, 0, &exImgMax);
		RECT rc2 = {512, 0, 1024, 48};
		drawtext("max", &rc2, DT_CENTER | DT_VCENTER);

		IMAGE exImgWeight;
		loadimage(&exImgWeight, src_gray_file3);
		putimage(1024, 0, &exImgWeight);
		RECT rc3 = {1024, 0, 1536, 48};
		drawtext("weight", &rc3, DT_CENTER | DT_VCENTER);

		Sleep(3000);
	}
	return 0;
}

// set name=01.gray&&g++ %name%.cpp libeasyx.a -o %name%.exe -I. -L. -lFreeImage&&%name%.exe
