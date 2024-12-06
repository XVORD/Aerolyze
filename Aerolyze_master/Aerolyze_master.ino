#define BLYNK_PRINT Serial
#include <painlessMesh.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
BlynkTimer timer;

#define MESH_SSID "MeshNetworkSSID"
#define MESH_PASSWORD "password123"
#define MESH_PORT 5555
painlessMesh mesh;

#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "SSID";
char pass[] = "";

#define MQ2 34
#define GREEN 18
#define RED 19
int sensorValue = 0;
boolean state = false;

void setup()
{
  // Debug console
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  pinMode(MQ2, INPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  timer.setInterval(1000L, sendUptime);
}

void sendUptime()
{
  
  sensorValue = analogRead(MQ2);
  Blynk.virtualWrite(V1, sensorValue);
  Serial.println(sensorValue);

  if (sensorValue > 600)
  {
    // Blynk.notify("Gas Detected!");
    digitalWrite(GREEN, LOW);
    digitalWrite(RED, HIGH);
  }

  else
  {
    digitalWrite(GREEN, HIGH);
    digitalWrite(RED, LOW);
  }
}

void loop()
{
  Blynk.run();
  timer.run();
}