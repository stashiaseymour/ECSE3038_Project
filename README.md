# Simple Smart Hub – IoT Project

This project is submitted for the course **ECSE3038 – Engineering Internet of Things Systems** at The University of the West Indies. It demonstrates the application of IoT principles to create a real-time smart home automation system that uses environmental sensor data to control devices such as a fan and light.

---

##  Project Overview

The Simple Smart Hub is an Internet of Things (IoT) system that simulates automated home control. It consists of three key parts:

1. **ESP32 Microcontroller**  
   - Collects real-time environmental data:
     - Temperature (via DS18B20 sensor)
     - Motion presence (via PIR sensor)
   - Sends sensor readings to a cloud-based backend using HTTPS
   - Receives control signals from the backend to turn on/off a fan and light

2. **FastAPI Backend (Hosted on Render)**  
   - Receives sensor data from the ESP32
   - Stores recent readings in memory
   - Accepts user-defined settings from the frontend (e.g., desired room temperature, light schedule)
   - Applies logic to decide whether the fan or light should be turned on
   - Provides a decision response to the ESP32 in real-time

3. **Frontend Interface (Hosted on Netlify)**  
   - Allows users to configure:
     - Target temperature threshold
     - Light-on time (specific time or 'sunset')
     - Duration the light should remain on
   - Displays sensor data graphically
   - Communicates with the backend API for settings and updates

---

##  Features

-  Wi-Fi-enabled ESP32 firmware
-  Real-time temperature and motion detection
-  FastAPI backend with RESTful API endpoints
-  Frontend with live chart visualization and user configuration
-  HTTPS-secured data transmission (via `WiFiClientSecure`)
-  Backend decision logic to automate fan and light control
-  In-memory processing without a database (for simplicity and speed)

---

##  How It Works

1. The ESP32 starts up, connects to Wi-Fi, and begins reading temperature and motion values.
2. Every 10 seconds, the ESP32 sends the data to the `/api/data` endpoint on the backend.
3. The backend stores this data and evaluates it against user preferences:
   - If the temperature exceeds the threshold **and** motion is detected → the fan turns on.
   - If the current time is within the user-defined light window **and** motion is detected → the light turns on.
4. These decisions are returned to the ESP32 via `/api/decision`, and the hardware reacts accordingly.
5. Users interact with the system through a Netlify-hosted web app.

---

##  Technologies Used

- **Microcontroller:** ESP32 with Arduino framework
- **Sensors:** DS18B20 temperature sensor, PIR motion sensor
- **Programming Language:** C++ (ESP32), Python (FastAPI)
- **Backend Framework:** FastAPI
- **Frontend:** Provided Netlify-based dashboard
- **Hosting:** Render (for backend), Netlify (for frontend)
- **Data Transfer:** JSON over HTTPS using `WiFiClientSecure`

---

##  Deployment Guide

###  ESP32
- Code is written using PlatformIO (VS Code extension)
- Configuration credentials are stored in `config.h`
- Upload code to ESP32 and monitor output via Serial Monitor

###  Backend
- Deployed to Render as a Python Web Service
- Exposes API endpoints under `/api/`
- No database is used — sensor data and settings are stored in memory

###  Frontend
- Hosted on Netlify
- Requires user to enter the Render API URL (e.g., `https://iot-fastapi-backend.onrender.com/api`)

---

##  Notes on Security

- Sensitive credentials (Wi-Fi SSID, password, server address) are stored in `config.h`
- `config.h` is excluded from version control using `.gitignore`
- HTTPS is used for all ESP32 → Backend communication to ensure secure data transfer

---

##  Project Details

- **Course:** ECSE3038 – Engineering Internet of Things Systems  
- **University:** The University of the West Indies  
- **Author:** Stashia Seymour 
- **Instructor:** Mr Phillip Logan

---

## Status

All system components have been implemented, tested, and deployed successfully.  
The project meets the functional requirements of ECSE3038 and demonstrates a practical implementation of IoT concepts, cloud integration, and real-time control logic.
