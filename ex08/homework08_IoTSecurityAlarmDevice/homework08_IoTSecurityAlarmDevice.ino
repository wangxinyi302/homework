#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "AAA批发市场王总";
const char* password = "qazwsxedc";

WebServer server(80);

// 引脚设置
const int ledPin = 2;        // ESP32 板载 LED 一般是 GPIO 2
const int touchPin = 4;      // T0 = GPIO 4

// 系统状态
bool isArmed = false;        // 是否布防
bool isAlarm = false;        // 是否报警

// 触摸阈值：数值越小代表触摸越明显
int touchThreshold = 300;

// LED 闪烁控制
unsigned long lastBlinkTime = 0;
bool ledState = false;
int blinkInterval = 80;      // 高频闪烁速度，越小越快

String getStatusText() {
  if (!isArmed) {
    return "未布防";
  } else if (isAlarm) {
    return "报警中";
  } else {
    return "已布防";
  }
}

String getStatusClass() {
  if (!isArmed) {
    return "safe";
  } else if (isAlarm) {
    return "alarm";
  } else {
    return "armed";
  }
}

void handleRoot() {
  int touchValue = touchRead(touchPin);

  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 安防报警器</title>
  <style>
    body {
      margin: 0;
      padding: 0;
      font-family: Arial, "Microsoft YaHei", sans-serif;
      background: linear-gradient(135deg, #1f2937, #111827);
      color: white;
      min-height: 100vh;
      display: flex;
      justify-content: center;
      align-items: center;
    }

    .card {
      width: 90%;
      max-width: 420px;
      background: rgba(255, 255, 255, 0.1);
      border-radius: 22px;
      padding: 30px;
      box-shadow: 0 15px 35px rgba(0,0,0,0.4);
      text-align: center;
      backdrop-filter: blur(8px);
    }

    h1 {
      margin-top: 0;
      font-size: 28px;
    }

    .subtitle {
      color: #d1d5db;
      margin-bottom: 25px;
    }

    .status {
      font-size: 26px;
      font-weight: bold;
      padding: 18px;
      border-radius: 16px;
      margin: 20px 0;
    }

    .safe {
      background: #374151;
      color: #d1d5db;
    }

    .armed {
      background: #2563eb;
      color: white;
    }

    .alarm {
      background: #dc2626;
      color: white;
      animation: flash 0.6s infinite alternate;
    }

    @keyframes flash {
      from { opacity: 1; }
      to { opacity: 0.55; }
    }

    .info {
      margin: 15px 0;
      color: #e5e7eb;
      font-size: 15px;
    }

    .buttons {
      display: flex;
      gap: 15px;
      justify-content: center;
      margin-top: 25px;
    }

    button {
      border: none;
      padding: 14px 26px;
      border-radius: 12px;
      font-size: 17px;
      cursor: pointer;
      color: white;
      font-weight: bold;
    }

    .arm-btn {
      background: #2563eb;
    }

    .disarm-btn {
      background: #dc2626;
    }

    button:hover {
      transform: scale(1.05);
      transition: 0.2s;
    }

    .footer {
      margin-top: 25px;
      font-size: 13px;
      color: #9ca3af;
    }
  </style>
</head>

<body>
  <div class="card">
    <h1>ESP32 安防报警器</h1>
    <div class="subtitle">物联网安防主机模拟实验</div>

    <div class="status STATUS_CLASS">STATUS_TEXT</div>

    <div class="info">触摸引脚：GPIO 4 / T0</div>
    <div class="info">当前触摸值：TOUCH_VALUE</div>

    <div class="buttons">
      <form action="/arm" method="get">
        <button class="arm-btn">布防 Arm</button>
      </form>

      <form action="/disarm" method="get">
        <button class="disarm-btn">撤防 Disarm</button>
      </form>
    </div>

    <div class="footer">
      未布防时触摸无反应；布防后触摸会锁定报警，只有撤防才能重置。
    </div>
  </div>

  <script>
    setTimeout(() => {
      location.reload();
    }, 1000);
  </script>
</body>
</html>
)rawliteral";

  html.replace("STATUS_TEXT", getStatusText());
  html.replace("STATUS_CLASS", getStatusClass());
  html.replace("TOUCH_VALUE", String(touchValue));

  server.send(200, "text/html", html);
}

void handleArm() {
  isArmed = true;
  isAlarm = false;
  digitalWrite(ledPin, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleDisarm() {
  isArmed = false;
  isAlarm = false;
  digitalWrite(ledPin, LOW);
  ledState = false;
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  WiFi.begin(ssid, password);

  Serial.println("正在连接 WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi 已连接！");
  Serial.print("网页地址：http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);

  server.begin();
  Serial.println("Web 服务器已启动");
}

void loop() {
  server.handleClient();

  int touchValue = touchRead(touchPin);

  // 只有在布防状态下，触摸才会触发报警
  if (isArmed && !isAlarm && touchValue < touchThreshold) {
    isAlarm = true;
  }

  // 报警后 LED 高频闪烁，且不会因为松手停止
  if (isAlarm) {
    unsigned long now = millis();

    if (now - lastBlinkTime >= blinkInterval) {
      lastBlinkTime = now;
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
    }
  } else {
    digitalWrite(ledPin, LOW);
  }
}