#include <WiFi.h>
#include <WiFiUdp.h>
#include <ESP32Servo.h>
#include "servo_control.h"

//udp setup
WiFiUDP UDP;
const int udpPort = 4210;
char incomingPacket[255];
char reply[32];
WiFiServer server(80);


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
    server.begin();
    UDP.begin(udpPort);
    delay(1000);

    Serial.print("Listening on UDP port ");
    Serial.println(udpPort);
}


void processUDP() {
    WiFiClient client = server.available();   // Listen for incoming clients
    if (client) {
        Serial.println("New Client.");
        String currentLine = "";
        client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
    }
    int packetSize = UDP.parsePacket();
    if (packetSize) {
        char packet[255];
        int len = UDP.read(packet, 255);  // len will be 2 (two characters: ‘0’ and ‘A’)
        packet[len] = '\0';               // null-terminate the string
        int val = strtol(packet, NULL, 16); // convert ASCII hex string to int
        snprintf(reply, sizeof(reply), "ACK:%d", val);

        Serial.println(val);

        Serial.printf("Received: %s\n", packet);
          /*  if (val == 1) {
            Serial.println("Packet received: 1");
            }
            else if (val == 0) {
            Serial.println("Packet received: 0");
            } else if (val == 2){
            //push_actuator();
            } else if (val == 3){
            //pull_actuator();
            } else if (val == 4){
            //stepper_clockwise();
            } else if (val == 5){
            //stepper_counterclockwise();
            }
            */
    switch (val) {
        case 0:
            Serial.println("Packet received: 0");
            break;

        case 1:
            Serial.println("Packet received: 1");
            break;

        case 2:
            // push_actuator();
            break;

        case 3:
            // pull_actuator();
            break;

        case 4:
            // stepper_clockwise();
            break;

        case 5:
            // stepper_counterclockwise();
            break;

        default:
            Serial.printf("Unknown packet: %d\n", val);
            break;
}

            UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
            UDP.write((uint8_t*)reply, strlen(reply));
            UDP.endPacket();
        }
    }

void setup() {
    initializeAll();
    setupWiFi();
}

void loop() {
    processUDP();
}
