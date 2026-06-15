from datetime import datetime
from sqlalchemy import Column, Integer, String, Float, Boolean, DateTime
from app.database import Base

class SensorData(Base):
    __tablename__ = "sensors_data"

    id = Column(Integer, primary_key=True, index=True)
    device_id = Column(String, index=True, nullable=False)
    battery_voltage = Column(Float, nullable=False)
    battery_current = Column(Float, nullable=False)
    solar_voltage = Column(Float, nullable=False)
    solar_current = Column(Float, nullable=False)
    relay_status = Column(Boolean, default=False)
    timestamp = Column(DateTime, default=datetime.utcnow, nullable=False)

class DeviceStatus(Base):
    __tablename__ = "device_status"

    device_id = Column(String, primary_key=True, index=True)
    last_seen = Column(DateTime, default=datetime.utcnow, onupdate=datetime.utcnow, nullable=False)
    relay_status = Column(Boolean, default=False)
    battery_voltage = Column(Float, nullable=False)
    battery_current = Column(Float, nullable=False)
    solar_voltage = Column(Float, nullable=False)
    solar_current = Column(Float, nullable=False)
