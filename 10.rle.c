#include <stdio.h>
#include <stdlib.h>

#define bool int
#define false 0
#define true 1

// 至少3个字符相等
bool IsRepetitionStart(unsigned char *start, int length)
{
    if (length <= 2)
        return false;
    return start[0] == start[1] && start[1] == start[2];
}

// 限制返回长度不超过127
int GetRepetitionCount(unsigned char *start, int length)
{
    if (length <= 1)
        return length;
    else if (length > 127)
        length = 127;

    for (int i = 1; i < length; i++)
        if (start[i] != start[i - 1])
            return i;
    return length;
}

// 限制返回长度不超过127
int GetNonRepetitionCount(unsigned char *start, int length)
{
    if (length <= 2)
        return length;
    else if (length > 127)
        length = 127;

    for (int i = 0; i < length; i++)
        if (IsRepetitionStart(start + i, length - i))
            return i;
    return length;
}

int Rle_Encode(unsigned char *inbuf, int inSize, unsigned char *outbuf, int onuBufSize)
{
    int count;
    unsigned char *src = inbuf;
    int encSize = 0;
    for (int srcLeft = inSize; srcLeft > 0; srcLeft -= count)
    {
        if (IsRepetitionStart(src, srcLeft)) // 是否连续三个字节数据相同？
        {
            if ((encSize + 2) > onuBufSize) // 输出缓冲区空间不够了
                return -1;

            count = GetRepetitionCount(src, srcLeft);
            outbuf[encSize++] = count | 0x80; // 1000 0000
            outbuf[encSize++] = *src;
            src += count;
        }
        else
        {
            count = GetNonRepetitionCount(src, srcLeft);
            if ((encSize + 1 + count) > onuBufSize) // 输出缓冲区空间不够了
                return -1;

            outbuf[encSize++] = count;
            for (int i = 0; i < count; i++) // 逐个复制这些数据
                outbuf[encSize++] = *src++;
        }
    }
    return encSize;
}

int Rle_Decode(unsigned char *inbuf, int inSize, unsigned char *outbuf, int onuBufSize)
{
    int decSize = 0;
    unsigned char *src = inbuf;
    while (src < (inbuf + inSize))
    {
        unsigned char sign = *src++;
        // int count = sign & 0x3F;
        int count = sign & 0x7F;            // 0111 1111
        if ((decSize + count) > onuBufSize) // 输出缓冲区空间不够了
            return -1;

        if ((sign & 0x80) == 0x80) // 连续重复数据标志, 1000 0000
        {
            for (int i = 0; i < count; i++)
                outbuf[decSize++] = *src;
            src++;
        }
        else
        {
            for (int i = 0; i < count; i++)
                outbuf[decSize++] = *src++;
        }
    }
    return decSize;
}

// PCX文件三类：黑白、16色、256色
// 文件头（128字节）、图像数据、256调色板（256色才存在，1字节颜色数+768字节调色板数据）
typedef struct
{
    char manufacturer;         // 0x0a, PCX文件标志
    char version;              // 5开始支持256色
    char encoding;             // 1表示RLE压缩编码
    char bits_per_pixel;       // 256色，不用或者8
    unsigned short xmin, ymin; // 多数情况下为0
    unsigned short xmax, ymax; // xmax-xmin：宽度，ymax-ymin：高度，单位：像素
    unsigned short hres;       // 水平分辨率
    unsigned short vres;       // 垂直分辨率
    char palette[48];          // 彩色调色板，用于16色
    char reserved;
    char colour_planes;
    unsigned short bytes_per_line; // 每一行图像数据需要字节数（直接分配解码缓冲区）
    unsigned short palette_type;
    char filler[58];
} PCX_HEAD;

