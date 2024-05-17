#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "format.h"

typedef enum {
    INT,
    SYMBOL,
} AtomTag;

typedef struct {
    AtomTag tag;
    void* content;
} Atom;

typedef enum {
    ATOM,
    LIST,
} DataType;

typedef struct {
    DataType type;
    void* content;
} Data;

typedef struct List {
    Data data;
    struct List* next;
} List;

Data makeInt(int x) {
    Atom* atom = (Atom*)malloc(sizeof(Atom));
    atom->tag = INT;
    atom->content = malloc(sizeof(int));
    memcpy(atom->content, &x, sizeof(int));

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
        case INT:
            printf("%d", *(int*)atom->content);
            return;
        case SYMBOL:
            printf("%s", (char*)atom->content);
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

#define Empty NULL

int main() {
    Data four = makeInt(4);
    Data two = makeInt(2);
    List* sublist = cons(two, cons(four, cons(four, Empty)));
    List* list = cons(datify(sublist), cons(four, Empty));
    ppList(list);
    newline();
}
