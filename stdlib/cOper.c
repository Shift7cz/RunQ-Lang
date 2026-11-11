#include <stdio.h>

int cPutc(int c) {
    return putchar(c);
}

int cGetc() {
    return getchar();
}

int cNewLine() {
    return putchar('\n'); // todo: move this to RunQ
}