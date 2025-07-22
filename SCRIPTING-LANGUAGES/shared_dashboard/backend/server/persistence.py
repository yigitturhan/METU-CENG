import sqlite3
import pickle


def _serialize_tab(tab):
    return {
        'name': tab.name,
        'rows': [[comp.id if comp else None for comp in row]
                 for row in tab.get_rows()]
    }


class DashboardPersistence:
    def __init__(self, db_path="dashboards.db"):
        self.db_path = db_path
        self._init_db()

    def _init_db(self):
        with sqlite3.connect(self.db_path) as conn:
            conn.execute("""
                CREATE TABLE IF NOT EXISTS dashboards (
                    id TEXT PRIMARY KEY,
                    name TEXT,
                    data BLOB,
                    last_modified TIMESTAMP DEFAULT CURRENT_TIMESTAMP
                )
            """)

            conn.execute("""
                CREATE TABLE IF NOT EXISTS components (
                    id INTEGER,
                    tab_name TEXT,
                    dashboard_id TEXT,
                    row INTEGER,
                    col INTEGER,
                    type TEXT,
                    data BLOB,
                    FOREIGN KEY (dashboard_id) REFERENCES dashboards(id)
                )
            """)

    def save_dashboard(self, dash):
        with sqlite3.connect(self.db_path) as conn:
            # Serialize dashboard data
            dash_data = pickle.dumps({
                'name': dash.name,
                '_tabs': {name: _serialize_tab(tab)
                          for name, tab in dash.get_tabs().items()}
            })

            # Save dashboard
            conn.execute("""
                INSERT OR REPLACE INTO dashboards (id, name, data)
                VALUES (?, ?, ?)
            """, (dash.get_id(), dash.name, dash_data))

            # Save components
            for tab in dash.get_tabs().values():
                conn.execute("""
                    DELETE FROM components
                    WHERE dashboard_id = ? AND tab_name = ?
            """, (dash.get_id(), tab.name))
                for row in tab.get_rows():
                    for component in row:
                        if component is not None:
                            comp_data = pickle.dumps({
                                'env': component.env,
                                'param': component.param,
                                'attributes': {
                                    'name': component.name,
                                    'title': component.title,
                                    'height': component.height,
                                    'width': component.width,
                                    'refresh_interval': component.refresh_interval
                                },
                                'messages': component.messages if hasattr(component, 'messages') else []
                            })

                            try:
                                r, c = tab.get_location(component.id)
                                conn.execute("""
                                    INSERT INTO components 
                                    (id, tab_name, dashboard_id, row, col, type, data)
                                    VALUES (?, ?, ?, ?, ?, ?, ?)
                                """, (component.id, tab.name, dash.get_id(), r, c,
                                      component.type(), comp_data))
                            except:
                                continue

    def load_dashboard(self, dash_id):
        with sqlite3.connect(self.db_path) as conn:
            cursor = conn.execute(
                "SELECT data FROM dashboards WHERE id = ?",
                (dash_id,)
            )
            row = cursor.fetchone()
            if not row:
                return None

            dash_data = pickle.loads(row[0])
            from backend.core.repo import repo
            dash = repo.create(name=dash_data['name'])

            cursor = conn.execute(
                "SELECT id, tab_name, type, data, row, col FROM components WHERE dashboard_id = ?",
                (dash_id,)
            )
            components = []
            for id, tab_name, comp_type, comp_data, cr, cc in cursor.fetchall():
                comp_data = pickle.loads(comp_data)
                component = repo.components.create(comp_type)
                component.id = id
                component.env.update(comp_data['env'])
                component.param.update(comp_data['param'])
                if 'messages' in comp_data.keys():
                    component.messages = comp_data['messages']
                for key, value in comp_data['attributes'].items():
                    setattr(component, key, value)

                components.append((component, tab_name, cr, cc))
                repo.components._id_counter += 1

            dash = repo.get_objects()[dash]
            for tab_name, tab_data in dash_data['_tabs'].items():
                tab = dash.create(tab_name)
                self._deserialize_tab(tab, components)

            return dash

    @classmethod
    def _deserialize_tab(cls, tab, components):
        for comp, tn, r, c in components:
            if tn == tab.name:
                tab.place(comp, r, c)


class AutoSaveDashboard:
    """Decorator to auto-save dashboard on detach"""

    def __init__(self, persistence):
        self.persistence = persistence

    def __call__(self, func):
        def wrapper(repo_instance, dash_id, user):
            result = func(repo_instance, dash_id, user)
            if dash_id in repo_instance._attached and \
                    not repo_instance._attached[dash_id]:
                # Last user detached, save dashboard
                if dash_id in repo_instance._objects:
                    self.persistence.save_dashboard(
                        repo_instance._objects[dash_id]
                    )
            return result

        return wrapper


persistence = DashboardPersistence()
