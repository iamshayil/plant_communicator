// Libraries
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <BlynkSimpleEsp8266.h>


// WiFi credentials
const char* ssid = "";
const char* password = "";

//blynk credentials
char auth[] = "";
char blynk_template_id[] = "";
char blynk_device_name[] = "";


// Soil Moisture,relay  pin
const int moisturePin = A0;
const int relayPin = D1;


// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,19800);



void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set relay pin as output
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);

  // Initialize a NTPClient to get time
  timeClient.begin();

  //initialize blynk
  Blynk.begin(auth, ssid, password);
}

bool manualControl = false;

BLYNK_WRITE(V1) {
  if (param.asInt()) {
    digitalWrite(relayPin,LOW );
    manualControl = true;
  } else {
    digitalWrite(relayPin, HIGH);
    manualControl = false;
  }
}


void loop() {
  // Update the NTP client and get the current time
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();

  // Set the desired hour, minute, and second for the relay module to turn on
  int relayOnHour = 11; //  hours
  int relayOnMinute = 19; //  minutes
  int relayOnSecond = 0; // seconds
  

  // Read the analog value from the soil moisture sensor
  int moistureValue = analogRead(moisturePin);

 
   //send moisture data to blynk
   Blynk.virtualWrite(V0, moistureValue);
   Blynk.virtualWrite(V1, relayPin);

  // Turn on the LED if the moisture value is below a threshold
  if (!manualControl) {
if (moistureValue < 400 || currentHour == relayOnHour && currentMinute == relayOnMinute && currentSecond == relayOnSecond) {
    digitalWrite(relayPin, LOW);
    delay(5000);
    digitalWrite(relayPin, HIGH);
} else {
    digitalWrite(relayPin, HIGH);
}
  }
  // Print the scaled moisture value to the Serial Monitor
  Serial.print(" Soil Moisture: ");
  Serial.println(moistureValue);

  Blynk.run();

}
