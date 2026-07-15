# BMP File IO

- BMP 파일 처리 코드
- C언어
- 왼쪽 아래가 (0, 0), 오른쪽이 x축 방향, 위쪽이 y축 방향
- 프로그래밍 시 2차원 시각화가 필요하다면 간단히 사용해볼 수 있음

## Usage

```c
#include "bmp.h"

void CreateUV(const char* file)
{
    Bmp bmp;
    BmpRgb rgb;
    
    int xResolution = 256;
    int yResolution = 256;

    // 1. BMP 파일 열기 또는 만들기
    TryCreateBmp(&bmp, file, xResolution, yResolution);

    // 2. 이미지 처리 (UV Map 생성 코드)
    rgb.b = 0;

    for (int y = 0; y < yResolution; ++y)
    {
        rgb.g = y;

        for (int x = 0; x < xResolution; ++x)
        {
            rgb.r = x;
            SetRgbP(&bmp, &rgb, x, y);
        }
    }

    // 3. 처리 내용을 파일에 쓰기
    Commit(&bmp);

    // 4. 파일 닫기
    TryCloseBmp(&bmp);
}

int main()
{
    const char* file = "C:\\MyDirectory\\TestImage.bmp";
    CreateUV(file);
}
```