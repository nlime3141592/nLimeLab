# PRNG

- PRNG, Pseudo Random Number Generator
- 의사 난수 생성기
- 만약 랜덤성을 부여하면서도 예측 가능한(결정적인) 시스템을 만들어야 한다면 사용해야 할 알고리즘들

> #### 참고
> - 지속적으로 추가할 예정임

## 함수 목록

- `PrngSplitMix64()` → 랜덤 64비트 난수 생성
- `SeedToFloat01()` → 64비트 난수로부터 0 이상 1 미만의 실수를 생성