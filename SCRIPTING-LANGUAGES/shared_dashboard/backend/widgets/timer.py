# widgets/timer.py
from .base import BaseWidget
import time


class Timer(BaseWidget):
    def __init__(self):
        super().__init__("Timer", "Timer Widget")
        self.env = {"value": 0}
        self.events = ["refresh", "pause", "play", "reset", "start", "stop"]
        self._running = False
        self._start_time = None
        self._elapsed = 0

    def view(self, user=None):
        current = self._elapsed
        if self._running and self._start_time:
            current += time.time() - self._start_time
        seconds = int(current)
        minutes = seconds // 60
        remaining_seconds = seconds % 60

        svg = f'''<svg viewBox="0 0 100 100" class="w-32 h-32">
            <circle cx="50" cy="50" r="45" fill="none" stroke="#eee" stroke-width="6"/>
            <path d="M50 5 A45 45 0 1 1 49.99 5" 
                  fill="none" 
                  stroke="#3b82f6" 
                  stroke-width="6"
                  stroke-dasharray="282.743"
                  stroke-dashoffset="{282.743 * (1 - (seconds % 60) / 60)}"/>
            <text x="50" y="25" text-anchor="middle" font-size="20" fill="#333">
                {minutes:02d}:{remaining_seconds:02d}
            </text>
            <text x="50" y="57" text-anchor="middle" font-size="12" fill="#666">
                {self._running and "Running" or "Stopped"}
            </text>
        </svg>'''
        return svg

    @classmethod
    def desc(cls):
        return "A timer that supports start/stop/play/pause/reset/refresh."

    def trigger(self, event, params=None):  # simple logic in order to handle timer
        if event == "start":
            self._running = True
            self._start_time = time.time()
            return "timer started"
        elif event == "stop":
            if self._running and self._start_time:
                self._elapsed += time.time() - self._start_time
            self._running = False
        elif event == "play":
            self._running = True
            self._start_time = time.time()
        elif event == "pause":
            if self._running and self._start_time:
                self._elapsed += time.time() - self._start_time
            self._running = False
        elif event == "reset":
            self._running = False
            self._start_time = None
            self._elapsed = 0
        elif event == "refresh":
            pass
        else:
            raise ValueError(f"Unknown event: {event}")



