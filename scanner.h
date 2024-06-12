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

unsigned scan(Token* buffer, unsigned count);
void ppToken(Token token);
void ppTokens(Token* tokens, int count);
