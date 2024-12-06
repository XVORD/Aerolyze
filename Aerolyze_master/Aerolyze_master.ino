#include <painlessMesh.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <painlessMesh.h>

painlessMesh mesh;

#define MESH_SSID "MeshNetworkSSID"
#define MESH_PASSWORD "password123"
#define MESH_PORT 5555

QueueHandle_t MQ2Queue;

void sendUptime();
void taskMQ2(void *pvParameters);
void receivedCallback(uint32_t from, String &msg);
void sendMessage();

#define MESH_PREFIX "meshNetwork"
#define MESH_PASSWORD "meshpassword"
#define MESH_PORT 5555

#define MQ2 34
#define GREEN 18
#define RED 19

int sensorValue = 0;
boolean state = false;

struct GasData {
  float gasvalue;
};

void setup() {
  Serial.begin(115200);

  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT);
  mesh.onReceive(&receivedCallback);

  pinMode(MQ2, INPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);

  MQ2Queue = xQueueCreate(5, sizeof(GasData));
}

void loop() {
  mesh.update();
}

void sendUptime() {
  sensorValue = analogRead(MQ2);
  Serial.println(sensorValue);

  if (sensorValue > 600) {
    digitalWrite(GREEN, LOW);
    digitalWrite(RED, HIGH);
  } else {
    digitalWrite(GREEN, HIGH);
    digitalWrite(RED, LOW);
  }

  if (xQueueSend(MQ2Queue, &sensorValue, pdMS_TO_TICKS(10)) != pdPASS) {
    Serial.println("Failed to send MQ2 data to queue");
  } else {
    Serial.println("MQ2 data sent to queue successfully!");
  }

  sendMessage();
}

void sendMessage() {
  String msg = String(sensorValue);
  while (1) {
    // Menerima data dari queue
    if (xQueueReceive(MQ2Queue, &sensorValue, portMAX_DELAY) == pdTRUE) { 
      mesh.sendBroadcast(msg);
      Serial.printf("Broadcast message: %s\n", msg.c_str());
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received from %u: %s\n", from, msg.c_str());
}
