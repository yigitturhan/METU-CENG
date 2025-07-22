from .component import Component
from .dash import Dash


class Repo:
    _instance = None

    def __new__(cls):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
            cls._instance._init()
        return cls._instance

    def _init(self):
        self._objects = {}
        self._attached = {}
        self._next_id = 1
        self.components = Component  # Interface to Component class

    def create(self, **kwargs):
        obj_id = str(self._next_id)
        self._next_id += 1
        dash = Dash(obj_id, **kwargs)
        self._objects[obj_id] = dash
        return obj_id

    def get_objects(self):
        return self._objects

    def list(self):
        return [(obj_id, obj.desc()) for obj_id, obj in self._objects.items()]

    def listattached(self, user):
        return [(obj_id, obj.desc())
                for obj_id, obj in self._objects.items()
                if user in self._attached.get(obj_id, set())]

    def attach(self, obj_id, user):
        if str(obj_id) not in self._objects.keys():  # checks the object is created before if not raises an error
            return "Unknown object id"
        if obj_id not in self._attached:  # checks if it is attached before if not attaches
            self._attached[obj_id] = set()
        self._attached[obj_id].add(user)  # adds the user to the corresponding ids set ex: '2': {'onur'}
        return self._objects[str(obj_id)]  # return the object

    def detach(self, obj_id, user):
        print(obj_id)
        if obj_id in self._attached.keys() and user in self._attached[obj_id]:  # if attached by someone and user is the one
            # who attaches it then an detach
            self._attached[obj_id].remove(user)

    def delete(self, obj_id): # if attached then raises the error else deletes the object
        if obj_id in self._attached:
            raise ValueError("Cannot delete attached object")
        if obj_id in self._objects:
            del self._objects[obj_id]


repo = Repo()
