import sqlite3
from .base import BaseWidget

path = "/Users/ahmetyigitturhan/Documents/script_web_dashboard/backend/widgets/dbs/"


class DBQuery(BaseWidget):
    def __init__(self):
        super().__init__("DBQuery", "Database Query Viewer")
        self.env = {
            "database": "",
            "query": ""
        }
        self._results = {}

    @classmethod
    def desc(cls):
        return "Handles the queries for database."

    def _get_specific_attrs(self):
        return [("database", "string"), ("query", "string")]

    def get_results(self, username):
        if username in self._results.keys():
            return self._results[username]
        else:
            return "Not executed yet"

    def view(self, user=None):
        if user not in self._results.keys():
            return f"<p>[DBQuery: No results for {user}]</p>"

        results = self._results[user]
        if not results:
            return f"<p>[DBQuery: No results for {user}]</p>"

        svg_content = '<svg xmlns="http://www.w3.org/2000/svg" font-family="Arial" font-size="18">'
        x, y, col_width, row_height = 20, 40, 150, 20

        if "description" in self._results:
            headers = [desc[0] for desc in self._results["description"]]
            for i, header in enumerate(headers):
                svg_content += f'<text x="{x + i * col_width}" y="{y}" font-weight="bold">{header}</text>'
            y += row_height

        for row in results:
            for i, cell in enumerate(row):
                svg_content += f'<text x="{x + i * col_width}" y="{y}">{cell}</text>'
            y += row_height

        svg_content += '</svg>'

        return svg_content

    def trigger(self, event, params):
        if event == "execute":
            if not params["db"] or not params["query"]:
                return
            try:
                with sqlite3.connect(path + params["db"]) as conn:
                    cursor = conn.cursor()
                    cursor.execute(params["query"])  # executing the query
                    self._results["description"] = cursor.description  # save column headers
                    self._results[params["username"]] = cursor.fetchall()  # save query results
            except sqlite3.Error as e:
                self._results["error"] = str(e)  # save error if query fails

    def refresh(self):
        return
