#include <stdio.h>
#include "sexpr.h"
#include "utils.h"
#include "tests.h"

void run(FILE *input_file);

void run(FILE *input_file)
{
    Env *global_env = make_env(NULL);

    if (input_file == stdin)
    {
        printf("Enter S-expression (or type 'exit' to quit):\n");

        char input[1024];
        const char *ptr;
        while (true)
        {
            printf("> ");

            if (fgets(input, sizeof(input), input_file) == NULL)
                break;

            size_t len = strlen(input);
            if (len > 0 && input[len - 1] == '\n')
                input[len - 1] = '\0';

            if (strcmp(input, "exit") == 0)
                break;

            ptr = input;
            SExpr *sexpr = parseSExpr(&ptr);
            SExpr *result = eval(sexpr, global_env);

            printSExpr(result);
            printf("\n");
        }

        printf("Thanks for using Yisp.\n");
    }
    else
    {
        fseek(input_file, 0, SEEK_END);
        long size = ftell(input_file);
        fseek(input_file, 0, SEEK_SET);

        char *buffer = malloc(size + 1);
        fread(buffer, 1, size, input_file);
        buffer[size] = '\0';

        const char *ptr = buffer;
        SExpr *sexpr = parseSExpr(&ptr);
        SExpr *result = eval(sexpr, global_env);

        printSExpr(result);
        printf("\n");

        free(buffer);
    }
}

s
