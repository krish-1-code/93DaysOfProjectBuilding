# Undo/Redo Text Editor

Day 2 of my **93DaysOfProjectBuilding** series.

A command-line text editor built in C++ with AI assistance to explore how stacks can support undo and redo.

## Features

- Append text to the document.
- Erase text from the end.
- Undo and redo multiple edits.
- Clear redo history when a new edit creates a different history path.
- Reject invalid commands and deletion counts.
- Preserve document history when an operation changes nothing.
