# ========================================
# IoT Smart Hub FastAPI Backend (app.py)
# ========================================

from fastapi import FastAPI, APIRouter, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
from typing import List
from datetime import datetime, timedelta
from uuid import uuid4
import re

# --- FastAPI App and Router Setup ---
app = FastAPI()
api = APIRouter()

# --- CORS Configuration for Frontend ---
origins = ["https://simple-smart-hub-client.netlify.app"]
app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"]
)

# --- Data Models ---
class UserSettings(BaseModel):
    user_temp: float
    user_light: str       # Format: "sunset" or HH:MM:SS
    light_duration: str   # Format: e.g. "1h", "30m", "45s"

class SensorData(BaseModel):
    temperature: float
    presence: bool
    datetime: str

# --- In-Memory Storage ---
sensor_history: List[SensorData] = []
user_settings = {}

# --- Helper: Convert duration string to timedelta ---
def parse_duration(time_str: str) -> timedelta:
    """
    Converts a duration string like '1h30m45s' into a timedelta object.
    """
    regex = re.compile(r'((?P<hours>\d+?)h)?((?P<minutes>\d+?)m)?((?P<seconds>\d+?)s)?')
    parts = regex.match(time_str)
    if not parts:
        return timedelta()
    time_params = {k: int(v) for k, v in parts.groupdict().items() if v}
    return timedelta(**time_params)

# =====================================================
#                    API ROUTES
# =====================================================

# --- PUT /api/settings ---
@api.put("/settings")
def update_settings(settings: UserSettings):
    """
    Saves user-defined fan/light settings and returns a response
    with a generated _id to simulate a database document.
    """
    now = datetime.now().replace(microsecond=0)
    duration = parse_duration(settings.light_duration)

    # Use fixed time for 'sunset' or parse the given time
    user_light_time = (
        now.replace(hour=17, minute=43, second=21)
        if settings.user_light.lower() == "sunset"
        else datetime.strptime(settings.user_light, "%H:%M:%S")
    )

    light_off_time = (user_light_time + duration).time()

    user_settings.update({
        "user_temp": settings.user_temp,
        "user_light": user_light_time.time(),
        "light_time_off": light_off_time
    })

    return {
        "_id": str(uuid4()),  # Simulated database-generated ID
        **user_settings
    }

# --- POST /api/data ---
@api.post("/data")
def receive_sensor_data(data: SensorData):
    """
    Receives and stores sensor data from ESP32.
    """
    sensor_history.append(data)
    return {"message": "Data received"}

# --- GET /api/graph?size=N ---
@api.get("/graph")
def get_graph(size: int):
    """
    Returns the last N sensor data points for graphing.
    """
    return sensor_history[-size:]

# --- GET /api/decision ---
@api.get("/decision")
def get_decision():
    """
    Returns control logic for fan and light based on the
    latest sensor data and current time.
    """
    if not user_settings or not sensor_history:
        raise HTTPException(status_code=400, detail="Missing data")

    last = sensor_history[-1]
    now = datetime.now().time()

    fan = last.temperature >= user_settings["user_temp"] and last.presence
    light = (
        user_settings["user_light"] <= now <= user_settings["light_time_off"]
        and last.presence
    )

    return {
        "turn_fan_on": fan,
        "turn_light_on": light
    }

# --- Register Router under /api prefix ---
app.include_router(api, prefix="/api")
