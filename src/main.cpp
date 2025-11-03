// Load Wi-Fi library
#include <WiFi.h>
#include <ESP32Servo.h>
#include <WiFiUdp.h>
#include <Stepper.h>
#include "servo_control.h"

// Stepper code
const int stepsPerRevolution = 512;
Stepper myStepper(stepsPerRevolution, 4, 16, 17, 18);
//int solenoidPin = 13;                    //This is the output pin on the Arduino

// Linear actuator code

// Define servo pins
//const int servoPin1 = 26;
//const int servoPin2 = 27;
//const int servoPin3 = 25;
//const int servoPin4 = 32;
//const int servoPin5 = 33;

#define UDP_PORT 4210

// Servo objects

// Replace with your network credentials
const char* ssid = "ESP32-Access-Point";
const char* password = "123456789";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliary variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

// UDP
WiFiUDP UDP;
char packet[255];
char reply[] = "Packet received!";

void setup() {
    Serial.begin(115200);
    myStepper.setSpeed(70);
    //pinMode(solenoidPin, OUTPUT);          //Sets that pin as an output

    // Initialize the output variables as outputs
    //pinMode(output26, OUTPUT);
    //pinMode(output27, OUTPUT);

    // Set outputs to LOW
    digitalWrite(output26, LOW);
    digitalWrite(output27, LOW);

    // Initialize servos
    servo1.attach(servoPin1);
    servo2.attach(servoPin2);
    servo3.attach(servoPin3);
    servo4.attach(servoPin4);
    servo5.attach(servoPin5);

    // Connect to Wi-Fi network with SSID and password
    Serial.print("Setting AP (Access Point)…");

    // Remove the password parameter if you want the AP (Access Point) to be open
    WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    server.begin();

    // Begin listening to UDP port
    UDP.begin(UDP_PORT);
    Serial.print("Listening on UDP port ");
    Serial.println(UDP_PORT);
}

void loop() {
    WiFiClient client = server.available();   // Listen for incoming clients
    if (client) {
        Serial.println("New Client.");
        String currentLine = "";
        header = "";
        client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
    }

    // If packet received...
    int packetSize = UDP.parsePacket();
    if (packetSize) {
        Serial.print("Received packet! Size: ");
        Serial.println(packetSize);
        
        char packet[255];
        int len = UDP.read(packet, 255);  // len will be 2 (two characters: ‘0’ and ‘A’)
        packet[len] = '\0';               // null-terminate the string
        int val = strtol(packet, NULL, 16); // convert ASCII hex string to int
        Serial.println(val);
        if (val == 1) {
            Serial.println("Packet received");
            move_180_clockwise(0,2500,20);
        } else if (val == 0) {
            Serial.println("Other packet received");
            move_180_counterclockwise(0,2500,20);
        } //else if (val == 2){
          //push_actuator();
        //} //else if (val == 3){
          //pull_actuator();
        //} //else if (val == 4){
          //stepper_clockwise();
        //} //else if (val == 5){
          //stepper_counterclockwise();
        //}
        
        Serial.print("Packet received: ");
        Serial.println(packet);

        // Send return packet
        UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
        UDP.write((uint8_t*)reply, strlen(reply));
        UDP.endPacket();
    }
}

