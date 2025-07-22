# config.py
from dotenv import load_dotenv
import os

load_dotenv()

MONGO_URI = os.getenv('MONGODB_URI')
SECRET_KEY = os.getenv('SECRET_KEY')