int PcxRle_Encode(unsigned char *inbuf, int inSize, unsigned char *outbuf, int onuBufSize)
{
    int encSize = 0;
    unsigned char *src = inbuf;
    while (src < (inbuf + inSize))
    {
        unsigned char value = *src++;
        int i = 1;
        while ((*src == value) && (i < 63)) // 00 111111, 63
        {
            src++;
            i++;
        }

        if ((encSize + 1 + i) > onuBufSize) // 输出缓冲区空间不够了
            return -1;

        if (i > 1) // 重复数据至少两个
        {
            outbuf[encSize++] = i | 0xC0;
            outbuf[encSize++] = value;
        }
        else
        {
            // 如果非重复数据最高两位是1，插入标识字节
            if ((value & 0xC0) == 0xC0)
                outbuf[encSize++] = 0xC1;

            outbuf[encSize++] = value;
        }
    }
    return encSize;
}

int PcxRle_Decode(unsigned char *inbuf, int inSize, unsigned char *outbuf, int onuBufSize)
{
    int decSize = 0;
    unsigned char *src = inbuf;
    while (src < (inbuf + inSize))
    {
        unsigned char value = *src++;
        int count = 1;
        if ((value & 0xC0) == 0xC0) // 是否有块属性标志，重复
        {
            count = value & 0x3F; // 低6位是count
            value = *src++;
        }

        if ((decSize + count) > onuBufSize) // 输出缓冲区空间不够了
            return -1;

        for (int i = 0; i < count; i++)
            outbuf[decSize++] = value;
    }
    return decSize;
}

bool GetPcxfileHeader(FILE *fp, PCX_HEAD *header)
{
    return true;
}
void DrawPixel(int x, int y, int colorIdx)
{
}
int DecodePcxLine(PCX_HEAD *header, unsigned char *imgData, unsigned char *lineBuf)
{
    int offset = 0;
    for (int i = 0; i < header->bytes_per_line;)
    {
        unsigned char value = imgData[offset++];
        if ((value & 0xc0) == 0xc0) // 判断标志
        {
            value = value & 0x3F; // count, 0011 1111
            for (int repeat = 0; repeat < value; repeat++)
                lineBuf[i++] = imgData[offset];
            offset++;
        }
        else
        {
            lineBuf[i++] = value;
        }
    }
    return offset;
}
void ShowPcxFile(FILE *fp)
{
    PCX_HEAD header = {0};
    if (!GetPcxfileHeader(fp, &header))
        return;

    unsigned char *imgData = NULL;// (unsigned char *)malloc(sizeof(unsigned char) * (pcx文件字节数-128-769));
    //从PCX文件中读取图像数据

    unsigned char *bitsLine = (unsigned char *)malloc(sizeof(unsigned char) * header.bytes_per_line);
    int height = header.ymax - header.ymin;
    int width = header.xmax - header.xmin;
    int srcIdx = 0;
    for (int y = 0; y < height; y++)
    {
        srcIdx += DecodePcxLine(&header, imgData + srcIdx, bitsLine);
        for (int x = 0; x < width; x++)
            DrawPixel(x, y, bitsLine[x]);
    }
}

int main()
{
#if 0
    unsigned char srcData[] = {'A', 'A', 'A', 'B', 'B', 'B', 'B', 'B', 'C', 'D'};
    // unsigned char srcData[] = {'A', 'A', 'A', 'B', 'B', 'B', 'B', 'B', 'C', 'A', 'B', 'C', 'D', 'D', 'D'};

    unsigned char procBuf[64] = {0};
    unsigned char srcBuf[64] = {0};
    int procLen = Rle_Encode(srcData, sizeof(srcData), procBuf, 64);
    int srcLen = Rle_Decode(procBuf, procLen, srcBuf, 64);

    for (int i = 0; i < 64 && procBuf[i];)
    {
        unsigned char sign = procBuf[i++];
        int count = sign & 0x7F;
        printf("(%d)", count);
        if ((sign & 0x80) == 0x80) // 连续重复数据标志, 1000 0000
        {
            printf("%c, ", procBuf[i++]);
        }
        else
        {
            for (int ii = 0; ii < count; ii++)
                printf("%c, ", procBuf[i++]);
        }
    }
    printf("\n");

    for (int i = 0; i < 64 && srcBuf[i]; i++)
        printf("%c, ", srcBuf[i]);
    printf("\n");
#endif

    return 0;
}
