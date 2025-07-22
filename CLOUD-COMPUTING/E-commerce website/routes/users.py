# routes/users.py
from flask import Blueprint, render_template, session, redirect, url_for, flash
from models.user import User

bp = Blueprint('users', __name__, url_prefix='/users')


@bp.route('/profile')
def profile():
    if 'user_id' not in session:
        flash('You must be logged in to view your profile')
        return redirect(url_for('auth.login'))

    user = User.get_by_id(session['user_id'])

    if 'reviews' not in user:
        user['reviews'] = []

    return render_template('users/profile.html', user=user)