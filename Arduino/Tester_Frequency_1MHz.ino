// Tester_Frequency_1MHz.ino
// 주파수 측정기 (최대 1MHz)

// 사용법
// 1. 코드를 아두이노 보드에 업로드
// 2. 측정하고자 하는 부분을 아두이노 2번 핀에 연결
// 3. Serial Monitor에서 주파수 확인

void setup()
{
  Serial.begin(9600);
  pinMode(2, INPUT);
}

void loop()
{
  unsigned long tHigh = pulseIn(2, HIGH, 1000000);
  unsigned long tLow  = pulseIn(2, LOW, 1000000);

  if (tHigh == 0 || tLow == 0)
  {
    Serial.println("No signal");
  }
  else
  {
    float period = tHigh + tLow;
    float freq = 1000000.0 / period;
    Serial.print("Freq = ");
    Serial.print(freq);
    Serial.println(" Hz");
  }

  delay(500);
}
