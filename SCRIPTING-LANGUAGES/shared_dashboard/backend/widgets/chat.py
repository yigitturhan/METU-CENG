from .base import BaseWidget
import time


class Chat(BaseWidget):
    def __init__(self):
        super().__init__("Chat", "Chat Widget")
        self.events = ["refresh", "submit"]
        self.messages = []
        self.param = {"mess": "", "username": ""}  # FIXME: değişiklik yaptım şu satırdan itibaren.

    @classmethod
    def desc(cls):
        return "Simple chat program"

    def view(self, user=None):
        if not self.messages:
            return '<div class="text-gray-500 text-center">No messages yet</div>'

        current_user, svg_messages, y = user, [], 10
        for msg in self.messages:
            username, content = msg.split(": ", 1)
            username = username.split("|")[1]
            is_current_user = username == current_user
            text_anchor, x, timestamp = "start", 10, msg.split("|")[0]
            bubble_fill = "#c5e1ff" if is_current_user else "#e5e7eb"
            svg_messages.append(f"""
                <g transform="translate(0,{y})">
                    <rect x="{x}" y="0" width="200" height="50" rx="10" ry="10" fill="{bubble_fill}"/>
                    <text x="{x + 10}" y="20" font-size="12" fill="#6b7280">{username}</text>
                    <text x="{x + 10}" y="40" font-size="14">{content}</text>
                    <text x="{210}" y="48" text-anchor="{text_anchor}" font-size="10" fill="#6b7280">{timestamp}</text>
                </g>
            """)
            y += 60

        return f"""
                <svg viewBox="0 0 600 {y + 10}" width="600" height="{y + 10}">
                    <rect x="0" y="0" width="100%" height="100%" fill="white"/>
                    {''.join(svg_messages)}
                </svg>
            """

    def trigger(self, event, param):
        super().trigger(event, param)
        if event == "submit" and param["mess"]:
            self.messages.append(f"{time.strftime('%H:%M')}|{param['username']}: {param['mess']}")
            param["mess"] = ""
