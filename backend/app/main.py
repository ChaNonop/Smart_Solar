import json
from contextlib import asynccontextmanager
from fastapi import FastAPI, Depends, HTTPException
from fastapi.middleware.cors import CORSMiddleware
import paho.mqtt.client as mqtt
from sqlalchemy.orm import Session
from typing import List

from app import models, schemas, crud, config
from app.database import engine, get_db, SessionLocal

# Ensure database tables are created
models.Base.metadata.create_all(bind=engine)

# Global MQTT Client setup
mqtt_client = mqtt.Client(client_id=config.MQTT_CLIENT_ID)

def on_connect(client, userdata, flags, rc):
    print(f"Connected to MQTT Broker with result code {rc}")
    # Subscribe to telemetry topics for all devices
    for device_id in config.DEVICE_IDS:
        topic = config.TELEMETRY_TOPIC_TEMPL.format(device_id=device_id)
        client.subscribe(topic)
        print(f"Subscribed to topic: {topic}")

def on_message(client, userdata, msg):
    try:
        topic = msg.topic
        parts = topic.split("/")
        if len(parts) >= 3:
            device_id = parts[2]
            payload = json.loads(msg.payload.decode())
            
            # Save telemetry log and update device status in SQLite
            db = SessionLocal()
            try:
                sensor_create = schemas.SensorDataCreate(
                    device_id=device_id,
                    battery_voltage=float(payload.get("battery_voltage", 0.0)),
                    battery_current=float(payload.get("battery_current", 0.0)),
                    solar_voltage=float(payload.get("solar_voltage", 0.0)),
                    solar_current=float(payload.get("solar_current", 0.0)),
                    relay_status=bool(payload.get("relay_status", False))
                )
                crud.save_sensor_data(db, sensor_create)
            except Exception as e:
                print(f"Database error saving MQTT telemetry: {e}")
            finally:
                db.close()
    except Exception as e:
        print(f"Error parsing MQTT message payload: {e}")

mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message

# FastAPI Lifespan to manage MQTT background thread
@asynccontextmanager
async def lifespan(app: FastAPI):
    # Startup: Connect to MQTT and start background loop
    try:
        mqtt_client.connect(config.MQTT_BROKER, config.MQTT_PORT, 60)
        mqtt_client.loop_start()
        print(f"MQTT Client loop started. Connected to {config.MQTT_BROKER}:{config.MQTT_PORT}")
    except Exception as e:
        print(f"MQTT connection failed on startup: {e}. (Will continue running, but MQTT features will fail)")
    
    yield
    
    # Shutdown: Stop loop and disconnect
    mqtt_client.loop_stop()
    mqtt_client.disconnect()
    print("MQTT Client loop stopped.")

app = FastAPI(
    title="IoT Solar Dashboard API", 
    description="Backend service for monitoring ESP32 Solar Panels and controlling relays",
    lifespan=lifespan
)

# CORS middleware for React app running on Vite (port 5173 by default)
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # Allow all origins for dev/testing ease
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

@app.get("/")
def read_root():
    return {
        "status": "online",
        "message": "Solar IoT Dashboard Backend API is running.",
        "broker": config.MQTT_BROKER,
        "monitored_devices": config.DEVICE_IDS
    }

@app.get("/api/devices", response_model=List[schemas.DeviceStatusResponse])
def get_all_devices(db: Session = Depends(get_db)):
    """
    Get the latest status of all monitored ESP32 devices.
    If a device has never sent telemetry, it will return zeros.
    """
    statuses = crud.get_devices_status(db)
    
    # Initialize empty records for monitored devices if they don't exist yet
    status_map = {s["device_id"]: s for s in statuses}
    result = []
    
    for dev_id in config.DEVICE_IDS:
        if dev_id in status_map:
            result.append(status_map[dev_id])
        else:
            # Placeholder for devices that haven't sent data yet
            result.append({
                "device_id": dev_id,
                "last_seen": "2000-01-01T00:00:00",
                "relay_status": False,
                "battery_voltage": 0.0,
                "battery_current": 0.0,
                "solar_voltage": 0.0,
                "solar_current": 0.0,
                "battery_power": 0.0,
                "solar_power": 0.0,
                "is_online": False
            })
    return result

@app.get("/api/devices/{device_id}/history", response_model=List[schemas.SensorDataResponse])
def get_device_history(device_id: str, limit: int = 50, db: Session = Depends(get_db)):
    """
    Get the historical telemetry log for a specific device.
    """
    if device_id not in config.DEVICE_IDS:
        raise HTTPException(status_code=404, detail="Device ID not monitored")
    return crud.get_device_history(db, device_id=device_id, limit=limit)

@app.post("/api/devices/{device_id}/relay")
def control_device_relay(device_id: str, payload: schemas.RelayControlRequest, db: Session = Depends(get_db)):
    """
    Send a command via MQTT to toggle the relay on the ESP32 device,
    and updates its local database state immediately.
    """
    if device_id not in config.DEVICE_IDS:
        raise HTTPException(status_code=404, detail="Device ID not monitored")
    
    # 1. Publish command to MQTT
    topic = config.CONTROL_TOPIC_TEMPL.format(device_id=device_id)
    control_payload = {"relay": payload.relay}
    
    try:
        mqtt_client.publish(topic, json.dumps(control_payload))
        print(f"Published control payload {control_payload} to topic {topic}")
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"Failed to publish MQTT message: {str(e)}")
    
    # 2. Speculatively update SQLite so the database reflects the transition immediately
    crud.update_relay_status(db, device_id=device_id, relay_status=payload.relay)
    
    return {
        "device_id": device_id,
        "status": "command_sent",
        "relay": payload.relay
    }
