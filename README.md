# Chemical-reaction-chamber-environment-monitoring-and-control-system-using-IoT
IoT-based system designed for environmental monitoring and control in industrial settings, such as chemical processes or materials manufacturing. The system continuously monitors critical environmental parameters and automates corrective actions to enhance operational safety and efficiency.


**Hardware and Components**: 
The system is built around an ESP32 Microcontroller , which acts as the central processing unit. It utilizes a DHT22 sensor for real-time temperature and humidity monitoring. An MQ135 air quality sensor detects harmful gases and pollutants , while a relay module controls external devices like fans.
Real-Time Feedback Loop: The ESP32 continuously evaluates sensor readings against predefined safety thresholds. When a threshold is exceeded, the relay module automatically activates a corrective device. The system uses a Green LED to indicate normal conditions and a Red LED to signal a critical state.
Remote Monitoring: A built-in Wi-Fi module allows for remote monitoring and control via a simple web interface, which displays real-time data and system status.


**Files and Directories**:
source_code: Contains the Arduino code for the ESP32, including Wi-Fi connectivity, web server setup, sensor data collection, and control logic.

stable_state.png: Image showing the system's output and physical setup during a stable state, where all parameters are within safe limits.

critical_state.png: Image demonstrating the system's response during a critical state, with the alarm triggered and the Red LED activated.

setup_image.png: Photograph of the complete project prototype, showcasing the integrated ESP32, sensors, and relay module.
