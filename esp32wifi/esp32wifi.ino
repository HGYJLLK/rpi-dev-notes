// ESP32 WiFi Web Serial Monitor
// Access via: http://<ESP32-IP>/

#include <WiFi.h>
#include <WebServer.h>

// WiFi credentials
const char* ssid = "GEEK2.4G";
const char* password = "34163416";

WebServer server(80);

// Buffer for serial messages
#define MAX_MESSAGES 50
#define MAX_MESSAGE_LENGTH 128
char serialMessages[MAX_MESSAGES][MAX_MESSAGE_LENGTH];
int messageIndex = 0;
int messageCount = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\nESP32 Web Serial Monitor Starting...");

  // Initialize message buffer
  for(int i = 0; i < MAX_MESSAGES; i++) {
    serialMessages[i][0] = '\0';
  }

  // Connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Web Serial: http://");
    Serial.println(WiFi.localIP());

    addMessage("WiFi connected!");
    char ipMsg[MAX_MESSAGE_LENGTH];
    snprintf(ipMsg, MAX_MESSAGE_LENGTH, "IP: %s", WiFi.localIP().toString().c_str());
    addMessage(ipMsg);
  } else {
    Serial.println("\nWiFi connection failed!");
    addMessage("WiFi connection failed!");
    return;
  }

  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/messages", handleMessages);
  server.on("/send", HTTP_POST, handleSend);

  server.begin();
  Serial.println("Web server started!");
  addMessage("Web server started!");
}

void loop() {
  server.handleClient();

  // Read serial input and add to buffer
  if (Serial.available()) {
    static char inputBuffer[MAX_MESSAGE_LENGTH];
    static int bufferPos = 0;

    while (Serial.available()) {
      char c = Serial.read();
      if (c == '\n' || c == '\r') {
        if (bufferPos > 0) {
          inputBuffer[bufferPos] = '\0';
          addMessage(inputBuffer);
          Serial.println(inputBuffer); // Echo back
          bufferPos = 0;
        }
      } else if (bufferPos < MAX_MESSAGE_LENGTH - 1) {
        inputBuffer[bufferPos++] = c;
      }
    }
  }

  // Send test message every 10 seconds
  static unsigned long lastTest = 0;
  if (millis() - lastTest > 10000) {
    lastTest = millis();

    char testMsg[MAX_MESSAGE_LENGTH];
    snprintf(testMsg, MAX_MESSAGE_LENGTH, "Uptime: %lu s | Heap: %d bytes",
             millis()/1000, ESP.getFreeHeap());
    addMessage(testMsg);
  }
}

