from django import forms


class LoginForm(forms.Form):
    username = forms.CharField()


class DashboardForm(forms.Form):
    name = forms.CharField()
