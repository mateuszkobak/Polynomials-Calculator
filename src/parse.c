/** @file
 * Implementacja parsowania
 */

#include <stdlib.h>
#include <limits.h>
#include "stack.h"
#include "parse.h"
#include <stdio.h>
#include "utils.h"

#define MAX_COMMAND_LENGTH 20


/**
 * Zmienna globalna przekazywana, gdy funkcja wykryła bład
 */
Poly fake_poly = (Poly) {.constant_value = 0, .monos_count = 0, .monos = NULL};


bool CheckFailed(bool b) {
    return !b;
}


void WrongCommand(int row) {
    fprintf(stderr, "ERROR %d WRONG COMMAND\n", row);
}


void StepForward(int *curr, int *column) {
    *curr = getchar();
    *column = *column + 1;
}


/**
 * Parsuje liczbę całkowitą na wykładnik w jednomianie
 * @param curr wskaźnik na numer aktualnej kolumny
 * @param column wskaźnik na aktualny wczytany znak
 * @param failed wskaźnik na boola "czy pasowanie się nie powiodło?"
 * @return liczba całkowita
 */
poly_exp_t ParseInt(int *curr, int *column, bool *failed) {
    poly_exp_t exp = 0;

    if (!IsDigit(*curr)) {
        *failed = true;
        return 0;
    }

    if (*curr == '0') {
        StepForward(curr, column);
        return 0;
    }

    while ( IsDigit(*curr) ) {
        *failed = CheckFailed(UINT_MAX / (exp + 2) >= 9);
        exp = 10 * exp + GetDigit(*curr);
        *failed = *failed || CheckFailed(exp > 0);
        if (*failed)
            return 0;
        StepForward(curr, column);
    }

    return exp;
}


/**
 * Parsuje jednomian
 * @param curr wskaźnik na numer aktualnej kolumny
 * @param column wskaźnik na aktualny wczytany znak
 * @param failed wskaźnik na boola "czy pasowanie się nie powiodło?"
 * @return jednomian
 */
Mono ParseMono(int *curr, int *column, bool *failed) {
    *failed = CheckFailed(*curr == '(');
    if (*failed)
        return MonoFromPoly(&fake_poly, 0);
    StepForward(curr, column);

    Mono m;
    m.p = ParsePoly(curr, column, failed);
    if (*failed) {
        PolyDestroy(&m.p);
        return MonoFromPoly(&fake_poly, 0);
    }

    *failed = *failed || CheckFailed(*curr == ',');
    if (*failed) {
        PolyDestroy(&m.p);
        return MonoFromPoly(&fake_poly, 0);
    }

    StepForward(curr, column);

    m.exp = ParseInt(curr, column, failed);
    if (*failed) {
        PolyDestroy(&m.p);
        return MonoFromPoly(&fake_poly, 0);
    }

    *failed = *failed || CheckFailed(*curr == ')');
    if (*failed) {
        PolyDestroy(&m.p);
        return MonoFromPoly(&fake_poly, 0);
    }

    StepForward(curr, column);

    return m;
}


Poly ParsePoly(int *curr, int *column, bool *failed) {
    if (IsDigit(*curr) || *curr == '-') {
        return PolyFromCoeff(ParsePolyCoeff(curr, column, failed));
    }
    else {
        MonoStack monoStack = EmptyMonoStack();
        unsigned count = 0;
        do {
            if (count != 0) {
                *failed = CheckFailed(*curr == '+');
                if (*failed) {
                    MonoStackDestroy(monoStack);
                    return fake_poly;
                }
                StepForward(curr, column);
            }

            MonoStackPush(monoStack, ParseMono(curr, column, failed));
            if (*failed) {
                MonoStackDestroy(monoStack);
                return fake_poly;
            }
            count++;
        } while (*curr != ',' && *curr != '\n');

        Poly p = AddMonoStack(monoStack, count);
        MonoStackSoftDestroy(monoStack);
        return p;
    }
}


poly_coeff_t ParsePolyCoeff(int *curr, int *column, bool *failed) {
    poly_coeff_t coeff = 0;
    int sign = -1;

    if (*curr == '-') {
        sign = 1;
        StepForward(curr, column);
    } else {
        if (!IsDigit(*curr)) {
            *failed = true;
            return 0;
        }
    }
    if (*curr == '0') {
        StepForward(curr, column);
        return 0;
    }

    while ( IsDigit(*curr) ) {
        *failed = CheckFailed(LONG_MIN / (coeff - 2) >= 9);
        coeff = 10 * coeff - GetDigit(*curr);
        *failed = *failed || CheckFailed(coeff < 0);
        *failed = *failed || (coeff == LONG_MIN && sign == -1);
        if (*failed)
            return 0;
        StepForward(curr, column);
    }

    return coeff * sign;
}


unsigned ParseUnsigned(int *curr, int *column, bool *failed) {
    unsigned index = 0;

    if (!IsDigit(*curr)) {
        *failed = true;
        return 0;
    }

    if (*curr == '0') {
        StepForward(curr, column);
        return 0;
    }

    while ( IsDigit(*curr) ) {
        *failed = CheckFailed(UINT_MAX / (index + 2) >= 9);
        index = 10 * index + GetDigit(*curr);
        *failed = *failed || CheckFailed(index > 0);
        if (*failed)
            return 0;
        StepForward(curr, column);
    }

    return index;
}


bool IsDigit(int x) {
    if (x >= 48 && x <= 57) return true;
    else return false;
}


poly_exp_t GetDigit(int x) {
    return x - 48;
}


bool IsLetter(int x) {
    if (x >= 'A' && x <= 'Z') return true; //TODO note small leters
    return false;
}


char *ParseCommand(int *curr, int *column, int row) {
    char *command = malloc(20 * sizeof(char));
    int i = 0;

    while (*curr != ' ' && *curr != '\n' && i < MAX_COMMAND_LENGTH - 1) {
        if (i >= MAX_COMMAND_LENGTH) WrongCommand(row);
        command[i] = (char) *curr;
        StepForward(curr, column);
        i++;
    }

    command[i] = '\0';

    return command;
}


void PrintPoly(Poly *p) {
    if (PolyIsCoeff(p)) PrintPolyCoeff(p);
    else {
        for (unsigned i = 0; i < p->monos_count; i++) {
            PrintMono(&p->monos[i]);
            if (i != p->monos_count - 1) {
                putchar('+');
            }
        }
    }
}


/**
 * Wypisuje na stdout wielomain stały
 * @param p wielomiman stały
 */
void PrintPolyCoeff(Poly *p) {
    printf("%ld", p->constant_value);
}


/**
 * Wypisuje wykładnik jednomianu na stdout
 * @param exp wykładnik jednomianu
 */
void PrintExp(poly_exp_t exp) {
    printf("%d", exp);
}


/**
 * Wypisuje na stdout jednomian
 * @param m jednomian
 */
void PrintMono(Mono *m) {
    putchar('(');
    PrintPoly(&m->p);
    putchar(',');
    PrintExp(m->exp);
    putchar(')');
}


