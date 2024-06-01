/*
 * WiFiConnection.cpp
 * 
 * Description: Implementation file for the WiFiConnection class which manages WiFi connectivity.
 * 
 * Author: Kevin Fox
 * Date: 2024-06-01
 */

#include "WiFiConnection.h"

/**
 * @brief Sets up WiFi connection by prompting the user to select a network if needed.
 */
void WiFiConnection::setup() {
    Serial.begin(115200);
    delay(1000);
    promptForNetworkSelection();
}

/**
 * @brief Attempts to reconnect to WiFi if not already connected.
 */
void WiFiConnection::connectToWiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected. Attempting to connect...");
        connectToSavedNetwork();
    }
}

/**
 * @brief Checks if the device is connected to WiFi.
 * 
 * @return true if connected, false otherwise.
 */
bool WiFiConnection::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

/**
 * @brief Retrieves the server IP address stored in flash memory.
 * 
 * @return String containing the server IP address.
 */
String WiFiConnection::getServerIPAddress() {
    char key[] = "server-ip";
    kv_info_t infoBuffer;
    int result = kv_get_info(key, &infoBuffer);
    if (result != MBED_SUCCESS) {
        Serial.println("Failed to read server IP");
        return "";
    }
    char readBuffer[infoBuffer.size];
    result = kv_get(key, readBuffer, infoBuffer.size, nullptr);
    if (result != MBED_SUCCESS) {
        Serial.println("Failed to read server IP");
        return "";
    }
    return String(readBuffer);
}

// Implementation of other private methods follows...

/**
 * @brief Prompts the user to select a WiFi network if desired.
 */
void WiFiConnection::promptForNetworkSelection() {
    Serial.println("Press 'n' within 10 seconds to select a new network.");
    unsigned long startAttemptTime = millis();
    while (millis() - startAttemptTime < 10000) {
        if (Serial.available() > 0) {
            char c = Serial.read();
            if (c == 'n') {
                Serial.println("Scanning for networks...");
                scanNetworks();
                int networkIndex = selectNetwork();
                if (networkIndex != -1) {
                    clearSerialBuffer();
                    String password = getPassword();
                    String serverIP = getServerIP();
                    saveNetworkCredentials(networkIndex, password, serverIP);
                    Serial.println("Network credentials saved. Attempting to connect...");
                    connectToSavedNetwork();
                } else {
                    Serial.println("Skipped network selection. Will attempt to use saved credentials if available.");
                    connectToSavedNetwork();
                }
                return;
            }
        }
    }
    Serial.println("Timeout reached. Using saved credentials.");
    connectToSavedNetwork();
}

/**
 * @brief Scans for available WiFi networks.
 */
void WiFiConnection::scanNetworks() {
    int n = WiFi.scanNetworks();
    Serial.println("Scan complete");
    if (n == 0) {
        Serial.println("No networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found:");
        for (int i = 0; i < n; ++i) {
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(" dBm) ");
            Serial.println(getEncryptionType(WiFi.encryptionType(i)));
            delay(10);
        }
    }
}

/**
 * @brief Prompts the user to select a WiFi network from the scanned list.
 * 
 * @return int index of the selected network or -1 if selection is skipped.
 */
int WiFiConnection::selectNetwork() {
    Serial.println("Enter the number of the network you want to connect to (or press 's' to skip): ");
    while (true) {
        if (Serial.available() > 0) {
            int networkIndex = Serial.parseInt();
            if (networkIndex > 0 && networkIndex <= WiFi.scanNetworks()) {
                Serial.print("Selected network: ");
                Serial.println(WiFi.SSID(networkIndex - 1));
                return networkIndex - 1;
            } else if (Serial.read() == 's') {
                return -1;
            }
        }
    }
}

/**
 * @brief Clears the serial buffer.
 */
void WiFiConnection::clearSerialBuffer() {
    while (Serial.available() > 0) {
        Serial.read();
    }
}

/**
 * @brief Prompts the user to enter the WiFi network password.
 * 
 * @return String containing the entered password.
 */
String WiFiConnection::getPassword() {
    Serial.println("Enter the password for the network: ");
    String password = "";
    while (true) {
        if (Serial.available() > 0) {
            char c = Serial.read();
            if (c == '\n' || c == '\r') {
                break;
            } else {
                password += c;
            }
        }
    }
    clearSerialBuffer();
    Serial.print("Password entered: ");
    Serial.println(password);
    return password;
}

/**
 * @brief Prompts the user to enter the server IP address.
 * 
 * @return String containing the entered server IP address.
 */
String WiFiConnection::getServerIP() {
    Serial.println("Enter the server IP address: ");
    String serverIP = "";
    while (true) {
        if (Serial.available() > 0) {
            char c = Serial.read();
            if (c == '\n' || c == '\r') {
                break;
            } else {
                serverIP += c;
            }
        }
    }
    clearSerialBuffer();
    Serial.print("Server IP entered: ");
    Serial.println(serverIP);
    return serverIP;
}

/**
 * @brief Saves the network credentials and server IP address to flash memory.
 * 
 * @param networkIndex Index of the selected network.
 * @param password Password of the selected network.
 * @param serverIP IP address of the server.
 */
