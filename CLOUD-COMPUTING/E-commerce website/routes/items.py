# routes/items.py
from flask import Blueprint, request, render_template, redirect, url_for, session, flash
from models.item import Item
from bson import ObjectId

from models.user import User

bp = Blueprint('items', __name__, url_prefix='/items')


@bp.route('/')
def index():
    category = request.args.get('category')
    items = Item.get_all(category=category)
    categories = ["Vinyls", "Antique Furniture", "GPS Sport Watches", "Running Shoes"]
    return render_template('index.html', items=items, categories=categories, selected_category=category)


@bp.route('/<item_id>')
def view(item_id):
    item = Item.get_by_id(item_id)
    if not item:
        flash('Item not found')
        return redirect(url_for('items.index'))

    return render_template('items/view.html', item=item)


@bp.route('/<item_id>/review', methods=['POST'])
def review(item_id):
    if 'user_id' not in session:
        flash('You must be logged in to review items')
        return redirect(url_for('items.view', item_id=item_id))

    rating = int(request.form['rating'])
    review_text = request.form['review']

    if rating < 1 or rating > 10:
        flash('Rating must be between 1 and 10')
        return redirect(url_for('items.view', item_id=item_id))

    is_new_review = Item.add_review(
        item_id=item_id,
        user_id=session['user_id'],
        user_name=session['username'],
        rating=rating,
        review_text=review_text
    )

    item = Item.get_by_id(item_id)

    User.update_review_stats(
        session['user_id'],
        rating,
        item_id=item_id,
        item_name=item['name'],
        review_text=review_text,
        is_new_review=is_new_review
    )

    flash('Your review has been submitted')
    return redirect(url_for('items.view', item_id=item_id))