void addMessage(const char* message) {
  // Add timestamp
  unsigned long seconds = millis() / 1000;
  snprintf(serialMessages[messageIndex], MAX_MESSAGE_LENGTH, "[%lu] %s", seconds, message);

  messageIndex = (messageIndex + 1) % MAX_MESSAGES;
  if (messageCount < MAX_MESSAGES) {
    messageCount++;
  }
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 Web Serial</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: 'Courier New', monospace;
      background: #1e1e1e;
      color: #d4d4d4;
      padding: 20px;
    }
    .container {
      max-width: 1000px;
      margin: 0 auto;
    }
    h1 {
      color: #4ec9b0;
      margin-bottom: 20px;
      font-size: 24px;
    }
    .info {
      background: #252526;
      padding: 15px;
      border-radius: 5px;
      margin-bottom: 20px;
      border-left: 3px solid #4ec9b0;
    }
    .info p { margin: 5px 0; }
    .serial-monitor {
      background: #1e1e1e;
      border: 2px solid #3c3c3c;
      border-radius: 5px;
      padding: 15px;
      height: 400px;
      overflow-y: auto;
      font-size: 14px;
      margin-bottom: 15px;
    }
    .serial-monitor::-webkit-scrollbar {
      width: 10px;
    }
    .serial-monitor::-webkit-scrollbar-track {
      background: #252526;
    }
    .serial-monitor::-webkit-scrollbar-thumb {
      background: #3c3c3c;
      border-radius: 5px;
    }
    .message {
      margin: 5px 0;
      padding: 5px;
      border-left: 2px solid #007acc;
    }
    .input-area {
      display: flex;
      gap: 10px;
      margin-bottom: 15px;
    }
    input[type="text"] {
      flex: 1;
      padding: 12px;
      background: #252526;
      border: 1px solid #3c3c3c;
      color: #d4d4d4;
      border-radius: 5px;
      font-family: 'Courier New', monospace;
      font-size: 14px;
    }
    input[type="text"]:focus {
      outline: none;
      border-color: #007acc;
    }
    button {
      padding: 12px 24px;
      background: #0e639c;
      color: white;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      font-size: 14px;
      font-weight: bold;
    }
    button:hover {
      background: #1177bb;
    }
    button:active {
      background: #007acc;
    }
    .controls {
      display: flex;
      gap: 10px;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>ESP32 Web Serial Monitor</h1>

    <div class="info">
      <p><strong>Status:</strong> <span id="status">Connected</span></p>
      <p><strong>IP:</strong> )rawliteral" + WiFi.localIP().toString() + R"rawliteral(</p>
      <p><strong>Free Heap:</strong> <span id="heap">)rawliteral" + String(ESP.getFreeHeap()) + R"rawliteral(</span> bytes</p>
    </div>

    <div class="serial-monitor" id="monitor"></div>

    <div class="input-area">
      <input type="text" id="input" placeholder="Type message and press Enter...">
      <button onclick="sendMessage()">Send</button>
    </div>

    <div class="controls">
      <button onclick="clearMonitor()">Clear</button>
      <button onclick="location.reload()">Refresh</button>
    </div>
  </div>

  <script>
    let autoScroll = true;

    function updateMessages() {
      fetch('/messages')
        .then(response => response.json())
        .then(data => {
          const monitor = document.getElementById('monitor');
          monitor.innerHTML = '';
          data.messages.forEach(msg => {
            const div = document.createElement('div');
            div.className = 'message';
            div.textContent = msg;
            monitor.appendChild(div);
          });
          if (autoScroll) {
            monitor.scrollTop = monitor.scrollHeight;
          }
          document.getElementById('heap').textContent = data.heap;
        })
        .catch(err => console.error('Error:', err));
    }

    function sendMessage() {
      const input = document.getElementById('input');
      const message = input.value.trim();
      if (message) {
        fetch('/send', {
          method: 'POST',
          headers: {'Content-Type': 'application/x-www-form-urlencoded'},
          body: 'msg=' + encodeURIComponent(message)
        })
        .then(() => {
          input.value = '';
          setTimeout(updateMessages, 100);
        });
      }
    }

    function clearMonitor() {
      document.getElementById('monitor').innerHTML = '';
    }

    document.getElementById('input').addEventListener('keypress', function(e) {
      if (e.key === 'Enter') {
        sendMessage();
      }
    });

    // Update every 1 second
    setInterval(updateMessages, 1000);
    updateMessages();
  </script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

void handleMessages() {
  String json = "{\"messages\":[";

  // Get messages in chronological order
  int start = (messageCount < MAX_MESSAGES) ? 0 : messageIndex;
  for (int i = 0; i < messageCount; i++) {
    int idx = (start + i) % MAX_MESSAGES;
    if (i > 0) json += ",";
    json += "\"";
    json += serialMessages[idx];
    json += "\"";
  }

  json += "],\"heap\":";
  json += ESP.getFreeHeap();
  json += "}";

  server.send(200, "application/json", json);
}

void handleSend() {
  if (server.hasArg("msg")) {
    String message = server.arg("msg");

    char msgBuffer[MAX_MESSAGE_LENGTH];
    snprintf(msgBuffer, MAX_MESSAGE_LENGTH, "Web> %s", message.c_str());
    addMessage(msgBuffer);

    Serial.print("Web> ");
    Serial.println(message);

    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing message");
  }
}
