# models/item.py
from pymongo import MongoClient
from bson import ObjectId
from config import MONGO_URI

client = MongoClient(MONGO_URI)
db = client.ecommerce_db


class Item:
    @staticmethod
    def get_all(category=None):
        query = {}
        if category:
            query["category"] = category
        return list(db.items.find(query))

    @staticmethod
    def get_by_id(item_id):
        return db.items.find_one({"_id": ObjectId(item_id)})

    @staticmethod
    def create(item_data):
        item_data["rating"] = 0
        item_data["review_count"] = 0
        item_data["reviews"] = []

        result = db.items.insert_one(item_data)
        return result.inserted_id

    @staticmethod
    def update(item_id, item_data):
        db.items.update_one(
            {"_id": ObjectId(item_id)},
            {"$set": item_data}
        )

    @staticmethod
    def delete(item_id):
        item = db.items.find_one({"_id": ObjectId(item_id)})

        if item:
            for review in item.get("reviews", []):
                user_id = review.get("user_id")
                if user_id:
                    db.users.update_one(
                        {"_id": ObjectId(user_id)},
                        {
                            "$pull": {"reviews": {"item_id": ObjectId(item_id)}},
                            "$inc": {"rating_count": -1}
                        }
                    )

            db.items.delete_one({"_id": ObjectId(item_id)})

    @staticmethod
    def add_review(item_id, user_id, user_name, rating, review_text):
        item = Item.get_by_id(item_id)
        if not item:
            return False

        existing_review = next(
            (r for r in item.get("reviews", []) if str(r.get("user_id")) == str(user_id)),
            None
        )

        if existing_review:
            db.items.update_one(
                {"_id": ObjectId(item_id), "reviews.user_id": ObjectId(user_id)},
                {
                    "$set": {
                        "reviews.$.rating": rating,
                        "reviews.$.text": review_text
                    }
                }
            )
            is_new_review = False
        else:
            review = {
                "user_id": ObjectId(user_id),
                "user_name": user_name,
                "rating": rating,
                "text": review_text
            }
            db.items.update_one(
                {"_id": ObjectId(item_id)},
                {"$push": {"reviews": review}, "$inc": {"review_count": 1}}
            )
            is_new_review = True

        update_avg_rating(item_id)

        return is_new_review


def update_avg_rating(item_id):
    item = db.items.find_one({"_id": ObjectId(item_id)})
    if not item or "reviews" not in item:
        return

    reviews = item["reviews"]
    if not reviews:
        db.items.update_one(
            {"_id": ObjectId(item_id)},
            {"$set": {"rating": 0}}
        )
        return

    total = sum(review["rating"] for review in reviews)
    avg_rating = total / len(reviews)

    db.items.update_one(
        {"_id": ObjectId(item_id)},
        {"$set": {"rating": round(avg_rating, 1)}}
    )