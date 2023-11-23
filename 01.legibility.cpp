#include <iostream>
#include <algorithm>
#include <cmath>
using namespace std;

#undef UNICODE

#include "FreeImage.h"

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

void SaveGrayScaleFile(FIBITMAP *bmp, const char *filename, BYTE (*calculator)(BYTE r, BYTE g, BYTE b))
{
	int width = FreeImage_GetWidth(bmp);
	int height = FreeImage_GetHeight(bmp);

	FIBITMAP *graybmp = FreeImage_Allocate(width, height, 8);
	InitGrayScalePalette(graybmp);
	ConvertColorToGray(bmp, graybmp, calculator);
	FreeImage_Save(FIF_BMP, graybmp, filename, 0);
	FreeImage_Unload(graybmp);
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

double Brenner(FIBITMAP *lenabmp)
{
	double result = 0.0f;
	int width = FreeImage_GetWidth(lenabmp);
	int height = FreeImage_GetHeight(lenabmp);
	BYTE cidx, nidx;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width - 2; x++) // 控制范围
		{
			FreeImage_GetPixelIndex(lenabmp, x, y, &cidx);	   // 当前像素点
			FreeImage_GetPixelIndex(lenabmp, x + 2, y, &nidx); // 隔一个像素点

			result += pow(nidx - cidx, 2); // 计算差平方，并累加到 result
		}
	}

	return result / (height * double(width - 2)); // 计算平均值
}

/*
double Brenner(FIBITMAP* lenabmp)
{
	double result = 0.0f;
	int width = FreeImage_GetWidth(lenabmp);
	int height = FreeImage_GetHeight(lenabmp);
	for (int y = 0; y < height; y++)
	{
		BYTE *scanLine = FreeImage_GetScanLine(lenabmp, y);
		for (int x = 0; x < width - 2; x++) //控制范围
			result += pow(scanLine[x + 2] - scanLine[x], 2); //计算差平方，并累加到 result
	}
	return result / (height * double(width - 2)); //计算平均值
}
*/

/*
double Eva(FIBITMAP* lenabmp)
{
	double result = 0.0f;
	int width = FreeImage_GetWidth(lenabmp);
	int height = FreeImage_GetHeight(lenabmp);
	for (int y = 1; y < height - 1; y++)
	{
		for (int x = 1; x < width - 1; x++) //控制范围
		{
			BYTE ltidx, tidx, rtidx, lidx, cidx, ridx, lbidx, bidx, rbidx;
			FreeImage_GetPixelIndex(lenabmp, x - 1, y - 1, &ltidx); //Left, Top 点
			FreeImage_GetPixelIndex(lenabmp, x, y - 1, &tidx); //Top 点
			FreeImage_GetPixelIndex(lenabmp, x + 1, y - 1, &rtidx); //Right, Top 点

			FreeImage_GetPixelIndex(lenabmp, x - 1, y, &lidx); //Left 点
			FreeImage_GetPixelIndex(lenabmp, x, y, &cidx); //This 点
			FreeImage_GetPixelIndex(lenabmp, x + 1, y, &ridx); //Right 点

			FreeImage_GetPixelIndex(lenabmp, x - 1, y + 1, &lbidx); //Left, Bottom 点
			FreeImage_GetPixelIndex(lenabmp, x, y + 1, &bidx); //Bottom 点
			FreeImage_GetPixelIndex(lenabmp, x + 1, y + 1, &rbidx); //Right, Bottom 点

			result += (abs(ltidx - cidx) * 0.7 + abs(tidx - cidx) + abs(rtidx - cidx) * 0.7
					+ abs(lidx - cidx) + abs(ridx - cidx)
					+ abs(lbidx - cidx) * 0.7 + abs(bidx - cidx) + abs(rbidx - cidx) * 0.7);  //带权重计算 8 个点
		}
	}
	return result / (double(height - 2) * double(width - 2)); //计算平均值
}
*/

