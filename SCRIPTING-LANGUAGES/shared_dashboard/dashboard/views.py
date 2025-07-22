import pickle
import json
from django.shortcuts import render, redirect
from django.contrib import messages
from .forms import LoginForm
from .tcp_client import DashboardClient
from django.http import JsonResponse
import asyncio
from asgiref.sync import sync_to_async
from django.contrib.sessions.backends.db import SessionStore
from functools import wraps
from django.contrib.sessions.middleware import SessionMiddleware


def async_view(view_func):
    @wraps(view_func)
    async def wrapper(request, *args, **kwargs):
        if not hasattr(request, 'session'):
            session_middleware = SessionMiddleware(get_response=request)
            session_middleware.process_request(request)

        request.session = await sync_to_async(SessionStore)(request.session.session_key)

        try:
            response = await view_func(request, *args, **kwargs)
            return response
        except Exception as e:
            print(f"Error in {view_func.__name__}: {str(e)}")
            return redirect('dashboard_list')

    return wrapper


notifications = {}
download = False
current_indexes = {}

@async_view
async def create_dashboard(request):
    if request.method == 'POST':
        name = request.POST.get('name')
        username = await sync_to_async(request.session.get)('username', 'default_user')
        client = DashboardClient(username)
        try:
            await client.send_command('USER', {'username': username})
            await client.send_command('create', {'name': name})
            return redirect('dashboard_list')
        finally:
            await client.disconnect()
    return redirect('dashboard_list')


@async_view
async def dashboard_list(request):
    username = await sync_to_async(request.session.get)('username', 'default_user')
    client = DashboardClient(username)
    try:
        await client.send_command('USER', {'username': username})
        dashboards_response = await client.send_command('dash', {'action': 'list'})

        if isinstance(dashboards_response, dict):
            dashboards = dashboards_response.get('data', {}).get('dashboards', [])
        else:
            try:
                dashboards_data = json.loads(dashboards_response)
                dashboards = dashboards_data.get('data', {}).get('dashboards', [])
            except json.JSONDecodeError:
                dashboards = []

        return await sync_to_async(render)(request, 'dashboard/list.html', {'dashboards': dashboards})
    finally:
        await client.disconnect()


@async_view
async def get_dashboard_data(request, dash_id):
    username = await sync_to_async(request.session.get)('username', 'default_user')
    client = DashboardClient(username)
    try:
        await client.send_command('USER', {'username': username})
        dashboard_data = await client.send_command('attach', {'id': dash_id})

        json_response = dashboard_data if isinstance(dashboard_data, dict) else json.loads(dashboard_data)
        dashboard_data = pickle.loads(json_response.get('data').encode('latin1'))

        tabs = []
        for tab_name, tab in dashboard_data.get_tabs().items():
            components = []
            for cols in tab.get_rows():
                tm = []
                for component in cols:
                    if not component:
                        continue
                    elif component.name == "FileShare":
                        tm.append({
                            'title': component.name,
                            'views': component.view(),
                            'id': component.id,
                            'files': component._get_file_info()
                        })
                        global download
                        if download:
                            await sync_to_async(save_file)(
                                component.env["path"] + "/" + component.param["filename"],
                                component.param["content"]
                            )
                            download = False
                    elif component.name == "DBUpdate":
                        tm.append({
                            'title': component.name,
                            'views': component.view(username),
                            'id': component.id,
                            'qry': component.env["query"]
                        })
                    elif component.name == "MessageRotate":
                        if component.id not in current_indexes.keys():
                            current_indexes[component.id] = 0
                        else:
                            component._current_index = current_indexes[component.id]
                            current_indexes[component.id] += 1
                        tm.append({
                            'title': component.name,
                            'views': component.view(username),
                            'id': component.id
                        })
                    else:
                        tm.append({
                            'title': component.name,
                            'views': component.view(username),
                            'id': component.id
                        })
                components.append(tm)
            tabs.append({'name': tab_name, 'components': components})
        global notifications
        if username not in notifications.keys():
            notifications[username] = []
        if len(notifications[username]) != 0:
            notif = notifications[username][0]
            notifications[username] = notifications[username][1:]
            return JsonResponse({
                'dashboard_id': dash_id,
                'dashboard_data': {
                    'name': dashboard_data.name
                },
                'tabs': tabs,
                'notifications': notif
            })
        return JsonResponse({
            'dashboard_id': dash_id,
            'dashboard_data': {
                'name': dashboard_data.name
            },
            'tabs': tabs
        })
    finally:
        await client.disconnect()


