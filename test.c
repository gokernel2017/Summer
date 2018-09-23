
#include <stdio.h>

typedef struct {
    void  *target;
    int   offsetX;
    int   offsetY;
    int   which;
}EVENT;

void add (void *a, int b, int c) {

}

void funcao (EVENT *p) {
    add (p->target, p->offsetX, p->offsetY);
}

int main (void) {
    return 0;
}
