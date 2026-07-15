#ifndef __FFT_H__
#define __FFT_H__

#include <stdint.h>

typedef struct _FTComplex
{
    float re; // 실수 성분
    float im; // 허수 성분
} FTComplex;

/// @brief 시간 영역 샘플을 단일 주파수 인덱스에 대해 이산푸리에변환을 수행합니다.
/// @param data 시간 영역 샘플
/// @param n 시간 영역 샘플 수, 1 이상
/// @param k 주파수 인덱스, 0 이상 n 미만
/// @return 복소수 푸리에계수
FTComplex dftSingle(FTComplex* data, int32_t n, int32_t k);

/// @brief 주파수 영역 샘플을 단일 시간 인덱스에 대해 이산푸리에역변환을 수행합니다.
/// @param data 주파수 영역 샘플(푸리에계수 리스트)
/// @param n 주파수 영역 샘플 수, 1 이상
/// @param k 시간 인덱스, 0이상 n 미만
/// @return 복소수 시간 영역 샘플
FTComplex idftSingle(FTComplex* data, int32_t n, int32_t k);

/// @brief 시간 영역 샘플을 모든 주파수 인덱스에 대해 이산푸리에변환을 수행합니다.
/// @param out 출력; 주파수 영역 샘플(푸리에계수 리스트)
/// @param data 시간 영역 샘플
/// @param n 시간 영역 샘플 수, 1 이상
void dft(FTComplex* out, FTComplex* data, int32_t n);

/// @brief 주파수 영역 샘플을 모든 시간 인덱스에 대해 이산푸리에역변환을 수행합니다.
/// @param out 출력; 시간 영역 샘플
/// @param data 주파수 영역 샘플(푸리에계수 리스트)
/// @param n 주파수 영역 샘플 수, 1 이상
void idft(FTComplex* out, FTComplex* data, int32_t n);

/// @brief 시간 영역 샘플을 모든 주파수 인덱스에 대해 고속푸리에변환을 수행합니다.
/// @param out 출력; 주파수 영역 샘플(푸리에계수 리스트)
/// @param data 시간 영역 샘플
/// @param n 시간 영역 샘플 수, 1 이상
void fft(FTComplex* out, FTComplex* data, int32_t n);

/// @brief 주파수 영역 샘플을 모든 시간 인덱스에 대해 고속푸리에역변환을 수행합니다.
/// @param out 출력; 시간 영역 샘플
/// @param data 주파수 영역 샘플(푸리에계수 리스트)
/// @param n 주파수 영역 샘플 수, 1 이상
void ifft(FTComplex* out, FTComplex* data, int32_t n);

/// @brief 복소수의 크기를 계산합니다.
/// @param c 복소수
/// @return 복소수의 크기(실수)
float Magnitude(FTComplex c);

#endif
