"""
app.py

Description: Flask server to receive BME680 sensor data and store it in a MySQL database.
             Provides endpoints for adding data and retrieving data within specific time ranges.

Author: Kevin Fox
Date: 2024-06-01
"""

from flask import Flask, request, jsonify
from flask_cors import CORS
from datetime import datetime
import mysql.connector
from mysql.connector import Error
import logging

app = Flask(__name__)
CORS(app)

# Database configuration
db_config = {
    'host': 'localhost',
    'user': 'your_username',  # Replace with your database username
    'password': 'your_password',  # Replace with your database password
    'database': 'SensorData'
}

# Configure logging
logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)

def create_connection():
    """
    Create a database connection and return the connection object.

    :return: Connection object or None
    """
    try:
        connection = mysql.connector.connect(**db_config)
        if connection.is_connected():
            logger.info("Connected to MariaDB database")
            return connection
    except Error as e:
        logger.error(f"Error: {e}")
        return None

@app.route('/add_bme680_data', methods=['POST'])
def add_bme680_data():
    """
    Endpoint to add BME680 sensor data to the database.

    :return: JSON response with success or failure message
    """
    data = request.json
    temperature = data.get('temperature')
    pressure = data.get('pressure')
    humidity = data.get('humidity')
    gas_resistance = data.get('gas_resistance')

    connection = create_connection()
    if connection:
        try:
            cursor = connection.cursor()
            query = """INSERT INTO BME680Readings 
                       (temperature, pressure, humidity, gas_resistance) 
                       VALUES (%s, %s, %s, %s)"""
            cursor.execute(query, (temperature, pressure, humidity, gas_resistance))
            connection.commit()
            cursor.close()
            connection.close()
            logger.info("BME680 data added successfully")
            return jsonify({'message': 'BME680 data added successfully!'}), 200
        except Error as e:
            logger.error(f"Error while inserting BME680 data: {e}")
            return jsonify({'message': 'Failed to insert BME680 data'}), 500
    else:
        return jsonify({'message': 'Failed to connect to database'}), 500

@app.route('/get_bme680_data', methods=['GET'])
def get_bme680_data():
    """
    Endpoint to retrieve BME680 sensor data within a specified time range.

    :return: JSON response with sensor data
    """
    range_param = request.args.get('range', default='minute')
    query = "SELECT * FROM BME680Readings WHERE timestamp > NOW() - INTERVAL 1 MINUTE ORDER BY timestamp DESC"
    if range_param == 'hour':
        query = "SELECT * FROM BME680Readings WHERE timestamp > NOW() - INTERVAL 1 HOUR ORDER BY timestamp DESC"
    elif range_param == 'day':
        query = "SELECT * FROM BME680Readings WHERE timestamp > NOW() - INTERVAL 1 DAY ORDER BY timestamp DESC"
    elif range_param == 'week':
        query = "SELECT * FROM BME680Readings WHERE timestamp > NOW() - INTERVAL 1 WEEK ORDER BY timestamp DESC"

    logger.debug(f"Executing query: {query}")
    logger.debug(f"Current server time: {datetime.now()}")

    connection = create_connection()
    if connection:
        try:
            cursor = connection.cursor(dictionary=True)
            cursor.execute(query)
            rows = cursor.fetchall()
            cursor.close()
            connection.close()
            for row in rows:
                row['timestamp'] = row['timestamp'].isoformat()
            logger.debug(f"Rows fetched: {rows}")
            return jsonify(rows), 200
        except Error as e:
            logger.error(f"Error while fetching data: {e}")
            return jsonify({'message': 'Failed to fetch data'}), 500
    else:
        return jsonify({'message': 'Failed to connect to database'}), 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
