/*
 * Experiment 05 — MJPEG Stream + AsyncWebServer + WebSocket combined
 * Board  : AI Thinker ESP32-CAM (OV2640)
 * Status : WIP
 */

#include "esp_camera.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

const char* WIFI_SSID     = "Bishuwifi";
const char* WIFI_PASSWORD = "00000000";

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
String g_html = "";

// ── Camera init ────────────────────────────────────────────
bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM; config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM; config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM; config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM; config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size   = FRAMESIZE_QVGA;
  config.jpeg_quality = 15;
  config.fb_count     = 2;
  return esp_camera_init(&config) == ESP_OK;
}

// ── MJPEG stream via AsyncWebServer ───────────────────────
// Runs on the same port 80 at /stream
void streamJpg(AsyncWebServerRequest* req) {
  AsyncResponseStream* response = req->beginResponseStream("multipart/x-mixed-replace;boundary=frame");

  // This approach sends one frame then closes — client JS will reload it in a loop
  // Better than fighting header conflicts with httpd
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    req->send(503, "text/plain", "Camera capture failed");
    return;
  }
  response->printf("--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", fb->len);
  response->write(fb->buf, fb->len);
  esp_camera_fb_return(fb);
  req->send(response);
}

// ── HTML ───────────────────────────────────────────────────
const char PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32-CAM</title>
  <style>
    * { box-sizing:border-box; margin:0; padding:0; }
    body { background:#111; color:#eee; font-family:sans-serif;
           display:flex; flex-direction:column; align-items:center; padding:12px; }
    h2 { margin-bottom:10px; }
    #mjpeg { width:100%; max-width:640px; border-radius:8px;
             margin-bottom:14px; background:#222; min-height:240px; }
    .pad { display:grid; grid-template-columns:repeat(3,1fr);
           gap:8px; width:100%; max-width:280px; }
    button { padding:20px; font-size:22px; background:#2a2a2a;
             color:#eee; border:1px solid #444; border-radius:8px;
             cursor:pointer; user-select:none; -webkit-user-select:none; }
    button:active { background:#555; }
    .blank { visibility:hidden; }
    #status { margin-top:12px; font-size:13px; color:#8f8; }
  </style>
</head>
<body>
  <h2>ESP32-CAM Live Control</h2>
  <img id="mjpeg">
  <div class="pad">
    <div class="blank"></div>
    <button onmousedown="mv('F')" onmouseup="mv('S')" ontouchstart="mv('F')" ontouchend="mv('S')">&#11014;</button>
    <div class="blank"></div>
    <button onmousedown="mv('L')" onmouseup="mv('S')" ontouchstart="mv('L')" ontouchend="mv('S')">&#11013;</button>
    <button onclick="mv('S')">&#9646;</button>
    <button onmousedown="mv('R')" onmouseup="mv('S')" ontouchstart="mv('R')" ontouchend="mv('S')">&#10145;</button>
    <div class="blank"></div>
    <button onmousedown="mv('B')" onmouseup="mv('S')" ontouchstart="mv('B')" ontouchend="mv('S')">&#11015;</button>
    <div class="blank"></div>
  </div>
  <p id="status">Connecting...</p>
  <script>
    var sock = null;
    var img  = document.getElementById('mjpeg');
    var base = 'http://' + location.hostname;

    function reloadFrame() {
      img.src = base + '/stream?' + Date.now();
      img.onload  = function() { setTimeout(reloadFrame, 50); };
      img.onerror = function() { setTimeout(reloadFrame, 500); };
    }
    reloadFrame();

    function wsConnect() {
      sock = new WebSocket('ws://' + location.hostname + '/ws');
      sock.onopen  = function() { document.getElementById('status').textContent = 'Connected'; };
      sock.onclose = function() {
        document.getElementById('status').textContent = 'Reconnecting...';
        setTimeout(wsConnect, 1500);
      };
    }
    function mv(c) { if (sock && sock.readyState === 1) sock.send(c); }

    var km = { ArrowUp:'F', ArrowDown:'B', ArrowLeft:'L', ArrowRight:'R' };
    document.addEventListener('keydown', function(e) { if(km[e.key]) mv(km[e.key]); });
    document.addEventListener('keyup',   function(e) { if(km[e.key]) mv('S'); });

    wsConnect();
  </script>
</body>
</html>
)rawliteral";

// ── WebSocket handler ──────────────────────────────────────
void onWsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
               AwsEventType type, void* arg, uint8_t* data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("Client #%u connected\n", client->id());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("Client #%u disconnected\n", client->id());
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo* info = (AwsFrameInfo*)arg;
    if (info->opcode == WS_TEXT) {
      String cmd = String((char*)data).substring(0, len);
      Serial.println("CMD: " + cmd);
      if      (cmd == "F") Serial.println("-> Forward");
      else if (cmd == "B") Serial.println("-> Backward");
      else if (cmd == "L") Serial.println("-> Left");
      else if (cmd == "R") Serial.println("-> Right");
      else if (cmd == "S") Serial.println("-> Stop");
    }
  }
}

// ── Setup ──────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  if (!initCamera()) {
    Serial.println("Camera init failed!");
    while (true) delay(1000);
  }
  Serial.println("Camera OK");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print("."); }
  String ip = WiFi.localIP().toString();
  Serial.println("\nIP: " + ip);

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.on("/", [](AsyncWebServerRequest* req) {
    req->send_P(200, "text/html", PAGE);
  });

  server.on("/stream", [](AsyncWebServerRequest* req) {
    streamJpg(req);
  });

  server.begin();
  Serial.println("Open: http://" + ip);
}

// ── Loop ───────────────────────────────────────────────────
void loop() {
  ws.cleanupClients();
}