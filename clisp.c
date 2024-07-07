#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"

/*
Data = Atom | List | ErrInfo
Atom = Number | String | Symbol | Quote | Proc
List = List Data | Null
*/

// # Utils
void newline() {
    printf("\n");
}

void space() {
    printf(" ");
}

// # Model
typedef enum {
    DATA_TYPE_ATOM,
    DATA_TYPE_LIST,
    DATA_TYPE_ERR_INFO,
} DataType;

typedef struct {
    DataType type;
    void* content;
} Data;

typedef enum {
    ATOM_TAG_INT,
    ATOM_TAG_SYMBOL,
    ATOM_TAG_STRING,
    ATOM_TAG_QUOTE,
    ATOM_TAG_PROC,
} AtomTag;

typedef struct {
    AtomTag tag;
    void* content;
} Atom;

typedef struct List {
    Data data;
    struct List* next;
} List;

#define Empty NULL

typedef Data (*Proc)(List* list);

Data makeInt(int x) {
    Atom* atom = (Atom*)malloc(sizeof(Atom));
    atom->tag = ATOM_TAG_INT;
    atom->content = malloc(sizeof(int));
    memcpy(atom->content, &x, sizeof(int));

    Data data = {DATA_TYPE_ATOM, atom};

    return data;
}

Data makeString(char* s) {
    Atom* atom = (Atom*)malloc(sizeof(Atom));
    atom->tag = ATOM_TAG_STRING;
    atom->content = malloc(strlen(s));
    memcpy(atom->content, s, strlen(s));

    Data data = {DATA_TYPE_ATOM, atom};

    return data;
}

Data makeSymbol(char* s) {
    Atom* atom = (Atom*)malloc(sizeof(Atom));
    atom->tag = ATOM_TAG_SYMBOL;
    atom->content = malloc(strlen(s));
    memcpy(atom->content, s, strlen(s));

    Data data = {DATA_TYPE_ATOM, atom};

    return data;
}

Data makeQuote(Data q) {
    Atom* atom = (Atom*)malloc(sizeof(Atom));
    atom->tag = ATOM_TAG_QUOTE;
    atom->content = malloc(sizeof(Data));
    memcpy(atom->content, &q, sizeof(Data));

    Data data = {DATA_TYPE_ATOM, atom};
    
    return data;
}

Data makeProc(void* proc) {
    Atom* atom = (Atom*)malloc(sizeof(Atom));
    atom->tag = ATOM_TAG_PROC;
    atom->content = malloc(sizeof(void*));
    memcpy(atom->content, &proc, sizeof(void*));

    Data data = {DATA_TYPE_ATOM, atom};

    return data;
}

Data makeErrInfo(char* errInfo) {
    Data data = {DATA_TYPE_ERR_INFO, errInfo};
    return data;
}

Data datify(List* list) {
    Data data = {DATA_TYPE_LIST, list};

    return data;
}

List* cons(Data data, List* list) {
    List* res = (List*)malloc(sizeof(List));
    res->data = data;
    res->next = list;

    return res;
}

Data car(List* list) {
    return list->data;
}

List* cdr(List* list) {
    return list->next;
}

bool isEmptyList(List* list) {
    return list == NULL;
}

bool isListInQuote(Data data) {
    if (data.type != DATA_TYPE_ATOM) {
        return false;
    }

    Atom* atom = (Atom*)data.content;

    if (atom->tag != ATOM_TAG_QUOTE) {
        return false;
    }
    
    Data* innerData = (Data*)atom->content;

    return innerData->type == DATA_TYPE_LIST;
}

List* reverse(List* list);

Data reverseData(Data data) {
    switch (data.type) {
        case DATA_TYPE_ATOM: {
            Atom* atom = (Atom*)data.content;
            if (atom->tag == ATOM_TAG_QUOTE) {
                return makeQuote(reverseData(*(Data*)atom->content));
            }
            break;
        }
        case DATA_TYPE_LIST:
            data.content = reverse((List*)data.content);
            break;
        default:
            break;
    }

    return data;
}

List* reverse(List* list) {
    if (isEmptyList(list)) {
        return list;
    }

    // reverse head
    list->data = reverseData(list->data);

    if (list->next == NULL) {
        return list;
    }

    List* tmp = cdr(list);

    // reverse tail
    List* reversed = reverse(cdr(list));

    // put back head
    list->next = NULL;
    tmp->next = list;

    return reversed;
}

void ppData(Data data);

void ppAtom(Atom* atom) {
    if (atom == NULL) {
        printf("error: null atom\n");
    }

    switch (atom->tag) {
        case ATOM_TAG_INT:
        printf("%d", *(int*)atom->content);
        return;
        case ATOM_TAG_SYMBOL:
        printf("%s", (char*)atom->content);
        return;
        case ATOM_TAG_STRING:
        printf("\"%s\"", (char*)atom->content);
        return;
        case ATOM_TAG_QUOTE:
        printf("'");
        ppData(*(Data*)atom->content);
        return;
        case ATOM_TAG_PROC:
        printf("#proc<%p>", *(Proc*)atom->content);
        return;
        default:
        printf("unrecognizable type %d\n", atom->tag);
    }
}

