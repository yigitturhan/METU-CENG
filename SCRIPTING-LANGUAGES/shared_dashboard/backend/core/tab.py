class Tab:

    def __init__(self, name):
        self.name = name
        self._rows = []

    def newrow(self, row=-1):
        if row == -1:
            self._rows.append([])
        else:
            self._rows.insert(row + 1, [])

    def place(self, component, row, col=-1):
        while len(self._rows) <= row:
            self.newrow()
        # Place component
        if col == -1:
            self._rows[row].append(component)
        else:
            # Ensure column exists
            while len(self._rows[row]) <= col:
                self._rows[row].append(None)  # makes the padding with None
            self._rows[row][col] = component  # insert mü olmali override mı etmeli

    def get_location(self, comp_id):
        for ind1, row in enumerate(self._rows):
            for ind2, comp in enumerate(row):
                if not comp:
                    continue
                if comp.id == comp_id:
                    return ind1, ind2
        return -1, -1

    def __getitem__(self, pos):
        row, col = pos
        return self._rows[row][col]

    def __delitem__(self, pos):
        row, col = pos
        self._rows[row][col] = None

    def remove(self, component):
        for row in self._rows:
            if component in row:
                row[row.index(component)] = None

    def get_rows(self):
        return self._rows

    def view(self):
        result = [f"Tab: {self.name}"]  # a simple view function to return currently used items
        for i, row in enumerate(self._rows):
            row_str = "Row {}: ".format(i)
            components = [comp.view() if comp else "Empty" for comp in row]
            row_str += " | ".join(components)
            result.append(row_str)
        return "\n".join(result)

    def refresh(self):  # it refreshes all the components in the tab (calls the component's refresh function)
        for row in self._rows:
            for component in row:
                if component:
                    component.refresh()

    def serialize(self):
        return {
            "name": self.name,
            "rows": [[comp.serialize() if comp else None for comp in row] for row in self._rows]
        }
