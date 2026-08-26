#include <Wire.h>
#include <BH1750.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>

// ---------- WiFi ----------
char ssid[] = "AM";
char pass[] = "12345678";

// ---------- IFTTT ----------
const char IFTTT_HOST[] = "maker.ifttt.com";
const char IFTTT_KEY[] = "kQUQoMKkkZ0k05I11Ht5oJCwEjN_RhnY60wQwwJyKei";

// ---------- BH1750 ----------
BH1750 lightMeter;

const float LIGHT_THRESHOLD = 500.0;

bool sunlightDetected = false;

WiFiSSLClient wifi;
HttpClient client(wifi, IFTTT_HOST, 443);


// SEND IFTTT NOTIFICATION


void sendIFTTT(String eventName, float lux) {

  String path = "/trigger/" + eventName +
                "/with/key/" + IFTTT_KEY +
                "?value1=" + String(lux, 2);

  Serial.println();
  Serial.println("Sending IFTTT notification...");
  Serial.print("Event: ");
  Serial.println(eventName);
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lux");

  Serial.println("Connecting to IFTTT...");

  int statusCode = client.get(path);

  Serial.print("IFTTT Status Code: ");
  Serial.println(statusCode);

  String response = client.responseBody();

  Serial.print("IFTTT Response: ");
  Serial.println(response);

  Serial.println("Notification process finished.");
}



// SETUP


void setup() {

  Serial.begin(9600);
  delay(2000);

  Wire.begin();

  Serial.println("Starting BH1750...");

  if (!lightMeter.begin()) {

    Serial.println("BH1750 ERROR!");

    while (1);
  }

  Serial.println("BH1750 OK!");

  // ---------- WiFi ----------

  Serial.println("Connecting to WiFi...");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {

    Serial.println("WiFi connection failed. Trying again...");

    delay(5000);
  }

  Serial.println("WiFi connected!");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("System ready!");
}


// LOOP


void loop() {

  float lux = lightMeter.readLightLevel();

  Serial.print("Light level: ");
  Serial.print(lux);
  Serial.println(" lux");


  // ---------- SUNLIGHT STARTED ----------

  if (lux >= LIGHT_THRESHOLD && sunlightDetected == false) {

    Serial.println("Sunlight detected!");

    sendIFTTT("sunlight_started", lux);

    sunlightDetected = true;
  }


  // ---------- SUNLIGHT STOPPED ----------

  if (lux < LIGHT_THRESHOLD && sunlightDetected == true) {

    Serial.println("Sunlight stopped!");

    sendIFTTT("sunlight_stopped", lux);

    sunlightDetected = false;
  }


  delay(5000);
}