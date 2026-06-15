from datetime import datetime, timedelta
from sqlalchemy.orm import Session
from sqlalchemy import desc
from app import models, schemas

def save_sensor_data(db: Session, data: schemas.SensorDataCreate):
    # 1. Save to historical logs
    db_log = models.SensorData(
        device_id=data.device_id,
        battery_voltage=data.battery_voltage,
        battery_current=data.battery_current,
        solar_voltage=data.solar_voltage,
        solar_current=data.solar_current,
        relay_status=data.relay_status,
        timestamp=datetime.utcnow()
    )
    db.add(db_log)

    # 2. Upsert DeviceStatus
    db_status = db.query(models.DeviceStatus).filter(models.DeviceStatus.device_id == data.device_id).first()
    if not db_status:
        db_status = models.DeviceStatus(device_id=data.device_id)
        db.add(db_status)
    
    db_status.battery_voltage = data.battery_voltage
    db_status.battery_current = data.battery_current
    db_status.solar_voltage = data.solar_voltage
    db_status.solar_current = data.solar_current
    db_status.relay_status = data.relay_status
    db_status.last_seen = datetime.utcnow()

    db.commit()
    db.refresh(db_status)
    return db_log

def get_devices_status(db: Session):
    statuses = db.query(models.DeviceStatus).all()
    result = []
    now = datetime.utcnow()
    
    for s in statuses:
        # A device is considered online if it sent data within the last 10 seconds
        is_online = (now - s.last_seen) < timedelta(seconds=10)
        
        # Power in Watts (W) = Voltage (V) * Current (A)
        battery_power = round(s.battery_voltage * s.battery_current, 2)
        solar_power = round(s.solar_voltage * s.solar_current, 2)
        
        result.append({
            "device_id": s.device_id,
            "last_seen": s.last_seen,
            "relay_status": s.relay_status,
            "battery_voltage": s.battery_voltage,
            "battery_current": s.battery_current,
            "solar_voltage": s.solar_voltage,
            "solar_current": s.solar_current,
            "battery_power": battery_power,
            "solar_power": solar_power,
            "is_online": is_online
        })
    return result

def get_device_history(db: Session, device_id: str, limit: int = 100):
    logs = db.query(models.SensorData) \
             .filter(models.SensorData.device_id == device_id) \
             .order_by(desc(models.SensorData.timestamp)) \
             .limit(limit) \
             .all()
    # Reverse so it's chronological (oldest to newest) for charting
    logs.reverse()
    return logs

def update_relay_status(db: Session, device_id: str, relay_status: bool):
    db_status = db.query(models.DeviceStatus).filter(models.DeviceStatus.device_id == device_id).first()
    if db_status:
        db_status.relay_status = relay_status
        db.commit()
        db.refresh(db_status)
        return db_status
    return None
