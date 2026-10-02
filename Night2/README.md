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

## Tools Used

- C++17
- C++ STL: `std::stack` and `std::string`
- `std::istringstream` for parsing commands

No third-party libraries are required.

## How It Works

The editor stores the current document and two stacks of previous document states:

- **Undo stack:** states available to restore when undoing.
- **Redo stack:** states available to restore when redoing.

### Making an Edit

Before a successful append or deletion, the editor saves the current document in the undo stack. It then changes the document and clears the redo stack.

### Undo

The editor saves the current document in the redo stack, restores the top state from the undo stack, and removes that undo entry.

### Redo

The editor saves the current document in the undo stack, restores the top state from the redo stack, and removes that redo entry.

An empty append or invalid deletion leaves both the document and its history unchanged.

## Build and Run

You need a C++17-compatible compiler.

From the `Night2` directory, run these commands in PowerShell:

```powershell
g++ -std=c++17 -Wall -Wextra editor.cpp -o editor.exe
.\editor.exe
```

## Commands

| Command | Description |
|---|---|
| `append TEXT` | Append text, including spaces |
| `erase N` | Remove the last N bytes; for ASCII text, N characters |
| `undo` | Undo the most recent edit |
| `redo` | Restore the most recently undone edit |
| `show` | Display the document inside brackets |
| `help` | Display available commands |
| `quit` | Exit the editor |

Deletion requires a positive whole number no larger than the document length. Commands are case-sensitive.

## Example Session

The following is an expected interaction after the command menu appears:

```text
> append A
> append B
> show
[AB]
> undo
> show
[A]
> redo
> show
[AB]
> undo
> append C
> redo
Nothing to redo.
> show
[AC]
> erase 1
> show
[A]
> undo
> show
[AC]
> quit
Goodbye!
```

Appending `C` after undo creates a new history path. The previous `AB` state is no longer available through redo.

## Manual Validation Checklist

- [ ] Append text containing spaces.
- [ ] Undo multiple edits back to an empty document.
- [ ] Redo multiple edits.
- [ ] Handle undo and redo when their stacks are empty.
- [ ] Clear redo history after a successful new edit.
- [ ] Undo and redo a deletion.
- [ ] Delete the entire document, then undo.
- [ ] Reject negative, zero, excessive, and nonnumeric deletion counts.
- [ ] Preserve an available redo after an invalid deletion.
- [ ] Handle unknown commands and exit with `quit`.

## Design Tradeoff

This implementation stores complete document snapshots. That makes the history logic straightforward, but each saved state copies the document.

For a document of length L, saving or restoring a snapshot takes O(L) time. Retaining K snapshots of up to L bytes can require O(K × L) storage. Clearing redo history also requires discarding its stored snapshots.

A future version could store individual edit operations and compare their memory cost with full snapshots.

## Current Limitations

- Documents and history exist only in memory and are lost when the program exits.
- Editing is limited to appending and deleting at the end.
- History has no configured size limit.
- Deletion counts bytes, so it is intended for ASCII text rather than Unicode-aware editing.

## What I Practiced

- Applying stacks to document history.
- Managing state across undo, redo, and new edits.
- Validating input before changing state.
- Separating editor behavior from the command-line interface.