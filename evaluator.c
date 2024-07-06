#include "stddef.h"
#include "string.h"
#include "stdio.h"
#include "stdbool.h"

#include "lisp.h"
#include "scanner.h"
#include "parser.h"

int plus(int a, int b) {
    return a + b;
}

typedef struct {
    char* symbol;
    void* proc;
} Binding;

typedef struct {
    int count;
    Binding* bindings;
} SymbolTable;

Binding bindings[] = {{"+", plus}};

SymbolTable SYMBOL_TABLE = {1, bindings};

void* getProcForSymbol(SymbolTable st, char* symbol) {
    for (int i = 0; i < st.count; i++) {
        if (strcmp(st.bindings[i].symbol, symbol) == 0) {
            return st.bindings[i].proc;
        }
    }

    return NULL;
}

bool isProc(Data data) {

}

Data eval(Data data) {
    switch (data.type) {
        case ATOM:
        Atom* atom = (Atom*)data.content;
        switch (atom->tag) {
            case ATOM_TAG_INT:
            case ATOM_TAG_STRING:
            return data;
            case ATOM_TAG_SYMBOL:
            void* proc = getProcForSymbol(SYMBOL_TABLE, (char*)atom->content);
            if (proc != NULL) {
                return makeProc(proc);
            }
            return data;
            case ATOM_TAG_QUOTE:
            // todo:
        }
        case LIST:
        List* list = (List*)data.content;
        Data proc = eval(car(list));
        if (proc.type == ATOM || ((Atom*)proc.content)->tag == ATOM_TAG_PROC) {

        }
        return data;
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
