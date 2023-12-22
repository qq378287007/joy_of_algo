
#include <conio.h>
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <random>
#include <ctime>
using namespace std;

#undef UNICODE

#include "graphics.h"
#include "FreeImage.h"

class CSnow
{
	friend CSnow MakeSnow(int maxWidth);

protected:
	POINT m_pos;
	int m_speed;
	int m_layers;

protected:
	CSnow(int x, int y, int speed, int layers)
	{
		m_pos = {x, y};
		m_speed = speed;
		m_layers = layers;
	}

public:
	CSnow(const CSnow &snow)
	{
		m_pos = snow.m_pos;
		m_speed = snow.m_speed;
		m_layers = snow.m_layers;
	}

	POINT &GetPosition() { return m_pos; }
	const POINT &GetPosition() const { return m_pos; }
	int GetSpeed() const { return m_speed; }
	int GetLayers() const { return m_layers; }
	int DecreaseLayers() { return --m_layers; }

	void UpdatePosition(int dps = 0)
	{
		if (dps == 0)
			m_pos.y += m_speed;
		else
			m_pos.y += dps;
	}
};

CSnow MakeSnow(int maxWidth)
{
	static default_random_engine e(time(nullptr));

	uniform_int_distribution<unsigned int> ypos(0, maxWidth - 1);
	uniform_int_distribution<unsigned int> rspeed(1, 5);
	uniform_int_distribution<unsigned int> rlayers(1, 30);

	int x = ypos(e);
	int speed = rspeed(e);
	int layers = rlayers(e);

	return CSnow(x, 0, speed, layers);
}

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
	for (int y = 0; y < color_height; y++)
	{
		for (int x = 0; x < color_width; x++)
		{
			FreeImage_GetPixelColor(color_bmp, x, y, &color);
			BYTE cidx = calculator(color.rgbRed, color.rgbGreen, color.rgbBlue);
			FreeImage_SetPixelIndex(gray_bmp, x, y, &cidx);
		}
	}
	return true;
}

FIBITMAP *LoadImageFile(const char *file_name)
{
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(file_name);
	if (fif == FIF_UNKNOWN)
		return nullptr;

	return FreeImage_Load(fif, file_name, 0);
}

FIBITMAP *LoadAsGrayImage(const char *color_file)
{
	FIBITMAP *colorbmp = LoadImageFile(color_file);
	if (colorbmp == nullptr)
		return nullptr;

	int width = FreeImage_GetWidth(colorbmp);
	int height = FreeImage_GetHeight(colorbmp);

	FIBITMAP *graybmp = FreeImage_Allocate(width, height, 8);
	if (graybmp != nullptr)
	{
		InitGrayScalePalette(graybmp);
		ConvertColorToGray(colorbmp, graybmp, RGB2GrayWeight);
	}
	FreeImage_Unload(colorbmp);

	return graybmp;
}

FIBITMAP *LaplacianImage(FIBITMAP *grayimg)
{
	int width = FreeImage_GetWidth(grayimg);
	int height = FreeImage_GetHeight(grayimg);

	FIBITMAP *lcimg = FreeImage_Allocate(width, height, 1);
	InitBwBinPalette(lcimg);

	for (int y = 1; y < height - 1; y++)
	{
		BYTE *lastLine = FreeImage_GetScanLine(grayimg, y - 1);
		BYTE *curLine = FreeImage_GetScanLine(grayimg, y);
		BYTE *nextLine = FreeImage_GetScanLine(grayimg, y + 1);
		for (int x = 1; x < width - 1; x++) // 控制范围
		{
			double lxy = lastLine[x - 1] + 4 * curLine[x - 1] + nextLine[x - 1] + 4 * lastLine[x] - 20 * curLine[x] + 4 * nextLine[x] + lastLine[x + 1] + 4 * curLine[x + 1] + nextLine[x + 1];

			BYTE didx;
			if (abs(lxy) > 216)
				didx = 1;
			else
				didx = 0;

			FreeImage_SetPixelIndex(lcimg, x, y, &didx);
		}
	}

	return lcimg;
}

