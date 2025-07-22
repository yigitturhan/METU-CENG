# routes/admin.py
from flask import Blueprint, request, render_template, redirect, url_for, session, flash
from models.item import Item
from models.user import User
from functools import wraps

bp = Blueprint('admin', __name__, url_prefix='/admin')


def admin_required(f):
    @wraps(f)
    def decorated_function(*args, **kwargs):
        if 'user_id' not in session or not session.get('is_admin', False):
            flash('Admin access required')
            return redirect(url_for('home'))
        return f(*args, **kwargs)

    return decorated_function


@bp.route('/')
@admin_required
def index():
    return render_template('admin/index.html')


@bp.route('/items')
@admin_required
def items():
    items_list = Item.get_all()
    return render_template('admin/items.html', items=items_list)


@bp.route('/items/add', methods=['GET', 'POST'])
@admin_required
def add_item():
    if request.method == 'POST':
        item_data = {
            'name': request.form['name'],
            'description': request.form['description'],
            'category': request.form['category'],
            'price': float(request.form['price']),
            'seller': request.form['seller'],
            'image': request.form['image']
        }

        if item_data['category'] == 'GPS Sport Watches':
            item_data['battery_life'] = request.form['battery_life']

        if item_data['category'] in ['Antique Furniture', 'Vinyls']:
            item_data['age'] = request.form['age']

        if item_data['category'] in ['Running Shoes']:
            item_data['size'] = request.form['size']

        if item_data['category'] in ['Antique Furniture', 'Running Shoes']:
            item_data['material'] = request.form['material']

        Item.create(item_data)
        flash('Item added successfully')
        return redirect(url_for('admin.items'))

    return render_template('admin/add_item.html')


@bp.route('/items/<item_id>/delete', methods=['POST'])
@admin_required
def delete_item(item_id):
    Item.delete(item_id)
    flash('Item deleted successfully')
    return redirect(url_for('admin.items'))


@bp.route('/users')
@admin_required
def users():
    users_list = User.get_all()
    return render_template('admin/users.html', users=users_list)


@bp.route('/users/add', methods=['GET', 'POST'])
@admin_required
def add_user():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        is_admin = 'is_admin' in request.form

        user_id = User.create(username, password, is_admin)
        if user_id:
            flash('User added successfully')
            return redirect(url_for('admin.users'))
        else:
            flash('Username already exists')

    return render_template('admin/add_user.html')


@bp.route('/users/<user_id>/delete', methods=['POST'])
@admin_required
def delete_user(user_id):
    User.delete(user_id)
    flash('User deleted successfully')
    return redirect(url_for('admin.users'))