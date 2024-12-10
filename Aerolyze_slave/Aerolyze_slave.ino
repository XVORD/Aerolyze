#include <painlessMesh.h>

painlessMesh mesh;

#define MESH_SSID "Mesh_Kelompok_10"
#define MESH_PASSWORD "Mesh_Kelompok_10"
#define MESH_PORT 5555

QueueHandle_t MQ2Queue;

void MQ2task(void* pvParameters);
void receivedCallback(uint32_t from, String& msg);
void MeshTask(void* pvParameters);

#define MQ2 34
#define GREEN 18
#define RED 19

struct GasData {
  float gasvalue;
};

void setup() {
  Serial.begin(115200);

  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION);
  mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
  mesh.onReceive(&receivedCallback);

  pinMode(MQ2, INPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);

  MQ2Queue = xQueueCreate(5, sizeof(GasData));
  if (MQ2Queue == NULL) {
    Serial.println("Failed to create MQ2 queue");
    while (1)
      ;
  }

  xTaskCreate(MQ2task, "MQ2 Task", 4096, NULL, 1, NULL);
  xTaskCreate(MeshTask, "Mesh Task", 4096, NULL, 1, NULL);
}

void loop() {
  mesh.update();
}

void MQ2task(void* pvParameters) {
  while (1) {
    GasData gasData;
    gasData.gasvalue = analogRead(MQ2);
    Serial.printf("MQ2 Sensor Value: %.2f\n", gasData.gasvalue);

    if (gasData.gasvalue > 1200) {
      digitalWrite(GREEN, LOW);
      digitalWrite(RED, HIGH);
    } else {
      digitalWrite(GREEN, HIGH);
      digitalWrite(RED, LOW);
    }

    if (xQueueSend(MQ2Queue, &gasData, pdMS_TO_TICKS(10)) != pdPASS) {
      Serial.println("Failed to send MQ2 data to queue");
    } else {
      Serial.println("MQ2 data sent to queue successfully!");
    }

    vTaskDelay(5000 / portTICK_PERIOD_MS);  // Delay to prevent overwhelming the task
  }
}

void MeshTask(void* pvParameters) {
  GasData receivedData;
  while (1) {
    if (xQueueReceive(MQ2Queue, &receivedData, portMAX_DELAY) == pdTRUE) {
        String msg = String(receivedData.gasvalue);
        mesh.sendBroadcast(msg);
        Serial.printf("Broadcast message: %s\n", msg.c_str());
    }
    vTaskDelay(5000 / portTICK_PERIOD_MS);  // Delay to control broadcast frequency
  }
}

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received from %u: %s\n", from, msg.c_str());
}