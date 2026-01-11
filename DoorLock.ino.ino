#include <WiFi.h>
#include <WebServer.h>

// ========= WIFI =========
const char* WIFI_SSID = "Osai - Gerencia";
const char* WIFI_PASS = "gerencia54321";

// ========= SEGURIDAD SIMPLE (LAN) =========
const char* OPEN_TOKEN = "doorlock";
constexpr unsigned long COOLDOWN_MS = 7000;

// ========= PINES =========
constexpr int RELAY_PIN = 22;   // IN1 del relé (active LOW)
constexpr int LED_PIN   = 2;    // LED interno (active LOW)
constexpr int PULSE_MS  = 300;  // duración del "apriete"

// ========= SERVER =========
WebServer server(80);
unsigned long lastOpenMs = 0;

void relaySet(bool on) {  // on=true => relé activado
  digitalWrite(RELAY_PIN, on ? LOW : HIGH);
}
void ledSet(bool on) {    // on=true => LED encendido
  digitalWrite(LED_PIN, on ? LOW : HIGH);
}

bool tokenOkFromArg() {
  if (!server.hasArg("token")) return false;
  return server.arg("token") == OPEN_TOKEN;
}

bool tokenOkFromBody() {
  if (!server.hasArg("token")) return false;
  return server.arg("token") == OPEN_TOKEN;
}

bool cooldownOk() {
  unsigned long now = millis();
  if (now - lastOpenMs < COOLDOWN_MS) return false;
  lastOpenMs = now;
  return true;
}

void pulseGate() {
  Serial.println("[ACTION] Activando relé (simulando pulsador)");
  ledSet(true);
  relaySet(true);
  delay(PULSE_MS);
  relaySet(false);
  ledSet(false);
  Serial.println("[ACTION] Relé desactivado");
}

String htmlPage() {
  // Nota: mostramos la IP actual por DHCP en runtime
  String ip = WiFi.localIP().toString();

  String page =
    "<!doctype html><html><head>"
    "<meta charset='utf-8'/>"
    "<meta name='viewport' content='width=device-width, initial-scale=1'/>"
    "<title>Puerta</title>"
    "<style>"
    "body{font-family:Arial, sans-serif; margin:0; padding:24px; background:#0b0f14; color:#e7eef7;}"
    ".card{max-width:520px; margin:0 auto; background:#121925; padding:20px; border-radius:16px; box-shadow:0 8px 30px rgba(0,0,0,.35);}"
    "h1{margin:0 0 10px 0; font-size:22px;}"
    "p{margin:6px 0 14px 0; color:#b8c4d3;}"
    "input{width:100%; padding:14px; border-radius:12px; border:1px solid #2a3a52; background:#0b0f14; color:#e7eef7; font-size:16px;}"
    "button{width:100%; margin-top:12px; padding:16px; border-radius:14px; border:0; background:#2b7cff; color:white; font-size:18px; font-weight:700;}"
    "button:active{transform:scale(.99)}"
    ".small{font-size:12px; opacity:.85; margin-top:12px;}"
    "</style></head><body>"
    "<div class='card'>"
    "<h1>Control de Puerta (LAN - DHCP)</h1>"
    "<p>ESP32 IP: " + ip + " — Relé en GPIO22</p>"
    "<form method='POST' action='/open'>"
    "<label>Token</label>"
    "<input name='token' type='password' placeholder='Ingresá tu token' required />"
    "<button type='submit'>ABRIR</button>"
    "</form>"
    "<div class='small'>Estado: <a href='/health' style='color:#9cc6ff'>/health</a></div>"
    "</div></body></html>";

  return page;
}

void handleRoot() {
  server.send(200, "text/html", htmlPage());
}

void handleHealth() {
  server.send(200, "text/plain", "OK");
}

void handleOpenGET() {
  if (!tokenOkFromArg()) {
    Serial.println("[DENY] GET token invalido o faltante");
    server.send(401, "text/plain", "Unauthorized");
    return;
  }
  if (!cooldownOk()) {
    Serial.println("[BLOCK] Cooldown activo");
    server.send(429, "text/plain", "Too Many Requests (cooldown)");
    return;
  }
  Serial.println("[EVENT] Apertura solicitada (GET)");
  pulseGate();
  server.send(200, "text/plain", "OPENED");
}

void handleOpenPOST() {
  if (!tokenOkFromBody()) {
    Serial.println("[DENY] POST token invalido o faltante");
    server.send(401, "text/plain", "Unauthorized");
    return;
  }
  if (!cooldownOk()) {
    Serial.println("[BLOCK] Cooldown activo");
    server.send(429, "text/plain", "Too Many Requests (cooldown)");
    return;
  }
  Serial.println("[EVENT] Apertura solicitada (POST)");
  pulseGate();

  server.sendHeader("Location", "/");
  server.send(303);
}

void wifiConnectedBlink() {
  // 3 parpadeos cortos
  for (int i = 0; i < 3; i++) {
    ledSet(true);
    delay(150);
    ledSet(false);
    delay(150);
  }

  // pausa breve
  delay(400);

  // 1 parpadeo largo
  ledSet(true);
  delay(800);
  ledSet(false);
}


void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  relaySet(false);
  ledSet(false);

  Serial.begin(115200);
  delay(300);

  Serial.println("=================================");
  Serial.println("ESP32 Gate Control - LAN (DHCP)");
  Serial.println("GPIO22 -> Relé (active LOW)");
  Serial.println("GPIO2  -> LED interno (active LOW)");
  Serial.println("=================================");

  WiFi.mode(WIFI_STA);

  // ✅ DHCP: NO llamamos WiFi.config(...)
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("[WIFI] Conectando");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    ledSet(true); delay(120);
    ledSet(false); delay(280);
  }

  Serial.println("\n[WIFI] Conectado!");
  Serial.print("[WIFI] IP: ");
  Serial.println(WiFi.localIP());
  //Señal visual de conexión exitosa
  wifiConnectedBlink();


  server.on("/", handleRoot);
  server.on("/health", HTTP_GET, handleHealth);
  server.on("/open", HTTP_GET, handleOpenGET);
  server.on("/open", HTTP_POST, handleOpenPOST);

  server.begin();
  Serial.println("[HTTP] Server iniciado en puerto 80");

  Serial.print("[WIFI] Gateway: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("[WIFI] Subnet: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("[WIFI] DNS: ");
  Serial.println(WiFi.dnsIP());
}

void loop() {
  server.handleClient();

  static unsigned long lastHb = 0;
  if (millis() - lastHb >= 10000) {
    lastHb = millis();
    Serial.println("[HEARTBEAT] vivo");
  }
}
