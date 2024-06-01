/*
 * main.ino
 * 
 * Description: Main file for Arduino project that reads data from a BME680 sensor
 *              and sends it to a server over WiFi.
 * 
 * Author: Kevin Fox
 * Date: 2024-06-01
 */

#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include "WiFiConnection.h"
#include "BME68xSensor.h"

// Server details
WiFiClient wifiClient;
String serverIPAddress;
HttpClient* client;

BME68xSensor bmeSensor;

void setup() {
    Serial.begin(115200);

    // Initialize WiFi connection
    WiFiConnection::getInstance().setup();
    if (WiFiConnection::getInstance().isConnected()) {
        Serial.println("Connected to WiFi");
    } else {
        Serial.println("Failed to connect to WiFi");
    }

    // Get server IP address from WiFiConnection class
    serverIPAddress = WiFiConnection::getInstance().getServerIPAddress();
    Serial.print("Server IP address: ");
    Serial.println(serverIPAddress);

    // Initialize HTTP client
    client = new HttpClient(wifiClient, serverIPAddress.c_str(), 5000);

    // Initialize BME680 sensor
    if (!bmeSensor.begin()) {
        Serial.println("Failed to initialize BME680 sensor!");
    }
}

void loop() {
    // Reconnect to WiFi if disconnected
    WiFiConnection::getInstance().connectToWiFi();
    if (WiFiConnection::getInstance().isConnected()) {
        Serial.println("WiFi is connected");

        // Read data from BME680 sensor
        bmeSensor.readSensor();
        float temperature = bmeSensor.getTemperature();
        float pressure = bmeSensor.getPressure();
        float humidity = bmeSensor.getHumidity();
        float gasResistance = bmeSensor.getGasResistance();

        // Create JSON payload
        String payload = "{\"temperature\":";
        payload += temperature;
        payload += ",\"pressure\":";
        payload += pressure;
        payload += ",\"humidity\":";
        payload += humidity;
        payload += ",\"gas_resistance\":";
        payload += gasResistance;
        payload += "}";

        // Send data to server
        client->beginRequest();
        client->post("/add_bme680_data");
        client->sendHeader("Content-Type", "application/json");
        client->sendHeader("Content-Length", payload.length());
        client->beginBody();
        client->print(payload);
        client->endRequest();

        // Get response from server
        int statusCode = client->responseStatusCode();
        String response = client->responseBody();

        Serial.print("Status code: ");
        Serial.println(statusCode);
        Serial.print("Response: ");
        Serial.println(response);

        if (statusCode == 200) {
            Serial.println("BME680 data sent successfully");
        } else {
            Serial.print("Failed to send BME680 data. Status code: ");
            Serial.println(statusCode);
            Serial.print("Response: ");
            Serial.println(response);
        }

        // Wait before sending next data
        delay(5000);
    } else {
        Serial.println("WiFi not connected");
    }
    delay(5000);
}
