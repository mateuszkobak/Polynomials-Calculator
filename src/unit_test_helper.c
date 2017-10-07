

#include "stack.h"
#include <stdlib.h>
#include "utils.h"

void StackPushPtr(Stack stack, Poly *p) {
    StackPush(stack, *p);
}

Poly* StackPopPtr(Stack stack) {
    Poly *p = malloc(sizeof(Poly));
    *p = StackPop(stack);
    return p;
}

Poly* PolyPtrConst(int x) {
    Poly *p = malloc(sizeof(Poly));
    *p = PolyFromCoeff(x);
    return p;
}

Poly* PolyPtrVar() {
    Mono *m = malloc(sizeof(Mono));
    Poly one = PolyFromCoeff(1);
    m[0] = MonoFromPoly(&one, 1);
    Poly *p = malloc(sizeof(Poly));
    p->monos_count = 1;
    p->constant_value = 1;
    p->monos = m;
    return p;
}

void PolyPtrDestroy(Poly *p) {
    PolyDestroy(p);
    free(p);
}

bool StackIsEq(Stack stack) {
    Poly p = StackView(stack);
    Poly q = StackViewSecond(stack);
    if (PolyIsEq(&p, &q)) return true;
    else return false;
}