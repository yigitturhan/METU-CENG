from .tab import Tab


class Dash:
    def __init__(self, obj_id, name):
        self._id = obj_id  # Set by Repo
        self.name = name
        self._tabs = {}

    def desc(self):
        return f"Dashboard {self.name}"

    # Dictionary-like access to tabs
    def __getitem__(self, tab_name):
        return self._tabs[tab_name]

    def __setitem__(self, tab_name, tab):
        self._tabs[tab_name] = tab

    def __delitem__(self, tab_name):
        del self._tabs[tab_name]

    # Tab management
    def create(self, name):  # if given name in tabs raises the error else creates it
        if name in self._tabs:
            raise ValueError(f"Tab {name} already exists")
        tab = Tab(name)
        self._tabs[name] = tab
        return tab

    def get_tabs(self):
        return self._tabs

    def get_id(self):
        return self._id

    def serialize(self):
        return {
            "id": self._id,
            "name": self.name,
            "tabs": {name: tab.serialize() for name, tab in self._tabs.items()}
        }
