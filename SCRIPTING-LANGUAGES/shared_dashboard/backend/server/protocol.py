import json
import websockets
from websockets.exceptions import ConnectionClosedError, ConnectionClosedOK


class ProtocolError(Exception):
    pass


class Protocol:
    """Protocol handler for dashboard server communication"""

    def __init__(self, wsock, timeout=60):
        self.wsock = wsock
        self.timeout = timeout

    async def send_message(self, message):
        """Send a message with JSON"""
        if isinstance(message, (dict, list)):
            message = json.dumps(message)

        try:
            await self.wsock.send(message)
        except websockets.exceptions.ConnectionClosedError as e:
            raise ProtocolError(f"Send failed: {e}")

    async def receive_message(self):
        """Receive a message with JSON"""
        try:
            message = await self.wsock.recv()
            return json.loads(message)
        except (websockets.exceptions.ConnectionClosedError, json.JSONDecodeError) as e:
            raise ProtocolError(f"Receive failed: {e}")
