import os
from backend.widgets.base import BaseWidget

path_of_files = "/Users/ahmetyigitturhan/Documents/script_web_dashboard/backend/widgets/shared"


class FileShare(BaseWidget):
    """Component for sharing files in a directory"""

    def __init__(self):
        super().__init__("FileShare", "File Sharing Widget")
        self.env = {"path": ""}
        self.param = {"filename": "", "content": ""}
        self.events = ["refresh", "upload", "download", "delete"]
        self._file_list = []
        self.refresh_interval = 0

    @classmethod
    def desc(cls):
        return "Manages shared files in a directory"

    def _get_specific_attrs(self):
        return [("path", "string")]

    @classmethod
    def _ensure_dir(cls):
        """Ensure shared directory exists"""
        if not os.path.exists(path_of_files):
            os.makedirs(path_of_files)

    def _get_file_info(self):
        global path_of_files
        """Get information about files in shared directory"""
        self._ensure_dir()
        self._file_list = []
        for filename in os.listdir(path_of_files):
            path = os.path.join(path_of_files, filename)
            if os.path.isfile(path):
                stats = os.stat(path)
                self._file_list.append({
                    "name": filename,
                    "size": stats.st_size,
                    "modified": stats.st_mtime
                })
        return self._file_list

    def view(self, user=None):
        """Display file listing"""
        res = ""
        for f in self._get_file_info():
            res += f["name"] + " "
        return res

    def trigger(self, event, params=None):
        """Handle file operations"""
        if not params:
            return "Params is not set"
        self._ensure_dir()
        filename = params.get("filename", "")
        if not filename:
            return "File name is not set"
        filepath = os.path.join(path_of_files, filename)
        try:
            if event == "upload":
                content = params.get("content", "")
                with open(filepath, "w") as f:
                    f.write(content)
                self.refresh()
                return "successfully uploaded"

            elif event == "download":
                if os.path.exists(filepath):
                    self.param["filename"] = filename
                    with open(filepath, "r") as f:
                        self.param["content"] = f.read()
                else:
                    params["content"] = ""
                self.refresh()
                return "successfully downloaded"

            elif event == "delete":
                if os.path.exists(filepath):
                    os.remove(filepath)
                self.refresh()
                return "successfully deleted"

        except Exception as e:
            return f"Error in FileShare {event}: {str(e)}"

    def refresh(self):
        print(f"refresh fileshare: {self._get_file_info()}")
