from django.urls import path
from . import views

urlpatterns = [
    path('', views.dashboard_list, name='dashboard_list'),
    path('create/', views.create_dashboard, name='create_dashboard'),
    path('attach/<str:dash_id>/', views.attach_dashboard, name='attach_dashboard'),
    path('dashboard/<str:dash_id>/create-tab/', views.create_tab, name='create_tab'),
    path('dashboard/<str:dash_id>/component/<int:component_id>/trigger/',
         views.trigger_component, name='trigger_component'),
    path('dashboard/<str:dash_id>/detach/', views.detach_dashboard, name='detach_dashboard'),
    path('dashboard/<str:dash_id>/tab/<str:tab_name>/component/create/',
         views.create_component, name='create_component'),
    path('dashboard/<int:dash_id>/data/', views.get_dashboard_data, name='get_dashboard_data'),
    path('logout/', views.logout_view, name='logout'),
]
