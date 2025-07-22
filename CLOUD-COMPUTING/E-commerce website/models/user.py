from pymongo import MongoClient
from bson import ObjectId
from werkzeug.security import generate_password_hash, check_password_hash
from config import MONGO_URI

client = MongoClient(MONGO_URI)
db = client.ecommerce_db


class User:
    @staticmethod
    def get_all():
        return list(db.users.find())

    @staticmethod
    def get_by_id(user_id):
        return db.users.find_one({"_id": ObjectId(user_id)})

    @staticmethod
    def get_by_username(username):
        return db.users.find_one({"username": username})

    @staticmethod
    def create(username, password, is_admin=False):
        if User.get_by_username(username):
            return None

        user_data = {
            "username": username,
            "password": generate_password_hash(password),
            "is_admin": is_admin,
            "reviews": [],
            "review_count": 0,
            "rating_count": 0,
            "total_rating": 0,
            "avg_rating": 0
        }

        result = db.users.insert_one(user_data)
        return result.inserted_id

    @staticmethod
    def verify_password(username, password):
        user = User.get_by_username(username)
        if not user:
            return False
        return check_password_hash(user["password"], password)

    @staticmethod
    def delete(user_id):
        user = db.users.find_one({"_id": ObjectId(user_id)})

        if user:
            for review in user.get("reviews", []):
                item_id = review.get("item_id")
                if item_id:
                    db.items.update_one(
                        {"_id": ObjectId(item_id)},
                        {"$pull": {"reviews": {"user_id": ObjectId(user_id)}}}
                    )
                    item = db.items.find_one({"_id": ObjectId(item_id)})
                    if item and "reviews" in item:
                        reviews = item["reviews"]
                        avg = sum(r["rating"] for r in reviews) / len(reviews) if reviews else 0
                        db.items.update_one(
                            {"_id": ObjectId(item_id)},
                            {"$set": {"rating": round(avg, 1)}}
                        )

            db.users.delete_one({"_id": ObjectId(user_id)})

    @staticmethod
    def update_review_stats(user_id, rating, item_id=None, item_name=None, review_text=None, is_new_review=True):
        user = User.get_by_id(user_id)

        existing_review_index = next(
            (i for i, r in enumerate(user.get("reviews", []))
             if str(r.get("item_id")) == str(item_id)),
            None
        )

        if existing_review_index is not None:
            old_rating = user["reviews"][existing_review_index]["rating"]

            db.users.update_one(
                {"_id": ObjectId(user_id), "reviews.item_id": ObjectId(item_id)},
                {"$set": {
                    "reviews.$.rating": rating,
                    "reviews.$.text": review_text
                }}
            )

            db.users.update_one(
                {"_id": ObjectId(user_id)},
                {"$inc": {"total_rating": rating - old_rating}}
            )
        elif item_id and item_name:
            review = {
                "item_id": ObjectId(item_id),
                "item_name": item_name,
                "rating": rating,
                "text": review_text
            }

            db.users.update_one(
                {"_id": ObjectId(user_id)},
                {
                    "$push": {"reviews": review},
                    "$inc": {
                        "review_count": 1,
                        "total_rating": rating
                    }
                }
            )

        user = User.get_by_id(user_id)
        if user and user.get("reviews"):
            total = sum(review["rating"] for review in user["reviews"])
            count = len(user["reviews"])

            db.users.update_one(
                {"_id": ObjectId(user_id)},
                {"$set": {
                    "avg_rating": round(total / count, 1) if count > 0 else 0
                }}
            )

    @staticmethod
    def add_review_to_user(user_id, review):
        db.users.update_one(
            {"_id": ObjectId(user_id)},
            {"$push": {"reviews": review}}
        )
