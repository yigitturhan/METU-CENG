import os
import sys
import asyncio
import websockets
import json
from backend.server.persistence import DashboardPersistence
from backend.server.timerthread import TimerThread
import sqlite3
from threading import Lock
from backend.server.notificationmanager import NotificationManager

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from backend.core.repo import repo


class DashboardServer:
    """Enhanced dashboard server with WebSocket support"""

    def __init__(self, port, db_path="dashboards.db"):
        self.port = port
        self.persistence = DashboardPersistence(db_path)
        self.notification_manager = NotificationManager()
        self.timer_thread = TimerThread()
        self._running = False
        self.lock = asyncio.Lock()

    def _load_saved_dashboards(self):
        """Load all saved dashboards"""
        try:
            with sqlite3.connect(self.persistence.db_path) as conn:
                cursor = conn.execute("SELECT id FROM dashboards")
                for (dash_id,) in cursor:
                    try:
                        self.persistence.load_dashboard(dash_id)
                    except Exception as e:
                        print(f"Error loading dashboard {dash_id}: {e}")
        except Exception as e:
            print(f"Error loading saved dashboards: {e}")

    async def handle_client(self, websocket):
        """Handle WebSocket connection"""
        from backend.server.clienthandler import WebSocketClientHandler
        handler = WebSocketClientHandler(
            websocket,
            self.notification_manager,
            self.timer_thread,
            self.persistence,
            self.lock
        )
        try:
            await handler.run()
        except websockets.exceptions.ConnectionClosedOK:
            print("Connection closed peacefully")
        except websockets.exceptions.ConnectionClosedError:
            print("Client connection terminated with error")
        except Exception as e:
            print(f"Error handling client: {e}")

    async def start_server(self):
        """Start WebSocket server asynchronously"""
        self._running = True
        self._load_saved_dashboards()
        self.timer_thread.start()

        async with websockets.serve(self.handle_client, "0.0.0.0", self.port):
            print(f"WebSocket server listening on port {self.port}")
            # Keep the server running
            await asyncio.Future()  # run forever

    def start(self):
        """Start the server in the main thread"""
        try:
            asyncio.run(self.start_server())
        except KeyboardInterrupt:
            print("\nShutting down server...")
        finally:
            self.stop()

    def stop(self):
        """Stop server"""
        self._running = False
        self.timer_thread.stop()
        self._save_all_dashboards()

    def _save_all_dashboards(self):
        """Save all active dashboards"""
        try:
            for dash_id, dash in repo.get_objects().items():
                try:
                    self.persistence.save_dashboard(dash)
                except Exception as e:
                    print(f"Error saving dashboard {dash_id}: {e}")
        except Exception as e:
            print(f"Error saving dashboards: {e}")


def main():
    """Main entry point"""
    import argparse
    parser = argparse.ArgumentParser(description="Dashboard WebSocket Server")
    parser.add_argument("--port", type=int, default=1234,
                        help="Port to listen on")
    args = parser.parse_args()

    server = DashboardServer(args.port)
    server.start()


if __name__ == "__main__":
    main()