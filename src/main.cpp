#include <WiFi.h>
#include <WiFiUdp.h>
#include <ESP32Servo.h>
#include "servo_control.h"

//udp setup
WiFiUDP UDP;
const int udpPort = 4210;
char incomingPacket[255];
char reply[] = "Packet received";

//UNCOMMENT FOR HOT SPOT USE

// const char* ssid = "WIFI_NAME";
// const char* password = "WIFI_PASSWORD";

// void setupWiFi() {
//     Serial.begin(115200);
//     Serial.printf("Connecting to %s", ssid);
//     WiFi.begin(ssid, password);
//     while (WiFi.status() != WL_CONNECTED) {
//         delay(500);
//         Serial.print(".");
//     }
//     Serial.println("\nWiFi connected!");
//     Serial.print("IP address: ");
//     Serial.println(WiFi.localIP());

//     udp.begin(udpPort);
//     Serial.printf("Listening on UDP port %d\n", udpPort);
// }

//FOR ACCESS POINT CREATION
const char* ssid = "Test_AP";
const char* password = "12345678";

void setupWiFi(){
    Serial.begin(115200);
    Serial.println("Booting...");
    delay(500);

    WiFi.softAP(ssid, password);
    Serial.println("Access point started.");

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    UDP.begin(udpPort);
    delay(1000);

    Serial.print("Listening on UDP port ");
    Serial.println(udpPort);
}

const int servoPin1 = 12;
const int servoPin2 = 27;
const int servoPin3 = 25;
const int servoPin4 = 32;
const int servoPin5 = 35;

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
Servo tempServo;

void processUDP() {
    int packetSize = UDP.parsePacket();
    if (packetSize) {
        int len = UDP.read(incomingPacket, 255);
        if (len > 0) incomingPacket[len] = 0;

        Serial.printf("Received: %s\n", incomingPacket);

        //servo control functions
        if (strcmp(incomingPacket, "CW") == 0) {
            move_180_clockwise(500, 2500, 20);
        } else if (strcmp(incomingPacket, "CCW") == 0) {
            move_180_counterclockwise(2500, 500, 20);
        } else if (strcmp(incomingPacket, "STOP") == 0) {
            move_180_all_servos_to(1500);
        }
    }
}

void setup() {
    initializeAll();
    setupWiFi();
}

void loop() {
    processUDP();
}
