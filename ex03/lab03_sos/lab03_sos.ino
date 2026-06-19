// Lab03 millis实现SOS：三短·三长·三短，一轮结束长停顿
#define LED_PIN 2
// 时间参数 ms
const unsigned long DOT = 200;    // 短闪
const unsigned long DASH = 600;   // 长闪
const unsigned long GAP = 200;    // 单次闪烁间隔
const unsigned long LOOP_GAP = 2000; // 一轮SOS结束停顿

// SOS流程数组：0=短闪，1=长闪，2=一轮结束
uint8_t sosSeq[] = {0,0,0,1,1,1,0,0,0,2};
uint8_t seqIndex = 0;
unsigned long prevTime = 0;
bool ledOn = false;
unsigned long currentDelay = DOT;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  unsigned long now = millis();
  if(now - prevTime >= currentDelay){
    prevTime = now;
    uint8_t cmd = sosSeq[seqIndex];
    switch(cmd){
      case 0: // 短闪
        ledOn = !ledOn;
        digitalWrite(LED_PIN, ledOn);
        currentDelay = ledOn ? DOT : GAP;
        if(!ledOn) seqIndex++;
        break;
      case 1: // 长闪
        ledOn = !ledOn;
        digitalWrite(LED_PIN, ledOn);
        currentDelay = ledOn ? DASH : GAP;
        if(!ledOn) seqIndex++;
        break;
      case 2: // 一轮结束，长熄灭停顿
        digitalWrite(LED_PIN, LOW);
        seqIndex = 0;
        currentDelay = LOOP_GAP;
        break;
    }
  }
}