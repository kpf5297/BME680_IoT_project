#include "BME68xSensor.h"

BME68xSensor::BME68xSensor(uint8_t i2cAddress) : i2cAddress(i2cAddress) {}

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

void BME68xSensor::readSensor() {
    bme.setOpMode(BME68X_FORCED_MODE);
    delayMicroseconds(bme.getMeasDur());

    if (bme.fetchData()) {
        bme.getData(data);
    } else {
        Serial.println("Failed to fetch data from BME68x sensor");
    }
}

float BME68xSensor::getTemperature() const {
    return data.temperature;
}

float BME68xSensor::getPressure() const {
    return data.pressure;
}

float BME68xSensor::getHumidity() const {
    return data.humidity;
}

float BME68xSensor::getGasResistance() const {
    return data.gas_resistance;
}

String BME68xSensor::getStatusString() {
    return bme.statusString();
}
