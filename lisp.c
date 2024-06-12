#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "format.h"
#include "lisp.h"

Data makeInt(int x) {
    Atom* atom = (Atom*)malloc(sizeof(Atom));
    atom->tag = ATOM_TAG_INT;
    atom->content = malloc(sizeof(int));
    memcpy(atom->content, &x, sizeof(int));

    Data data = {ATOM, atom};

    return data;
}

Data makeString(char* s) {
    Atom* atom = (Atom*)malloc(sizeof(Atom));
    atom->tag = ATOM_TAG_STRING;
    atom->content = malloc(strlen(s));
    memcpy(atom->content, s, strlen(s));

    Data data = {ATOM, atom};

    return data;
}

Data makeSymbol(char* s) {
    Atom* atom = (Atom*)malloc(sizeof(Atom));
    atom->tag = ATOM_TAG_SYMBOL;
    atom->content = malloc(strlen(s));
    memcpy(atom->content, s, strlen(s));

    Data data = {ATOM, atom};

    return data;
}

Data makeQuote(Data data) {
    Atom* atom = (Atom*)malloc(sizeof(Atom));
    atom->tag = ATOM_TAG_QUOTE;
    atom->content = malloc(sizeof(Data));
    memcpy(atom->content, &data, sizeof(Data));

    Data data = {ATOM, atom};
    
    return data;
}

Data datify(List* list) {
    Data data = {LIST, list};

    return data;
}

List* cons(Data data, List* list) {
    List* res = (List*)malloc(sizeof(List));
    res->data = data;
    res->next = list;

    return res;
}

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
        default:
            printf("unrecognizable type %d\n", atom->tag);
    }
}

void ppData(Data data);

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
        case ATOM:
            ppAtom(data.content);
            break;
        case LIST:
            ppList(data.content);
            break;
        default:
            break;
    }
}

// int main() {
//     Data four = makeInt(4);
//     Data two = makeInt(2);
//     List* sublist = cons(two, cons(four, cons(four, Empty)));
//     List* list = cons(datify(sublist), cons(four, Empty));
//     ppList(list);
//     newline();
// }
