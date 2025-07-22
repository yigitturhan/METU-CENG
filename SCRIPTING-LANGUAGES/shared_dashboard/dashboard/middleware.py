from django.shortcuts import redirect
from django.urls import reverse
from django.conf import settings


class UserSessionMiddleware:
    def __init__(self, get_response):
        self.get_response = get_response

    def __call__(self, request):
        # Login sayfasına erişime her zaman izin ver
        if request.path == reverse('login'):
            return self.get_response(request)

        # Session'da username yoksa login'e yönlendir
        if not request.session.get('username'):
            return redirect('login')

        return self.get_response(request)