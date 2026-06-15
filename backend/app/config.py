import os

# Database Configuration
DATABASE_URL = os.getenv("DATABASE_URL", "sqlite:///./solar_iot.db")

# MQTT Configuration
MQTT_BROKER = os.getenv("MQTT_BROKER", "broker.hivemq.com")
MQTT_PORT = int(os.getenv("MQTT_PORT", 1883))
MQTT_CLIENT_ID = os.getenv("MQTT_CLIENT_ID", "solar_dashboard_backend")

# List of managed ESP32 devices
DEVICE_IDS = [
    "esp32_1",
    "esp32_2",
    "esp32_3",
    "esp32_4",
    "esp32_5"
]

# MQTT Topics
TELEMETRY_TOPIC_TEMPL = "solar_iot/device/{device_id}/telemetry"
CONTROL_TOPIC_TEMPL = "solar_iot/device/{device_id}/control"
