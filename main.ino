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
    WiFiConnection::getInstance().setup();
    if (WiFiConnection::getInstance().isConnected()) {
        Serial.println("Connected to WiFi");
    } else {
        Serial.println("Failed to connect to WiFi");
    }
    serverIPAddress = WiFiConnection::getInstance().getServerIPAddress();
    Serial.print("Server IP address: ");
    Serial.println(serverIPAddress);
    client = new HttpClient(wifiClient, serverIPAddress.c_str(), 5000);

    if (!bmeSensor.begin()) {
        Serial.println("Failed to initialize BME680 sensor!");
    }
}

void loop() {
    WiFiConnection::getInstance().connectToWiFi();
    if (WiFiConnection::getInstance().isConnected()) {
        Serial.println("WiFi is connected");

        // Read data from BME680 sensor
        bmeSensor.readSensor();
        float temperature = bmeSensor.getTemperature();
        float pressure = bmeSensor.getPressure();
        float humidity = bmeSensor.getHumidity();
        float gasResistance = bmeSensor.getGasResistance();

        String payload = "{\"temperature\":";
        payload += temperature;
        payload += ",\"pressure\":";
        payload += pressure;
        payload += ",\"humidity\":";
        payload += humidity;
        payload += ",\"gas_resistance\":";
        payload += gasResistance;
        payload += "}";

        client->beginRequest();
        client->post("/add_bme680_data");
        client->sendHeader("Content-Type", "application/json");
        client->sendHeader("Content-Length", payload.length());
        client->beginBody();
        client->print(payload);
        client->endRequest();

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

        delay(5000);  // Wait before sending next data
    } else {
        Serial.println("WiFi not connected");
    }
    delay(5000);
}
