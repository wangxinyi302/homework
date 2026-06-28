#define TOUCH_PIN 4
#define LED_PIN 2
#define THRESHOLD 500       // 触摸阈值，可根据硬件微调
#define DEBOUNCE_DELAY 80   // 软件防抖时间ms

// 核心要求布尔状态变量：LED自锁状态
bool ledState = false;
// 触摸边缘检测变量：记录上一次触摸状态
bool lastTouchState = false;
// 防抖时间记录
unsigned long touchDebounceTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  // 1. 读取当前触摸引脚数值
  int touchVal = touchRead(TOUCH_PIN);
  // 判断当前是否触摸
  bool currentTouchState = (touchVal < THRESHOLD);

  // 2. 软件防抖：时间间隔满足才判定有效触摸
  if (millis() - touchDebounceTime > DEBOUNCE_DELAY) {
    // 3. 边缘检测：上一次没触摸，本次触摸 = 触摸触发瞬间
    if (currentTouchState == true && lastTouchState == false) {
      // 翻转LED自锁状态
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
      Serial.print("触摸触发，LED状态：");
      Serial.println(ledState ? "亮" : "灭");
      // 更新防抖计时
      touchDebounceTime = millis();
    }
  }

  // 更新上一轮触摸状态，用于下一次边缘判断
  lastTouchState = currentTouchState;
  delay(10);
}