#include <xxtea-iot-crypt.h>
#include "MPU9250.h"
#include <LoRa.h>
#include <SPI.h>

#define I2Cclock 400000
#define I2Cport Wire
#define MPU9250_ADDRESS 0x68 // I2C Address of the module
MPU9250 myIMU(MPU9250_ADDRESS, I2Cport, I2Cclock);

void getTemperatureFromMPU() {
    myIMU.tempCount = myIMU.readTempData();
    myIMU.temperature = ((float)myIMU.tempCount) / 333.87 +
                        14.0; // convert temperature from fahrenheit
}

boolean runEvery(unsigned long interval) {
    static unsigned long previousMillis = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        return true;
    }
    return false;
}

void sendAndPrintTemperature() {
    getTemperatureFromMPU();
    String encryptedTemperature = xxtea.encrypt(String(myIMU.temperature));
    encryptedTemperature.toLowerCase();
    Serial.print("Temperature is ");
    Serial.print(myIMU.temperature);
    Serial.println(" degrees C");
    LoRa.beginPacket();
    LoRa.print(encryptedTemperature);
    LoRa.endPacket(true);
}

void setup() {
    Wire.begin();
    Serial.begin(9600);
    while (!Serial)
        ;

    Serial.println("LoRa Sender");

    if (!LoRa.begin(433E6)) {
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }
    LoRa.setTxPower(5);
    LoRa.setSyncWord(0xF3);

    // Read the WHO_AM_I register, this is a good test of communication
    byte c = myIMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
    Serial.print(F("MPU9250 I AM 0x"));
    Serial.print(c, HEX);
    Serial.print(F(" I should be 0x"));
    Serial.println(0x71, HEX);
    myIMU.initMPU9250();

    xxtea.setKey("Tigrou007");
}

void loop() {
    if (runEvery(5000)) {
        sendAndPrintTemperature();
    }
}