#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// ==================== DATA STRUCTURES ====================

typedef enum SExprType
{
    TYPE_ATOM_NUMBER, // Numeric atom
    TYPE_ATOM_STRING, // String atom
    TYPE_ATOM_SYMBOL, // Symbol atom
    TYPE_CONS,        // Cons cell
    TYPE_NIL,         // Represents nil / empty list
} SExprType;

typedef struct SExpr
{
    SExprType type;
    union
    {
        double number; // For numeric atoms
        char *string;  // For strings or symbols
        struct cons
        {
            struct SExpr *car; // Head of the list
            struct SExpr *cdr; // Tail of the list
        } cons;
    };
} SExpr;

typedef struct Env
{
    SExpr *symbols;     // list of symbols in this env
    SExpr *values;      // list of corresponding values
    struct Env *parent; // enclosing environment (NULL for global)
} Env;

typedef struct TestCase
{
    const char *input;
    bool shouldBeNil;
    bool shouldBeNumber;
    bool shouldBeSymbol;
    bool shouldBeString;
    bool shouldBeList;
    bool shouldBeSExpr;
} TestCase;

// ==================== FUNCTION DECLARATIONS ====================

Env *make_env(Env *parent);
void set(Env *env, SExpr *symbol, SExpr *value);
SExpr *lookup(Env *env, SExpr *symbol);

SExpr *nil();
SExpr *number(double value);
SExpr *string(const char *val);
SExpr *symbol(const char *val);
SExpr *cons(SExpr *car, SExpr *cdr);
SExpr *car(SExpr *list);
SExpr *cdr(SExpr *list);

void skipWhitespace(const char **input);

SExpr *parseNumber(const char **input);
SExpr *parseString(const char **input);
SExpr *parseSymbol(const char **input);
SExpr *parseList(const char **input);
SExpr *parseSExpr(const char **input);

SExpr *add(SExpr *a, SExpr *b);
SExpr *sub(SExpr *a, SExpr *b);
SExpr *mul(SExpr *a, SExpr *b);
SExpr *division(SExpr *a, SExpr *b);
SExpr *mod(SExpr *a, SExpr *b);
SExpr *lt(SExpr *a, SExpr *b);
SExpr *gt(SExpr *a, SExpr *b);
SExpr *lte(SExpr *a, SExpr *b);
SExpr *gte(SExpr *a, SExpr *b);
SExpr *eq(SExpr *a, SExpr *b);
SExpr *not(SExpr *a);

bool is_truthy(SExpr *sexp);
void init_symbols();
bool isNil(const char *input);
bool isNumber(const char *input);
bool isSymbol(const char *input);
bool isString(const char *input);
bool isList(const char *input);
bool isSExpr(const char *input);

SExpr *eval(SExpr *sexp, Env *env);
SExpr *eval_list(SExpr *args, Env *env);
SExpr *dispatch_builtin(const char *fn_name, SExpr *args);
SExpr *eval_lambda_call(SExpr *lambda, SExpr *call_expr, Env *env);

void printList(SExpr *s);
void printSExpr(SExpr *s);

void run();
void runTests();

// ==================== MANAGE ENVIRONMENT ====================

SExpr *sym_true;
SExpr *sym_nil;

void init_symbols()
{
    sym_true = symbol("t"); // true symbol
    sym_nil = nil();        // nil singleton from your code
}

Env *make_env(Env *parent)
{
    Env *env = malloc(sizeof(Env));
    env->symbols = nil();
    env->values = nil();
    env->parent = parent;
    return env;
}

void set(Env *env, SExpr *symbol, SExpr *value)
{
    if (!env)
    {
        fprintf(stderr, "Error: no current environment\n");
        exit(1);
    }

    // Add symbol and value to current env frame (head of lists)
    env->symbols = cons(symbol, env->symbols);
    env->values = cons(value, env->values);
}

SExpr *lookup(Env *env, SExpr *symbol)
{
    while (env)
    {
        SExpr *syms = env->symbols;
        SExpr *vals = env->values;

        while (syms && syms->type == TYPE_CONS)
        {
            SExpr *cur_sym = car(syms);
            if (cur_sym->type == TYPE_ATOM_SYMBOL && strcmp(cur_sym->string, symbol->string) == 0)
            {
                return car(vals); // return corresponding value
            }
            syms = cdr(syms);
            vals = cdr(vals);
        }

        env = env->parent;
    }
    if (symbol->type == TYPE_ATOM_SYMBOL && strcmp(symbol->string, "nil") == 0)
    {
        return nil(); // Return canonical nil for symbol "nil"
    }
    return symbol; // else return symbol itself
}

