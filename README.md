# Kaleidoscope Language 

Kaleidoscope (derived from *“meaning beautiful, form, and view”*) is a toy procedural language used to illustrate compiler design concepts. This tutorial walks through building Kaleidoscope step by step, introducing features such as:

- Function definitions
- Conditionals (`if/then/else`)
- Loops (`for`)
- User-defined operators
- JIT compilation with a simple command-line interface
- Debug information

---

## Language Overview

- **Single Datatype**: All values are represented as 64-bit floating point (`double` in C parlance).
- **Implicit Typing**: No type declarations are required; every value is treated as a double.
- **Simple Syntax**: Minimalistic design makes it easy to focus on compiler internals rather than language complexity.

---

## Example

A simple Kaleidoscope program to compute a mathematical expression:

```kaleidoscope
def test(x y)
  x * y + (x - y) * (x + y);

test(4, 3)
