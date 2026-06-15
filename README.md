# IoT Solar Energy Monitor (Student Project)

This project is a scalable, dynamic **IoT Solar Energy Monitor** designed to run on a Raspberry Pi Zero 2W. It monitors solar panel inputs, battery charging metrics, and load consumption across multiple stations (nodes).

The project includes a **Node.js/Express.js Backend**, a **SQLite Database**, a **React Dashboard Frontend** (built with Vite), and a **Python Hardware Simulator** representing 5 virtual solar stations.

---

## 🚀 How to Run the Project

The project includes an automation script `run_project.ps1` in the root directory to launch the entire stack on Windows.

1. **Prerequisites:**
   - [Node.js](https://nodejs.org/) (v18+)
   - [Python 3](https://www.python.org/) with `paho-mqtt` library (install via `pip install paho-mqtt`)
   - An active internet connection (to connect to the public HiveMQ MQTT broker)

2. **Start the Stack:**
   Open PowerShell in the project root folder and execute:
   ```powershell
   .\run_project.ps1
   ```
   This will spin up two separate terminal windows:
   - **Express.js Backend** (running on port `5000`)
   - **Python Hardware Simulator** (simulating 5 nodes)

3. **Open the Web UI:**
   Open your browser and navigate to:
   ```
   http://localhost:5000
   ```

4. **Register a Simulated Node:**
   - Copy one of the generated **MAC Addresses** shown in the Simulator console (e.g. `00:1A:2B:3C:4D:01`).
   - Click the **"Add Solar Node"** button on the web dashboard.
   - Enter the MAC Address, a name for the station, and hardware specifications.
   - Once submitted, you will immediately see live stats and charts on your dashboard!

---

## 🛠️ System Architecture & Specifications

```
  [Python Simulator]                          [Raspberry Pi Zero 2W]
Simulated Arduino Nano + ESP8266            Express.js Backend + SQLite + Mosquitto
  │ (Sends UART Frame every 5s)               │
  ▼                                           ▼
solarcharger/sensors/<mac> ─────────────► MQTT Broker (broker.hivemq.com)
                                              │
                                              ▼
                                       Express.js Engine
                                              │
                                     ┌────────┴────────┐
                                     ▼                 ▼
                               SQLite Database   React Dashboard
                             (1-mo raw, 3-mo avg) (HTTP Polling 5s)
```

### 1. MQTT Topics & Prefix Isolation
To avoid collisions on the public HiveMQ broker, all topics are prefixed with `solarcharger`:
- **Sensors (ESP8266 → Mosquitto → Express):** `solarcharger/sensors/<mac_address>`
- **Control (Express → Mosquitto → ESP8266 → Arduino):** `solarcharger/control/<mac_address>`
- **Status (LWT Online/Offline):** `solarcharger/status/<mac_address>`

### 2. JSON Telemetry Format (UART Frame)
The ESP8266 publishes telemetry sent by the Arduino Nano via UART every 5 seconds:
```json
{
  "v_solar": 18.2,    // Solar panel voltage (V)
  "i_solar": 3.4,     // Solar panel current (A)
  "i_charge": 3.1,    // Battery charging current (A)
  "v_bat": 13.2,      // Battery voltage (V)
  "v_load": 13.2,     // Load voltage (V)
  "i_load": 1.2,      // Load current (A)
  "lux": 850,         // Light intensity (Lux)
  "relay1": 1,        // Relay 1 status (Charge path: 0=OFF, 1=ON)
  "relay2": 1         // Relay 2 status (Load path: 0=OFF, 1=ON)
}
```

### 3. SQLite Database & Data Retention (ADR-0003)
Because the Raspberry Pi Zero 2W has limited storage and compute:
- **`nodes`:** Stores registered MAC addresses, names, and capacity specs.
- **`telemetry_raw`:** Stores raw telemetry points (every 5s) for **1 month**.
- **`telemetry_hourly`:** Stores hourly aggregated averages.
- **Data Cleanup Job:** Runs daily at midnight.
  - Aggregates raw telemetry older than 1 month into hourly averages.
  - Deletes raw telemetry older than 1 month.
  - Deletes aggregated hourly data older than 3 months.

---

## 📂 Project Structure

```
SolarCharger/
│
├── backend/
│   ├── database.js          # SQLite setup and table schemas
│   ├── index.js             # Express server, APIs, MQTT listener, and cron job
│   └── package.json         # Backend node dependencies
│
├── frontend/
│   ├── dist/                # Compiled production static files served by Express
│   ├── src/
│   │   ├── App.jsx          # Dashboard root layout, node registration modal
│   │   └── components/
│   │       ├── Navbar.jsx           # Top header and status indicator
│   │       ├── StatsOverview.jsx    # Aggregated metrics (Total solar, active nodes)
│   │       ├── DashboardGrid.jsx    # Display grid for solar node cards
│   │       ├── DeviceCard.jsx       # Individual node panel with dual relay controls
│   │       └── DeviceDetailsModal.js# Recharts line charts (Power, Bat, Solar, Logs)
│   └── package.json         # Frontend build tools and libraries
│
├── simulator/
│   ├── simulator.py         # Multi-node MQTT simulator script
│   └── start_simulator.ps1  # Helper script to launch Python simulator
│
└── run_project.ps1          # Main automation script to start the stack
```
