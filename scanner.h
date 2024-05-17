typedef enum {
    LEFT_PAREN,
    RIGHT_PAREN,
    WHITESPACE,
    QUOTE,
    DIGIT,
    CHAR,
} TokenType;

typedef struct {
    TokenType type;
    char value;
} Token;

int scan(Token* buffer, int count);
