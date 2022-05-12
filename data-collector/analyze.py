import ast
from pathlib import Path
import json

# Keyword extractor
class Analyzer(ast.NodeVisitor):
    def __init__(self):
        # Initialize statistics
        self.stats = {"variable_names": []}

    def visit_Assign(self, node):
        # Collect all variable names
        for alias in node.targets:
            if isinstance(alias, ast.Name):
                self.stats["variable_names"].append(alias.id)

        # Continue recursively
        self.generic_visit(node)

def main():
    # General variables
    data = []
    keywords_processed = {}

    # Analyze files
    for filename in Path('data').rglob('*.py'):
        # Read file contents
        f = open(filename, "r")
        contents = f.read()
        f.close()

        # Count keywords
        # TODO: rewrite this to use AST
        keywords = ['False', 'await', 'else', 'import', 'pass', 'None', 'break', 'except', 'in', 'raise', 'True', 'class', 'finally', 'is', 'return', 'and', 'continue', 'for', 'lambda', 'try', 'as', 'def', 'from', 'nonlocal', 'while', 'assert', 'del', 'global', 'not', 'with', 'async', 'elif', 'if', 'or', 'yield']
        for keyword in keywords:
            if keyword not in keywords_processed:
                keywords_processed[keyword] = 0
            keywords_processed[keyword] += contents.count(keyword)


        try:
            tree = ast.parse(contents)
            analyzer = Analyzer()
            analyzer.visit(tree)
            data.extend(analyzer.stats["variable_names"])
        except:
            # Die in silence
            pass
            

    # Parse data
    data_processed = {}
    for instance in data:
        if instance not in data_processed:
            data_processed[instance] = 0

        data_processed[instance] += 1

    # Sort data for review purposes
    data_sorted = dict(sorted(data_processed.items(), key=lambda item: item[1], reverse=True))
    k_data_sorted = dict(sorted(keywords_processed.items(), key=lambda item: item[1], reverse=True))
    t = {
        "keywords": k_data_sorted,
        "variable_names": data_sorted
    }

    # Dump data
    out_file = open("output.json", "w")
    json.dump(t, out_file, indent = 4)
    out_file.close()


main()