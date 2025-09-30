# README

This project is a Lisp/Scheme interpreter implemented in C. It supports interactive mode, file input mode, and an automated test suite.

***

## File Structure

- **main.c**  
  Contains the main program logic including `main()` function and the `run()` loop for interactive or file evaluation.

- **sexpr.h**  
  Implements S-expression data structures, parsing, printing, and related manipulation functions.

- **utils.h**  
  Contains utility functions supporting core functionalities, such as string handling and helper methods.

- **tests.h**  
  Implements the automated test suite with a set of expressions, expected outputs, and a test runner function.

***

## How to Build

Use a standard C compiler like `gcc` or `clang`. Example:

```bash
gcc -o yisp main.c -lm
```

Add other `.c` files as necessary depending on project organization.

***

## How to Run

### 1. Interactive Mode

Start the interpreter without arguments for interactive Lisp prompt:

```bash
./yisp
```

You will see a prompt `>`. Enter S-expressions and see their evaluation results immediately.

Type `exit` to quit.

***

### 2. File Input Mode

Provide the path to a file containing Lisp expressions as an argument:

```bash
./yisp input.txt
```

The interpreter will read the entire file, parse and evaluate the expressions, and print the final result.

***

### 3. Run Automated Tests

Run the program with the `--test` argument to execute the full test suite:

```bash
./yisp --test
```

This runs a predefined set of test expressions against expected results, printing pass/fail status and details for each.

***

## Notes

- The interpreter maintains state (variable and function definitions) during interactive and test suite runs.
- Error messages are printed for invalid expressions (e.g., division by zero).
- The codebase modularly separates core S-expression logic (`sexpr.h`), utilities (`utils.h`), main program (`main.c`), and tests (`tests.h`).

***

For questions or contributions, please reach out or submit pull requests.

***

This guide should help you build, run, and test the interpreter efficiently.
