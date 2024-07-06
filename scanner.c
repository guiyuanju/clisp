#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"

#include "format.h"
#include "scanner.h"

bool isDigit(char c) {
    return '0' <= c && c <= '9';
}

bool isBlank(char c) {
    return c == ' ' || c == '\n' || c == '\t';
}

bool isChar(char c) {
    return !isDigit(c) && !isBlank(c);
}

bool isParen(char c) {
    return c == '(' || c == ')';
}

bool isDoubleQuote(char c) {
    return c == '"';
}

bool isQuote(char c) {
    return c == '\'';
}

bool isReserved(char c) {
    return isParen(c) || isDoubleQuote(c) || isQuote(c);
}

unsigned charToDigit(char c) {
    return c - '0';
}

bool isEnd(char* raw, int idx) {
    return raw[idx] == '\0';
}

bool scanNumber(char* raw, unsigned* idx, int* number) {
    int res = 0;
    int curIdx = *idx;

    while (!isEnd(raw, curIdx) && isDigit(raw[curIdx])) {
        res = res * 10 + charToDigit(raw[curIdx++]);
    }

    *idx = curIdx;
    *number = res;

    return true;
}

bool scanSymbol(char* raw, unsigned* idx, char** symbolAddr) {
    int curIdx = *idx;
    int length = 0;

    while (!isEnd(raw, curIdx + length)
        && (isChar(raw[curIdx + length]) || isDigit(raw[curIdx + length]))
        && !isReserved(raw[curIdx + length])) {
        length++;
    }

    *symbolAddr = (char*)malloc(sizeof(char) * length);
    memcpy(*symbolAddr, raw + curIdx, length);
    *idx += length;

    return true;
}

bool scanString(char* raw, unsigned* idx, char** stringAddr) {
    int curIdx = *idx;
    int length = 0;

    while (!isEnd(raw, curIdx + length) && raw[curIdx + length] != '"') {
        length++;
    }

    if (raw[curIdx +length] != '"') {
        return false;
    }

    *stringAddr = (char*)malloc(sizeof(char) * length);
    memcpy(*stringAddr, raw + curIdx, length);
    *idx += length + 1;

    return true;
}

bool tokenize(char* raw, Token* tokens, unsigned* tokenCount) {
    if (raw == NULL || tokens == NULL) {
        return false;
    }

    unsigned curCount = 0;
    Token curToken;

    unsigned leftParenCount = 0;

    for (unsigned i = 0; !isEnd(raw, i) && curCount < *tokenCount;) {
        switch (raw[i]) {
            case '(':
                curToken.type = TOKENTYPE_LEFT_PAREN;
                curToken.value.single = '(';
                tokens[curCount++] = curToken;
                i++;
                leftParenCount++;
                break;
            case ')':
                curToken.type = TOKENTYPE_RIGHT_PAREN;
                curToken.value.single = ')';
                tokens[curCount++] = curToken;
                i++;
                if (leftParenCount == 0) {
                    return false;
                }
                leftParenCount--;
                break;
            case '\'':
                curToken.type = TOKENTYPE_QUOTE;
                curToken.value.single = '\'';
                tokens[curCount++] = curToken;
                i++;
                break;
            case '"':
                curToken.type = TOKENTYPE_STRING;
                i++; // skip the opening "
                if (!scanString(raw, &i, &curToken.value.string)) {
                    *tokenCount = curCount - 1;
                    return false;
                }
                tokens[curCount++] = curToken;
                break;
            case ' ':
            case '\t':
            case '\n':
                i++;
                break;
            default:
                if (isDigit(raw[i])) {
                    curToken.type = TOKENTYPE_NUMBER;
                    scanNumber(raw, &i, &curToken.value.number);
                    tokens[curCount++] = curToken;
                    break;
                }
                if (isChar(raw[i])) {
                    curToken.type = TOKENTYPE_SYMBOL;
                    scanSymbol(raw, &i, &curToken.value.symbol);
                    tokens[curCount++] = curToken;
                    break;
                }

                *tokenCount = curCount - 1;
                return false;
        }
    }

    if (leftParenCount != 0) {
        return false;
    }

    *tokenCount = curCount;
    return true;
}

unsigned scan(Token* buffer, unsigned count) {
    char* line = (char*)malloc(sizeof(char) * count);

    if (fgets(line, count, stdin) != NULL) {
        count = strlen(line);
        if (!tokenize(line, buffer, &count)) {
            count = 0;
            printf("syntax error!\n");
        }
    }

    free(line);
    return count;
}

void ppToken(Token token) {
    switch (token.type) {
        case TOKENTYPE_STRING:
            printf("\"%s\"", token.value.string);
            break;
        case TOKENTYPE_NUMBER:
            printf("%d", token.value.number);
            break;
        case TOKENTYPE_SYMBOL:
            printf("%s", token.value.symbol);
            break;
        default:
            printf("%c", token.value.single);
            break;
    }
}

void ppTokens(Token* tokens, int count) {
    for (int i = 0; i < count; i++) {
        ppToken(tokens[i]);
        space();
    }
}
