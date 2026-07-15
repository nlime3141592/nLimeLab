#ifndef __BMP_H__
#define __BMP_H__

#include <stdint.h>
#include <stdio.h>

#define SIZE_BMP_FILE_HEADER 14
#define SIZE_BMP_INFO_HEADER 40

#pragma pack(push, 1)

// 14 bytes
typedef struct _BmpFileHeader
{
    uint16_t bfType; // 0x424D, big endian.
    uint32_t bfSize; // total file size.
    uint16_t bfReserved1; // 0
    uint16_t bfReserved2; // 0
    uint32_t bfOffBits; // first offset of pixel data.
} BmpFileHeader;

// 40 bytes
typedef struct _BmpInfoHeader
{
    uint32_t biSize; // sizeof(BmpInfoHeader)
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes; // 1
    uint16_t biBitCount; // pixel per bits (1, 4, 8, 16, 24, 32)
    uint32_t biCompression; // 0
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed; // 0
    uint32_t biClrImportant; // 0
} BmpInfoHeader;

typedef struct _BmpHeader
{
    BmpFileHeader file;
    BmpInfoHeader info;
} BmpHeader;

#pragma pack(pop)

typedef struct _BmpRgb
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} BmpRgb;

typedef struct _BmpGray
{
    uint8_t gray;
} BmpGray;

typedef struct _Bmp
{
    BmpHeader header;
    FILE* fp;
    uint8_t* data;
} Bmp;

int TryCreateBmp(Bmp* outBmp, const char* path, int32_t w, int32_t h);
int TryOpenBmp(Bmp* outBmp, const char* path);
int TryCloseBmp(Bmp* pBmp);

int SetGrayP(Bmp* pBmp, BmpGray* pGray, int32_t w, int32_t h);
int SetGray(Bmp* pBmp, BmpGray gray, int32_t w, int32_t h);
int SetGrayByte(Bmp* pBmp, uint8_t gray, int32_t w, int32_t h);
int SetRgbP(Bmp* pBmp, BmpRgb* pRgb, int32_t w, int32_t h);
int SetRgb(Bmp* pBmp, BmpRgb rgb, int32_t w, int32_t h);
int SetRgbByte(Bmp* pBmp, uint8_t r, uint8_t g, uint8_t b, int32_t w, int32_t h);

int SetR(Bmp* pBmp, uint8_t r, int32_t w, int32_t h);
int SetG(Bmp* pBmp, uint8_t g, int32_t w, int32_t h);
int SetB(Bmp* pBmp, uint8_t b, int32_t w, int32_t h);

int GetR(Bmp* pBmp, uint8_t* outR, int32_t w, int32_t h);
int GetG(Bmp* pBmp, uint8_t* outG, int32_t w, int32_t h);
int GetB(Bmp* pBmp, uint8_t* outB, int32_t w, int32_t h);
int GetGrayByte(Bmp* pBmp, uint8_t* outGray, int32_t w, int32_t h);
int GetGray(Bmp* pBmp, BmpGray* outGray, int32_t w, int32_t h);
int GetRgb(Bmp* pBmp, BmpRgb* outRgb, int32_t w, int32_t h);

int Commit(Bmp* pBmp);

#endif