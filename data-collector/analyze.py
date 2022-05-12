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
✅ break
✅ except
✅ in
✅ raise
✅ class
✅ finally
✅ is
✅ return
✅ and
✅ continue
✅ for
✅ lambda
✅ try
✅ as
✅ def
✅ from
✅ nonlocal
✅ while
✅ assert
✅ del
✅ global
✅ not
✅ with
✅ async
✅ or
✅ yield
"""

# Keyword extractor
class Analyzer(ast.NodeVisitor):
    def __init__(self):
        # Initialize statistics
        self.stats = {"variable_names": {}, "keywords": {}}
        self.skip = []

    def get_stats(self):
        # Sort data for review purposes
        stats_new = {}
        for key in self.stats.keys():
            stats_new[key] = dict(sorted(self.stats[key].items(), key=lambda item: item[1], reverse=True))

        return stats_new

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

    def visit_Try(self, node):
        # Handle try
        self.count_keyword("try")

        # Handle finally
        if len(node.finalbody) > 0:
            self.count_keyword("finally")

        # Handle else
        if len(node.orelse) > 0:
            self.count_keyword("else")
        self.generic_visit(node)

    def visit_ExceptHandler(self, node):
        # Handle as (part 2)
        if (node.name):
            self.count_keyword("as")
        # Handle except
        self.count_keyword("except")
        self.generic_visit(node)

    def visit_Import(self, node):
        # Handle import
        self.count_keyword("import")
        self.generic_visit(node)

    def visit_ImportFrom(self, node):
        # Handle import
        self.count_keyword("import")
        # Handle frm
        self.count_keyword("from")
        self.generic_visit(node)

    def visit_alias(self, node):
        # Handle as (part 1)
        if node.asname:
            self.count_keyword("as")
        
        self.generic_visit(node)

    def visit_Pass(self, node):
        # Handle pass
        self.count_keyword("pass")
        self.generic_visit(node)

    def visit_Break(self, node):
        # Handle break
        self.count_keyword("break")
        self.generic_visit(node)

    def visit_ClassDef(self, node):
        # Handle class
        self.count_keyword("class")
        self.generic_visit(node)

    def visit_In(self, node):
        # Handle in
        self.count_keyword("in")
        self.generic_visit(node)

    def visit_Is(self, node):
        # Handle is
        self.count_keyword("is")
        self.generic_visit(node)

    def visit_Raise(self, node):
        # Handle raise
        self.count_keyword("raise")
        self.generic_visit(node)

    def visit_Return(self, node):
        # Handle return
        self.count_keyword("return")
        self.generic_visit(node)

    def visit_And(self, node):
        # Handle and
        self.count_keyword("and")
        self.generic_visit(node)

    def visit_Or(self, node):
        # Handle or
        self.count_keyword("or")
        self.generic_visit(node)

    def visit_Continue(self, node):
        # Handle continue
        self.count_keyword("continue")
        self.generic_visit(node)

    def visit_While(self, node):
        # Handle while
        self.count_keyword("while")
        self.generic_visit(node)

    def visit_For(self, node):
        # Handle for
        self.count_keyword("for")
        self.generic_visit(node)

    def visit_Lambda(self, node):
        # Handle lambda
        self.count_keyword("lambda")
        self.generic_visit(node)

    def visit_FunctionDef(self, node):
        # Handle def
        self.count_keyword("def")
        self.generic_visit(node)

    def visit_AsyncFunctionDef(self, node):
        # Handle async
        self.count_keyword("async")
        # Handle def
        self.count_keyword("def")
        self.generic_visit(node)

    def visit_With(self, node):
        # Handle with
        self.count_keyword("with")
        self.generic_visit(node)

    def visit_Yield(self, node):
        # Handle yield
        self.count_keyword("yield")
        self.generic_visit(node)

    def visit_Del(self, node):
        # Handle del
        self.count_keyword("del")
        self.generic_visit(node)

    def visit_Global(self, node):
        # Handle global
        self.count_keyword("global")
        self.generic_visit(node)

    def visit_Nonlocal(self, node):
        # Handle nonlocal
        self.count_keyword("nonlocal")
        self.generic_visit(node)

    def visit_Assert(self, node):
        # Handle assert
        self.count_keyword("assert")
        self.generic_visit(node)

    def visit_Not(self, node):
        # Handle assert
        self.count_keyword("not")
        self.generic_visit(node)

# Merge two statistics dictionaries
def merge(dict1, dict2):
    result = dict1

    for type in ["variable_names", "keywords"]:
        for key in dict2[type].keys():
            if key in result[type]:
                result[type][key] += dict2[type][key]
            else:
                result[type][key] = dict2[type][key]

    return result

def main():
    # General variables
    stats = {"variable_names": {}, "keywords": {}}

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
            merge(stats, analyzer.get_stats())
        except Exception as e:
            # Die in silence
            pass

    # Dump data
    out_file = open("output.json", "w")
    json.dump(stats, out_file, indent = 4)
    out_file.close()


main()