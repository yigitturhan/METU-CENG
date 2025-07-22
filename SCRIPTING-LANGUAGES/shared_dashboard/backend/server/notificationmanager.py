import asyncio


class NotificationManager:
    """Manages notifications for all clients"""

    def __init__(self):
        self._observers = {}
        self._lock = asyncio.Lock()

    async def register(self, dash_id, user, callback):
        async with self._lock:
            if dash_id not in self._observers:
                self._observers[dash_id] = {}
            self._observers[dash_id][user] = callback

    async def unregister(self, dash_id, user):
        async with self._lock:
            if dash_id in self._observers and user in self._observers[dash_id]:
                del self._observers[dash_id][user]
                if not self._observers[dash_id]:
                    del self._observers[dash_id]

    async def notify(self, dash_id, component, message):
        async with self._lock:
            if dash_id in self._observers.keys():
                for callback in self._observers[dash_id].values():
                    try:
                        await callback(component, message)
                    except Exception as e:
                        print(f"Error in notification callback: {e}")
