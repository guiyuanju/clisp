typedef enum {
    ATOM,
    LIST,
    ERR_INFO,
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

Data makeInt(int x);
Data makeSymbol(char* s);
Data makeString(char* s);
Data makeQuote(Data list);
Data makeProc(void* proc);
Data makeErrInfo(char* erfInfo);
Data datify(List* list);
List* cons(Data data, List* list);
Data car(List* list);
List* cdr(List* list);
bool isEmptyList(List* list);
Data reverseData(Data data);
void ppData(Data data);
void ppList(List* list);
