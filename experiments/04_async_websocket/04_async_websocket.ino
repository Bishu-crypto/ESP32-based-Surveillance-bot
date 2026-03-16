/*
 * Experiment 04 — ESPAsyncWebServer + WebSocket control
 * Board  : AI Thinker ESP32-CAM
 * Libs   : ESPAsyncWebServer (ESP32Async), AsyncTCP (ESP32Async),
 *          ArduinoJson 6.x (Benoit Blanchon)
 * UI     : http://<IP>
 * Status : DONE
 */

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>
#include "secrets.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32-CAM Control</title>
  <style>
    body { font-family: sans-serif; background: #111; color: #eee; text-align: center; }
    button { margin: 8px; padding: 16px 24px; font-size: 18px;
             background: #333; color: #eee; border: 1px solid #555;
             border-radius: 8px; cursor: pointer; }
    button:active { background: #555; }
    #status { margin-top: 20px; color: #8f8; }
  </style>
</head>
<body>
  <h2>ESP32-CAM WebSocket Test</h2>
  <div><button onclick="send('FORWARD')">⬆ Forward</button></div>
  <div>
    <button onclick="send('LEFT')">⬅ Left</button>
    <button onclick="send('STOP')">⏹ Stop</button>
    <button onclick="send('RIGHT')">Right ➡</button>
  </div>
  <div><button onclick="send('BACKWARD')">⬇ Back</button></div>
  <p id="status">Not connected</p>
  <script>
    let ws;
    function connect() {
      ws = new WebSocket('ws://' + location.hostname + '/ws');
      ws.onopen    = () => document.getElementById('status').textContent = 'Connected ✓';
      ws.onclose   = () => { document.getElementById('status').textContent = 'Disconnected'; setTimeout(connect, 2000); };
      ws.onmessage = (e) => console.log('ESP32:', e.data);
    }
    function send(cmd) {
      if (ws && ws.readyState === 1) ws.send(JSON.stringify({cmd}));
    }
    connect();
  </script>
</body>
</html>
)rawliteral";

void onWsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
               AwsEventType type, void* arg, uint8_t* data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("Client #%u connected\n", client->id());
    client->text("{\"status\":\"hello\"}");
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("Client #%u disconnected\n", client->id());
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo* info = (AwsFrameInfo*)arg;
    if (info->opcode == WS_TEXT) {
      String msg = String((char*)data).substring(0, len);
      Serial.println("Received: " + msg);

      DynamicJsonDocument doc(256);
      if (!deserializeJson(doc, msg)) {
        String cmd = doc["cmd"].as<String>();
        if      (cmd == "FORWARD")  Serial.println("→ Go forward");
        else if (cmd == "BACKWARD") Serial.println("→ Go backward");
        else if (cmd == "LEFT")     Serial.println("→ Turn left");
        else if (cmd == "RIGHT")    Serial.println("→ Turn right");
        else if (cmd == "STOP")     Serial.println("→ Stop");
        ws.textAll("{\"ack\":\"" + cmd + "\"}");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print("."); }
  Serial.println("\nIP: " + WiFi.localIP().toString());

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
    req->send_P(200, "text/html", index_html);
  });

  server.begin();
  Serial.println("Server started");
}

void loop() {
  ws.cleanupClients();
}