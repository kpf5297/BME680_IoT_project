import sys
import requests
from datetime import datetime
from PyQt5.QtWidgets import (QApplication, QMainWindow, QVBoxLayout, QLabel, 
                             QPushButton, QWidget, QLineEdit, QHBoxLayout, 
                             QCheckBox, QGridLayout, QComboBox)
from PyQt5.QtCore import QTimer
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

class SensorApp(QMainWindow):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        """Initialize the UI elements of the application."""
        self.setWindowTitle('Sensor Data Dashboard')

        # Set up the central widget
        self.central_widget = QWidget()
        self.setCentralWidget(self.central_widget)
        self.layout = QVBoxLayout(self.central_widget)

        # Server IP input
        self.ip_layout = QHBoxLayout()
        self.ip_label = QLabel('Server IP:')
        self.ip_input = QLineEdit(self)
        self.ip_input.setPlaceholderText('Enter server IP address')
        self.ip_input.setText('localhost')  # Default value

        self.ip_layout.addWidget(self.ip_label)
        self.ip_layout.addWidget(self.ip_input)
        self.layout.addLayout(self.ip_layout)

        # Time range selection
        self.time_range_layout = QHBoxLayout()
        self.time_range_label = QLabel('Time Range:')
        self.time_range_combo = QComboBox(self)
        self.time_range_combo.addItems(['Last Minute', 'Last Hour', 'Last Day', 'Last Week'])
        self.time_range_combo.currentIndexChanged.connect(self.update_data)

        self.time_range_layout.addWidget(self.time_range_label)
        self.time_range_layout.addWidget(self.time_range_combo)
        self.layout.addLayout(self.time_range_layout)

        # Data labels
        self.temperature_label = QLabel('Temperature: N/A')
        self.layout.addWidget(self.temperature_label)
        self.pressure_label = QLabel('Pressure: N/A')
        self.layout.addWidget(self.pressure_label)
        self.humidity_label = QLabel('Humidity: N/A')
        self.layout.addWidget(self.humidity_label)
        self.gas_label = QLabel('Gas Resistance: N/A')
        self.layout.addWidget(self.gas_label)

        # Checkboxes for selecting units
        self.checkbox_layout = QGridLayout()
        self.checkbox_temp_c = QCheckBox('Temperature (°C)')
        self.checkbox_temp_f = QCheckBox('Temperature (°F)')
        self.checkbox_pressure_hpa = QCheckBox('Pressure (hPa)')
        self.checkbox_pressure_inhg = QCheckBox('Pressure (inHg)')
        self.checkbox_humidity = QCheckBox('Humidity (%)')
        self.checkbox_gas = QCheckBox('Gas Resistance (Ohms)')

        self.checkbox_layout.addWidget(self.checkbox_temp_c, 0, 0)
        self.checkbox_layout.addWidget(self.checkbox_temp_f, 0, 1)
        self.checkbox_layout.addWidget(self.checkbox_pressure_hpa, 1, 0)
        self.checkbox_layout.addWidget(self.checkbox_pressure_inhg, 1, 1)
        self.checkbox_layout.addWidget(self.checkbox_humidity, 2, 0)
        self.checkbox_layout.addWidget(self.checkbox_gas, 2, 1)
        self.layout.addLayout(self.checkbox_layout)

        # Graph canvases
        self.temp_canvas = FigureCanvas(Figure())
        self.layout.addWidget(self.temp_canvas)
        self.temp_ax = self.temp_canvas.figure.add_subplot(111)

        self.pressure_canvas = FigureCanvas(Figure())
        self.layout.addWidget(self.pressure_canvas)
        self.pressure_ax = self.pressure_canvas.figure.add_subplot(111)

        self.humidity_canvas = FigureCanvas(Figure())
        self.layout.addWidget(self.humidity_canvas)
        self.humidity_ax = self.humidity_canvas.figure.add_subplot(111)

        self.gas_canvas = FigureCanvas(Figure())
        self.layout.addWidget(self.gas_canvas)
        self.gas_ax = self.gas_canvas.figure.add_subplot(111)

        # Start/Stop buttons
        self.button_layout = QHBoxLayout()
        self.start_button = QPushButton('Start')
        self.start_button.clicked.connect(self.start_timer)
        self.stop_button = QPushButton('Stop')
        self.stop_button.clicked.connect(self.stop_timer)
        self.button_layout.addWidget(self.start_button)
        self.button_layout.addWidget(self.stop_button)
        self.layout.addLayout(self.button_layout)

        # Timer for updating data
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.update_data)

    def start_timer(self):
        """Start the timer to refresh data every 5 seconds."""
        self.timer.start(5000)

    def stop_timer(self):
        """Stop the timer."""
        self.timer.stop()

    def update_data(self):
        """Fetch and update the data from the server."""
        server_ip = self.ip_input.text()
        time_range = self.time_range_combo.currentText()

        # Construct the URL based on the selected time range
        if time_range == 'Last Minute':
            url = f'http://{server_ip}:5000/get_bme680_data?range=minute'
        elif time_range == 'Last Hour':
            url = f'http://{server_ip}:5000/get_bme680_data?range=hour'
        elif time_range == 'Last Day':
            url = f'http://{server_ip}:5000/get_bme680_data?range=day'
        elif time_range == 'Last Week':
            url = f'http://{server_ip}:5000/get_bme680_data?range=week'

        # Fetch the data and update the UI
        try:
            response = requests.get(url)
            data = response.json()
            self.plot_data(data)
            if data:
                latest_data = data[0]
                temperature_c = latest_data['temperature']
                temperature_f = (temperature_c * 9/5) + 32
                pressure_hpa = latest_data['pressure']
                pressure_inhg = pressure_hpa * 0.02953

                self.temperature_label.setText(f"Temperature: {temperature_c:.2f} °C / {temperature_f:.2f} °F")
                self.pressure_label.setText(f"Pressure: {pressure_hpa:.2f} hPa / {pressure_inhg:.2f} inHg")
                self.humidity_label.setText(f"Humidity: {latest_data['humidity']} %")
                self.gas_label.setText(f"Gas Resistance: {latest_data['gas_resistance']} Ohms")
        except Exception as e:
            print(f"Failed to update data: {e}")

    def plot_data(self, data):
        """Plot the data on the graphs."""
        timestamps = [datetime.fromisoformat(row['timestamp']) for row in data]
        temperatures_c = [row['temperature'] for row in data]
        temperatures_f = [(temp * 9/5) + 32 for temp in temperatures_c]
        pressures_hpa = [row['pressure'] for row in data]
        pressures_inhg = [pressure * 0.02953 for pressure in pressures_hpa]
        humidities = [row['humidity'] for row in data]
        gas_resistances = [row['gas_resistance'] for row in data]

        # Clear previous plots
        self.temp_ax.clear()
        self.pressure_ax.clear()
        self.humidity_ax.clear()
        self.gas_ax.clear()

        # Plot temperature data
        if self.checkbox_temp_c.isChecked():
            self.temp_ax.plot(timestamps, temperatures_c, label='Temperature (°C)')
        if self.checkbox_temp_f.isChecked():
            self.temp_ax.plot(timestamps, temperatures_f, label='Temperature (°F)')
        self.temp_ax.legend()
        self.temp_ax.set_title('Temperature over Time')
        self.temp_ax.set_xlabel('Timestamp')
        self.temp_ax.set_ylabel('Temperature')
        self.temp_ax.figure.autofmt_xdate()
        self.temp_canvas.draw()

        # Plot pressure data
        if self.checkbox_pressure_hpa.isChecked():
            self.pressure_ax.plot(timestamps, pressures_hpa, label='Pressure (hPa)')
        if self.checkbox_pressure_inhg.isChecked():
            self.pressure_ax.plot(timestamps, pressures_inhg, label='Pressure (inHg)')
        self.pressure_ax.legend()
        self.pressure_ax.set_title('Pressure over Time')
        self.pressure_ax.set_xlabel('Timestamp')
        self.pressure_ax.set_ylabel('Pressure')
        self.pressure_ax.figure.autofmt_xdate()
        self.pressure_canvas.draw()

        # Plot humidity data
        if self.checkbox_humidity.isChecked():
            self.humidity_ax.plot(timestamps, humidities, label='Humidity (%)')
        self.humidity_ax.legend()
        self.humidity_ax.set_title('Humidity over Time')
        self.humidity_ax.set_xlabel('Timestamp')
        self.humidity_ax.set_ylabel('Humidity')
        self.humidity_ax.figure.autofmt_xdate()
        self.humidity_canvas.draw()

        # Plot gas resistance data
        if self.checkbox_gas.isChecked():
            self.gas_ax.plot(timestamps, gas_resistances, label='Gas Resistance (Ohms)')
        self.gas_ax.legend()
        self.gas_ax.set_title('Gas Resistance over Time')
        self.gas_ax.set_xlabel('Timestamp')
        self.gas_ax.set_ylabel('Gas Resistance')
        self.gas_ax.figure.autofmt_xdate()
        self.gas_canvas.draw()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    ex = SensorApp()
    ex.show()
    sys.exit(app.exec_())
