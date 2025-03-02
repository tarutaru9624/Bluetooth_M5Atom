#include <M5Atom.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
String bluetooth_name = "M5Stack_slave";
uint32_t send_start_time = 0;
bool connected_last = false;     //!< 前回接続フラグ

void setup() {

    uint8_t macBT[6];

    M5.begin();

    SerialBT.begin(bluetooth_name);

    esp_read_mac(macBT, ESP_MAC_BT);
    Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\r\n", macBT[0], macBT[1], macBT[2], macBT[3], macBT[4], macBT[5]);


}

void loop() {
    uint8_t receive_data = 0;
    bool connected = false;

    connected = SerialBT.connected();
    if (connected != connected_last)
    {
        if (connected == true)
        {
            Serial.printf("connected : ¥r¥n");
        }
        else
        {
            Serial.printf("disconnected : ¥r¥n");
        }
    }
    
    if (SerialBT.available() > 0)
    {
        receive_data = SerialBT.read();
        if (receive_data == 0xC0)
        {
            send_start_time = millis();
        }
        if (receive_data == 0xE0)
        {
            Serial.printf("comm time : %u¥r¥n", (millis() - send_start_time));
        }

        Serial.printf("receive data : %u¥r¥n", receive_data);
        
        
    }
    
}