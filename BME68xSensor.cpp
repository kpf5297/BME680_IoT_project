/*
 * BME68xSensor.cpp
 * 
 * Description: Implementation file for the BME68xSensor class which manages interaction with the BME680 sensor.
 * 
 * Author: Kevin Fox
 * Date: 2024-06-01
 */

#include "BME68xSensor.h"

/**
 * @brief Constructor for the BME68xSensor class.
 * 
 * @param i2cAddress I2C address of the BME680 sensor (default is 0x77).
 */
BME68xSensor::BME68xSensor(uint8_t i2cAddress) : i2cAddress(i2cAddress) {}

/**
 * @brief Initializes the BME680 sensor.
 * 
 * @return true if initialization is successful, false otherwise.
 */
bool BME68xSensor::begin() {
    Wire.begin();
    bme.begin(i2cAddress, Wire);

    int status = bme.checkStatus();
    if (status == BME68X_ERROR) {
        Serial.println("Sensor error: " + bme.statusString());
        return false;
    } else if (status == BME68X_WARNING) {
        Serial.println("Sensor Warning: " + bme.statusString());
    } else {
        Serial.println("Sensor initialized successfully.");
    }
    
    bme.setTPH();
    bme.setHeaterProf(300, 100);
    
    return true;
}

/**
 * @brief Reads data from the BME680 sensor.
 */
void BME68xSensor::readSensor() {
    bme.setOpMode(BME68X_FORCED_MODE);
    delayMicroseconds(bme.getMeasDur());

    if (bme.fetchData()) {
        bme.getData(data);
    } else {
        Serial.println("Failed to fetch data from BME68x sensor");
    }
}

/**
 * @brief Gets the temperature reading from the BME680 sensor.
 * 
 * @return float Temperature in degrees Celsius.
 */
float BME68xSensor::getTemperature() const {
    return data.temperature;
}

/**
 * @brief Gets the pressure reading from the BME680 sensor.
 * 
 * @return float Pressure in hPa.
 */
float BME68xSensor::getPressure() const {
    return data.pressure;
}

/**
 * @brief Gets the humidity reading from the BME680 sensor.
 * 
 * @return float Humidity in %.
 */
float BME68xSensor::getHumidity() const {
    return data.humidity;
}

/**
 * @brief Gets the gas resistance reading from the BME680 sensor.
 * 
 * @return float Gas resistance in ohms.
 */
float BME68xSensor::getGasResistance() const {
    return data.gas_resistance;
}

/**
 * @brief Gets the status string of the BME680 sensor.
 * 
 * @return String Status string.
 */
String BME68xSensor::getStatusString() {
    return bme.statusString();
}