void WiFiConnection::saveNetworkCredentials(int networkIndex, const String& password, const String& serverIP) {
    const char* ssid = WiFi.SSID(networkIndex);
    uint8_t bssid[6];
    WiFi.BSSID(networkIndex, bssid);

    int result;

    result = kv_set("network-ssid", ssid, strlen(ssid) + 1, 0);
    if (result != MBED_SUCCESS) {
        Serial.print("Failed to save SSID with error code ");
        Serial.println(result);
        return;
    }
    result = kv_set("network-password", password.c_str(), password.length() + 1, 0);
    if (result != MBED_SUCCESS) {
        Serial.print("Failed to save password with error code ");
        Serial.println(result);
        return;
    }
    result = kv_set("network-bssid", bssid, sizeof(bssid), 0);
    if (result != MBED_SUCCESS) {
        Serial.print("Failed to save BSSID with error code ");
        Serial.println(result);
        return;
    }
    result = kv_set("server-ip", serverIP.c_str(), serverIP.length() + 1, 0);
    if (result != MBED_SUCCESS) {
        Serial.print("Failed to save server IP with error code ");
        Serial.println(result);
        return;
    }

    Serial.println("Credentials saved successfully:");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(password);
    Serial.print("BSSID: ");
    for (int i = 0; i < 6; ++i) {
        Serial.print(bssid[i], HEX);
        if (i < 5) Serial.print(":");
    }
    Serial.println();
    Serial.print("Server IP: ");
    Serial.println(serverIP);

    connect(ssid, bssid, password.c_str());
}

/**
 * @brief Connects to the saved WiFi network.
 */
void WiFiConnection::connectToSavedNetwork() {
    char ssidKey[] = "network-ssid";
    char passwordKey[] = "network-password";
    char bssidKey[] = "network-bssid";

    kv_info_t infoBuffer;

    int result = kv_get_info(ssidKey, &infoBuffer);
    if (result != MBED_SUCCESS) {
        Serial.print("Failed to read SSID info with error code ");
        Serial.println(result);
        return;
    }
    char ssidBuffer[infoBuffer.size];
    result = kv_get(ssidKey, ssidBuffer, infoBuffer.size, nullptr);
    if (result != MBED_SUCCESS) {
        Serial.print("Failed to read SSID with error code ");
        Serial.println(result);
        return;
    }

    result = kv_get_info(passwordKey, &infoBuffer);
    if (result != MBED_SUCCESS) {
        Serial.print("Failed to read password info with error code ");
        Serial.println(result);
        return;
    }
    char passwordBuffer[infoBuffer.size];
    result = kv_get(passwordKey, passwordBuffer, infoBuffer.size, nullptr);
    if (result != MBED_SUCCESS) {
        Serial.print("Failed to read password with error code ");
        Serial.println(result);
        return;
    }

    result = kv_get_info(bssidKey, &infoBuffer);
    bool bssidAvailable = (result == MBED_SUCCESS);
    uint8_t savedBSSID[6];

    if (bssidAvailable) {
        result = kv_get(bssidKey, savedBSSID, sizeof(savedBSSID), nullptr);
        if (result != MBED_SUCCESS) {
            Serial.print("Failed to read BSSID with error code ");
            Serial.println(result);
            bssidAvailable = false;
        }
    }

    if (bssidAvailable) {
        Serial.println("Attempting connection using saved BSSID...");
        connect(ssidBuffer, savedBSSID, passwordBuffer);
    } else {
        Serial.println("BSSID
        not available, attempting connection using SSID...");
        connect(ssidBuffer, nullptr, passwordBuffer);
    }
}

/**
 * @brief Connects to a WiFi network using the given SSID, BSSID, and password.
 * 
 * @param ssid SSID of the WiFi network.
 * @param bssid BSSID of the WiFi network (can be nullptr).
 * @param password Password of the WiFi network.
 */
void WiFiConnection::connect(const char* ssid, const uint8_t* bssid, const char* password) {
    if (bssid) {
        Serial.print("Connecting to WiFi network (BSSID): ");
        for (int i = 0; i < 6; ++i) {
            Serial.print(bssid[i], HEX);
            if (i < 5) Serial.print(":");
        }
        Serial.println();
        // Manual selection of the BSSID
        for (int i = 0; i < WiFi.scanNetworks(); ++i) {
            uint8_t currentBSSID[6];
            WiFi.BSSID(i, currentBSSID);
            if (memcmp(bssid, currentBSSID, 6) == 0) {
                WiFi.begin(WiFi.SSID(i), password);
                break;
            }
        }
    } else {
        Serial.print("Connecting to WiFi network (SSID): ");
        Serial.println(ssid);
        WiFi.begin(ssid, password);
    }

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
        delay(500);
        Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Connected to WiFi");
    } else {
        Serial.println("Failed to connect to WiFi");
    }
}

/**
 * @brief Returns a string representation of the encryption type.
 * 
 * @param thisType Encryption type.
 * @return const char* String representation of the encryption type.
 */
const char* WiFiConnection::getEncryptionType(int thisType) {
    switch (thisType) {
        case ENC_TYPE_NONE:
            return "Open";
        case ENC_TYPE_WEP:
            return "WEP";
        case ENC_TYPE_TKIP:
            return "WPA";
        case ENC_TYPE_CCMP:
            return "WPA2";
        case ENC_TYPE_AUTO:
            return "Auto";
        default:
            return "Unknown";
    }
}