// ==================== MANAGE S-EXPRESSION ====================

bool is_truthy(SExpr *sexp)
{
    return sexp != NULL && sexp->type != TYPE_NIL;
}

SExpr *nil()
{
    static SExpr *singletonNil = NULL;

    if (singletonNil == NULL)
    {
        singletonNil = malloc(sizeof(SExpr));
        singletonNil->type = TYPE_NIL;
    }

    return singletonNil;
}

SExpr *number(double value)
{
    SExpr *a = malloc(sizeof(SExpr));
    a->type = TYPE_ATOM_NUMBER;
    a->number = value;
    return a;
}

SExpr *string(const char *val)
{
    SExpr *a = malloc(sizeof(SExpr));
    a->type = TYPE_ATOM_STRING;
    a->string = strdup(val);
    return a;
}

SExpr *symbol(const char *val)
{
    SExpr *a = malloc(sizeof(SExpr));
    a->type = TYPE_ATOM_SYMBOL;
    a->string = strdup(val);
    return a;
}

SExpr *cons(SExpr *car, SExpr *cdr)
{
    SExpr *node = malloc(sizeof(SExpr));
    node->type = TYPE_CONS;
    node->cons.car = car;
    node->cons.cdr = cdr;
    return node;
}

SExpr *car(SExpr *list)
{
    if (list == NULL || list->type != TYPE_CONS)
    {
        printf("Error: car called on non-cons\n");
        return NULL; // or makeNil()
    }
    return list->cons.car;
}

SExpr *cdr(SExpr *list)
{
    if (list == NULL || list->type != TYPE_CONS)
    {
        printf("Error: cdr called on non-cons\n");
        return NULL; // or makeNil()
    }
    return list->cons.cdr;
}

SExpr *cddr(SExpr *sexp)
{
    return cdr(cdr(sexp));
}

SExpr *cadr(SExpr *sexp)
{
    return car(cdr(sexp));
}

SExpr *caddr(SExpr *sexp)
{
    return car(cdr(cdr(sexp)));
}

SExpr *cadddr(SExpr *sexp)
{
    return car(cdr(cdr(cdr(sexp))));
}

// ==================== PARSER ====================

void skipWhitespace(const char **input)
{
    while (isspace(**input))
        (*input)++;
}

SExpr *parseNumber(const char **input)
{
    char *end;
    double val = strtod(*input, &end);
    *input = end;

    return number(val);
}

SExpr *parseQuote(const char **input)
{
    // skip the quote character
    (*input)++;
    // parse the next s-expression
    SExpr *quoted = parseSExpr(input);
    // construct (quote quoted)
    return cons(symbol("quote"), cons(quoted, nil()));
}

SExpr *parseString(const char **input)
{
    (*input)++; // skip opening "

    const char *start = *input;

    while (**input && **input != '"')
    {
        (*input)++;
    }

    int len = *input - start;
    char *buf = malloc(len + 1); // +1 for storing null bit
    strncpy(buf, start, len);
    buf[len] = '\0';

    if (**input == '"')
    {
        (*input)++; // skip closing "
    }

    SExpr *str = string(buf);

    free(buf);

    return str;
}

SExpr *parseSymbol(const char **input)
{
    const char *start = *input;

    while (**input && !isspace(**input) && **input != '(' && **input != ')')
    {
        (*input)++;
    }

    int len = *input - start;
    char *buf = malloc(len + 1); // +1 for storing null bit
    strncpy(buf, start, len);
    buf[len] = '\0';

    SExpr *sym = symbol(buf);

    free(buf);

    return sym;
}

SExpr *parseList(const char **input)
{
    (*input)++; // skip '('
    skipWhitespace(input);

    if (**input == ')')
    {
        (*input)++;
        return nil(); // empty list
    }

    SExpr *head = NULL;
    SExpr *tail = NULL;

    while (**input && **input != ')')
    {
        SExpr *elem = parseSExpr(input);
        if (!head)
        {
            head = tail = cons(elem, nil());
        }
        else
        {
            tail->cons.cdr = cons(elem, nil());
            tail = tail->cons.cdr;
        }

        skipWhitespace(input);
    }

    if (**input == ')')
    {
        (*input)++; // skip closing ')'
    }

    return head ? head : nil();
}