void ppList(List* list) {
    printf("(");
    while (list != NULL) {
        ppData(list->data);
        list = list->next;
        if (list != NULL) {
            space();
        }
    }
    printf(")");
}

void ppData(Data data) {
    switch (data.type) {
        case DATA_TYPE_ATOM:
            ppAtom(data.content);
            break;
        case DATA_TYPE_LIST:
            ppList(data.content);
            break;
        case DATA_TYPE_ERR_INFO:
            printf("error: %s\n", (char*)data.content);
            break;
        default:
            break;
    }
}

// # Scanner
typedef enum {
    TOKENTYPE_LEFT_PAREN,
    TOKENTYPE_RIGHT_PAREN,
    TOKENTYPE_QUOTE,
    TOKENTYPE_STRING,
    TOKENTYPE_NUMBER,
    TOKENTYPE_SYMBOL,
} TokenType;

typedef struct {
    TokenType type;
    union {
        int number;
        char* symbol;
        char* string;
        char single;
    } value;
} Token;

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

// # Parser
Data parse(Token* tokens, unsigned count, unsigned* idx);

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
        default:
            // right paren will never be parsed
            return makeErrInfo("parse error: invalid right paren.");
    }
}

// # Evaluator
Data plus(List* list) {
    int res = 0;
    while (list != NULL) {
        int cur = *(int*)(((Atom*)(list->data.content))->content);
        res += cur;
        list = list->next;
    }
    return makeInt(res);
}

Data minus(List* list) {
    int res = 0;
    int count = 0;
    while (list != NULL) {
        int cur = *(int*)(((Atom*)(list->data.content))->content);
        if (count == 0) {
            res = cur;
        } else {
            res -= cur;
        }
        count++;
        list = list->next;
    }
    if (count == 1) {
        res = -res;
    }
    return makeInt(res);
}

Data multiply(List* list) {
    int res = 1;
    while (list != NULL) {
        int cur = *(int*)(((Atom*)(list->data.content))->content);
        res *= cur;
        list = list->next;
    }
    return makeInt(res);
}

Data divide(List* list) {
    int res = 0;
    int count = 0;
    while (list != NULL) {
        int cur = *(int*)(((Atom*)(list->data.content))->content);
        if (count == 0) {
            res = cur;
        } else {
            res /= cur;
        }
        count++;
        list = list->next;
    }
    if (count <= 1) {
        return makeErrInfo("evaluation failed, division requires at least 2 operands.");
    }
    return makeInt(res);
}

typedef Data (*SpecialForm)(List* list);

Data specialFormIf(List* list) {
    Data condition = car(list);
    Data trueBranch = car(cdr(list));
    Data falseBranch = car(cdr(cdr(list)));

    return condition;
}

typedef struct {
    char* symbol;
    Proc proc;
} Binding;

typedef struct {
    int count;
    Binding* bindings;
} SymbolTable;

Binding bindings[] = {
    {"+", plus},
    {"-", minus},
    {"*", multiply},
    {"/", divide},
};

SymbolTable SYMBOL_TABLE = {sizeof(bindings), bindings};

void* getProcForSymbol(SymbolTable st, char* symbol) {
    for (int i = 0; i < st.count; i++) {
        if (strcmp(st.bindings[i].symbol, symbol) == 0) {
            return st.bindings[i].proc;
        }
    }

    return NULL;
}

bool isProc(Data data) {
    return data.type == DATA_TYPE_ATOM && ((Atom*)data.content)->tag == ATOM_TAG_PROC;
}

Data eval(Data data) {
    switch (data.type) {
        case DATA_TYPE_ATOM: {
            Atom* atom = (Atom*)data.content;
            switch (atom->tag) {
                case ATOM_TAG_INT:
                case ATOM_TAG_STRING:
                case ATOM_TAG_PROC:
                    return data;
                case ATOM_TAG_SYMBOL: {
                    void* proc = getProcForSymbol(SYMBOL_TABLE, (char*)atom->content);
                    if (proc != NULL) {
                        return makeProc(proc);
                    }
                    return data;
                }
                case ATOM_TAG_QUOTE:
                    return *(Data*)atom->content;
            }
        }
        case DATA_TYPE_LIST: {
            List* list = (List*)data.content;

            // empty
            if (isEmptyList(list)) {
                return data;
            }

            // proc
            Data proc = eval(car(list));
            if (isProc(proc)) {
                List* tail = cdr(list);
                while (tail != NULL) {
                    tail->data = eval(tail->data);
                    tail = tail->next;
                }
                return ((*(Proc*)(((Atom*)(proc.content))->content)))(cdr(list));
            }

            return makeErrInfo("evaluation failed, op is not a proc.");
        }
        default:
            return data;
    }
}

// # Main
int main() {
    #define MAX 100
    Token tokens[MAX] = {0};
    unsigned idx = 0;
    ppData(eval(reverseData(parse(tokens, scan(tokens, MAX), &idx))));
}
