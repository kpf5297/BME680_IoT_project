-- setup_database.sql

-- Description: SQL commands to set up the database and table for storing BME680 sensor data.

-- Author: Kevin Fox
-- Date: 2024-06-01

-- Create database
CREATE DATABASE IF NOT EXISTS SensorData;

-- Use the newly created database
USE SensorData;

-- Create table to store BME680 sensor data
CREATE TABLE IF NOT EXISTS BME680Readings (
    id INT AUTO_INCREMENT PRIMARY KEY,
    temperature FLOAT NOT NULL,
    pressure FLOAT NOT NULL,
    humidity FLOAT NOT NULL,
    gas_resistance FLOAT NOT NULL,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
