from backend.core.component import Component


class BaseWidget(Component):
    """Base class for all widgets with common functionality"""

    def __init__(self, name, title, height=100, width=200, **attributes):
        super().__init__(**attributes)  # not sure
        self.name = name
        self.title = title
        self.height = height  # mm
        self.width = width  # mm
        self.env = {}  # Environment variables
        self.param = {}  # User parameters
        self.events = ["refresh"]  # Default event is refresh at phase1
        self.refresh_interval = 1

    def type(self):
        return self.name

    def attrs(self):
        base_attrs = [
            ("name", "string"),
            ("title", "string"),
            ("height", "int"),
            ("width", "int"),
            ("refresh_interval", "int")
        ]
        return base_attrs + self._get_specific_attrs()  # adds the base attributes than specific attributes based on
        # the widget definition

    def _get_specific_attrs(self):  # will override at subclasses if there is a specific attribute
        return []

    def view(self, user=None):
        raise NotImplementedError  # override at every widget. If there will be a widget that does not have view
        # function this need to change

    def refresh(self):  # all widgets override that method
        pass

    def trigger(self, event, params):
        if event not in self.events:
            raise ValueError(f"Unknown event: {event}")
        if event == "refresh":  # we do not know what are the events
            self.refresh()
