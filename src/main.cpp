#include <WiFi.h>
#include <WiFiUdp.h>
#include <ctype.h>
#include <ESP32Servo.h>
#include "servo_control.h"

// ---------------------------------------------------------------
// UDP setup
// ---------------------------------------------------------------
WiFiUDP UDP;
const int udpPort = 4210;
char reply[32];

// Servo pulse widths in microseconds (valid range ~500-2500).
// Tune these on the real hand so open/closed don't strain the servo.
static const int HAND_OPEN_US   = 500;
static const int HAND_CLOSED_US = 2500;

static void handOpen()  { move_180_all_servos_to(HAND_OPEN_US);  }
static void handClose() { move_180_all_servos_to(HAND_CLOSED_US); }

// ---------------------------------------------------------------
// UNCOMMENT FOR HOT SPOT USE
// ---------------------------------------------------------------
/*
const char* ssid = "WIFI_NAME";
const char* password = "WIFI_PASSWORD";
void setupWiFi() {
    Serial.begin(115200);
    Serial.printf("Connecting to %s", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    UDP.begin(udpPort);
    Serial.printf("Listening on UDP port %d\n", udpPort);
}
*/

// ---------------------------------------------------------------
// FOR ACCESS POINT CREATION
// ---------------------------------------------------------------
const char* ssid = "Test_AP";
const char* password = "12345678";

void setupWiFi() {
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

// ---------------------------------------------------------------
// Receive and parse one UDP packet ("0" = open, "1" = close)
// ---------------------------------------------------------------
void processUDP() {
    int packetSize = UDP.parsePacket();
    if (packetSize <= 0) return;                      

    char packet[16];
    int len = UDP.read(packet, sizeof(packet) - 1);   
    if (len <= 0) return;
    packet[len] = '\0';

    
    while (len > 0 && isspace((unsigned char)packet[len - 1])) {
        packet[--len] = '\0';
    }

    
    char* end = nullptr;
    long val = strtol(packet, &end, 10);
    bool valid = (len > 0 && *end == '\0');

    if (valid && val == 0) {
        Serial.println("Packet: 0 -> open");
        handOpen();
        snprintf(reply, sizeof(reply), "ACK:0");
    } else if (valid && val == 1) {
        Serial.println("Packet: 1 -> close");
        handClose();
        snprintf(reply, sizeof(reply), "ACK:1");
    } else {
        Serial.printf("Bad packet: \"%s\"\n", packet);
        snprintf(reply, sizeof(reply), "ERR");
    }

    UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
    UDP.write((const uint8_t*)reply, strlen(reply));
    UDP.endPacket();
}

void setup() {
    initializeAll();
    setupWiFi();
}

void loop() {
    processUDP();
}