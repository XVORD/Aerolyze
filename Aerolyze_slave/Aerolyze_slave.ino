#include <painlessMesh.h>
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

// Deklarasi fungsi
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);  // Fungsi callback saat WiFi terhubung
void taskHandleGasDetected(void* pvParameters);                      // Task untuk mengirimkan notifikasi serta menghidupkan alarm
void activateAlarm();                                                // Fungsi untuk menghidupkan LED dan buzzer
void sendMessage(String message);                                    // Fungsi untuk mengirimkan notifikasi peringatan ke WhatsApp
void vTimeoutCallback(TimerHandle_t xTimer);                         // Fungsi callback saat timer selesai
void taskHandleSafe(void* pvParameters);
void receivedCallback(uint32_t from, String &msg); // Tambahan: Callback untuk menerima pesan mesh

const int timeoutPeriod = 5000;  // Periode timeout = 60 detik (5 detik untuk percobaan)
TimerHandle_t xTimeoutTimer;     // Timeout timer handle
TaskHandle_t xTaskHandleSafe;    // Handle untuk taskHandleSafe

// Flag untuk memastikan notifikasi dan alarm hanya sekali
bool notificationSent = false;
bool alarmActivated = false;

void setup() {
  Serial.begin(115200);  // Memulai komunikasi serial

  pinMode(LED_PIN, OUTPUT);  // Inisialisasi LED_PIN sebagai output

  // Mendaftarkan WiFi event yang digunakan
  WiFi.onEvent(WiFiStationConnected, ARDUINO_EVENT_WIFI_STA_CONNECTED);

  // Memulai koneksi WiFi
  Serial.println("Menghubungkan ke WiFi dengan SSID: " + String(ssid));
  WiFi.begin(ssid, password);

  // Inisialisasi Mesh
  mesh.init(MESH_SSID, MESH_PASSWORD, &MESH_PORT);
  mesh.onReceive(receivedCallback);

  // Membuat timer
  xTimeoutTimer = xTimerCreate("Timeout Timer", timeoutPeriod, pdFALSE, 0, vTimeoutCallback);

  xTaskCreate(taskHandleGas, "Handle Gas", 4096, NULL, 1, NULL);  // Membuat tugas untuk handle gas berbahaya
}

void loop() {
  mesh.update();// Tidak digunakan
}

// WiFi Event untuk mencetak informasi bahwa koneksi WiFi berhasil
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("Berhasil terhubung dengan " + String(ssid));
}

void receivedCallback(uint32_t from, String &msg) {
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
}