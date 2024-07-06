#include "stddef.h"
#include "string.h"
#include "stdio.h"
#include "stdbool.h"

#include "lisp.h"
#include "scanner.h"
#include "parser.h"

typedef Data (*Proc)(List* list);

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
    return data.type == ATOM && ((Atom*)data.content)->tag == ATOM_TAG_PROC;
}

Data eval(Data data) {
    switch (data.type) {
        case ATOM: {
            Atom* atom = (Atom*)data.content;
            switch (atom->tag) {
                case ATOM_TAG_INT:
                case ATOM_TAG_STRING:
                return data;
                case ATOM_TAG_SYMBOL: {
                    void* proc = getProcForSymbol(SYMBOL_TABLE, (char*)atom->content);
                    if (proc != NULL) {
                        return makeProc(proc);
                    }
                    return data;
                }
                case ATOM_TAG_QUOTE:
                // todo:
            }
        }
        case LIST: {
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
    }
}

int main() {
    #define MAX 100
    Token tokens[MAX] = {0};
    int count = scan(tokens, MAX);
    // ppTokens(tokens, count);

    unsigned idx = 0;
    Data data = parse(tokens, count, &idx);
    // ppData(data);
    data = reverseData(data);
    data = eval(data);
    ppData(data);
}
