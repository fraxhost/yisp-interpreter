#ifndef TESTS_H
#define TESTS_H
#include "sexpr.h"
#include "utils.h"

typedef struct Test
{
    const char *input;
    const char *expected_output;
} Test;

void runTests()
{
    Test tests[] = {
        {"()", "()"},                       
        {"'a", "a"},                        
        {"'()", "()"},                       
        {"\"hello world\"", "\"hello world\""},
        {"123", "123"},                     
        {"-456.78", "-456.78"},             
        {"(cons 'a '())", "(a)"},           
        {"(cons 'a '(b c))", "(a b c)"},    
        {"(car '(a b c))", "a"},            
        {"(cdr '(a b c))", "(b c)"},        
        {"(cons 1 2)", "(1 . 2)"},          
        {"(quote (a b c))", "(a b c)"},     
        {"(quote (a . b))", "(a . b)"},     
        {"(define x 42)", "x"},             

        // Predicate & function tests
        {"(nil? '())", "t"},
        {"(nil? '(1 2 3))", "()"},
        {"(symbol? 'foo)", "t"},
        {"(symbol? 42)", "()"},
        {"(number? 42)", "t"},
        {"(number? \"hello\")", "()"},
        {"(string? \"hello\")", "t"},
        {"(string? 42)", "()"},
        {"(list? '(1 2 3))", "t"},
        {"(list? '())", "t"},
        {"(list? 42)", "()"},
        {"(sexp_to_bool '())", "()"},
        {"(sexp_to_bool 0)", "t"},
        {"(sexp_to_bool 'foo)", "t"},
        {"(sexp_to_bool '(1))", "t"},
        {"(cons 'a 'b)", "(a . b)"},
        {"(car '(a b c))", "a"},
        {"(cdr '(a b c))", "(b c)"},

        // New arithmetic and comparison tests
        {"(add 1 2)", "3"},
        {"(sub 7 3)", "4"},
        {"(mul 2 5)", "10"},
        {"(div 8 2)", "4"},
        {"(mod 8 3)", "2"},
        {"(lt 2 3)", "1"},
        {"(lt 3 2)", "0"},
        {"(gt 5 1)", "1"},
        {"(gt 1 5)", "0"},
        {"(lte 4 4)", "1"},
        {"(gte 7 2)", "1"},
        {"(gte 3 5)", "0"},
        {"(eq 5 5)", "t"},
        {"(eq \"foo\" \"foo\")", "t"},
        {"(eq 'a 'b)", "()"},
        {"(not 1)", "0"},
        {"(not 0)", "1"},

        // Evaluation and environment tests
        {"()", "()"},
        {"'a", "a"},
        {"42", "42"},
        {"\"cat\"", "\"cat\""},
        {"(quote (1 2 3))", "(1 2 3)"},
        {"(set x 33)", "33"},
        {"x", "33"},
        {"(set x 7)", "7"},
        {"x", "7"},
        {"(set y \"dog\")", "\"dog\""},
        {"y", "\"dog\""},
        {"(add 1 2)", "3"},
        {"(sub (add 5 2) 4)", "3"},
        {"(set z (add 2 2))", "4"},
        {"z", "4"},
        {"(quote x)", "x"},
        {"(set foo (quote (a b c)))", "(a b c)"},
        {"foo", "(a b c)"},
        {"unknown-symbol", "unknown-symbol"},
        // {"(div 7 0)", "Error: division by zero"},

        // New logical and control flow tests
        {"(and nil (error))", "()"},
        {"(and t 42)", "42"},
        {"(and 1 2)", "2"},
        {"(and nil nil)", "()"},
        {"(or t (error))", "t"},
        {"(or nil 10)", "10"},
        {"(or nil nil)", "()"},
        {"(if t 1 2)", "1"},
        {"(if nil 1 2)", "2"},
        {"(if 42 10 20)", "10"},
        {"(cond ((and t nil) 7) ((or nil t) 8))", "8"},
        {"(and (or nil t) (if t 3 4))", "3"},
        {"(or (and nil 5) (and t 6))", "6"},
        {"(if (and t nil) 1 0)", "0"},
        {"(if (or nil nil) 1 0)", "0"},
        {"(and (set x t) (set y nil) (or x y))", "t"},
        {"(cond ((eq 1 2) 10) ((eq 2 2) 20) (else 30))", "20"},

        // Lambda, higher-order, recursion tests
        {"(lambda (x) (add x 1))", "(lambda (x) (add x 1))"},
        {"((lambda (x) (add x 1)) 5)", "6"},
        {"(define inc (lambda (x) (add x 1)))", "inc"},
        {"(inc 10)", "11"},
        {"(define sum (lambda (a b) (add a b)))", "sum"},
        {"(sum 7 3)", "10"},
        {"((lambda () 42))", "42"},
        {"((lambda (x y) (* x y)) 4 5)", "20"},
        {"(define twice (lambda (f x) (f (f x))))", "twice"},
        {"(define add3 (lambda (n) (add n 3)))", "add3"},
        {"(twice add3 10)", "16"},
        {"((lambda (f) (f 5)) (lambda (x) (add x 2)))", "7"},
        {"(define make-adder (lambda (x) (lambda (y) (add x y))))", "make-adder"},
        {"(define factorial (lambda (n) (if (= n 0) 1 (* n (factorial (sub n 1))))))", "factorial"},
        {"(factorial 5)", "120"},
        {"(define compose (lambda (f g) (lambda (x) (f (g x)))))", "compose"},
        {"(define id (lambda (x) x))", "id"},
        {"(id \"hello\")", "\"hello\""}
    };

    Env *test_env = make_env(NULL);
    init_symbols();

    int n = sizeof(tests) / sizeof(tests[0]);

    printf("Running %d tests...\n", n);
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < n; i++)
    {
        const char *input_str = tests[i].input;
        const char *expected_str = tests[i].expected_output;

        const char *ptr = input_str;
        SExpr *expr = parseSExpr(&ptr);

        SExpr *result = eval(expr, test_env);

        char output_buffer[1024];
        sexp_to_string(result, output_buffer, sizeof(output_buffer));

        // Compare expected and actual
        bool pass = (strcmp(expected_str, output_buffer) == 0);

        // Print green tick or red cross using UTF-8 Unicode characters
        const char *symbol = pass ? "PASSED" : "FAILED";

        printf("TEST %2d %s \n", i + 1, symbol);
        printf("Input:           %s\n", input_str);
        printf("Expected output: %s\n", expected_str);
        printf("Actual output:   %s\n", output_buffer);
        printf("------------------------------------------------------------\n");

        // Cleanup if needed
    }

}


#endif // TESTS_H