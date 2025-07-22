from .base import BaseWidget


class MessageRotate(BaseWidget):
    def __init__(self):
        super().__init__("MessageRotate", "Message Rotator", 50, 100)
        self.env = {"messages": []}
        self.events = ["refresh"]
        self.params = {}
        self.refresh_interval = 3
        self._current_index = 0

    @classmethod
    def desc(cls):
        return "Rotates through a list of messages"

    def type(self):
        return "MessageRotate"

    def attrs(self):
        return [
            ("messages", "list"),
            ("current_index", "int")
        ]

    def view(self, user=None):
        if not self.env["messages"]:
            return """
            <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 400 100" width="400" height="100">
                <rect x="0" y="0" width="400" height="100" fill="url(#gradient)" rx="15" />
                <text x="50%" y="50%" dominant-baseline="middle" text-anchor="middle" font-size="20" fill="#ccc" font-family="Arial, sans-serif">
                    No messages
                </text>
            </svg>
            """
        print(self.env["messages"])
        self._current_index = self._current_index % len(self.env["messages"])
        current_message = self.env["messages"][self._current_index]
        current_message = current_message.strip()

        return f"""
        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 400 100" width="400" height="100">
            <defs>
                <linearGradient id="gradient" x1="0%" y1="0%" x2="100%" y2="100%">
                    <stop offset="0%" style="stop-color:#56CCF2;stop-opacity:1" />
                    <stop offset="100%" style="stop-color:#2F80ED;stop-opacity:1" />
                </linearGradient>
            </defs>
            <style>
                @keyframes fadeIn {{
                    0% {{ opacity: 0; transform: scale(0.9); }}
                    50% {{ opacity: 1; transform: scale(1); }}
                    100% {{ opacity: 1; }}
                }}
                .message-box {{
                    animation: fadeIn 1s ease-out;
                }}
            </style>
            <!-- Background -->
            <rect x="0" y="0" width="400" height="100" fill="url(#gradient)" rx="15" />
            <!-- Message Text -->
            <text x="50%" y="50%" dominant-baseline="middle" text-anchor="middle" font-size="20" fill="#ffffff" font-family="Arial, sans-serif" class="message-box">
                {current_message}
            </text>
        </svg>
        """

    def refresh(self):
        print("refreşlendi")
        return
        if self.env["messages"]:
            self._current_index = (self._current_index + 1) % len(self.env["messages"])
