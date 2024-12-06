#include <WiFi.h>
#include <esp_now.h>

// Struktur data untuk komunikasi
typedef struct {
    char gasStatus[10]; // "ada" atau "tidak ada"
} GasData;

GasData gasData; // Variabel untuk menyimpan data yang diterima atau dikirim

// MAC Address Master Node
uint8_t masterMacAddress[] = {0x24, 0x6F, 0x28, 0xXX, 0xXX, 0xXX}; // Ganti dengan MAC Master Node

// Fungsi callback untuk menerima data dari Slave Node
void onDataRecv(const uint8_t *macAddr, const uint8_t *incomingData, int len) {
    memcpy(&gasData, incomingData, sizeof(gasData));

    Serial.print("Data diterima dari Slave: ");
    Serial.println(gasData.gasStatus);

    // Periksa apakah gas terdeteksi
    if (strcmp(gasData.gasStatus, "ada") == 0) {
        Serial.println("PERINGATAN: Gas berbahaya terdeteksi!");
        sendDataToMaster("ada");
    } else {
        Serial.println("Status Gas: Aman");
        sendDataToMaster("tidak ada");
    }
}

// Fungsi callback untuk konfirmasi pengiriman data
void onDataSent(const uint8_t *macAddr, esp_now_send_status_t status) {
    Serial.print("Pengiriman data ke Master ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "BERHASIL" : "GAGAL");
}

// Fungsi untuk mengirim data ke Master Node
void sendDataToMaster(const char *status) {
    strcpy(gasData.gasStatus, status); // Perbarui status gas
    esp_now_send(masterMacAddress, (uint8_t *)&gasData, sizeof(gasData));
}

void setup() {
    // Inisialisasi serial monitor
    Serial.begin(115200);

    // Inisialisasi WiFi dalam mode Station
    WiFi.mode(WIFI_STA);

    // Inisialisasi ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Mendaftarkan callback untuk menerima data
    esp_now_register_recv_cb(onDataRecv);

    // Mendaftarkan callback untuk mengonfirmasi pengiriman data
    esp_now_register_send_cb(onDataSent);

    // Tambahkan Master Node sebagai peer
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, masterMacAddress, 6);
    peerInfo.channel = 0; // Gunakan channel default
    peerInfo.encrypt = false; // Non-enkripsi

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Gagal menambahkan Master Node sebagai peer");
        return;
    }

    Serial.println("Leaf Node siap menerima data dari Slave Node");
}

void loop() {
    // Tidak diperlukan operasi dalam loop
}