@async_view
async def attach_dashboard(request, dash_id, mess=None):
    global notifications
    if not hasattr(request, 'session'):
        session_middleware = SessionMiddleware(get_response=request)
        session_middleware.process_request(request)

    username = await sync_to_async(request.session.get)('username', 'default_user')

    client = DashboardClient(username)
    try:
        await client.send_command('USER', {'username': username})
        dashboard_data = await client.send_command('attach', {'id': dash_id})

        json_response = dashboard_data if isinstance(dashboard_data, dict) else json.loads(dashboard_data)
        if json_response.get('status') == 'error':
            return redirect('dashboard_list')

        try:
            dashboard_data = pickle.loads(json_response.get('data').encode('latin1'))
        except Exception as e:
            print("Error at attach_dashboard: ", str(e))
            return redirect('dashboard_list')

        tabs = []
        for tab_name, tab in dashboard_data.get_tabs().items():
            components = []
            for cols in tab.get_rows():
                tm = []
                for component in cols:
                    if not component:
                        continue
                    elif component.name == "FileShare":
                        tm.append({
                            'title': component.name,
                            'views': component.view(username),
                            'id': component.id,
                            'files': component._get_file_info()
                        })
                        global download
                        if download:
                            await sync_to_async(save_file)(
                                component.env["path"] + "/" + component.param["filename"],
                                component.param["content"]
                            )
                            download = False
                    elif component.name == "DBUpdate":
                        tm.append({
                            'title': component.name,
                            'views': component.view(username),
                            'id': component.id,
                            'qry': component.env["query"]
                        })
                    else:
                        tm.append({
                            'title': component.name,
                            'views': component.view(username),
                            'id': component.id
                        })
                components.append(tm)
            tabs.append({'name': tab_name, 'components': components})

        components_response = await client.send_command('component', {'action': 'list'})
        json_response = components_response if isinstance(components_response, dict) else json.loads(
            components_response)
        components = json_response.get('data', {}).get('components', [])

        context = {
            'dashboard_id': dash_id,
            'dashboard_data': dashboard_data,
            'tabs': tabs,
            'components': components,
        }
        if username not in notifications.keys():
            notifications[username] = []
        if len(notifications[username]) != 0:
            context['notification'] = notifications[username][0]
            notifications[username] = notifications[username][1:]
        return await sync_to_async(render)(request, 'dashboard/view.html', context)
    finally:
        await client.disconnect()


@async_view
async def create_tab(request, dash_id):
    if request.method == 'POST':
        tab_name = request.POST.get('tab_name')
        username = await sync_to_async(request.session.get)('username', 'default_user')
        client = DashboardClient(username)
        try:
            await client.send_command('USER', {'username': username})
            await client.send_command('attach', {'id': dash_id})
            await client.send_command('dash', {'action': 'create tab', 'name': tab_name})
            return redirect('attach_dashboard', dash_id=dash_id)
        finally:
            await client.disconnect()
    return redirect('attach_dashboard', dash_id=dash_id)