double Eva(FIBITMAP *lenabmp)
{
	double result = 0.0f;
	int width = FreeImage_GetWidth(lenabmp);
	int height = FreeImage_GetHeight(lenabmp);
	for (int y = 1; y < height - 1; y++)
	{
		BYTE *lastLine = FreeImage_GetScanLine(lenabmp, y - 1);
		BYTE *curLine = FreeImage_GetScanLine(lenabmp, y);
		BYTE *nextLine = FreeImage_GetScanLine(lenabmp, y + 1);
		for (int x = 1; x < width - 1; x++) // 控制范围
			result += (abs(lastLine[x - 1] - curLine[x]) * 0.7 + abs(lastLine[x] - curLine[x]) + abs(lastLine[x + 1] - curLine[x]) * 0.7 + abs(curLine[x - 1] - curLine[x]) + abs(curLine[x + 1] - curLine[x]) + abs(nextLine[x - 1] - curLine[x]) * 0.7 + abs(nextLine[x] - curLine[x]) + abs(nextLine[x + 1] - curLine[x]) * 0.7); // 带权重计算 8 个点
	}

	return result / (double(height - 2) * double(width - 2)); // 计算平均值
}

double Tenengrad(FIBITMAP *lenabmp)
{
	double result = 0.0f;
	int width = FreeImage_GetWidth(lenabmp);
	int height = FreeImage_GetHeight(lenabmp);
	for (int y = 1; y < height - 1; y++)
	{
		BYTE *lastLine = FreeImage_GetScanLine(lenabmp, y - 1);
		BYTE *curLine = FreeImage_GetScanLine(lenabmp, y);
		BYTE *nextLine = FreeImage_GetScanLine(lenabmp, y + 1);
		for (int x = 1; x < width - 1; x++) // 控制范围
		{
			double gx = (lastLine[x + 1] + 2 * curLine[x + 1] + nextLine[x + 1]) - (lastLine[x - 1] + 2 * curLine[x - 1] + nextLine[x - 1]);
			double gy = (lastLine[x - 1] + 2 * lastLine[x] + lastLine[x + 1]) - (nextLine[x - 1] + 2 * nextLine[x] + nextLine[x + 1]);
			result += sqrt(gx * gx + gy * gy);
		}
	}

	return result / (double(height - 2) * double(width - 2)); // 计算平均值
}

double Laplacian(FIBITMAP *lenabmp)
{
	double result = 0.0f;
	int width = FreeImage_GetWidth(lenabmp);
	int height = FreeImage_GetHeight(lenabmp);
	for (int y = 1; y < height - 1; y++)
	{
		BYTE *lastLine = FreeImage_GetScanLine(lenabmp, y - 1);
		BYTE *curLine = FreeImage_GetScanLine(lenabmp, y);
		BYTE *nextLine = FreeImage_GetScanLine(lenabmp, y + 1);
		for (int x = 1; x < width - 1; x++) // 控制范围
		{
			double lxy = lastLine[x - 1] + 4 * curLine[x - 1] + nextLine[x - 1] + 4 * lastLine[x] - 20 * curLine[x] + 4 * nextLine[x] + lastLine[x + 1] + 4 * curLine[x + 1] + nextLine[x + 1];

			result += abs(lxy) / 6.0;
		}
	}

	return result / (double(height - 2) * double(width - 2)); // 计算平均值
}

int main()
{
	const char *src_lena_file = "lena.jpg";
	const char *src_blur_file = "lena_blur.jpg";
	const char *src_blur2_file = "lena_blur2.jpg";

	FIBITMAP *lenabmp = LoadAsGrayImage(src_lena_file);
	double c1 = Brenner(lenabmp);
	c1 = Eva(lenabmp);
	c1 = Tenengrad(lenabmp);
	c1 = Laplacian(lenabmp);
	FreeImage_Unload(lenabmp);

	FIBITMAP *blurbmp = LoadAsGrayImage(src_blur_file);
	double c2 = Brenner(blurbmp);
	c2 = Eva(blurbmp);
	c2 = Tenengrad(blurbmp);
	c2 = Laplacian(blurbmp);
	FreeImage_Unload(blurbmp);

	FIBITMAP *blur2bmp = LoadAsGrayImage(src_blur2_file);
	double c3 = Brenner(blur2bmp);
	c3 = Eva(blur2bmp);
	c3 = Tenengrad(blur2bmp);
	c3 = Laplacian(blur2bmp);
	FreeImage_Unload(blur2bmp);

	return 0;
}

// set name=01.legibility&&g++ %name%.cpp libeasyx.a -o %name%.exe -I. -L. -lFreeImage&&%name%.exe
