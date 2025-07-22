from .base import BaseWidget
import urllib.request
import json
import ssl


class URLGetter(BaseWidget):

    def __init__(self):
        super().__init__("URLGetter", "URL Content Viewer", 100, 200)
        self.env = {
            "url": ""  # URL to fetch content from
        }
        self.param = {}  # No user parameters needed
        self.events = ["refresh"]
        self.refresh_interval = 0  # No auto refresh in Phase 1
        self._content = "No content fetched yet"

    @classmethod
    def desc(cls):
        return "Fetches and displays content from URL"

    def type(self):
        return "URLGetter"

    def attrs(self):
        return [
            ("url", "string"),
            ("content", "string")
        ]

    def view(self, user=None):
        self.refresh()
        if not self.env["url"]:
            return "[URLGetter: No URL set]"
        return f"{self._content}"  # Show first 50 chars

    def refresh(self):
        if not self.env["url"]:
            self._content = "No URL set"
            return

        try:
            context = ssl.create_default_context()  # creates a ssl context
            context.check_hostname = False  # currently not checking hostname (may be changed later)
            context.verify_mode = ssl.CERT_NONE  # no certificate is required

            request = urllib.request.Request(
                self.env["url"],
                headers={'User-Agent': 'Mozilla/5.0'}
            )

            with urllib.request.urlopen(request, context=context, timeout=5) as response:  # try to open the url
                content = response.read().decode('utf-8')  # then get the data

                try:
                    json_content = json.loads(content)
                    self._content = json.dumps(json_content, indent=2)  # return it at json format
                except json.JSONDecodeError:
                    self._content = content

        except Exception as e:
            self._content = f"Error fetching URL: {str(e)}"


