# app.py
from flask import Flask, render_template
from pymongo import MongoClient
from config import MONGO_URI, SECRET_KEY

app = Flask(__name__)
app.secret_key = SECRET_KEY

client = MongoClient(MONGO_URI)
db = client.ecommerce_db

from routes import admin, auth, items, users

app.register_blueprint(admin.bp)
app.register_blueprint(auth.bp)
app.register_blueprint(items.bp)
app.register_blueprint(users.bp)

@app.route('/')
def home():
    items = list(db.items.find())
    categories = db.items.distinct("category")
    return render_template('index.html', items=items, categories=categories)

if __name__ == '__main__':
    app.run(debug=True)