#define TEST
#include "clisp.c"

#define assert(exp) if (!(exp)) {\
    printf("assertion failed (%d): %s\n", __LINE__, #exp);\
}

int main() {
    assert(1 == 2);
}