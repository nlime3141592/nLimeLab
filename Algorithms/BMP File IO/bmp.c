#include "bmp.h"

#include <assert.h>
#include <stdlib.h>

//
// TODO: 오류 처리가 필요한 경우 수행합니다.
//

static void WriteBmpFileHeader(BmpFileHeader* header, FILE* fp)
{
    fwrite(&header->bfType, sizeof(header->bfType), 1, fp);
    fwrite(&header->bfSize, sizeof(header->bfSize), 1, fp);
    fwrite(&header->bfReserved1, sizeof(header->bfReserved1), 1, fp);
    fwrite(&header->bfReserved2, sizeof(header->bfReserved2), 1, fp);
    fwrite(&header->bfOffBits, sizeof(header->bfOffBits), 1, fp);
}

static void WriteBmpInfoHeader(BmpInfoHeader* header, FILE* fp)
{
    fwrite(&header->biSize, sizeof(header->biSize), 1, fp);
    fwrite(&header->biWidth, sizeof(header->biWidth), 1, fp);
    fwrite(&header->biHeight, sizeof(header->biHeight), 1, fp);
    fwrite(&header->biPlanes, sizeof(header->biPlanes), 1, fp);
    fwrite(&header->biBitCount, sizeof(header->biBitCount), 1, fp);
    fwrite(&header->biCompression, sizeof(header->biCompression), 1, fp);
    fwrite(&header->biSizeImage, sizeof(header->biSizeImage), 1, fp);
    fwrite(&header->biXPelsPerMeter, sizeof(header->biXPelsPerMeter), 1, fp);
    fwrite(&header->biYPelsPerMeter, sizeof(header->biYPelsPerMeter), 1, fp);
    fwrite(&header->biClrUsed, sizeof(header->biClrUsed), 1, fp);
    fwrite(&header->biClrImportant, sizeof(header->biClrImportant), 1, fp);
}

static void ReadBmpFileHeader(BmpFileHeader* header, FILE* fp)
{
    fread(&header->bfType, sizeof(header->bfType), 1, fp);
    fread(&header->bfSize, sizeof(header->bfSize), 1, fp);
    fread(&header->bfReserved1, sizeof(header->bfReserved1), 1, fp);
    fread(&header->bfReserved2, sizeof(header->bfReserved2), 1, fp);
    fread(&header->bfOffBits, sizeof(header->bfOffBits), 1, fp);
}

static void ReadBmpInfoHeader(BmpInfoHeader* header, FILE* fp)
{
    fread(&header->biSize, sizeof(header->biSize), 1, fp);
    fread(&header->biWidth, sizeof(header->biWidth), 1, fp);
    fread(&header->biHeight, sizeof(header->biHeight), 1, fp);
    fread(&header->biPlanes, sizeof(header->biPlanes), 1, fp);
    fread(&header->biBitCount, sizeof(header->biBitCount), 1, fp);
    fread(&header->biCompression, sizeof(header->biCompression), 1, fp);
    fread(&header->biSizeImage, sizeof(header->biSizeImage), 1, fp);
    fread(&header->biXPelsPerMeter, sizeof(header->biXPelsPerMeter), 1, fp);
    fread(&header->biYPelsPerMeter, sizeof(header->biYPelsPerMeter), 1, fp);
    fread(&header->biClrUsed, sizeof(header->biClrUsed), 1, fp);
    fread(&header->biClrImportant, sizeof(header->biClrImportant), 1, fp);
}

