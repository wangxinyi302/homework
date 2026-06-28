#define TOUCH_PIN 4
#define LED_PIN 2
#define THRESHOLD 500
#define DEBOUNCE_DELAY 80  // 复用ex04软件防抖参数

// 触摸相关（继承作业4自锁边缘检测逻辑）
bool lastTouchState = false;
unsigned long touchDebounceTime = 0;

// 作业5新增：呼吸档位控制
int speedGear = 1;    // 档位：1/2/3档，初始1档
int pwmVal = 0;       // PWM亮度值 0~255
int pwmStep;          // 亮度变化步长，由档位决定
bool brightUp = true; // 呼吸渐变方向：增亮/变暗

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // ========== 1. 触摸检测（复用ex04边缘检测+防抖逻辑） ==========
  int touchRaw = touchRead(TOUCH_PIN);
  bool currTouch = (touchRaw < THRESHOLD);

  if (millis() - touchDebounceTime > DEBOUNCE_DELAY) {
    // 边缘检测：仅触摸瞬间触发，长按不重复切换档位
    if (currTouch == true && lastTouchState == false) {
      // 3档循环切换 1→2→3→1
      speedGear = speedGear + 1;
      if (speedGear > 3) speedGear = 1;
      Serial.print("切换至档位：");
      Serial.println(speedGear);
      touchDebounceTime = millis();
    }
  }
  lastTouchState = currTouch;

  // ========== 2. 根据档位设置呼吸速度（步长越大呼吸越快） ==========
  switch(speedGear){
    case 1: pwmStep = 1; break;  // 慢呼吸
    case 2: pwmStep = 3; break;  // 中等速度
    case 3: pwmStep = 6; break;  // 急促快速呼吸
  }

  // ========== 3. PWM呼吸灯逻辑（实验3基础PWM） ==========
  analogWrite(LED_PIN, pwmVal);

  // 亮度增减
  if(brightUp){
    pwmVal += pwmStep;
    if(pwmVal >= 255) brightUp = false;
  }else{
    pwmVal -= pwmStep;
    if(pwmVal <= 0) brightUp = true;
  }

  delay(10);
}