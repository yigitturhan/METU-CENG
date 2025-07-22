import json
import pickle
import asyncio
import websockets
from typing import Optional, List, Tuple, Any
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))
from backend.core.repo import repo


class WebSocketClientHandler:
    """Handles individual WebSocket client connections"""

    def __init__(self, websocket, notification_manager, timer_thread, persistence, lock):
        self.websocket = websocket
        self.notification_manager = notification_manager
        self.timer_thread = timer_thread
        self.persistence = persistence
        self._user = None
        self._components: List[Tuple[int, Any]] = []
        self._attached_dash = None
        self._param_store = {}
        self._running = True
        self.lock = lock

    async def handle_notification(self, component, message):
        try:
            notification = {
                "type": "notification",
                "user": str(self._user),
                "dashboard_id": str(self._attached_dash.get_id()),
                "component": component.name,
                "message": message
            }
            await self.websocket.send(json.dumps(notification))
        except Exception as e:
            print(f"Error sending notification: {e}")

    async def handle_command(self, command_data: str) -> str:
        """Process a command from the client"""
        try:
            command = json.loads(command_data)
            cmd, args = command['method'], command['data']
            print(cmd, args)

            if not self._user and cmd != "USER":
                return json.dumps({"error": "Please set username first with USER command"})

            if cmd == "USER":
                self._user = args['username']
                return json.dumps({"status": "success", "message": f"Username set to {self._user}"})

            elif cmd == "list":
                async with self.lock:
                    res = repo.list()
                return json.dumps({"status": "success", "data": res})

            elif cmd == "attach":
                if self._attached_dash:
                    return json.dumps({"status": "success", "data": pickle.dumps(self._attached_dash).decode('latin1')})
                try:
                    async with self.lock:
                        self._attached_dash = repo.attach(args['id'], self._user)
                        await self.append_comps()
                        await self.notification_manager.register(args['id'], self._user, self.handle_notification)
                    return json.dumps({"status": "success", "data": pickle.dumps(self._attached_dash).decode('latin1')})
                except ValueError as e:
                    return json.dumps({"status": "error", "message": str(e)})

            elif cmd == "detach":
                try:
                    if not self._attached_dash:
                        return json.dumps({"status": "error", "message": "Not attached to any dashboard"})
                    dash_id = self._attached_dash.get_id()
                    async with self.lock:
                        repo.detach(dash_id, self._user)
                        for _, comp in self._components:
                            self.timer_thread.remove_timer(comp)
                        await self.notification_manager.unregister(dash_id, self._user)
                        self._attached_dash = None
                    return json.dumps({"status": "success", "message": f"Detached from dashboard {dash_id}"})
                except Exception as e:
                    print(f"Exception at detach: {str(e)}")
                    self.lock.release()
                    return json.dumps({"status": "error", "message": str(e)})

            elif cmd == "create":
                try:
                    async with self.lock:
                        dash_id = repo.create(**args)
                    return json.dumps({"status": "success", "message": f"Created dashboard {dash_id}"})
                except Exception as e:
                    return json.dumps({"status": "error", "message": f"Error creating dashboard: {str(e)}"})

            elif cmd == "dash":
                try:
                    if args['action'] == 'create tab':
                        async with self.lock:
                            tab = self._attached_dash.create(args['name'])

                        return json.dumps({"status": "success", "data": tab.serialize()})
                    if args['action'] == "list":
                        async with self.lock:
                            res = repo.list()
                        return json.dumps({"status": "success", "data": {"dashboards": res}})
                except Exception as e:
                    print(str(e))
                    return json.dumps({"status": "error", "message": f"Error handling dash command: {str(e)}"})

            elif cmd == "tab":
                try:
                    if args['action'] == "place":
                        row, col, component_id = int(args['row']), int(args['col']), int(args['comp_id'])
                        async with self.lock:
                            comp = await self.find_component(component_id)
                            self._attached_dash[args['tab_name']].place(comp, row, col)
                        return json.dumps({
                            "status": "success",
                            "message": f"Component {component_id} placed on {row}, {col} on {args['tab_name']}"
                        })
                except Exception as e:
                    return json.dumps({"status": "error", "message": f"Error placing component: {str(e)}"})

            elif cmd == "save":
                if not self._attached_dash:
                    return json.dumps({"status": "error", "message": "Not attached to any dashboard"})
                try:
                    async with self.lock:
                        await self.save_dashboard(self._attached_dash)
                    return json.dumps({"status": "success", "message": "Dashboard saved successfully"})
                except Exception as e:
                    return json.dumps({"status": "error", "message": f"Error saving dashboard: {str(e)}"})

            elif cmd == "component":
                try:
                    action = args['action']
                    if action == "create":
                        async with self.lock:
                            component = repo.components.create(args['type'])
                            for key, val in args['env'].items():
                                component.env[key] = val
                            self._components.append((component.id, component))
                            if component.name != "URLGetter" or component.name != "MessageRotate":
                                self.timer_thread.add_timer(component)
                            try:
                                notify_message = str(self._user) + " created " + component.name +" !"
                                await self.notification_manager.notify(self._attached_dash.get_id(), component, notify_message)
                            except:
                                pass
                        return json.dumps({"status": "success", "data": {"id": component.id}})
                    elif action == "register":
                        async with self.lock:
                            repo.components.register(args['type'], eval(args['type']))
                        return json.dumps({
                            "status": "success",
                            "message": f"Component {args['type']} registered successfully"
                        })
                    elif action == "trigger":
                        comp_id, event, params = int(args['id']), args['event'], args['params']
                        async with self.lock:
                            component = await self.find_component(comp_id)
                            result = component.trigger(event, params)
                            notify_message = str(self._user) + " made changes on " + component.name + " !"
                            await self.notification_manager.notify(self._attached_dash.get_id(), component, notify_message)
                        return json.dumps({"status": "success", "data": {"result": result}})
                    elif action == "list":
                        async with self.lock:
                            res = repo.components.list()
                        return json.dumps({"status": "success", "data": {"components": res}})

                except Exception as e:
                    print(str(e))
                    return json.dumps({"status": "error", "message": f"Error handling component command: {str(e)}"})

            return json.dumps({"status": "error", "message": f"Unknown command: {cmd}"})

        except Exception as e:
            return json.dumps({"status": "error", "message": f"Error processing command: {str(e)}"})

    async def save_dashboard(self, dash):
        """Async wrapper for dashboard saving"""
        try:
            await asyncio.to_thread(self.persistence.save_dashboard, dash)
        except Exception as e:
            print(f"Error saving dashboard {dash.get_id()}: {e}")
            raise

    async def find_component(self, comp_id: int):
        """Find component by ID"""
        for c_id, comp in self._components:
            if c_id == comp_id:
                return comp
        return None

    async def append_comps(self):
        """Append components from attached dashboard"""
        if len(self._components) != 0:
            return
        for tab in self._attached_dash.get_tabs().values():
            for rows in tab.get_rows():
                for cmp in rows:
                    if not cmp:
                        continue
                    self._components.append((cmp.id, cmp))
                    self.timer_thread.add_timer(cmp)

    async def run(self):
        """Main WebSocket handler loop"""
        try:
            while self._running:
                try:
                    message = await self.websocket.recv()
                    if not message:
                        continue
                    response = await self.handle_command(message)
                    try:
                        await self.websocket.send(response)
                    except websockets.exceptions.ConnectionClosed:
                        break

                except websockets.exceptions.ConnectionClosedOK:
                    print("Client disconnected normally")
                    break
                except websockets.exceptions.ConnectionClosedError as e:
                    print(f"Connection closed with error: {e}")
                    break
                except Exception as e:
                    print(f"Error handling client message: {e}")
                    try:
                        await self.websocket.send(json.dumps({
                            "status": "error",
                            "message": str(e)
                        }))
                    except Exception as e:
                        print("Exception at run/clienthandler.py : ", str(e))
                        break

        finally:
            if self._attached_dash:
                try:
                    async with self.lock:
                        repo.detach(self._attached_dash.get_id(), self._user)
                        await self.notification_manager.unregister(self._attached_dash.get_id(), self._user)
                        for _, comp in self._components:
                            self.timer_thread.remove_timer(comp)
                except Exception as e:
                    print(f"Error during cleanup: {e}")