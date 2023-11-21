#include <stdio.h>
#include <stdlib.h>

#define bool int
#define false 0
#define true 1

int PcxRle_Encode(unsigned char *inbuf, int inSize, unsigned char *outbuf, int onuBufSize)
{
    unsigned char *src = inbuf;
    int encSize = 0;
    while (src < (inbuf + inSize))
    {
        unsigned char value = *src++;
        int i = 1;
        while ((*src == value) && (i < 63))
        {
            src++;
            i++;
        }

        if ((encSize + i + 1) > onuBufSize) // 输出缓冲区空间不够了
            return -1;

        if (i > 1)
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
    unsigned char *src = inbuf;
    int decSize = 0;
    while (src < (inbuf + inSize))
    {
        unsigned char value = *src++;
        int count = 1;
        if ((value & 0xC0) == 0xC0) // 是否有块属性标志
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

bool IsRepetitionStart(unsigned char *start, int length)
{
    if (length <= 2)
        return false;

    return start[1] == start[0] && start[2] == start[0];
}

// 限制返回长度不超过127
int GetRepetitionCount(unsigned char *start, int length)
{
    if (length <= 1)
        return length;

    unsigned char value = *start;

    int i = 1;
    for (unsigned char *src = start + 1; (src < (start + length)) && (i < 127); src++)
    {
        if (*src != value)
            break;
        i++;
    }

    return i;
}

// 限制返回长度不超过127
int GetNonRepetitionCount(unsigned char *start, int length)
{
    if (length <= 1)
        return length;

    unsigned char value = *start;
    int i = 0;
    for (unsigned char *src = start; (src < (start + length)) && (i < 127); src++)
    {
        if (IsRepetitionStart(src, length - i))
            break;
        i++;
    }
    return i;
}

int Rle_Encode(unsigned char *inbuf, int inSize, unsigned char *outbuf, int onuBufSize)
{
    unsigned char *src = inbuf;
    int encSize = 0;

    int count;
    for (int srcLeft = inSize; srcLeft > 0; srcLeft -= count)
    {
        if (IsRepetitionStart(src, srcLeft)) // 是否连续三个字节数据相同？
        {
            if ((encSize + 2) > onuBufSize) // 输出缓冲区空间不够了
                return -1;

            count = GetRepetitionCount(src, srcLeft);
            outbuf[encSize++] = count | 0x80;
            outbuf[encSize++] = *src;
            src += count;
        }
        else
        {
            count = GetNonRepetitionCount(src, srcLeft);
            if ((encSize + count + 1) > onuBufSize) // 输出缓冲区空间不够了
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
    unsigned char *src = inbuf;
    int decSize = 0;
    while (src < (inbuf + inSize))
    {
        unsigned char sign = *src++;
        int count = sign & 0x3F;
        if ((decSize + count) > onuBufSize) // 输出缓冲区空间不够了
            return -1;

        if ((sign & 0x80) == 0x80) // 连续重复数据标志
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

typedef struct
{
    char manufacturer;
    char version;
    char encoding;
    char bits_per_pixel;
    unsigned short xmin, ymin;
    unsigned short xmax, ymax;
    unsigned short hres;
    unsigned short vres;
    char palette[48];
    char reserved;
    char colour_planes;
    unsigned short bytes_per_line;
    unsigned short palette_type;
    char filler[58];
} PCX_HEAD;

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
            value = value & 0x3F; // count
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

    unsigned char *imgData = NULL;
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
    // unsigned char srcData[] = {'A', 'A', 'A', 'B', 'B', 'B', 'B', 'B', 'C', 'D' };
    unsigned char srcData[] = {'A', 'A', 'A', 'B', 'B', 'B', 'B', 'B', 'C', 'A', 'B', 'C', 'D', 'D', 'D'};
    unsigned char procBuf[64] = {0};
    unsigned char srcBuf[64] = {0};

    // int procLen = Rle_Encode_N(srcData, 15, procBuf, 64);
    // int srcLen = Rle_Decode_N(procBuf, procLen, srcBuf, 64);

    // int procLen = Rle_Encode_P(srcData, 15, procBuf, 64);
    // int srcLen = Rle_Decode_P(procBuf, procLen, srcBuf, 64);

    int procLen = Rle_Encode(srcData, 15, procBuf, 64);
    int srcLen = Rle_Decode(procBuf, procLen, srcBuf, 64);
    return 0;
}
