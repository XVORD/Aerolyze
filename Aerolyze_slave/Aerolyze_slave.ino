#include <painlessMesh.h>
<<<<<<< HEAD
#include <WiFi.h>
#include <HTTPClient.h>
#include <UrlEncode.h>
#include <time.h>

#define LED_PIN 2

#define MESH_SSID "MeshNetworkSSID"
#define MESH_PASSWORD "password123"
#define MESH_PORT "5555"

painlessMesh mesh;

// Kredensial WiFi
const char* ssid = "realmeC15";
const char* password = "realmethebest";

// Dapatkan dari: https://www.electronicwings.com/esp32/send-a-whatsapp-message-using-esp32
String MobileNumber = "6281386847527";
String APIKey = "7493606";
=======

painlessMesh mesh;

#define MESH_SSID "Mesh_Kelompok_10"
#define MESH_PASSWORD "Mesh_Kelompok_10"
#define MESH_PORT 5555

QueueHandle_t MQ2Queue;
>>>>>>> 2bdd886ebd790ca06203862e3743a84a206cc7ad

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
<<<<<<< HEAD
  Serial.printf("Pesan diterima dari Node ID %u: %s\n", from, msg.c_str());
}

// Tugas untuk handle gas berbahaya
void taskHandleGas(void* pvParameters) {
  while (true) {
    if (Serial.available() > 0) {
      String input = Serial.readStringUntil('\n');

      if (input == "ada") {  // Untuk percobaan, nanti menggunakan data yang diterima dari root
        if (!notificationSent && !alarmActivated) {
          notificationSent = true;
          alarmActivated = true;

          activateAlarm();
          sendMessage("Gas berbahaya terdeteksi!");
        }
      } else {
        notificationSent = false;
        alarmActivated = false;

        if (!xTimerIsTimerActive(xTimeoutTimer)) {  // Jika timer tidak aktif
          xTimerStart(xTimeoutTimer, 0);            // Memulai timer
          Serial.println("\nMemulai timer untuk mematikan alarm!");
        }
      }
    }
  }
}

// Fungsi untuk menghidupkan LED dan buzzer
void activateAlarm() {
  digitalWrite(LED_PIN, HIGH);
  //digitalWrite(BUZZER_PIN, HIGH);
  Serial.println("ALARM: Gas berbahaya terdeteksi!");
}

// Fungsi untuk mengirimkan notifikasi peringatan ke WhatsApp
void sendMessage(String message) {
  String url = "https://api.callmebot.com/whatsapp.php?phone=" + MobileNumber + "&apikey=" + APIKey + "&text=" + urlEncode(message);
  HTTPClient http;
  http.begin(url);

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200) {
    Serial.println("Berhasil mengirimkan notifikasi");
  } else {
    Serial.println("Terjadi kesalahan saat mengirimkan notifikasi");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

// Fungsi callback milik timer
void vTimeoutCallback(TimerHandle_t xTimer) {
  if (xTaskHandleSafe == NULL) {
    xTaskCreate(taskHandleSafe, "Handle Safe", 4096, NULL, 1, &xTaskHandleSafe);
  }
}

// Tugas untuk mematikan alarm
void taskHandleSafe(void* pvParameters) {
  Serial.println("Gas berbahaya tidak terdeteksi. Mematikan alarm!");

  digitalWrite(LED_PIN, LOW);
  // digitalWrite(BUZZER_PIN, LOW);

  sendMessage("Gas berbahaya sudah tidak terdeteksi!");

  // Menghapus task saat selesai
  xTaskHandleSafe = NULL;
  vTaskDelete(NULL);
=======
  Serial.printf("Received from %u: %s\n", from, msg.c_str());
>>>>>>> 2bdd886ebd790ca06203862e3743a84a206cc7ad
}