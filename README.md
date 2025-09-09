# Yisp Interpreter

This project is a simple interpreter for the Yisp language, written in C.

## Organization of Code

- `main.c`: Contains the entry point (`main` function) and core interpreter logic.
- `test_plan.txt`: Describes the testing strategy and specific test cases.
- `main`: Compiled executable generated from `main.c`.

## Directory Structure

```text
/Users/ahmedryan/Projects/yisp-interpreter
├── main.c          # Main source file (entry point)
├── main            # Compiled executable
├── test_plan.txt   # Test plan and descriptions
```

## Build Process

To build the main interpreter:

```sh
gcc -o main main.c
```

## Running

To run the interpreter:

```sh
./main
```

## Main Function Location

The `main` function is located in `main.c`.

---

Feel free to modify or extend the interpreter and tests as needed.
