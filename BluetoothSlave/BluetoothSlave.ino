#include <M5Atom.h>
#include "BluetoothSerial.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"

BluetoothSerial SerialBT;
String bluetooth_name = "M5Stack_slave";
uint32_t send_start_time = 0;
bool connected_last = false;
uint8_t receive_buffer[50] = {0};

// CPU周波数を最大に設定
void setCpuFrequencyMax() {
    setCpuFrequencyMhz(240);
}

// Bluetoothの設定を最適化
void optimizeBluetooth() {
    // BLEメモリを解放してClassic BTに割り当て
    esp_bt_controller_mem_release(ESP_BT_MODE_BLE);
    
    // Bluetoothコントローラーの設定
    esp_bt_dev_set_device_name(bluetooth_name.c_str());
}

void setup() {
    M5.begin(true, false, true); // シリアル有効、I2C無効、表示有効
    setCpuFrequencyMax();
    
    // Bluetooth初期化と最適化
    optimizeBluetooth();
    SerialBT.begin(bluetooth_name);
    
    uint8_t macBT[6];
    esp_read_mac(macBT, ESP_MAC_BT);
    Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\r\n", macBT[0], macBT[1], macBT[2], macBT[3], macBT[4], macBT[5]);
}

// 高速応答用の関数
void fastRespond(uint8_t* data, int length) {
    // 開始バイトを検出
    if (length > 0 && data[0] == 0xC0) {
        send_start_time = millis();
    }
    
    // 即座に送り返す
    SerialBT.write(data, length);
    
    // 終了バイトを検出
    if (length > 0 && data[length-1] == 0xE0) {
        Serial.printf("comm time : %u\r\n", (millis() - send_start_time));
    }
}

void loop() {
    bool connected = SerialBT.connected();
    if (connected != connected_last) {
        if (connected) {
            Serial.printf("connected : \r\n");
        } else {
            Serial.printf("disconnected : \r\n");
        }
    }
    connected_last = connected;
    
    // 利用可能なデータがある場合
    if (SerialBT.available() > 0) {
        // 利用可能なデータ量を確認
        int available = SerialBT.available();
        
        // データを読み込む（最大50バイト）
        int bytes_read = SerialBT.readBytes(receive_buffer, min(available, 50));
        
        // 高速応答
        fastRespond(receive_buffer, bytes_read);
    }
}