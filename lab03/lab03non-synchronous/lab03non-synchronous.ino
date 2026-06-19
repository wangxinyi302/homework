// 定义两路LED引脚
const int ledPinX = 4;
const int ledPinY = 5;

// PWM统一参数，两路LED共用相同频率、分辨率
const int freq = 5000;          // PWM频率 5000Hz
const int resolution = 8;       // 8位分辨率 亮度范围0~255

void setup() {
  Serial.begin(115200);

  // 分别绑定两个引脚PWM
  ledcAttach(ledPinX, freq, resolution);
  ledcAttach(ledPinY, freq, resolution);
}

void loop() {
  // 同步渐亮：x、y共用同一个dutyCycle亮度变量，同时赋值
  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
    ledcWrite(ledPinX, dutyCycle); // 控制引脚4
    ledcWrite(ledPinY, 255-dutyCycle); // 控制引脚5，同步亮度
    delay(10);
  }

  // 同步渐暗
  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
    ledcWrite(ledPinX, dutyCycle);
    ledcWrite(ledPinY, 255-dutyCycle);
    delay(10);
  }
  
  Serial.println("双LED不同步呼吸周期完成");
}