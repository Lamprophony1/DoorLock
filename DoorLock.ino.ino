// ESP32 NodeMCU + Relay 2CH (LOW level trigger)
// LED interno (GPIO2) + Logs por Serial

constexpr int RELAY_PIN = 22;   // IN1 del relé
constexpr int LED_PIN   = 2;    // LED interno (activo LOW)
constexpr int PULSE_MS  = 300;  // duración del "apriete"
constexpr unsigned long PULSE_INTERVAL = 10000; // 10 segundos

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH); // relé OFF (active LOW)
  digitalWrite(LED_PIN, HIGH);   // LED OFF (active LOW)

  Serial.begin(115200);
  delay(500); // tiempo para abrir el monitor serie

  Serial.println("=================================");
  Serial.println("ESP32 iniciado correctamente");
  Serial.println("GPIO23 -> Relé (active LOW)");
  Serial.println("GPIO2  -> LED interno (active LOW)");
  Serial.println("Esperando eventos...");
  Serial.println("=================================");
}

void pulseGate() {
  Serial.println("[ACTION] Activando relé (simulando pulsador)");
  
  digitalWrite(RELAY_PIN, LOW);  // relé ON
  digitalWrite(LED_PIN, LOW);    // LED ON

  delay(PULSE_MS);

  digitalWrite(RELAY_PIN, HIGH); // relé OFF
  digitalWrite(LED_PIN, HIGH);   // LED OFF

  Serial.println("[ACTION] Relé desactivado");
}

void loop() {
  // Heartbeat
  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat >= 1000) {
    lastHeartbeat = millis();
    Serial.println("[HEARTBEAT] ESP32 vivo");
  }

  // Disparo del portón
  static unsigned long lastPulse = 0;
  if (millis() - lastPulse >= PULSE_INTERVAL) {
    lastPulse = millis();
    Serial.println("[EVENT] Tiempo cumplido → disparo del portón");
    pulseGate();
  }
}
