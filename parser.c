#include "scanner.h"
#include "lisp.h"
#include "stddef.h"
#include "stdio.h"

List* parse(Token* tokens, unsigned count, unsigned* idx) {
    if (*idx == count) {
        return NULL;
    }

    Token cur = tokens[*idx];
    switch (cur.type) {
        case TOKENTYPE_LEFT_PAREN:
            (*idx)++;
            return cons(datify(parse(tokens, count, idx)), parse(tokens, count, idx));
        case TOKENTYPE_RIGHT_PAREN:
            (*idx)++;
            return NULL;
        case TOKENTYPE_QUOTE:
            (*idx)++;
            makeQuote()
            return NULL;
        case TOKENTYPE_STRING:
            (*idx)++;
            return cons(makeString(cur.value.string), parse(tokens, count, idx));
        case TOKENTYPE_NUMBER:
            (*idx)++;
            return cons(makeInt(cur.value.number), parse(tokens, count, idx));
        case TOKENTYPE_SYMBOL:
            (*idx)++;
            return cons(makeSymbol(cur.value.symbol), parse(tokens, count, idx));
        default:
            (*idx)++;
            return NULL;
    }

    return NULL;
}

int main() {
    #define MAX 100
    Token tokens[MAX] = {0};
    int count = scan(tokens, MAX);
    // ppTokens(tokens, count);

    unsigned idx = 0;
    List* list = parse(tokens, count, &idx);
    ppList(list);
}
