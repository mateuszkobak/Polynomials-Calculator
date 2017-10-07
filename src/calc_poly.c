/** @file
 *  Kalkulator wielomianów
 *      @author Jakub Pawlewicz <pan@mimuw.edu.pl>, Mateusz Kobak <mk385760@students.mimuw.edu.pl>
 *      @copyright Uniwersytet Warszawski
 *       @date 2017-05-13
 */

#include <stdlib.h>
#include "parse.h"
#include "stack.h"
#include <stdio.h>
#include <string.h>
#include "utils.h"

/**
 * Wykonuje podaną komendę
 * @param command nazwa komendy
 * @param stack stos
 * @param curr wskaźnik aktualnie wczytany znak, gdyby coś jeszcze trzeba było wczytać
 * @param column wskaźnik na numer kolumny
 * @param row numer wiersza
 */
void ExectuteCommand(char *command, Stack stack, int *curr, int *column, int row) {

    bool commad_ok = true;
    bool wrongCommand = false;

    if (*curr == '\n') {
        if (strcmp(command, "ZERO") == 0) {
            StackOpZero(stack);
        } else if (strcmp(command, "IS_COEFF") == 0) {
            commad_ok = StackOpIsCoeff(stack);
        } else if (strcmp(command, "IS_ZERO") == 0) {
            commad_ok = StackOpIsZero(stack);
        } else if (strcmp(command, "CLONE") == 0) {
            commad_ok = StackOpClone(stack);
        } else if (strcmp(command, "ADD") == 0) {
            commad_ok = StackOpAdd(stack);
        } else if (strcmp(command, "MUL") == 0) {
            commad_ok = StackOpMul(stack);
        } else if (strcmp(command, "NEG") == 0) {
            commad_ok = StackOpNeg(stack);
        } else if (strcmp(command, "SUB") == 0) {
            commad_ok = StackOpSub(stack);
        } else if (strcmp(command, "IS_EQ") == 0) {
            commad_ok = StackOpIsEq(stack);
        } else if (strcmp(command, "DEG") == 0) {
            commad_ok = StackOpDeg(stack);
        } else if (strcmp(command, "PRINT") == 0) {
            commad_ok = StackOpPrint(stack);
        } else if (strcmp(command, "POP") == 0) {
            commad_ok = StackOpPop(stack);
        } else {
            wrongCommand = true;
        }
    }

    if (strcmp(command, "DEG_BY") == 0) {
        bool failed = false;
        failed = failed || (*curr != ' ');
        if (!failed) {
            StepForward(curr, column);
            unsigned index = ParseUnsigned(curr, column, &failed);
            failed = failed || (*curr != '\n');
            if (!failed) commad_ok = StackOpDegBy(stack, index);
        }
        if (failed) {
            fprintf(stderr, "ERROR %d WRONG INDEX\n", row);
        }
    }
    else if (strcmp(command, "COMPOSE") == 0) {
        bool failed = false;
        failed = failed || (*curr != ' ');
        if (!failed) {
            StepForward(curr, column);
            unsigned x = ParseUnsigned(curr, column, &failed);
            failed = failed || (*curr != '\n');
            if (!failed) commad_ok = StackOpCompose(stack, x);
        }
        if (failed) {
            fprintf(stderr, "ERROR %d WRONG COUNT\n", row);
        }
    }
    else if (strcmp(command, "AT") == 0) {
        bool failed = false;
        failed = failed || (*curr != ' ');
        if (!failed) {
            StepForward(curr, column);
            poly_coeff_t x = ParsePolyCoeff(curr, column, &failed);
            failed = failed || (*curr != '\n');
            if (!failed) commad_ok = StackOpAt(stack, x);
        }
        if (failed) {
            fprintf(stderr, "ERROR %d WRONG VALUE\n", row);
        }

    } else if (wrongCommand == true){
        WrongCommand(row);
    }

    if (!commad_ok) {
        fprintf(stderr, "ERROR %d STACK UNDERFLOW\n", row);
    }

    free(command);
}


int main() {

    Stack stack = EmptyStack();

    int a = 48, b = -1;
    int *curr = &a;
    int *column = &b;
    int row = 0;
    *curr = getchar();


    while (*curr != EOF) {
        *column = 1;
        row++;
        bool failed = false;
        if (IsLetter(*curr)) {
            ExectuteCommand(ParseCommand(curr, column, row), stack, curr, column, row);
        }
        else {
            Poly p = ParsePoly(curr, column, &failed);
            failed = failed || CheckFailed(*curr == '\n');
            if (failed) {
                fprintf(stderr, "ERROR %d %d\n", row, *column);
            }
            else {
                StackPush(stack, p);
            }
        }

        while(*curr != '\n') {
            *curr = getchar();
            if (*curr == EOF) break;
        }
        *curr = getchar();
    }

    StackDestroy(stack);
}