@async_view
async def trigger_component(request, dash_id, component_id):
    global download
    if request.method == 'POST':
        username = await sync_to_async(request.session.get)('username', 'default_user')
        client = DashboardClient(username)
        try:
            await client.send_command('USER', {'username': username})
            await client.send_command('attach', {'id': dash_id})

            params = {k: v for k, v in request.POST.items() if k != 'csrfmiddlewaretoken'}
            await client.send_command('component', {
                'action': 'trigger',
                'id': component_id,
                'event': params.get('event'),
                'params': params
            })

            if params.get('event') == "download":
                download = True
            return redirect('attach_dashboard', dash_id=dash_id)
        finally:
            await client.disconnect()


def save_file(path, content):
    with open(path, 'w') as file:
        file.write(content)


@async_view
async def detach_dashboard(request, dash_id):
    if request.method == 'POST':
        username = await sync_to_async(request.session.get)('username', 'default_user')
        client = DashboardClient(username)
        try:
            await client.send_command('detach', {'id': dash_id})
            return redirect('dashboard_list')
        finally:
            await client.disconnect()


@async_view
async def create_component(request, dash_id, tab_name):
    username = await sync_to_async(request.session.get)('username', 'default_user')
    client = DashboardClient(username)

    if request.method == 'POST':
        try:
            await client.send_command('USER', {'username': username})
            await client.send_command('attach', {'id': dash_id})

            component_type = request.POST.get('type')
            env_vars = {}

            for key, value in request.POST.items():
                if key.startswith('env_vars[') and key.endswith('[key]'):
                    index = key.split('[')[1].split(']')[0]
                    if value == "messages":
                        message_value = request.POST.get(f'env_vars[{index}][value]')
                        if message_value:
                            try:
                                message_value = message_value.split(",")
                                for m in message_value:
                                    env_vars["messages"].append(m)
                            except KeyError:
                                env_vars["messages"] = message_value
                    elif value == "filename":
                        path_value = request.POST.get(f'env_vars[{index}][value]')
                        env_vars["filename"] = path_value
                    elif value == "url":
                        val = request.POST.get(f'env_vars[{index}][value]')
                        env_vars["url"] = val if val else ""
                    elif value == "path":
                        path_value = request.POST.get(f'env_vars[{index}][value]')
                        env_vars["path"] = path_value if path_value else ""
                    elif value == "query":
                        query_value = request.POST.get(f'env_vars[{index}][value]')
                        env_vars["query"] = query_value if query_value else ""

            response = await client.send_command('component', {
                'action': 'create',
                'type': component_type,
                'env': env_vars
            })

            json_response = response if isinstance(response, dict) else json.loads(response)
            comp_id = json_response.get('data', {}).get('id')

            if not comp_id:
                return redirect('attach_dashboard', dash_id=dash_id)

            row = int(request.POST.get('row', 0))
            col = int(request.POST.get('col', -1))

            await client.send_command('tab', {
                'action': 'place',
                'tab_name': tab_name,
                'row': row,
                'col': col,
                'comp_id': comp_id
            })

            return redirect('attach_dashboard', dash_id=dash_id)
        finally:
            await client.disconnect()

    try:
        await client.send_command('USER', {'username': username})
        await client.send_command('attach', {'id': dash_id})
        response = await client.send_command('component', {'action': 'list'})

        json_response = response if isinstance(response, dict) else json.loads(response)
        components = json_response.get('data', {}).get('components', [])

        return await sync_to_async(render)(request, 'dashboard/create_component.html', {
            'dash_id': dash_id,
            'tab_name': tab_name,
            'components': components
        })
    finally:
        await client.disconnect()


@async_view
async def login_view(request):
    if request.method == 'POST':
        form = LoginForm(request.POST)
        if await sync_to_async(form.is_valid)():
            await sync_to_async(request.session.__setitem__)('username', form.cleaned_data['username'])
            return redirect('dashboard_list')
    else:
        form = LoginForm()
    return await sync_to_async(render)(request, 'dashboard/login.html', {'form': form})


@async_view
async def logout_view(request):
    await sync_to_async(request.session.flush)()
    return redirect('login')
