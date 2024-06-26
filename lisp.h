typedef enum {
    ATOM,
    LIST,
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

Data makeInt(int x);
Data makeSymbol(char* s);
Data makeString(char* s);
Data makeQuote(List* list);
Data datify(List* list);
List* cons(Data data, List* list);
void ppData(Data data);
void ppList(List* list);
