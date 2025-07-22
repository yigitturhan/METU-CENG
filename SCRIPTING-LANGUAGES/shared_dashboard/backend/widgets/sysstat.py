import time

import psutil
from .base import BaseWidget


class SysStat(BaseWidget):
    def __init__(self):
        super().__init__("SysStat", "System Statistics")
        self._stats = {
            "cpu_usage": 0,
            "mem_usage": 0,
            "load_avg": []
        }

    @classmethod
    def desc(cls):
        return "A widget to check the health of the system"

    def view(self, user=None):
        cpu_usage = self._stats["cpu_usage"]
        mem_usage = self._stats["mem_usage"]

        svg_template = f"""
        <svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 220 120' width='300'>
            <style>
                .label {{ font: bold 12px sans-serif; fill: #444; }}
                .value {{ font: bold 14px sans-serif; fill: #555; }}
                .bar-bg {{ fill: #f1f1f1; stroke: #ddd; stroke-width: 0.5; rx: 5; ry: 5; }}
                .bar {{ fill: url(#gradient); rx: 5; ry: 5; }}
            </style>
            <defs>
                <linearGradient id="gradient" x1="0%" y1="0%" x2="100%" y2="0%">
                    <stop offset="0%" style="stop-color:#4CAF50;stop-opacity:1" />
                    <stop offset="100%" style="stop-color:#2196F3;stop-opacity:1" />
                </linearGradient>
            </defs>

            <!-- CPU Usage -->
            <rect x='10' y='20' width='200' height='15' class='bar-bg' />
            <rect x='10' y='20' width='{cpu_usage * 2}' height='15' class='bar' />
            <text x='10' y='15' class='label'>CPU Usage</text>
            <text x='210' y='15' class='value' text-anchor='end'>{cpu_usage:.1f}%</text>

            <!-- Memory Usage -->
            <rect x='10' y='60' width='200' height='15' class='bar-bg' />
            <rect x='10' y='60' width='{mem_usage * 2}' height='15' class='bar' />
            <text x='10' y='55' class='label'>Memory Usage</text>
            <text x='210' y='55' class='value' text-anchor='end'>{mem_usage:.1f}%</text>
        </svg>
        """

        return svg_template

    def refresh(self):
        self._stats["cpu_usage"] = psutil.cpu_percent()
        self._stats["mem_usage"] = psutil.virtual_memory().percent
        self._stats["load_avg"] = [x / psutil.cpu_count() * 100 for x in psutil.getloadavg()]
