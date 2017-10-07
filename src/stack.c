/** @file
 *  Implementacja stosu i operacji na nim
 */

#include <printf.h>
#include "stack.h"
#include "parse.h"
#include <stdio.h>
#include "utils.h"


MonoStack EmptyMonoStack() {
    MonoStack m = malloc(sizeof(MonoStackElem));
    m->next = NULL;
    return m;
}


Stack EmptyStack() {
    Stack p = malloc(sizeof(PolyStackElem));
    p->next = NULL;
    return p;
}



void StackPush(Stack stack, Poly p) {
    Stack pom = malloc(sizeof(PolyStackElem));
    pom->elem = stack->elem;
    pom->next = stack->next;
    stack->elem = p;
    stack->next = pom;
}


void MonoStackPush(MonoStack stack, Mono m) {
    MonoStack result = malloc(sizeof(MonoStackElem));
    result->elem = stack->elem;
    result->next = stack->next;
    stack->next = result;
    stack->elem = m;
}


bool IsNonEmptyStack(Stack stack) {
    if (stack->next == NULL) {
        return false;
    }
    else return true;
}


bool HasTwoStack(Stack stack) {
    if (!IsNonEmptyStack(stack)) return false;
    if (stack->next->next == NULL) {
        return false;
    }
    else return true;
}

/**
 * Sprawdza czy stos ma count elementów.
 * @param stack
 * @param count
 * @return
 */
bool StackHasElements(Stack stack, unsigned count) {
    if (count == 1) return IsNonEmptyStack(stack);
    if (!IsNonEmptyStack(stack)) return false;
    return StackHasElements(stack->next, count - 1);
}


void StackDestroy(Stack stack) {
    while (stack->next != NULL) {
        Poly p = StackPop(stack);
        PolyDestroy(&p);
    }
    free(stack);
}


void MonoStackDestroy(MonoStack stack) {
    while (stack->next != NULL) {
        Mono m = MonoStackPop(stack);
        MonoDestroy(&m);
    }
    free(stack);
}


void MonoStackSoftDestroy(MonoStack stack) {
    while (stack->next != NULL) {
        MonoStackPop(stack);
    }
    free(stack);
}


Poly StackPop(Stack stack) {
    Poly result = stack->elem;
    stack->elem = stack->next->elem;
    Stack pom = stack->next;
    stack->next = stack->next->next;
    free(pom);
    return result;
}


Mono MonoStackPop(MonoStack stack) {
    Mono result = stack->elem;
    stack->elem = stack->next->elem;
    MonoStack pom = stack->next;
    stack->next = stack->next->next;
    free(pom);
    return result;
}


Poly AddMonoStack(MonoStack monoStack, unsigned count) {
    Mono monos[count];

    for (unsigned i = 0; i < count; i++) {
        monos[i] = MonoStackPop(monoStack);
    }

    return PolyAddMonos(count, monos);
}


Poly StackView(Stack stack) {
    return stack->elem;
}


Poly StackViewSecond(Stack stack){
    return stack->next->elem;
}


void StackOpZero(Stack stack) {
    StackPush(stack, PolyFromCoeff(0));
}


bool StackOpIsCoeff(Stack stack) {
    bool b = IsNonEmptyStack(stack);
    if (b) {
        Poly p = StackView(stack);
        if (PolyIsCoeff(&p)) printf("1\n");
        else printf("0\n");
    }
    return b;
}


bool StackOpIsZero(Stack stack) {
    bool b = IsNonEmptyStack(stack);
    if (b) {
        Poly p = StackView(stack);
        if (PolyIsZero(&p)) printf("1\n");
        else printf("0\n");
    }
    return b;
}


bool StackOpClone(Stack stack) {
    bool b = IsNonEmptyStack(stack);
    if(b){
        Poly p = StackView(stack);
        StackPush(stack, PolyClone(&p));
    }
    return b;
}


bool StackOpAdd(Stack stack) {
    bool b = HasTwoStack(stack);
    if(b) {
      Poly p = StackPop(stack);
      Poly q = StackPop(stack);
      StackPush(stack, PolyAdd(&p, &q));
      PolyDestroy(&p);
      PolyDestroy(&q);
    }
    return b;
}


bool StackOpMul(Stack stack) {
    bool b = HasTwoStack(stack);
    if (b) {
        Poly p = StackPop(stack);
        Poly q = StackPop(stack);
        StackPush(stack, PolyMul(&p, &q));
        PolyDestroy(&p);
        PolyDestroy(&q);
    }
    return b;
}


bool StackOpNeg(Stack stack) {
    bool b = IsNonEmptyStack(stack);
    if (b) {
        Poly p = StackPop(stack);
        StackPush(stack, PolyNeg(&p));
        PolyDestroy(&p);
    }
    return b;
}


bool StackOpSub(Stack stack) {
    bool b = HasTwoStack(stack);
    if (b) {
        Poly p = StackPop(stack);
        Poly q = StackPop(stack);
        StackPush(stack, PolySub(&p, &q));
        PolyDestroy(&p);
        PolyDestroy(&q);
    }
    return b;
}


bool StackOpIsEq(Stack stack) {
    bool b = HasTwoStack(stack);
    if(b) {
        Poly p = StackView(stack);
        Poly q = StackViewSecond(stack);
        if (PolyIsEq(&p, &q)) printf("1\n");
        else printf("0\n");
    }
    return b;
}


bool StackOpDeg(Stack stack) {
    bool b = IsNonEmptyStack(stack);
    if(b) {
        Poly p = StackView(stack);
        int deg = PolyDeg(&p);
        printf("%d\n", deg);
    }
    return b;
}


bool StackOpDegBy(Stack stack, unsigned index) {
    bool b = IsNonEmptyStack(stack);
    if(b) {
        Poly p = StackView(stack);
        int deg = PolyDegBy(&p, index);
        printf("%d\n", deg);
    }
    return b;
}


bool StackOpAt(Stack stack, poly_coeff_t x) {
    bool b = IsNonEmptyStack(stack);
    if(b) {
        Poly p = StackPop(stack);
        StackPush(stack, PolyAt(&p, x));
        PolyDestroy(&p);
    }
    return b;
}


bool StackOpPop(Stack stack) {
    bool b = IsNonEmptyStack(stack);
    if(b) {
        Poly p = StackPop(stack);
        PolyDestroy(&p);
    }
    return b;
}


bool StackOpPrint(Stack stack) {
    bool b = IsNonEmptyStack(stack);
    if(b) {
        Poly p = StackView(stack);
        PrintPoly(&p);
        putchar('\n');
    }
    return b;
}


bool StackOpCompose(Stack stack, unsigned count) {
    bool b = StackHasElements(stack, count + 1);
    if(b) {
        Poly p = StackPop(stack);
        Poly x[count];
        for (unsigned i = 0; i < count; i++) {
            x[i] = StackPop(stack);
        }
        StackPush(stack, PolyCompose(&p, count, x));

        for (unsigned i = 0; i < count; i++) {
            PolyDestroy(&x[i]);
        }
        PolyDestroy(&p);
    }
    return b;
}
