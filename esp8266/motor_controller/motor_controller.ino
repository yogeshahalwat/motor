/*
 * Motor Controller — ESP8266 R&D Version
 *
 * Connects to Firebase via WiFi. Reads motor command from the Flutter app,
 * controls the built-in LED (simulates motor relay), writes status back.
 *
 * The Flutter app doesn't change at all — it reads/writes the same Firebase path.
 *
 * Built-in LED on NodeMCU = GPIO2 (active LOW: LOW = LED ON, HIGH = LED OFF)
 *
 * Setup: Copy config_example.h to config.h and fill in your WiFi + Firebase credentials.
 */

#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#include "config.h"

#define DEVICE_PATH   "devices/test_device_001"
#define LED_PIN       2       // NodeMCU built-in LED (GPIO2), active LOW
#define POLL_INTERVAL 5000    // Read Firebase every 5 seconds

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig firebaseConfig;

bool signupOK = false;
unsigned long lastPoll = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("=== Motor Controller ESP8266 R&D ===");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // LED OFF initially

  // --- Connect to WiFi ---
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // Blink LED while connecting
  }
  Serial.println();
  Serial.print("WiFi connected! IP: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_PIN, HIGH); // LED OFF after connected

  // --- Configure Firebase ---
  firebaseConfig.api_key = FIREBASE_API_KEY;
  firebaseConfig.database_url = FIREBASE_DB_URL;

  // Anonymous sign-in (empty email + password = anonymous)
  Serial.print("Connecting to Firebase...");
  if (Firebase.signUp(&firebaseConfig, &auth, "", ""))
  {
    Serial.println(" OK!");
    signupOK = true;
  }
  else
  {
    Serial.print(" FAILED: ");
    Serial.println(firebaseConfig.signer.signupError.message.c_str());
  }

  firebaseConfig.token_status_callback = tokenStatusCallback;
  Firebase.begin(&firebaseConfig, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Setup complete. Polling Firebase every 5 seconds...");
  Serial.println();
}

void loop()
{
  if (!Firebase.ready() || !signupOK)
  {
    Serial.println("Firebase not ready, waiting...");
    delay(1000);
    return;
  }

  if (millis() - lastPoll < POLL_INTERVAL)
    return;
  lastPoll = millis();

  // ---- READ command from Firebase ----
  String command = "OFF";
  if (Firebase.RTDB.getString(&fbdo, String(DEVICE_PATH) + "/command"))
  {
    command = fbdo.stringData();
  }
  else
  {
    Serial.print("Read failed: ");
    Serial.println(fbdo.errorReason());
    return; // Skip writing if we can't read
  }

  // ---- Control LED (simulates motor relay) ----
  bool motorOn = (command == "ON");
  digitalWrite(LED_PIN, motorOn ? LOW : HIGH); // Active LOW: LOW = LED ON

  // ---- WRITE status back to Firebase ----
  String path = String(DEVICE_PATH);
  Firebase.RTDB.setBool(&fbdo, path + "/motor_running", motorOn);
  Firebase.RTDB.setBool(&fbdo, path + "/fault", false);
  Firebase.RTDB.setBool(&fbdo, path + "/all_ok", true);

  // Server timestamp so the app can show "Device Online" / "Last seen: Xs ago"
  Firebase.RTDB.setTimestamp(&fbdo, path + "/last_seen");

  // ---- Print to Serial Monitor (for debugging) ----
  Serial.print("Command: ");
  Serial.print(command);
  Serial.print(" | LED: ");
  Serial.print(motorOn ? "ON" : "OFF");
  Serial.println(" | Status written to Firebase");
}