SExpr *parseSExpr(const char **input)
{
    skipWhitespace(input);

    if (**input == '\0')
    {
        return nil();
    }

    if (**input == '\'')
    {
        return parseQuote(input);
    }

    if (**input == '(')
    {
        return parseList(input);
    }

    if (**input == '"')
    {
        return parseString(input);
    }

    if (isdigit(**input) || **input == '-' || **input == '+')
    {
        return parseNumber(input);
    }

    return parseSymbol(input);
}

// ==================== CORE FUNCTIONALITY ====================

SExpr *add(SExpr *a, SExpr *b)
{
    if (a->type != TYPE_ATOM_NUMBER || b->type != TYPE_ATOM_NUMBER)
    {
        fprintf(stderr, "Error: add expects number atoms\n");
        exit(1);
    }

    return number(a->number + b->number);
}

SExpr *sub(SExpr *a, SExpr *b)
{
    if (a->type != TYPE_ATOM_NUMBER || b->type != TYPE_ATOM_NUMBER)
    {
        fprintf(stderr, "Error: sub expects number atoms\n");
        exit(1);
    }
    return number(a->number - b->number);
}

SExpr *mul(SExpr *a, SExpr *b)
{
    if (a->type != TYPE_ATOM_NUMBER || b->type != TYPE_ATOM_NUMBER)
    {
        fprintf(stderr, "Error: mul expects number atoms\n");
        exit(1);
    }
    return number(a->number * b->number);
}

SExpr *division(SExpr *a, SExpr *b)
{
    if (a->type != TYPE_ATOM_NUMBER || b->type != TYPE_ATOM_NUMBER)
    {
        fprintf(stderr, "Error: div expects number atoms\n");
        exit(1);
    }
    if (b->number == 0)
    {
        fprintf(stderr, "Error: division by zero\n");
        exit(1);
    }
    return number(a->number / b->number);
}

SExpr *mod(SExpr *a, SExpr *b)
{
    if (a->type != TYPE_ATOM_NUMBER || b->type != TYPE_ATOM_NUMBER)
    {
        fprintf(stderr, "Error: mod expects number atoms\n");
        exit(1);
    }
    int ia = (int)a->number;
    int ib = (int)b->number;
    if (ib == 0)
    {
        fprintf(stderr, "Error: modulus by zero\n");
        exit(1);
    }
    return number(ia % ib);
}

SExpr *lt(SExpr *a, SExpr *b)
{
    if (a->type != TYPE_ATOM_NUMBER || b->type != TYPE_ATOM_NUMBER)
    {
        fprintf(stderr, "Error: lt expects number atoms\n");
        exit(1);
    }
    return number(a->number < b->number ? 1 : 0);
}

SExpr *gt(SExpr *a, SExpr *b)
{
    if (a->type != TYPE_ATOM_NUMBER || b->type != TYPE_ATOM_NUMBER)
    {
        fprintf(stderr, "Error: gt expects number atoms\n");
        exit(1);
    }
    return number(a->number > b->number ? 1 : 0);
}

SExpr *lte(SExpr *a, SExpr *b)
{
    if (a->type != TYPE_ATOM_NUMBER || b->type != TYPE_ATOM_NUMBER)
    {
        fprintf(stderr, "Error: lte expects number atoms\n");
        exit(1);
    }
    return number(a->number <= b->number ? 1 : 0);
}

SExpr *gte(SExpr *a, SExpr *b)
{
    if (a->type != TYPE_ATOM_NUMBER || b->type != TYPE_ATOM_NUMBER)
    {
        fprintf(stderr, "Error: gte expects number atoms\n");
        exit(1);
    }
    return number(a->number >= b->number ? 1 : 0);
}

SExpr *eq(SExpr *a, SExpr *b)
{
    if (a == NULL || b == NULL)
        return sym_nil;
    if (a->type != b->type)
        return sym_nil;
    switch (a->type)
    {
    case TYPE_ATOM_NUMBER:
        return (a->number == b->number) ? sym_true : sym_nil;
    case TYPE_ATOM_STRING:
    case TYPE_ATOM_SYMBOL:
        return (strcmp(a->string, b->string) == 0) ? sym_true : sym_nil;
    case TYPE_NIL:
        return sym_true;
    default:
        return (a == b) ? sym_true : sym_nil;
    }
}

