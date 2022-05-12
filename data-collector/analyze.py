import ast
from pathlib import Path
import json

"""
Keywords overview:
✅ False
✅ True
✅ None
✅ await
✅ else
✅ if
✅ elif
✅ import
✅ pass
⛔️ break
⛔️ except
⛔️ in
⛔️ raise
⛔️ class
⛔️ finally
⛔️ is
⛔️ return
⛔️ and
⛔️ continue
⛔️ for
⛔️ lambda
⛔️ try
⛔️ as
⛔️ def
⛔️ from
⛔️ nonlocal
⛔️ while
⛔️ assert
⛔️ del
⛔️ global
⛔️ not
⛔️ with
⛔️ async
⛔️ or
⛔️ yield
"""

# Keyword extractor
class Analyzer(ast.NodeVisitor):
    def __init__(self):
        # Initialize statistics
        self.stats = {"variable_names": {}, "keywords": {}}
        self.skip = []

    def visit_Assign(self, node):
        # Collect all variable names
        for alias in node.targets:
            if isinstance(alias, ast.Name):
                self.count_variable_name(str(alias.id))

        # Continue recursively
        self.generic_visit(node)

    def count_keyword(self, keyword):
        if keyword not in self.stats["keywords"]:
            self.stats["keywords"][keyword] = 0

        self.stats["keywords"][keyword] += 1

    def count_variable_name(self, variable_name):
        if variable_name not in self.stats["variable_names"]:
            self.stats["variable_names"][variable_name] = 0

        self.stats["variable_names"][variable_name] += 1

    def visit_Constant(self, node):
        # Handle True and False
        if type(node.value) == bool:
            self.count_keyword(str(node.value))

        # Handle None
        elif type(node.value) == type(None):
            self.count_keyword(str(node.value))
            
        self.generic_visit(node)

    def visit_Await(self, node):
        # Handle await 
        self.count_keyword("await")
        self.generic_visit(node)

    def analyze_if_chain(self, node):
        # Let's check if there's an else/elif present
        if len(node.orelse) > 0:
            # Extract the else/elif
            orelse = node.orelse[0]
            if type(orelse) == ast.If:
                # We found an elif, let's count it and 
                # continue exploring
                self.skip.append(orelse)
                self.count_keyword("elif")
                self.analyze_if_chain(orelse)

            elif type(orelse) == ast.Expr:
                # We've reached an else, we can stop here
                self.count_keyword("else")

    def visit_If(self, node):
        # Handle if and child else/elif(s)
        if node not in self.skip:
            self.count_keyword("if")
            self.analyze_if_chain(node)

        self.generic_visit(node)

    def visit_Import(self, node):
        # Handle import
        self.count_keyword("import")
        self.generic_visit(node)

    def visit_Pass(self, node):
        # Handle pass
        self.count_keyword("pass")
        self.generic_visit(node)

    def visit_Break(self, node):
        # Handle import
        self.count_keyword("break")
        self.generic_visit(node)

    def generic_visit(self, node):
        print(type(node).__name__)
        print(type(node).__name__)
        ast.NodeVisitor.generic_visit(self, node)

def main():
    # General variables
    stats = {}

    # Analyze files
    for filename in Path('data').rglob('*.py'):
        # Read file contents
        f = open(filename, "r")
        contents = f.read()
        f.close()

        try:
            tree = ast.parse(contents)
            analyzer = Analyzer()
            analyzer.visit(tree)
            stats = analyzer.stats
            print(stats)
        except Exception as e:
            print(e)
            # Die in silence
            pass
            
    # Sort data for review purposes
    stats_sorted = {}
    for key in stats.keys():
        stats_sorted[key] = dict(sorted(stats[key].items(), key=lambda item: item[1], reverse=True))

    # Dump data
    out_file = open("output.json", "w")
    json.dump(stats_sorted, out_file, indent = 4)
    out_file.close()


main()