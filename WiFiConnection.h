/*
 * WiFiConnection.h
 * 
 * Description: Header file for the WiFiConnection class which manages WiFi connectivity.
 * 
 * Author: Kevin Fox
 * Date: 2024-06-01
 */

#ifndef WIFICONNECTION_H
#define WIFICONNECTION_H

#include <WiFiNINA.h>
#include <kvstore_global_api.h>
#include <mbed_error.h>

class WiFiConnection {
public:
    static WiFiConnection& getInstance() {
        static WiFiConnection instance;
        return instance;
    }

    void setup();
    void connectToWiFi();
    bool isConnected();
    String getServerIPAddress();

private:
    WiFiConnection() {}
    WiFiConnection(const WiFiConnection&) = delete;
    WiFiConnection& operator=(const WiFiConnection&) = delete;

    void promptForNetworkSelection();
    void scanNetworks();
    int selectNetwork();
    void clearSerialBuffer();
    String getPassword();
    String getServerIP();
    void saveNetworkCredentials(int networkIndex, const String& password, const String& serverIP);
    void connectToSavedNetwork();
    void connect(const char* ssid, const uint8_t* bssid, const char* password);
    const char* getEncryptionType(int thisType);
};

#endif // WIFICONNECTION_H
