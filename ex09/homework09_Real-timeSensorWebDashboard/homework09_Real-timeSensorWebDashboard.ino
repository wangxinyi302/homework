#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "AAA批发市场王总";
const char* password = "qazwsxedc";

WebServer server(80);

const int touchPin = 4;   // T0 = GPIO 4

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 实时传感器仪表盘</title>

  <style>
    body {
      margin: 0;
      padding: 0;
      font-family: Arial, "Microsoft YaHei", sans-serif;
      background: radial-gradient(circle at top, #1e40af, #020617);
      color: white;
      height: 100vh;
      display: flex;
      justify-content: center;
      align-items: center;
    }

    .dashboard {
      width: 90%;
      max-width: 460px;
      background: rgba(255, 255, 255, 0.12);
      border-radius: 28px;
      padding: 35px 25px;
      text-align: center;
      box-shadow: 0 20px 45px rgba(0, 0, 0, 0.45);
      backdrop-filter: blur(10px);
    }

    h1 {
      margin-top: 0;
      font-size: 28px;
    }

    .subtitle {
      color: #cbd5e1;
      margin-bottom: 30px;
      font-size: 15px;
    }

    .meter {
      width: 230px;
      height: 230px;
      border-radius: 50%;
      margin: 0 auto;
      background: conic-gradient(#38bdf8 0deg, #1e293b 0deg);
      display: flex;
      justify-content: center;
      align-items: center;
      box-shadow: inset 0 0 30px rgba(255,255,255,0.15),
                  0 0 35px rgba(56,189,248,0.45);
    }

    .inner {
      width: 180px;
      height: 180px;
      border-radius: 50%;
      background: #020617;
      display: flex;
      flex-direction: column;
      justify-content: center;
      align-items: center;
    }

    #touchValue {
      font-size: 58px;
      font-weight: bold;
      color: #38bdf8;
      transition: 0.2s;
    }

    .label {
      margin-top: 8px;
      color: #94a3b8;
      font-size: 15px;
    }

    .status {
      margin-top: 28px;
      font-size: 17px;
      color: #e2e8f0;
    }

    .bar {
      margin: 25px auto 0;
      width: 85%;
      height: 12px;
      border-radius: 20px;
      background: #1e293b;
      overflow: hidden;
    }

    #barFill {
      height: 100%;
      width: 0%;
      background: linear-gradient(90deg, #22d3ee, #3b82f6);
      border-radius: 20px;
      transition: 0.2s;
    }

    .footer {
      margin-top: 25px;
      color: #94a3b8;
      font-size: 13px;
    }
  </style>
</head>

<body>
  <div class="dashboard">
    <h1>实时传感器 Web 仪表盘</h1>
    <div class="subtitle">ESP32 Touch Sensor Data Monitor</div>

    <div class="meter" id="meter">
      <div class="inner">
        <div id="touchValue">--</div>
        <div class="label">Touch Value</div>
      </div>
    </div>

    <div class="bar">
      <div id="barFill"></div>
    </div>

    <div class="status" id="statusText">正在读取传感器数据...</div>

    <div class="footer">
      手靠近 GPIO 4 时，数值会实时变小；离开后数值恢复。
    </div>
  </div>

  <script>
    function updateTouchValue() {
      fetch('/touch')
        .then(response => response.text())
        .then(value => {
          let num = parseInt(value);

          document.getElementById('touchValue').innerText = num;

          // 这里按照 0-100 做一个仪表盘比例
          let percent = mapValue(num, 0, 100, 100, 0);
          percent = Math.max(0, Math.min(100, percent));

          let degree = percent * 3.6;

          document.getElementById('meter').style.background =
            `conic-gradient(#38bdf8 ${degree}deg, #1e293b ${degree}deg)`;

          document.getElementById('barFill').style.width = percent + "%";

          if (num < 30) {
            document.getElementById('statusText').innerText = "检测到手指靠近 / 触摸";
          } else {
            document.getElementById('statusText').innerText = "未检测到明显触摸";
          }
        })
        .catch(error => {
          document.getElementById('statusText').innerText = "数据读取失败";
        });
    }

    function mapValue(x, inMin, inMax, outMin, outMax) {
      return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
    }

    setInterval(updateTouchValue, 200);
    updateTouchValue();
  </script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

void handleTouch() {
  int touchValue = touchRead(touchPin);
  server.send(200, "text/plain", String(touchValue));
}

void setup() {
  Serial.begin(115200);

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

  // 这个接口专门给 AJAX 获取实时触摸数值
  server.on("/touch", handleTouch);

  server.begin();
  Serial.println("Web Server 已启动");
}

void loop() {
  server.handleClient();
}