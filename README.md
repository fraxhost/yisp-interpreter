# Yisp Interpreter

This project is a simple interpreter for the Yisp language, written in C.

## Organization of Code

- `main.c`: Contains the entry point (`main` function), core interpreter logic, and data structures for S-expressions.
- `test_plan.txt`: Contains the test plan, listing test cases and expected behaviors for S-expression parsing and type checking.
- `test_results.txt`: Stores the results of running the test plan against the interpreter.
- `main`: Compiled executable generated from `main.c`.

## Directory Structure

```text
/Users/ahmedryan/Projects/yisp-interpreter
├── main.c            # Main source file (entry point)
├── main              # Compiled executable
├── test_plan.txt     # Test plan for S-expression parsing
├── test_results.txt  # Results of running tests
├── README.md         # Project documentation
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