SExpr *not(SExpr *a)
{
    if (a->type != TYPE_ATOM_NUMBER)
    {
        fprintf(stderr, "Error: not expects a number atom\n");
        exit(1);
    }

    return number(a->number == 0 ? 1 : 0);
}

// ==================== PRINT ====================

void printList(SExpr *s)
{
    if (!s)
        return;

    printf("(");

    while (s && s->type == TYPE_CONS)
    {
        printSExpr(s->cons.car);
        s = s->cons.cdr;

        if (s && s->type == TYPE_CONS)
            printf(" ");
        else if (s && s->type != TYPE_NIL)
            printf(" . ");
    }

    if (s && s->type != TYPE_NIL)
    {
        printSExpr(s);
    }

    printf(")");
}

void printSExpr(SExpr *s)
{
    if (!s)
    {
        printf("()");
        return;
    }

    switch (s->type)
    {
    case TYPE_ATOM_NUMBER:
        // use %g for floating or %d for integers depending on your number type
        printf("%g", s->number);
        break;
    case TYPE_ATOM_SYMBOL:
        printf("%s", s->string);
        break;
    case TYPE_ATOM_STRING:
        printf("\"%s\"", s->string); // Print with surrounding quotes
        break;
    case TYPE_CONS:
        printList(s);
        break;
    case TYPE_NIL:
        printf("()");
        break;
    default:
        printf("<unknown>");
        break;
    }
}

//  ==================== CHECK S-EXPRESSION ====================

bool isNil(const char *input)
{
    return strcmp(input, "nil") == 0;
}

bool isNumber(const char *input)
{
    if (input == NULL || *input == '\0')
        return false;

    char *endptr;
    strtod(input, &endptr); // try to parse as number

    return *endptr == '\0'; // true if whole string was a number
}

bool isSymbol(const char *input)
{
    if (input == NULL || *input == '\0')
        return false;

    // a symbol can’t start with a digit or '(' or ')'
    if (isdigit((unsigned char)*input) || *input == '(' || *input == ')')
        return false;

    // allow letters, digits, underscores, maybe others
    for (const char *p = input; *p; p++)
    {
        if (!(isalnum((unsigned char)*p) || *p == '_' || *p == '-'))
            return false;
    }

    return true;
}

bool isString(const char *input)
{
    if (input == NULL || *input == '\0')
        return false;

    // a string must start and end with double quotes
    size_t len = strlen(input);
    if (len < 2)
        return false;

    return input[0] == '"' && input[len - 1] == '"';
}

bool isList(const char *input)
{
    if (input == NULL || *input == '\0')
        return false;

    const char *p = input;
    skipWhitespace(&p);

    // Must start with '(' and end with ')'
    size_t len = strlen(p);
    if (len < 2)
        return false;

    if (p[0] != '(' || p[len - 1] != ')')
        return false;

    // Use your existing parser to validate inner content
    SExpr *expr = parseSExpr(&p);
    if (expr == NULL)
        return false;

    skipWhitespace(&p);
    return *p == '\0';
}

bool isSExpr(const char *input)
{
    const char *p = input;
    skipWhitespace(&p);

    // Try parsing
    SExpr *expr = parseSExpr(&p);
    if (expr == NULL)
        return false;

    skipWhitespace(&p);

    // Valid only if we've consumed the whole string
    bool valid = (*p == '\0');

    return valid;
}

// Helper to recursively evaluate all arguments in a list
SExpr *eval_list(SExpr *args, Env *env)
{
    if (args->type == TYPE_NIL)
        return nil();

    SExpr *first_eval = eval(car(args), env);
    SExpr *rest_eval = eval_list(cdr(args), env);
    return cons(first_eval, rest_eval);
}

