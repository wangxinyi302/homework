// Lab02：millis()实现1Hz LED闪烁（周期1s，亮500ms灭500ms）
#define LED_PIN 2
unsigned long previousMillis = 0;
const unsigned long interval = 500; // 500ms翻转一次，1Hz完整周期
bool ledState = LOW;

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }
}