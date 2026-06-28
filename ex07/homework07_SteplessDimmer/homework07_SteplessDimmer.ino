#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "AAA批发市场王总";
const char* password = "qazwsxedc";

WebServer server(80);

// 板载LED
#define LED_PIN 2

// PWM参数
const int pwmFreq = 5000;
const int pwmResolution = 8;

int brightness = 0;

// ===================== HTML网页 =====================
String htmlPage() {
  return R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">

<title>ESP32 LED调光器</title>

<style>

body{
    margin:0;
    height:100vh;
    display:flex;
    justify-content:center;
    align-items:center;
    background:linear-gradient(135deg,#667eea,#764ba2);
    font-family:Arial;
}

.card{

    width:360px;

    background:white;

    border-radius:20px;

    padding:30px;

    text-align:center;

    box-shadow:0 10px 25px rgba(0,0,0,.25);

}

h1{

    color:#333;

}

#value{

    font-size:48px;

    font-weight:bold;

    color:#667eea;

    margin:20px;

}

input[type=range]{

    width:100%;

}

.footer{

    margin-top:20px;

    color:gray;

}

</style>

</head>

<body>

<div class="card">

<h1>ESP32 LED 调光器</h1>

<p>拖动滑块调节LED亮度</p >

<div id="value">0</div>

<input type="range"
min="0"
max="255"
value="0"
id="slider">

<div class="footer">
Brightness : 0~255
</div>

</div>

<script>

let slider=document.getElementById("slider");

let value=document.getElementById("value");

slider.oninput=function(){

value.innerHTML=this.value;

fetch("/set?value="+this.value);

}

</script>

</body>

</html>

)rawliteral";
}

// ===================== 首页 =====================
void handleRoot() {
  server.send(200, "text/html", htmlPage());
}

// ===================== 设置亮度 =====================
void handleSet() {

  if (server.hasArg("value")) {

    brightness = server.arg("value").toInt();

    if (brightness < 0) brightness = 0;
    if (brightness > 255) brightness = 255;

    ledcWrite(LED_PIN, brightness);

    server.send(200, "text/plain", "OK");
  }
  else {
    server.send(400, "text/plain", "No Value");
  }
}

// ===================== Setup =====================
void setup() {

  Serial.begin(115200);

  // 新版ESP32 Core 3.x PWM初始化
  ledcAttach(LED_PIN, pwmFreq, pwmResolution);

  ledcWrite(LED_PIN, 0);

  WiFi.begin(ssid, password);

  Serial.println();

  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected!");

  Serial.print("IP Address: ");

  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);

  server.on("/set", handleSet);

  server.begin();

  Serial.println("Web Server Started");
}

// ===================== Loop =====================
void loop() {

  server.handleClient();

}