// Helper: Dispatch built-in functions by name and evaluated args
SExpr *dispatch_builtin(const char *fn_name, SExpr *args)
{
    if (strcmp(fn_name, "+") == 0 || strcmp(fn_name, "add") == 0)
        return add(car(args), car(cdr(args)));
    if (strcmp(fn_name, "-") == 0 || strcmp(fn_name, "sub") == 0)
        return sub(car(args), car(cdr(args)));
    if (strcmp(fn_name, "mul") == 0 || strcmp(fn_name, "*") == 0)
        return mul(car(args), car(cdr(args)));
    if (strcmp(fn_name, "div") == 0 || strcmp(fn_name, "/") == 0)
        return division(car(args), car(cdr(args)));
    if (strcmp(fn_name, "mod") == 0 || strcmp(fn_name, "%") == 0)
        return mod(car(args), car(cdr(args)));
    if (strcmp(fn_name, "=") == 0 || strcmp(fn_name, "eq") == 0)
        return eq(car(args), car(cdr(args)));
    if (strcmp(fn_name, "not") == 0)
        return not(car(args));
    if (strcmp(fn_name, "lt") == 0)
        return lt(car(args), car(cdr(args)));
    if (strcmp(fn_name, "lte") == 0)
        return lte(car(args), car(cdr(args)));
    if (strcmp(fn_name, "gt") == 0)
        return gt(car(args), car(cdr(args)));
    if (strcmp(fn_name, "gte") == 0)
        return gte(car(args), car(cdr(args)));
    if (strcmp(fn_name, "cons") == 0)
        return cons(car(args), car(cdr(args)));
    if (strcmp(fn_name, "car") == 0)
        return car(car(args));
    if (strcmp(fn_name, "cdr") == 0)
        return cdr(car(args));
    if (strcmp(fn_name, "cons") == 0)
        return cons(car(args), car(cdr(args)));

    return symbol("Error: unrecognized function");
}

// Helper: Evaluate a user-defined lambda function call
SExpr *eval_lambda_call(SExpr *lambda, SExpr *call_expr, Env *env)
{
    SExpr *formals = cadr(lambda);
    SExpr *body = caddr(lambda);

    SExpr *actuals = eval_list(cdr(call_expr), env);

    Env *new_env = make_env(env);
    SExpr *sym_it = formals;
    SExpr *val_it = actuals;

    while (sym_it->type == TYPE_CONS && val_it->type == TYPE_CONS)
    {
        set(new_env, car(sym_it), car(val_it));
        sym_it = cdr(sym_it);
        val_it = cdr(val_it);
    }
    // Optional: error if arg counts don't match

    SExpr *result = eval(body, new_env);

    // Optional: free new_env if needed

    return result;
}

// Main eval function
SExpr *eval(SExpr *sexp, Env *env)
{
    if (!sexp)
        return nil();

    if (sexp->type == TYPE_NIL)
        return sexp;

    if (sexp->type == TYPE_ATOM_SYMBOL)
        return lookup(env, sexp);

    if (sexp->type == TYPE_ATOM_NUMBER || sexp->type == TYPE_ATOM_STRING)
        return sexp;

    if (sexp->type == TYPE_CONS)
    {
        SExpr *fn = car(sexp);

        if (fn->type != TYPE_ATOM_SYMBOL)
            return symbol("Error: function name must be a symbol");

        // Handle special forms
        if (strcmp(fn->string, "quote") == 0)
            return cadr(sexp);

        if (strcmp(fn->string, "set") == 0)
        {
            SExpr *var = cadr(sexp);
            SExpr *val = eval(caddr(sexp), env);
            set(env, var, val);
            return val;
        }

        if (strcmp(fn->string, "define") == 0)
        {
            SExpr *name = cadr(sexp);
            SExpr *args = car(cddr(sexp));
            SExpr *body = car(cdr(cddr(sexp)));
            SExpr *lambda_sym = symbol("lambda");
            SExpr *lambda_list = cons(lambda_sym, cons(args, cons(body, nil())));
            set(env, name, lambda_list);
            return name;
        }

        if (strcmp(fn->string, "lambda") == 0)
            return sexp;

        if (strcmp(fn->string, "and") == 0)
        {
            SExpr *e1 = eval(cadr(sexp), env);
            if (!is_truthy(e1))
                return e1;
            return eval(caddr(sexp), env);
        }

        if (strcmp(fn->string, "or") == 0)
        {
            SExpr *e1 = eval(cadr(sexp), env);
            if (is_truthy(e1))
                return e1;
            return eval(caddr(sexp), env);
        }

        if (strcmp(fn->string, "if") == 0)
        {
            SExpr *test = eval(cadr(sexp), env);
            if (test->type != TYPE_NIL)
                return eval(caddr(sexp), env);
            SExpr *if_false = cadddr(sexp); // 4th element: (cdr (cdr (cdr sexp)))
            return eval(if_false, env);
        }

        if (strcmp(fn->string, "cond") == 0)
        {
            SExpr *branches = cdr(sexp);
            while (branches && branches->type == TYPE_CONS)
            {
                SExpr *pair = car(branches);
                SExpr *test_expr = car(pair);
                if (test_expr->type == TYPE_ATOM_SYMBOL && strcmp(test_expr->string, "else") == 0)
                    return eval(car(cdr(pair)), env);
                SExpr *test = eval(test_expr, env);
                if (test->type != TYPE_NIL)
                    return eval(car(cdr(pair)), env);
                branches = cdr(branches);
            }
            return nil();
        }

        // Evaluate the function symbol
        SExpr *fn_val = lookup(env, fn);

        // Lambda function call
        if (fn_val->type == TYPE_CONS && car(fn_val)->type == TYPE_ATOM_SYMBOL &&
            strcmp(car(fn_val)->string, "lambda") == 0)
        {
            return eval_lambda_call(fn_val, sexp, env);
        }

        // Built-in function call
        SExpr *evaled_args = eval_list(cdr(sexp), env);

        return dispatch_builtin(fn->string, evaled_args);
    }

    return nil();
}