int CreateBmp(BmpBuffer* buffer, const char* fileName, int32_t w, int32_t h)
{
    // NOTE: https://medium.com/@chiaracoetzee/maximum-resolution-of-bmp-image-file-8c729b3f833a
    assert(w > 0 && w < 23170);
    assert(h > 0 && h < 23170);

    BmpFileHeader bmpFileHeader;
    BmpInfoHeader bmpInfoHeader;

    bmpFileHeader.bfType = 0x4D42;
    bmpFileHeader.bfSize = 0; // NOTE: The field will set on next step.
    bmpFileHeader.bfReserved1 = 0;
    bmpFileHeader.bfReserved2 = 0;
    bmpFileHeader.bfOffBits = SIZE_BMP_FILE_HEADER + SIZE_BMP_INFO_HEADER;

    bmpInfoHeader.biSize = SIZE_BMP_INFO_HEADER;
    bmpInfoHeader.biWidth = w;
    bmpInfoHeader.biHeight = h;
    bmpInfoHeader.biPlanes = 1;
    bmpInfoHeader.biBitCount = 24;
    bmpInfoHeader.biCompression = 0;
    bmpInfoHeader.biSizeImage = 0; // NOTE: The field will set on next step.
    bmpInfoHeader.biXPelsPerMeter = 100;
    bmpInfoHeader.biYPelsPerMeter = 100;
    bmpInfoHeader.biClrUsed = 0;
    bmpInfoHeader.biClrImportant = 0;

    int32_t wBytes = ((w * 3) & 0xFFFFFFFC) + 4;
    int32_t hBytes = h;

    bmpInfoHeader.biSizeImage = wBytes * hBytes;
    bmpFileHeader.bfSize = bmpFileHeader.bfOffBits + bmpInfoHeader.biSizeImage;

    FILE* fp = fopen(fileName, "wb+");

    WriteBmpFileHeader(&bmpFileHeader, fp);
    WriteBmpInfoHeader(&bmpInfoHeader, fp);

    // initialize file size.
    fpos_t position = bmpFileHeader.bfSize - 1;
    char tmpBuffer = 0;

    int err = fsetpos(fp, &position);

    fwrite(&tmpBuffer, 1, 1, fp);

    buffer->bmpFileHeader = bmpFileHeader;
    buffer->bmpInfoHeader = bmpInfoHeader;
    buffer->fp = fp;
    buffer->data = (uint8_t*)malloc(3 * w * h);

    return 0;
}

int OpenBmp(BmpBuffer* buffer, const char* fileName)
{
    FILE* fp = fopen(fileName, "rb+");

    BmpFileHeader bmpFileHeader;
    BmpInfoHeader bmpInfoHeader;

    ReadBmpFileHeader(&bmpFileHeader, fp);
    ReadBmpInfoHeader(&bmpInfoHeader, fp);

    buffer->bmpFileHeader = bmpFileHeader;
    buffer->bmpInfoHeader = bmpInfoHeader;
    buffer->fp = fp;
    buffer->data = (uint8_t*)malloc(3 * bmpInfoHeader.biWidth * bmpInfoHeader.biHeight);

    return 0;
}

int CloseBmp(BmpBuffer* buffer)
{
    WriteBmp(buffer);

    fclose(buffer->fp);
    free(buffer->data);

    BmpFileHeader bmpFileHeader = { 0 };
    BmpInfoHeader bmpInfoHeader = { 0 };

    buffer->bmpFileHeader = bmpFileHeader;
    buffer->bmpInfoHeader = bmpInfoHeader;
    buffer->fp = NULL;
    buffer->data = NULL;

    return 0;
}

int WriteBmp(BmpBuffer* buffer)
{
    fseek(buffer->fp, 0, SEEK_SET);

    WriteBmpFileHeader(&buffer->bmpFileHeader, buffer->fp);
    WriteBmpInfoHeader(&buffer->bmpInfoHeader, buffer->fp);

    int32_t w = buffer->bmpInfoHeader.biWidth;
    int32_t r = (4 - ((w * 3) & 0x00000003)) % 4;
    int32_t tmpBuffer = 0;

    for (int32_t h = 0; h < buffer->bmpInfoHeader.biHeight; ++h)
    {
        fwrite(&buffer->data[3 * w * h], 3 * w, 1, buffer->fp);
        fwrite(&tmpBuffer, 1, r, buffer->fp);
    }

    return 0;
}

int SetPixel(BmpBuffer* buffer, BmpRgb* rgb, int32_t w, int32_t h)
{
    int32_t i = 3 * (buffer->bmpInfoHeader.biWidth * h + w);

    buffer->data[i] = rgb->rgbBlue;
    buffer->data[i + 1] = rgb->rgbGreen;
    buffer->data[i + 2] = rgb->rgbRed;

    return 0;
}

int GetPixel(BmpBuffer* buffer, BmpRgb* rgb, int32_t w, int32_t h)
{
    int32_t i = 3 * (buffer->bmpInfoHeader.biWidth * h + w);

    rgb->rgbBlue = buffer->data[i];
    rgb->rgbGreen = buffer->data[i + 1];
    rgb->rgbRed = buffer->data[i + 2];

    return 0;
}

int SetPixelGrayscale(BmpBuffer* buffer, uint8_t grayscale, int32_t w, int32_t h)
{
    BmpRgb rgb;

    rgb.rgbBlue = grayscale;
    rgb.rgbGreen = grayscale;
    rgb.rgbRed = grayscale;

    SetPixel(buffer, &rgb, w, h);
}