#pragma once

#include <stdint.h>
#include <stdio.h>

#define SIZE_BMP_FILE_HEADER 14
#define SIZE_BMP_INFO_HEADER 40

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

typedef struct _BmpRgb
{
    uint8_t rgbBlue;
    uint8_t rgbGreen;
    uint8_t rgbRed;
} BmpRgb;

typedef struct _BmpBuffer
{
    BmpFileHeader bmpFileHeader;
    BmpInfoHeader bmpInfoHeader;
    FILE* fp;
    uint8_t* data;
} BmpBuffer;

int CreateBmp(BmpBuffer* buffer, const char* fileName, int32_t w, int32_t h);
int OpenBmp(BmpBuffer* buffer, const char* fileName);
int CloseBmp(BmpBuffer* buffer);

int WriteBmp(BmpBuffer* buffer);
int SetPixel(BmpBuffer* buffer, BmpRgb* rgb, int32_t w, int32_t h);
int GetPixel(BmpBuffer* buffer, BmpRgb* rgb, int32_t w, int32_t h);
int SetPixelGrayscale(BmpBuffer* buffer, uint8_t grayscale, int32_t w, int32_t h);