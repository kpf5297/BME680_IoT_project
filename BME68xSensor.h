#ifndef BME68XSENSOR_H
#define BME68XSENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include "bme68xLibrary.h"

class BME68xSensor {
public:
    BME68xSensor(uint8_t i2cAddress = 0x77);
    bool begin();
    void readSensor();
    float getTemperature() const;
    float getPressure() const;
    float getHumidity() const;
    float getGasResistance() const;
    String getStatusString();

private:
    Bme68x bme;
    uint8_t i2cAddress;
    bme68xData data;
};

#endif // BME68XSENSOR_H
