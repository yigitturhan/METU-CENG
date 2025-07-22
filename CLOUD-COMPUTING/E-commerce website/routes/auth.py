# routes/auth.py
from flask import Blueprint, request, render_template, redirect, url_for, session, flash
from models.user import User

bp = Blueprint('auth', __name__, url_prefix='/auth')


@bp.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        if User.verify_password(username, password):
            user = User.get_by_username(username)
            session.clear()
            session['user_id'] = str(user['_id'])
            session['username'] = user['username']
            session['is_admin'] = user.get('is_admin', False)

            return redirect(url_for('home'))

        flash('Invalid username or password')

    return render_template('auth/login.html')


@bp.route('/logout')
def logout():
    session.clear()
    return redirect(url_for('home'))