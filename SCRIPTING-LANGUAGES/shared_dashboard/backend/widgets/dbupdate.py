import sqlite3
from backend.widgets.base import BaseWidget

path = "/Users/ahmetyigitturhan/Documents/script_web_dashboard/backend/widgets/dbs/"


class DBUpdate(BaseWidget):
    """Database update component that executes parameterized queries"""

    def __init__(self, database="", query=""):
        super().__init__("DBUpdate", "Database Update Widget")
        self.env = {
            "database": database,
            "query": query
        }
        self.param = {}
        self.events = ["submit"]
        self._last_result = None

    @classmethod
    def desc(cls):
        return "Executes parameterized database updates"

    def _get_specific_attrs(self):
        return [("database", "string"), ("query", "string")]

    def view(self, user=None):
        if self._last_result is None:
            return f"<p>[DBUpdate: No updates yet for {user}]</p>"
        svg_content = '<svg xmlns="http://www.w3.org/2000/svg" font-family="Arial" font-size="18">'
        x, y, col_width, row_height = 20, 40, 150, 20
        if isinstance(self._last_result, list) and self._last_result:
            headers = [str(i) for i in range(len(self._last_result[0]))]
            for i, header in enumerate(headers):
                svg_content += f'<text x="{x + i * col_width}" y="{y}" font-weight="bold">{header}</text>'
            y += row_height
            for row in self._last_result:
                for i, cell in enumerate(row):
                    svg_content += f'<text x="{x + i * col_width}" y="{y}">{str(cell)}</text>'
                y += row_height
        else:
            svg_content += '<text x="20" y="40">No data available</text>'

        svg_content += '</svg>'
        return svg_content

    def trigger(self, event, params=None):
        if event == "execute" and params:
            try:
                formatted_query = self.env["query"].format(params["query"])
                with sqlite3.connect(path + params["db"]) as conn:
                    cursor = conn.cursor()
                    cursor.execute(formatted_query)
                    self._last_result = cursor.fetchall()
                    conn.commit()
                return "Success"
            except Exception as e:
                self._last_result = f"Error: {str(e)}"
                return f"Error: {str(e)}"

    def refresh(self):
        return f"refresh: last result: {self._last_result}"
