#include <painlessMesh.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <UrlEncode.h>
#include <time.h>

#define LED_PIN 5
#define BUZZER_PIN 4

#define MESH_SSID "Mesh_Kelompok_10"
#define MESH_PASSWORD "Mesh_Kelompok_10"
#define MESH_PORT 5555

painlessMesh mesh;

// Kredensial WiFi
const char* ssid = "";
const char* password = "";

// Dapatkan dari: https://www.electronicwings.com/esp32/send-a-whatsapp-message-using-esp32
String MobileNumber = "";
String APIKey = "";

// Deklarasi fungsi
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);  // Fungsi callback saat WiFi terhubung
void taskHandleGas(void* pvParameters);                              // Task untuk mengirimkan notifikasi serta menghidupkan alarm
void activateAlarm();                                                // Fungsi untuk menghidupkan LED dan buzzer
void sendMessage(String message);                                    // Fungsi untuk mengirimkan notifikasi peringatan ke WhatsApp
void vTimeoutCallback(TimerHandle_t xTimer);                         // Fungsi callback saat timer selesai
void taskHandleSafe(void* pvParameters);                             // Fungsi untuk mematikan LED dan buzzer
void receivedCallback(uint32_t from, String& msg);                   // Fungsi callback untuk menerima pesan mesh

const int timeoutPeriod = 5000;      // Periode timeout = 60 detik (5 detik untuk percobaan)
int gasValue = 0;                    // Menyimpan data yang diterima dari leaf node dalam bentuk integer
TaskHandle_t xTaskHandleGas = NULL;  // Handle untuk taskHandleGas
TimerHandle_t xTimeoutTimer;         // Timeout timer handle
TaskHandle_t xTaskHandleSafe;        // Handle untuk taskHandleSafe

bool alarmActivated = false;    // Status alarm saat ini
bool notificationSent = false;  // Status notifikasi saat ini
int previousGasState = 0;       // 0 untuk normal, 1 untuk berbahaya

void setup() {
  Serial.begin(115200);  // Memulai komunikasi serial

  pinMode(LED_PIN, OUTPUT);     // Inisialisasi LED_PIN sebagai output
  pinMode(BUZZER_PIN, OUTPUT);  // Inisialisasi BUZZER_PIN sebagai output

  // Mendaftarkan WiFi event yang digunakan
  WiFi.onEvent(WiFiStationConnected, ARDUINO_EVENT_WIFI_STA_CONNECTED);

  // Memulai koneksi WiFi
  Serial.println("Menghubungkan ke WiFi dengan SSID: " + String(ssid));
  WiFi.begin(ssid, password);

  // Inisialisasi Mesh
  mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
  mesh.onReceive(receivedCallback);

  // Membuat timer
  xTimeoutTimer = xTimerCreate("Timeout Timer", timeoutPeriod, pdFALSE, 0, vTimeoutCallback);

  xTaskCreate(taskHandleGas, "Handle Gas", 4096, NULL, 1, &xTaskHandleGas);  // Membuat tugas untuk handle gas berbahaya
}

void loop() {
  mesh.update();
}

// WiFi Event untuk mencetak informasi bahwa koneksi WiFi berhasil
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("Berhasil terhubung dengan " + String(ssid));
}

void receivedCallback(uint32_t from, String& msg) {
  String gasStr = msg.substring(0, 5);  // Mengambil data yang diterima dari leaf node
  gasValue = atoi(gasStr.c_str());      // Konversi nilai gas ke integer

  // Menerapkan priority inversion
  if (gasValue > 1300) {
    Serial.printf("Pesan diterima dari Node ID %u: %s\n", from, msg.c_str());
    vTaskPrioritySet(xTaskHandleGas, 2);  // Prioritas tugas handle gas dinaikkan
  } else {
    vTaskPrioritySet(xTaskHandleGas, 1);  // Prioritas tugas handle gas dikembalikan ke awal
  }
}

// Tugas untuk handle gas berbahaya
void taskHandleGas(void* pvParameters) {
  while (true) {
    // Jika gas dalam kondisi berbahaya
    if (gasValue > 1200) {
      if (previousGasState == 0) {                 // Jika sebelumnya kondisi normal
        activateAlarm();                           // Aktifkan LED dan buzzer
        sendMessage("Gas berbahaya terdeteksi!");  // Kirim notifikasi
        notificationSent = true;
        alarmActivated = true;
        previousGasState = 1;  // Ubah status menjadi berbahaya
      }
    } else {
      // Jika gas kembali normal
      if (previousGasState == 1) {
        if (!xTimerIsTimerActive(xTimeoutTimer)) {  // Jika timer tidak aktif
          xTimerStart(xTimeoutTimer, 0);            // Memulai timer
          Serial.println("\nMemulai timer untuk mematikan alarm!");
        }                                                // Jika sebelumnya dalam kondisi berbahaya
        sendMessage("Gas berbahaya tidak terdeteksi!");  // Kirim notifikasi kembali normal
        notificationSent = false;
        alarmActivated = false;
        previousGasState = 0;  // Ubah status menjadi normal
      }
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);  // Delay untuk efisiensi loop
  }
}


// Fungsi untuk menghidupkan LED dan buzzer
void activateAlarm() {
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  Serial.println("ALARM: Gas berbahaya terdeteksi!");
}

// Fungsi untuk mengirimkan notifikasi peringatan ke WhatsApp
void sendMessage(String message) {
  String url = "https://api.callmebot.com/whatsapp.php?phone=" + MobileNumber + "&text=" + urlEncode(message) + "&apikey=" + APIKey;
  HTTPClient http;
  http.begin(url, true);

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.setTimeout(5000);  // Timeout 5 detik

  int httpResponseCode = http.POST(url);
  //delay(2000);
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
  digitalWrite(BUZZER_PIN, LOW);

  sendMessage("Gas berbahaya sudah tidak terdeteksi!");

  // Menghapus task saat selesai
  xTaskHandleSafe = NULL;
  vTaskDelete(NULL);
}