//  ==================== MAIN ====================

void run()
{
    char input[1024];
    const char *ptr;

    Env *global_env = make_env(NULL);

    printf("Enter S-expression (or type 'exit' to quit):\n");

    while (true)
    {
        printf("> ");

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            break; // EOF
        }

        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n')
        {
            input[len - 1] = '\0';
        }

        if (strcmp(input, "exit") == 0)
        {
            break;
        }

        ptr = input;

        SExpr *sexpr = parseSExpr(&ptr);

        SExpr *result = eval(sexpr, global_env);

        printSExpr(result);
        printf("\n");

        // freeSExpr(sexpr);
        // freeSExpr(result);
    }

    printf("Thanks for using Yisp.\n");

    return;
}

void runTests()
{
    // Redirect stdout to a file
    FILE *f = freopen("test_results.txt", "w", stdout);
    if (!f)
    {
        perror("Failed to open test_results.txt");
        return;
    }

    TestCase tests[] = {
        // input nil number symbol string list s-expression
        {"nil", true, false, false, false, false, true},
        {"42", false, true, false, false, false, true},
        {"3.14", false, true, false, false, false, true},
        {"symbol", false, false, true, false, false, true},
        {"foo_bar-123", false, false, true, false, false, true},
        {"\"hello\"", false, false, false, true, false, true},
        {"(1 2 3)", false, false, false, false, true, true},
        {"((a b) c)", false, false, false, false, true, true},
        {"(", false, false, false, false, false, false},
        {")", false, false, false, false, false, false},
        {"1a", false, false, false, false, false, false},
        {"(1 2", false, false, false, false, false, false},
        {"(1 (2 3)", false, false, false, false, false, false},
        {"\"unterminated", false, false, false, false, false, false}};

    int n = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < n; i++)
    {
        printf("Test %2d: \"%s\"\n", i + 1, tests[i].input);

        bool resultNil = isNil(tests[i].input);
        bool resultNum = isNumber(tests[i].input);
        bool resultSym = isSymbol(tests[i].input);
        bool resultStr = isString(tests[i].input);
        bool resultList = isList(tests[i].input);
        bool resultSExpr = isSExpr(tests[i].input);

        printf("  isNil:     %s (%s)\n", resultNil ? "true" : "false",
               resultNil == tests[i].shouldBeNil ? "PASS" : "FAIL");

        printf("  isNumber:  %s (%s)\n", resultNum ? "true" : "false",
               resultNum == tests[i].shouldBeNumber ? "PASS" : "FAIL");

        printf("  isSymbol:  %s (%s)\n", resultSym ? "true" : "false",
               resultSym == tests[i].shouldBeSymbol ? "PASS" : "FAIL");

        printf("  isString:  %s (%s)\n", resultStr ? "true" : "false",
               resultStr == tests[i].shouldBeString ? "PASS" : "FAIL");

        printf("  isList:    %s (%s)\n", resultList ? "true" : "false",
               resultList == tests[i].shouldBeList ? "PASS" : "FAIL");

        printf("  isSExpr:   %s (%s)\n", resultSExpr ? "true" : "false",
               resultSExpr == tests[i].shouldBeSExpr ? "PASS" : "FAIL");

        printf("\n");
    }

    // Restore stdout back to console
    fclose(f);
    freopen("/dev/tty", "w", stdout); // on Unix/macOS
}

int main()
{
    init_symbols();

    run();

    // runTests();

    return 0;
}
