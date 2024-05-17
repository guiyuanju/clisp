#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "format.h"
#include "scanner.h"

typedef struct {
    char c;
    Token token;
} Pair;

Token tokenize(char c) {
   Pair pair[] = {
       {'(', {LEFT_PAREN, '('}},
       {')', {RIGHT_PAREN, ')'}},
       {' ', {WHITESPACE, ' '}},
       {'\n', {WHITESPACE, ' '}},
       {'\'', {QUOTE, '\''}},
   };

   for (int i = 0; i < sizeof(pair); i++) {
       if (pair[i].c == c) {
           return pair[i].token;
       }
   }

   // digit
   if ('0' <= c && c <= '9') {
       Token token = {DIGIT, c};
       return token;
   }

   // char
   Token token = {CHAR, c};
   return token;
}

int scan(Token* buffer, int count) {
    char* line = (char*)malloc(sizeof(char) * count);
    int i = 0;

    if (fgets(line, count, stdin) != NULL) {
        for (; i < count && line[i] != 0; i++) {
            buffer[i] = tokenize(line[i]);
        }
    }

    free(line);
    return i;
}

void ppToken(Token token) {
    printf("{ type: %d, value: '%c'}", token.type, token.value);
}

void ppTokens(Token* tokens, int count) {
    for (int i = 0; i < count; i++) {
        ppToken(tokens[i]);
        space();
    }
}
