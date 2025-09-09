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

bool isNil(const char *input);
bool isNumber(const char *input);
bool isSymbol(const char *input);
bool isString(const char *input);
bool isList(const char *input);
bool isSExpr(const char *input);

void printList(SExpr *s);
void printSExpr(SExpr *s);

void run();
void runTests();

// ==================== MANAGE S-EXPRESSION ====================

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

SExpr *parseString(const char **input)
{
    (*input)++; // skip opening "

    const char *start = *input;

    while (**input && **input != '"')
        (*input)++;

    int len = *input - start;
    char *buf = malloc(len + 1); // +1 for storing null bit
    strncpy(buf, start, len);
    buf[len] = '\0';

    if (**input == '"')
        (*input)++; // skip closing "

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
        (*input)++; // skip closing ')'

    return head ? head : nil();
}

SExpr *parseSExpr(const char **input)
{
    skipWhitespace(input);

    if (**input == '\0')
        return nil();

    if (**input == '(')
        return parseList(input);

    if (**input == '"')
        return parseString(input);

    if (isdigit(**input) || **input == '-' || **input == '+')
        return parseNumber(input);

    return parseSymbol(input);
}

// ==================== PRINT ====================

void printList(SExpr *s)
{
    if (!s)
    {
        return;
    }

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
    switch (s->type)
    {
    case TYPE_ATOM_NUMBER:
        printf("%g", s->number);
        break;
    case TYPE_ATOM_SYMBOL:
    case TYPE_ATOM_STRING:
        printf("%s", s->string);
        break;
    case TYPE_CONS:
        printList(s);
        break;
    case TYPE_NIL:
        printf("()"); // optional
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

//  ==================== MAIN ====================

void run()
{
    const char *input = "((a (7 8 9) x) (b (4 5 6) y))";
    const char *ptr = input;

    SExpr *sexpr = parseSExpr(&ptr);

    printf("Parsed S-Expression:\n");
    printSExpr(sexpr);
    printf("\n");
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
        // input           nil    number  symbol  string  list   s-expression
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
    runTests();
    return 0;
}
