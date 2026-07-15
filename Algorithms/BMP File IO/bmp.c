#include "bmp.h"

#include <assert.h>
#include <stdlib.h>

int TryCreateBmp(Bmp* outBmp, const char* path, int32_t w, int32_t h)
{
    // NOTE: https://medium.com/@chiaracoetzee/maximum-resolution-of-bmp-image-file-8c729b3f833a
    assert(w > 0 && w < 23170);
    assert(h > 0 && h < 23170);

    BmpHeader header;

    header.file.bfType = 0x4D42;
    header.file.bfSize = 0; // NOTE: The field will set on next step.
    header.file.bfReserved1 = 0;
    header.file.bfReserved2 = 0;
    header.file.bfOffBits = SIZE_BMP_FILE_HEADER + SIZE_BMP_INFO_HEADER;

    header.info.biSize = SIZE_BMP_INFO_HEADER;
    header.info.biWidth = w;
    header.info.biHeight = h;
    header.info.biPlanes = 1;
    header.info.biBitCount = 24;
    header.info.biCompression = 0;
    header.info.biSizeImage = 0; // NOTE: The field will set on next step.
    header.info.biXPelsPerMeter = 100;
    header.info.biYPelsPerMeter = 100;
    header.info.biClrUsed = 0;
    header.info.biClrImportant = 0;

    int32_t wBytes = ((w * 3) + 3) & ~3;
    int32_t hBytes = h;

    header.info.biSizeImage = wBytes * hBytes;
    header.file.bfSize = header.file.bfOffBits + header.info.biSizeImage;

    FILE* fp = fopen(path, "wb+");

    fwrite(&header, sizeof(BmpHeader), 1, fp);
    fseek(fp, header.file.bfSize - 1, SEEK_SET);
    fputc(0, fp);

    outBmp->header = header;
    outBmp->fp = fp;
    outBmp->data = (uint8_t*)calloc(3 * w * h, sizeof(uint8_t));

    return 0;
}

int TryOpenBmp(Bmp* outBmp, const char* path)
{
    BmpHeader header;

    FILE* fp = fopen(path, "rb+");

    fread(&header, sizeof(BmpHeader), 1, fp);

    int w = header.info.biWidth;
    int h = header.info.biHeight;
    int32_t wBytes = ((w * 3) + 3) & ~3;
    int32_t hBytes = h;

    outBmp->header = header;
    outBmp->fp = fp;
    outBmp->data = (uint8_t*)malloc(3 * w * h);

    int32_t padding = wBytes - (w * 3);
    int32_t base = 0;

    for (int y = 0; y < h; ++y)
    {
        fread((outBmp->data + base), sizeof(uint8_t), w * 3, fp);
        fseek(fp, padding, SEEK_CUR);
        base += (w * 3);
    }

    return 0;
}

int TryCloseBmp(Bmp* pBmp)
{
    fclose(pBmp->fp);
    free(pBmp->data);

    pBmp->header = (BmpHeader){ 0 };
    pBmp->fp = NULL;
    pBmp->data = NULL;

    return 0;
}

int SetGrayP(Bmp* pBmp, BmpGray* pGray, int32_t w, int32_t h)
{
    int32_t base = (w + h * pBmp->header.info.biWidth) * 3;
    pBmp->data[base] = pGray->gray;
    pBmp->data[base + 1] = pGray->gray;
    pBmp->data[base + 2] = pGray->gray;

    return 0;
}

int SetGray(Bmp* pBmp, BmpGray gray, int32_t w, int32_t h)
{
    int32_t base = (w + h * pBmp->header.info.biWidth) * 3;
    pBmp->data[base] = gray.gray;
    pBmp->data[base + 1] = gray.gray;
    pBmp->data[base + 2] = gray.gray;

    return 0;
}

int SetGrayByte(Bmp* pBmp, uint8_t gray, int32_t w, int32_t h)
{
    int32_t base = (w + h * pBmp->header.info.biWidth) * 3;
    pBmp->data[base] = gray;
    pBmp->data[base + 1] = gray;
    pBmp->data[base + 2] = gray;

    return 0;
}

