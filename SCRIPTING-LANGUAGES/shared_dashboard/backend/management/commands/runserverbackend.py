from django.core.management.base import BaseCommand
from backend import DashboardServer


class Command(BaseCommand):
    help = "Run the dashboard server"

    def add_arguments(self, parser):
        parser.add_argument(
            "--port", type=int, default=None, help="Port to listen on"
        )

    def handle(self, *args, **options):
        port = options.get("port")
        server = DashboardServer(port=port, db_path="/Users/ahmetyigitturhan/Documents/script_web_dashboard/db.sqlite3")

        try:
            server.start()
        except KeyboardInterrupt:
            self.stdout.write("\nShutting down server...")
            server.stop()
