const int ledPin = 2;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // ========== 发送S：三次短闪 ==========
  for(int i = 0; i < 3; i++){
    digitalWrite(ledPin, HIGH);
    delay(200);    // 短亮200ms
    digitalWrite(ledPin, LOW);
    delay(500);    // 每个短闪间隔500ms
  }
  // 字母S与O之间间隔
  delay(500);

  // ========== 发送O：三次长闪 ==========
  for(int i = 0; i < 3; i++){
    digitalWrite(ledPin, HIGH);
    delay(600);    // 长亮600ms
    digitalWrite(ledPin, LOW);
    delay(500);    // 每个长闪间隔500ms
  }
  // 字母O与S之间间隔
  delay(500);

  // ========== 发送S：三次短闪 ==========
  for(int i = 0; i < 3; i++){
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(500);
  }

  // 一整组SOS发送完成，长时间停顿2秒，再重复循环
  delay(2000);
  Serial.println("已完成一组SOS求救信号");
}