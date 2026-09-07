#include <Wire.h>              // Enables I2C communication
#include <BH1750.h>            // Library for BH1750 light sensor
#include <WiFiNINA.h>          // Enables WiFi connectivity
#include <ArduinoHttpClient.h> // Enables HTTP requests to IFTTT


// ======================================================
//                    WiFi SETTINGS
// ======================================================

// WiFi network name and password
char ssid[] = "AM";
char pass[] = "12345678";


// ======================================================
//                    IFTTT SETTINGS
// ======================================================

// IFTTT server address
const char IFTTT_HOST[] = "maker.ifttt.com";

// IFTTT Webhooks key used to trigger events
const char IFTTT_KEY[] = "kQUQoMKkkZ0k05I11Ht5oJCwEjN_RhnY60wQwwJyKei";


// ======================================================
//                  BH1750 SENSOR
// ======================================================

// Create an object for the BH1750 light sensor
BH1750 lightMeter;

// Light level threshold in lux.
// If the measured light is 500 lux or more,
// the system considers it to be sunlight.
const float LIGHT_THRESHOLD = 500.0;


// Stores the current sunlight state.
// false = sunlight is not detected
// true  = sunlight is currently detected
bool sunlightDetected = false;


// ======================================================
//                NETWORK CLIENT SETUP
// ======================================================

// WiFiSSLClient provides a secure HTTPS connection
WiFiSSLClient wifi;

// HTTP client used to communicate with the IFTTT server
HttpClient client(wifi, IFTTT_HOST, 443);


// ======================================================
//              SEND IFTTT NOTIFICATION
// ======================================================

// This function sends the current light level to IFTTT.
// eventName determines which IFTTT event will be triggered.
void sendIFTTT(String eventName, float lux) {

  // Create the IFTTT Webhooks URL.
  // The measured lux value is sent as value1.
  String path = "/trigger/" + eventName +
                "/with/key/" + IFTTT_KEY +
                "?value1=" + String(lux, 2);


  // Display information in Serial Monitor
  Serial.println();
  Serial.println("Sending IFTTT notification...");

  Serial.print("Event: ");
  Serial.println(eventName);

  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lux");

  Serial.println("Connecting to IFTTT...");


  // Send an HTTPS GET request to IFTTT
  int statusCode = client.get(path);


  // Display the HTTP response status code
  // 200 normally means that the request was successful.
  Serial.print("IFTTT Status Code: ");
  Serial.println(statusCode);


  // Read and display the response received from IFTTT
  String response = client.responseBody();

  Serial.print("IFTTT Response: ");
  Serial.println(response);

  Serial.println("Notification process finished.");
}


// ======================================================
//                        SETUP
// ======================================================

void setup() {

  // Start Serial communication for debugging
  // and displaying sensor/network information.
  Serial.begin(9600);

  // Give the board time to initialise
  delay(2000);


  // Start I2C communication for the BH1750 sensor
  Wire.begin();

  Serial.println("Starting BH1750...");


  // Initialise the BH1750 sensor.
  // If the sensor cannot be detected, stop the program.
  if (!lightMeter.begin()) {

    Serial.println("BH1750 ERROR!");

    // Stop here if the sensor is not working
    while (1);
  }

  Serial.println("BH1750 OK!");


  // ====================================================
  //                     WiFi CONNECTION
  // ====================================================

  Serial.println("Connecting to WiFi...");


  // Keep trying until the board successfully connects
  // to the configured WiFi network.
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {

    Serial.println("WiFi connection failed. Trying again...");

    // Wait 5 seconds before trying again
    delay(5000);
  }


  // WiFi connection was successful
  Serial.println("WiFi connected!");

  // Display the IP address assigned to the Arduino
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("System ready!");
}


// ======================================================
//                         LOOP
// ======================================================

void loop() {

  // Read the current light intensity from the BH1750.
  // The result is measured in lux.
  float lux = lightMeter.readLightLevel();


  // Display the current light level
  Serial.print("Light level: ");
  Serial.print(lux);
  Serial.println(" lux");


  // ====================================================
  //              SUNLIGHT STARTED CONDITION
  // ====================================================

  // If the light level reaches or exceeds 500 lux
  // AND sunlight was previously not detected,
  // trigger the "sunlight_started" IFTTT event.
  if (lux >= LIGHT_THRESHOLD && sunlightDetected == false) {

    Serial.println("Sunlight detected!");


    // Send notification with the current lux value
    sendIFTTT("sunlight_started", lux);


    // Update the state so the notification is not
    // repeatedly sent while the light remains above
    // the threshold.
    sunlightDetected = true;
  }


  // ====================================================
  //              SUNLIGHT STOPPED CONDITION
  // ====================================================

  // If the light level falls below 500 lux
  // AND sunlight was previously detected,
  // trigger the "sunlight_stopped" IFTTT event.
  if (lux < LIGHT_THRESHOLD && sunlightDetected == true) {

    Serial.println("Sunlight stopped!");


    // Send notification with the current lux value
    sendIFTTT("sunlight_stopped", lux);


    // Update the state because sunlight is no longer detected.
    sunlightDetected = false;
  }


  // Wait 5 seconds before taking the next reading.
  // This prevents the sensor and network from being
  // queried continuously.
  delay(5000);
}
