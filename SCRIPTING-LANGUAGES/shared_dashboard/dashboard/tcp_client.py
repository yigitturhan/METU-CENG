import websockets
import json
import asyncio
from websockets.exceptions import WebSocketException
from typing import Callable, Optional
from django.http import HttpRequest
from django.test import RequestFactory

class DashboardClient:
    def __init__(self, user, host='localhost', port=1234):
        self.host = host
        self.port = port
        self.wsock = None
        self.user = user
        self._connected = False
        self._lock = asyncio.Lock()
        self._notification_handler: Optional[Callable] = None
        self._listener_task = None
        self._command_response_queue = asyncio.Queue()

    async def ensure_connection(self):
        async with self._lock:
            if not self.wsock or not self._connected:
                try:
                    self.wsock = await websockets.connect(
                        f"ws://{self.host}:{self.port}",
                        ping_interval=None,
                        close_timeout=1
                    )
                    self._connected = True
                    # Start the notification listener when connection is established
                    if not self._listener_task:
                        self._listener_task = asyncio.create_task(self._listen_for_messages())
                except Exception as e:
                    self._connected = False
                    self.wsock = None
                    raise ConnectionError(f"Failed to connect: {str(e)}")

    async def send_command(self, command, args=None):
        try:
            await self.ensure_connection()
            cmd = {
                'method': command,
                'data': args or {}
            }

            await self.wsock.send(json.dumps(cmd))
            # Wait for the response from the queue instead of directly from websocket
            response = await self._command_response_queue.get()

            try:
                return json.loads(response) if isinstance(response, str) else response
            except json.JSONDecodeError:
                return response

        except WebSocketException as e:
            self._connected = False
            self.wsock = None
            return {
                'status': 'error',
                'message': f"WebSocket error: {str(e)}"
            }
        except Exception as e:
            return {
                'status': 'error',
                'message': str(e)
            }

    def set_notification_handler(self, handler: Callable):
        """Set the callback function to handle notifications"""
        self._notification_handler = handler

    async def _listen_for_messages(self):
        """Background task to listen for all messages and route them appropriately"""
        while self._connected:
            try:
                if self.wsock:
                    message = await self.wsock.recv()
                    try:
                        data = json.loads(message)
                        # If it's a notification, send it to the notification handler
                        if isinstance(data, dict) and data.get('type') == 'notification':
                            from .views import notifications
                            global notifications
                            for key, val in notifications.items():
                                if data.get("user") != key:
                                    notifications[key].append(data.get("message"))

                            print("çalıştı")
                            if self._notification_handler:
                                await self._notification_handler(data)
                        else:
                            # If it's not a notification, it's a command response
                            await self._command_response_queue.put(message)
                    except json.JSONDecodeError:
                        # If JSON parsing fails, treat it as a command response
                        await self._command_response_queue.put(message)
            except WebSocketException:
                self._connected = False
                self.wsock = None
                break
            except Exception as e:
                print(f"Error in message listener: {str(e)}")

    async def disconnect(self):
        if self.wsock and self._connected:
            try:
                if self._listener_task:
                    self._listener_task.cancel()
                    try:
                        await self._listener_task
                    except asyncio.CancelledError:
                        pass
                    self._listener_task = None
                await self.wsock.close()
            except Exception:
                pass
            finally:
                self._connected = False
                self.wsock = None