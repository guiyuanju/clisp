#include "scanner.h"
#include "lisp.h"
#include "stddef.h"
#include "stdio.h"
#include "parser.h"

Data parseNumber(Token* tokens, unsigned* idx) {
    return makeInt(tokens[(*idx)++].value.number);
}

Data parseSymbol(Token* tokens, unsigned* idx) {
    return makeSymbol(tokens[(*idx)++].value.symbol);
}

Data parseString(Token* tokens, unsigned* idx) {
    return makeString(tokens[(*idx)++].value.string);
}

Data parseQuote(Token* tokens, unsigned count, unsigned* idx) {
    (*idx)++;
    return makeQuote(parse(tokens, count, idx));
}

Data parseList(Token* tokens, unsigned count, unsigned* idx) {
    List* res = NULL;

    (*idx)++;
    while (*idx < count && tokens[*idx].type != TOKENTYPE_RIGHT_PAREN) {
        res = cons(parse(tokens, count, idx), res);
    }
    (*idx)++;

    return datify(res);
}

Data parse(Token* tokens, unsigned count, unsigned* idx) {
    Token curToken = tokens[*idx];
    switch (curToken.type) {
        case TOKENTYPE_LEFT_PAREN:
        return parseList(tokens, count, idx);
        case TOKENTYPE_QUOTE:
        return parseQuote(tokens, count, idx);
        case TOKENTYPE_STRING:
        return parseString(tokens, idx);
        case TOKENTYPE_NUMBER:
        return parseNumber(tokens, idx);
        case TOKENTYPE_SYMBOL:
        return parseSymbol(tokens, idx);
    }
}

// int main() {
//     #define MAX 100
//     Token tokens[MAX] = {0};
//     int count = scan(tokens, MAX);
//     // ppTokens(tokens, count);

//     unsigned idx = 0;
//     Data data = parse(tokens, count, &idx);
//     // ppData(data);
//     data = reverseData(data);
//     ppData(data);
// }
