import ast
from pathlib import Path
import json

# Keyword extractor
class Analyzer(ast.NodeVisitor):
    def __init__(self):
        self.stats = {"variable_names": []}

    def visit_Assign(self, node):
        for alias in node.targets:
            if isinstance(alias, ast.Name):
                self.stats["variable_names"].append(alias.id)
        self.generic_visit(node)

    def report(self):
        print(self.stats)

data = []
data_processed = {}
keywords_processed = {}

# Analyze files
for n in Path('data').rglob('*.py'):
    f = open(n, "r")
    contents = f.read()

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

    f.close()


# Parse data
for instance in data:
    if instance not in data_processed:
        data_processed[instance] = 0

    data_processed[instance] += 1

# Sort data
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