void GenerateSnows(int count, int maxWidth, list<CSnow> &snows)
{
	for (int i = 0; i < count; i++)
		snows.push_back(MakeSnow(maxWidth));
}

void DrawSnowOnDevice(const CSnow &sn, int width, int height)
{
	POINT pos = sn.GetPosition();
	putpixel(pos.x, pos.y, RGB(255, 255, 255));

	if ((pos.x - 1) >= 0)
		putpixel(pos.x - 1, pos.y, RGB(255, 255, 255));
	if ((pos.y - 1) >= 0)
		putpixel(pos.x, pos.y - 1, RGB(255, 255, 255));
	if ((pos.x + 1) < width)
		putpixel(pos.x + 1, pos.y, RGB(255, 255, 255));
	if ((pos.y + 1) < height)
		putpixel(pos.x, pos.y + 1, RGB(255, 255, 255));
}

void DrawSnowOnImage(IMAGE *img, const CSnow &sn, int width, int height)
{
	POINT pos = sn.GetPosition();
	DWORD *pMem = GetImageBuffer(img);
	pMem[pos.x + pos.y * width] = BGR(RGB(255, 255, 255));

	if ((pos.x - 1) >= 0)
		pMem[pos.x - 1 + pos.y * width] = BGR(RGB(255, 255, 255));
	if ((pos.y - 1) >= 0)
		pMem[pos.x + (pos.y - 1) * width] = BGR(RGB(255, 255, 255));
	if ((pos.x + 1) < width)
		pMem[pos.x + 1 + pos.y * width] = BGR(RGB(255, 255, 255));
	if ((pos.y + 1) < height)
		pMem[pos.x + (pos.y + 1) * width] = BGR(RGB(255, 255, 255));
}

bool CheckEdgeStop(FIBITMAP *lcImg, int x, int y, int ds, int width, int height)
{
	int ny = y + ds;
	if (ny >= height)
		return false;

	BYTE cidx;
	FreeImage_GetPixelIndex(lcImg, x, height - ny - 1, &cidx);
	return cidx == 1;
}

void UpdateAndDrawSnows(IMAGE *img, FIBITMAP *lcImg, list<CSnow> &snows)
{
	int width = img->getwidth();
	int height = img->getheight();

	list<CSnow>::iterator it = snows.begin();
	while (it != snows.end())
	{
		POINT pos = it->GetPosition();
		int speed = it->GetSpeed();
		bool bstop = CheckEdgeStop(lcImg, pos.x, pos.y, speed, width, height);
		if (bstop)
		{
			int layers = it->DecreaseLayers();
			if (layers <= 0)
			{
				it->UpdatePosition();
				DrawSnowOnImage(img, *it, width, height);
				it = snows.erase(it);
			}
			else
			{
				it->UpdatePosition();
				if (it->GetPosition().y >= height)
					it = snows.erase(it);
				else
					it++;
			}
		}
		else
		{
			it->UpdatePosition();
			if (it->GetPosition().y >= height)
				it = snows.erase(it);
			else
				it++;
		}
	}

	putimage(0, 0, img);
	for (const CSnow &sn : snows)
		DrawSnowOnDevice(sn, width, height);
}

int main()
{
	const char *src_sc_file = "lake.jpg";

	EasyXEnv init(1152, 720);
	BeginBatchDraw();

	FIBITMAP *lena_gray = LoadAsGrayImage(src_sc_file);
	FIBITMAP *lcImg = LaplacianImage(lena_gray);
	FreeImage_Unload(lena_gray);

	IMAGE exImgSc;
	loadimage(&exImgSc, src_sc_file);

	list<CSnow> snows;
	unsigned int count = 0;
	while (true)
	{
		if ((count++ % 5) == 0)
		{
			list<CSnow> newsn;
			GenerateSnows(16, exImgSc.getwidth(), newsn);
			snows.insert(snows.end(), newsn.begin(), newsn.end());
		}
		UpdateAndDrawSnows(&exImgSc, lcImg, snows);
		FlushBatchDraw();
		Sleep(20);
	}
	FreeImage_Unload(lcImg);
	return 0;
}

// set name=01.snow&&g++ %name%.cpp libeasyx.a -o %name%.exe -I. -L. -lFreeImage&&%name%.exe