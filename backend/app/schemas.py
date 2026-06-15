from datetime import datetime
from pydantic import BaseModel
from typing import Optional

class SensorDataBase(BaseModel):
    battery_voltage: float
    battery_current: float
    solar_voltage: float
    solar_current: float
    relay_status: bool

class SensorDataCreate(SensorDataBase):
    device_id: str

class SensorDataResponse(SensorDataBase):
    id: int
    device_id: str
    timestamp: datetime

    class Config:
        from_attributes = True

class DeviceStatusResponse(BaseModel):
    device_id: str
    last_seen: datetime
    relay_status: bool
    battery_voltage: float
    battery_current: float
    solar_voltage: float
    solar_current: float
    battery_power: float  # calculated
    solar_power: float    # calculated
    is_online: bool       # calculated based on last_seen

    class Config:
        from_attributes = True

class RelayControlRequest(BaseModel):
    relay: bool