int SetRgbP(Bmp* pBmp, BmpRgb* pRgb, int32_t w, int32_t h)
{
    int32_t base = (w + h * pBmp->header.info.biWidth) * 3;
    pBmp->data[base] = pRgb->b;
    pBmp->data[base + 1] = pRgb->g;
    pBmp->data[base + 2] = pRgb->r;

    return 0;
}

int SetRgb(Bmp* pBmp, BmpRgb rgb, int32_t w, int32_t h)
{
    int32_t base = (w + h * pBmp->header.info.biWidth) * 3;
    pBmp->data[base] = rgb.b;
    pBmp->data[base + 1] = rgb.g;
    pBmp->data[base + 2] = rgb.r;

    return 0;
}

int SetRgbByte(Bmp* pBmp, uint8_t r, uint8_t g, uint8_t b, int32_t w, int32_t h)
{
    int32_t base = (w + h * pBmp->header.info.biWidth) * 3;
    pBmp->data[base] = b;
    pBmp->data[base + 1] = g;
    pBmp->data[base + 2] = r;
    
    return 0;
}

int SetR(Bmp* pBmp, uint8_t r, int32_t w, int32_t h)
{
    int32_t base = (w + h * pBmp->header.info.biWidth) * 3;
    pBmp->data[base + 2] = r;

    return 0;
}

int SetG(Bmp* pBmp, uint8_t g, int32_t w, int32_t h)
{
    int32_t base = (w + h * pBmp->header.info.biWidth) * 3;
    pBmp->data[base + 1] = g;

    return 0;
}

int SetB(Bmp* pBmp, uint8_t b, int32_t w, int32_t h)
{
    int32_t base = (w + h * pBmp->header.info.biWidth) * 3;
    pBmp->data[base] = b;

    return 0;
}

int GetR(Bmp* pBmp, uint8_t* outR, int32_t w, int32_t h)
{
    int32_t base = (w + h * pBmp->header.info.biWidth) * 3;
    *outR = pBmp->data[base + 2];

    return 0;
}

int GetG(Bmp* pBmp, uint8_t* outG, int32_t w, int32_t h)
{
    int32_t base = (w + h * pBmp->header.info.biWidth) * 3;
    *outG = pBmp->data[base + 1];
    
    return 0;
}

int GetB(Bmp* pBmp, uint8_t* outB, int32_t w, int32_t h)
{
    int32_t base = (w + h * pBmp->header.info.biWidth) * 3;
    *outB = pBmp->data[base];
    
    return 0;
}

int GetGrayByte(Bmp* pBmp, uint8_t* outGray, int32_t w, int32_t h)
{
    int32_t base = (w + h * pBmp->header.info.biWidth) * 3;
    *outGray = pBmp->data[base + 2];
    
    return 0;
}

int GetGray(Bmp* pBmp, BmpGray* outGray, int32_t w, int32_t h)
{
    int32_t base = (w + h * pBmp->header.info.biWidth) * 3;
    outGray->gray = pBmp->data[base + 2];

    return 0;
}

int GetRgb(Bmp* pBmp, BmpRgb* outRgb, int32_t w, int32_t h)
{
    int32_t base = (w + h * pBmp->header.info.biWidth) * 3;
    outRgb->b = pBmp->data[base];
    outRgb->g = pBmp->data[base + 1];
    outRgb->r = pBmp->data[base + 2];

    return 0;
}

int Commit(Bmp* pBmp)
{
    int w = pBmp->header.info.biWidth;
    int h = pBmp->header.info.biHeight;
    int32_t wBytes = ((w * 3) + 3) & ~3;
    int32_t hBytes = h;
    int32_t padding = wBytes - (w * 3);
    char pad[4] = { 0, 0, 0, 0 };

    fseek(pBmp->fp, pBmp->header.file.bfOffBits, SEEK_SET);
    int32_t base = 0;

    for (int32_t y = 0; y < hBytes; ++y)
    {
        fwrite((pBmp->data + base), sizeof(uint8_t), w * 3, pBmp->fp);
        fwrite(pad, sizeof(uint8_t), padding, pBmp->fp);
        base += (w * 3);
    }

    return 0;
}