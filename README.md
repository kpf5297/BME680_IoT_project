# BME680 IoT Project

This project involves using a BME680 sensor connected to an Arduino, which sends data to a Flask server running on a Raspberry Pi. The data is then displayed on a Python desktop application using PyQt5 and Matplotlib.

## Project Structure

The project is divided into three main parts:
1. **Arduino Code**: Reads data from the BME680 sensor and sends it to the Flask server.
2. **Flask Server**: Receives data from the Arduino and stores it in a MySQL database.
3. **Python Desktop App**: Retrieves data from the Flask server and displays it using graphs.

### Branches

- `arduino_bme680_iot_project`: Contains the Arduino code.
- `server_bme680_iot_project`: Contains the Flask server code.
- `desktop_app_bme680_iot_project`: Contains the Python desktop application code.

## Requirements

### Arduino

- Arduino IDE
- BME680 sensor
- Arduino board (e.g., Arduino RP2040 Connect) - https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json (board manager url)
- Libraries: `Wire.h`, `WiFiNINA.h`, `ArduinoHttpClient.h`, `bme68xLibrary.h`

### Server

- Raspberry Pi with Raspbian OS
- Python 3
- MySQL server
- Flask
- Flask-MySQL
- Flask-CORS
- mysql-connector-python

### Desktop App

- Python 3
- PyQt5
- requests
- matplotlib

## Setup Instructions

### Arduino Setup

1. **Connect BME680 Sensor**: Connect the BME680 sensor to your Arduino board.
2. **Install Libraries**: Install the necessary libraries (`Wire.h`, `WiFiNINA.h`, `ArduinoHttpClient.h`, `bme68xLibrary.h`) via the Arduino Library Manager.
3. **Upload Code**: Upload the code from the `arduino_bme680_iot_project` branch to your Arduino board.

### Server Setup

1. **Install MySQL Server**:
   ```bash
   sudo apt-get update
   sudo apt-get install mysql-server
   ```

2. **Create Database and Table**:
   ```sql
   CREATE DATABASE SensorData;
   USE SensorData;
   CREATE TABLE BME680Readings (
       id INT AUTO_INCREMENT PRIMARY KEY,
       temperature FLOAT,
       pressure FLOAT,
       humidity FLOAT,
       gas_resistance FLOAT,
       timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
   );
   ```

3. **Set Up Flask Server**:
   - Create a virtual environment and install dependencies:
     ```bash
     python3 -m venv venv
     source venv/bin/activate
     pip install flask flask-mysql flask-cors mysql-connector-python
     ```

   - Place the Flask server code from the `server_bme680_iot_project` branch into a directory.
   - Run the Flask server:
     ```bash
     python app.py
     ```

### Desktop App Setup

1. **Install Dependencies**:
   ```bash
   pip install pyqt5 requests matplotlib
   ```

2. **Run the Desktop App**:
   - Place the Python desktop application code from the `desktop_app_bme680_iot_project` branch into a directory.
   - Run the application:
     ```bash
     python desktop_app.py
     ```

## Usage

1. **Start the Flask Server**: Ensure the Flask server is running on the Raspberry Pi.
2. **Start the Arduino**: Connect the Arduino to power and ensure it is connected to the same network as the Raspberry Pi.
3. **Run the Desktop App**: Open the Python desktop application to view the sensor data.

## Troubleshooting

- Ensure all devices are connected to the same network.
- Verify the IP address of the Raspberry Pi and update the desktop app if necessary.
- Check the Arduino Serial Monitor for any connection issues.

## License

This project is licensed under the MIT License.
```
