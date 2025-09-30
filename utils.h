#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdarg.h>
#include "sexpr.h"

static void append_to_buffer(char *buf, size_t size, size_t *pos, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int written = vsnprintf(buf + *pos, size > *pos ? size - *pos : 0, fmt, args);
    va_end(args);

    if (written > 0 && (size_t)written < size - *pos)
    {
        *pos += (size_t)written;
    }
    else if (written > 0)
    {
        *pos = size - 1;
    }
}

static void sexp_to_string_internal(SExpr *sexp, char *buf, size_t size, size_t *pos)
{
    if (!sexp)
    {
        append_to_buffer(buf, size, pos, "()");
        return;
    }

    switch (sexp->type)
    {
    case TYPE_ATOM_NUMBER:
        append_to_buffer(buf, size, pos, "%g", sexp->number);
        break;

    case TYPE_ATOM_SYMBOL:
        append_to_buffer(buf, size, pos, "%s", sexp->string);
        break;

    case TYPE_ATOM_STRING:
        append_to_buffer(buf, size, pos, "\"%s\"", sexp->string);
        break;

    case TYPE_NIL:
        append_to_buffer(buf, size, pos, "()");
        break;

    case TYPE_CONS:
    {
        append_to_buffer(buf, size, pos, "(");
        SExpr *cur = sexp;
        while (cur && cur->type == TYPE_CONS)
        {
            sexp_to_string_internal(cur->cons.car, buf, size, pos);
            cur = cur->cons.cdr;
            if (cur && cur->type == TYPE_CONS)
            {
                append_to_buffer(buf, size, pos, " ");
            }
        }
        if (cur && cur->type != TYPE_NIL)
        {
            append_to_buffer(buf, size, pos, " . ");
            sexp_to_string_internal(cur, buf, size, pos);
        }
        append_to_buffer(buf, size, pos, ")");
        break;
    }

    default:
        append_to_buffer(buf, size, pos, "<unknown>");
        break;
    }
}

void sexp_to_string(SExpr *sexp, char *buf, size_t size)
{
    size_t pos = 0;
    if (size == 0)
        return;
    sexp_to_string_internal(sexp, buf, size, &pos);
    buf[pos < size ? pos : size - 1] = '\0'; // Null-terminate safely
}


#endif // UTILS_H