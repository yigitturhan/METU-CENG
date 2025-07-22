


class Component:
    _id_counter = 0

    def __init__(self, **attributes):
        self.attributes = attributes

    @classmethod
    def list(cls):
        from backend.core.component_registry import COMPONENT_REGISTRY
        return [(key, value.desc()) for key, value in COMPONENT_REGISTRY.items()]

    @classmethod
    def _generate_id(cls):
        """Generates a unique ID for each component."""
        cls._id_counter += 1
        return cls._id_counter

    @classmethod
    def create(cls, type_name, **attributes):
        from backend.core.component_registry import COMPONENT_REGISTRY
        """Creates a new component instance with a unique ID."""
        if type_name not in COMPONENT_REGISTRY:
            raise ValueError(f"Unknown component type: {type_name}")
        component_cls = COMPONENT_REGISTRY[type_name]
        component = component_cls(**attributes)
        component.id = cls._generate_id()
        return component

    @classmethod
    def register(cls, type_name, component_cls):
        from backend.core.component_registry import COMPONENT_REGISTRY
        """Registers a new component type."""
        COMPONENT_REGISTRY[type_name] = component_cls

    @classmethod
    def unregister(cls, type_name):
        from backend.core.component_registry import COMPONENT_REGISTRY
        """Unregisters an existing component type."""
        if type_name in COMPONENT_REGISTRY:
            del COMPONENT_REGISTRY[type_name]

    def desc(self):
        return f"Component: {self.type()}"

    def type(self):
        return self.__class__.__name__

    def attrs(self):
        return self.attributes.keys()

    def draw(self):
        return f"{self.desc()} with attributes {self.attributes}"

