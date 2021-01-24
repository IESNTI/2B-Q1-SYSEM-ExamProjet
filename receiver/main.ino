#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LoRa.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <xxtea-iot-crypt.h>

#define PinGateMosfet 6
#define PinPWMFan 5
#define TempMaxPower 20.0
#define TempTrigger 18.0
#define ESPrxPin 8
#define ESPtxPin 4
#define ESPTimeout 2000
#define OLEDWidth 128
#define OLEDHeight 32
#define OLEDAddr 0x3C
#define PinButtonInterrupt 3

float temperature = 0;
boolean fanOn = true;

SoftwareSerial ESP8266(ESPrxPin, ESPtxPin);
Adafruit_SSD1306 display(OLEDWidth, OLEDHeight);

void waitForOk() {
    boolean ok = false;
    unsigned long previousMillis = millis();
    while (!ok && (millis() - previousMillis < ESPTimeout)) {
        while (ESP8266.available()) {
            String inData = ESP8266.readStringUntil('\n');
            inData.trim();
            if (inData.equals("OK") || inData.equals("SEND OK"))
                ok = true;
        }
    }
    if (millis() - previousMillis > ESPTimeout) {
        Serial.println("Timeout while waiting for AT command");
    }
}

void setup() {
    Serial.begin(9600);
    ESP8266.begin(9600);
    display.begin(SSD1306_SWITCHCAPVCC, OLEDAddr);
    delay(1000);
    while (!Serial)
        ;

    Serial.println("LoRa Receiver");

    if (!LoRa.begin(433E6)) {
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }

    pinMode(PinButtonInterrupt, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PinButtonInterrupt), inverseFanState, RISING);

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("PLEASE");
    display.println("WAIT...");
    display.display();

    LoRa.setSyncWord(0xF3);

    pinMode(PinGateMosfet, OUTPUT);
    pinMode(PinPWMFan, OUTPUT);

    xxtea.setKey("Hello Password");

    ESP8266.println("AT+CWMODE=2");
    waitForOk();
    ESP8266.println("AT+CWSAP_DEF=\"temperature\",\"Tigrou007\",1,3");
    waitForOk();
    ESP8266.println("AT+CIPMUX=1");
    waitForOk();
    ESP8266.println("AT+CIPSERVER=1,80");
}

int temperatureIntoPercentFan(float temperature) {
    return ((100) - 100.0 / (TempMaxPower - TempTrigger) *
                        (TempMaxPower - temperature));
}

void rotationPercentFan(int percent) {
    Serial.print("Percent: ");
    Serial.println(percent);
    Serial.print("rotation value: ");
    // rotation range 105 - 255
    Serial.println((percent * 1.5) + 105);
    analogWrite(PinPWMFan, (percent * 1.5) + 105);
}

void inverseFanState() {
    fanOn = !fanOn;
}

void loop() {
    // try to parse packet
    int packetSize = LoRa.parsePacket();
    boolean previousStateFan = false;
    if (packetSize) {
        // received a packet
        // Serial.print("Received packet '");

        String receivedMessage = "";
        // read packet
        while (LoRa.available()) {
            receivedMessage += (char)LoRa.read();
        }
        String message = xxtea.decrypt(receivedMessage);
        temperature = message.toFloat();
        Serial.print(temperature);
        Serial.println("°C");
        if (temperature < TempTrigger || !fanOn) {
            digitalWrite(PinGateMosfet, LOW);
        } else {
            digitalWrite(PinGateMosfet, HIGH);
            rotationPercentFan(temperatureIntoPercentFan(temperature));
        }
    }
    if (ESP8266.available()) {
        if (ESP8266.find("+IPD,")) {
            Serial.println("new client!");
            int connectionId = ESP8266.read() - 48;
            String cipSend = "AT+CIPSEND=";
            cipSend += connectionId;
            cipSend += ",";
            cipSend += String(temperature).length();
            ESP8266.println(cipSend);
            waitForOk();
            ESP8266.println(String(temperature));
            waitForOk();
            String closeCommand = "AT+CIPCLOSE=";
            closeCommand += connectionId; // append connection id
            ESP8266.println(closeCommand);
            waitForOk();
        }
    }
    if (temperature != 0) {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.print(temperature);
        display.println(" C");
        display.print("FAN: ");
        if (temperature < TempTrigger || !fanOn) {
            display.print("0");
        } else {
            display.print(temperatureIntoPercentFan(temperature));
        }
        display.println("%");
        display.display();
    }
}