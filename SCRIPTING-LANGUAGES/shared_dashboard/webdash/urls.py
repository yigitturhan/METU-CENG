from django.urls import path, include

from dashboard import views

urlpatterns = [
    path('', include('dashboard.urls')),
    path('login/', views.login_view, name='login'),
]