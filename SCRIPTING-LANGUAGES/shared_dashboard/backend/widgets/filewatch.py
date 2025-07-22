# widgets/filewatch.py
import os
from .base import BaseWidget

path_of_files = "/Users/ahmetyigitturhan/Documents/script_web_dashboard/backend/widgets/shared/"
class FileWatch(BaseWidget):
    def __init__(self):
        super().__init__("FileWatch", "File Watcher")
        self.env = {
            "filepath": "shared/",
            "filename": "",
            "numberoflines": 10
        }
        self._last_size = 0
        self._lines = []

    @classmethod
    def desc(cls):
        return "Watches the interface of the file"

    def view(self, user=None):
        result = ""
        with open(path_of_files + self.env["filename"], "r") as file:
            line = file.readline()
            while line:
                result += line + "\n"
                line = file.readline()

        return result

    def refresh(self):  # it does what is should do again in order to create new data
        if not self.env["filename"]:
            return
        try:
            current_size = os.path.getsize(self.env["filename"])
            if current_size > self._last_size:
                with open(self.env["filename"], 'r') as f:
                    content = f.readlines()
                    self._lines = content[-self.env["numberoflines"]:]
                self._last_size = current_size
        except:
            self._lines = []
