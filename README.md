<h1 align="center">Lightweight integrated development environment powered by finite automata</h1>

<p style="text-align: center;"><em>A project by Achraf Yandouzi, Flor Ronsmans De Vry, Jonas Caluwé and Michel Dierckx for the Automata Theory class at the University of Antwerp</em></p>

___

## The project
## Goals


## The infrastructure


## Technologies used
- C++
- TypeScript
- ReactJS
- Tailwind CSS
- Python
- Abstract syntax trees

## Project structure
```
|
|-- core (C++) - Core package that handles syntax highlighting and autocomplete logic 
|   |-- ...
|
|-- data-collector (Python) - Tooling to analyze keyword and variable naming trends
|   |-- analyze.py - Python script to analyze source code
|   |-- data - Python project(s) to analyze
|   |-- ...
| 
|-- data-visualizer (TypeScript) - Webpage built using ReactJS to visualize syntax highlighting results
|   |-- src - React source code
|       |-- App.tsx - React entrypoint
|       |-- Editor.tsx - Editor component
|       |-- ...
|
|-- ...
```

## Usage
### How to deploy

### Demo