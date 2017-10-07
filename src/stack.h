/** @file
 * Interfejs stosu
 */

#ifndef WIELOMIANY_STACK_H
#define WIELOMIANY_STACK_H

#include "poly.h"


/**
 * Struktura elementu stosu wielomianów
 *
 */
typedef struct PolyStackElem {
    Poly elem; ///< wartość w tym elemencie
    struct PolyStackElem *next; ///< wskaźnik na następny element
} PolyStackElem;


/**
 * Stos jest wskaźnikiem na pierwszy element stosu
 */
typedef PolyStackElem* Stack;


/**
 * Struktura elementu stosu jednomianów
 *
 */
typedef struct MonoStackElem {
    Mono elem; ///< wartość w tym elemencie
    struct MonoStackElem *next; ///< wskaźnik na następny element
} MonoStackElem;


/**
 * Stos jest wskaźnikiem na pierwszy element stosu
 */
typedef MonoStackElem* MonoStack;


/**
 * Zwraca pusty stos jednomianów
 * @return pusty stos jednomianów
 */
MonoStack EmptyMonoStack();


/**
 * Zwraca pusty stos wielomianów
 * @return pusty stos wielomianów
 */
Stack EmptyStack();


/**
 * Usuwa z pamięci stos wielomianów wraz z wartościami
 * @param stack
 */
void StackDestroy(Stack stack);


/**
 * Usuwa z pamięci stos jednomianów wraz z wartościami
 * @param stack
 */
void MonoStackDestroy(MonoStack stack);


/**
 * Usuwa z pamięci stos jednomianów nie usuwając wartości
 * @param stack
 */
void MonoStackSoftDestroy(MonoStack stack);


/**
 * Dodaje wielomian na wierzch stosu
 * @param stack stos wielomianów
 * @param p wielomian dodawany
 */
void StackPush(Stack stack, Poly p);


/**
 * Dodaje wielomian na wierzch stosu
 * @param stack stos jednomianów
 * @param m jednomian dodawany
 */
void MonoStackPush(MonoStack stack, Mono m);


/**
 * Sprawdza czy stos jest niepusty
 * @param stack stos wielomianów
 * @return czy stos @stack jest niepusty
 */
bool IsNonEmptyStack(Stack stack);


/**
 * Sprawdza czy stos ma co najmniej dwa elementy
 * @param stack stos wielomianów
 * @return czy stos @stack ma co najmniej dwa elementy
 */
bool HasTwoStack(Stack stack);


/**
 * Usuwa wielomian na wierzchu stosu i ją zwraca
 * @param stack stos wielomianów
 * @return wielomian na wierzchu stosu
 */
Poly StackPop(Stack stack);


/**
 * Usuwa jednomian na wierzchu stosu i ją zwraca
 * @param stack stos wielomianów
 * @return jednomian na wierzchu stosu
 */
Mono MonoStackPop(MonoStack stack);


/**
 * Tworzy wielomian będący sumą jednomianów na stosie
 * @param monoStack stos jednomianów
 * @param count liczba jednomianów
 * @return wielomian będący sumą jednomianów na stosie @monoStack
 */
Poly AddMonoStack(MonoStack monoStack, unsigned count);


/**
 * Zwraca wielomian na wierzchu stosu
 * @param stack stos wielomianów
 * @return wielomian na wierzchu stosu @stack
 */
Poly StackView(Stack stack);


/**
 * Zwraca drugi wielomian co do kolejności na stosie
 * @param stack stos wielomianów
 * @return drugi wielomian co do kolejności na stosie @stack
 */
Poly StackViewSecond(Stack stack);


/**
 * Dodaje zero na stos
 * @param stack stos wielomianów
 */
void StackOpZero(Stack stack);


/**
 * Wypisuje na stdout czy wielomian na wierzchu stosu
 * jest stały (1 jeśli tak, 0 jeśli nie)
 * @param stack stos wielomianów
 * @return Czy operacja się powiodła
 */
bool StackOpIsCoeff(Stack stack);


/**
 * Wypisuje na stdout czy wielomian na wierzchu stosu
 * jest zerowy (1 jeśli tak, 0 jeśli nie)
 * @param stack stos wielomianów
 * @return Czy operacja się powiodła
 */
bool StackOpIsZero(Stack stack);


/**
 * Klonuje wielomian na wierzchu stosu i odkłąda go na wierzch
 * @param stack stos wielomianów
 * @return Czy operacja się powiodła
 */
bool StackOpClone(Stack stack);


/**
 * Usuwa dwa wielomianu z wierzchu stosu i odkłada ich sumę
 * @param stack stos wielomianów
 * @return Czy operacja się powiodła
 */
bool StackOpAdd(Stack stack);


/**
 * Usuwa dwa wielomianu z wierzchu stosu i odkłada ich iloczyn
 * @param stack stos wielomianów
 * @return Czy operacja się powiodła
 */
bool StackOpMul(Stack stack);


/**
 * Zamienia wielommian na woerzchu stosu na wielomian przeciwny
 * @param stack stos wielomianów
 * @return Czy operacja się powiodła
 */
bool StackOpNeg(Stack stack);


/**
 * Usuwa dwa wielomianu z wierzchu stosu i odkłada ich różnicę
 * @param stack stos wielomianów
 * @return Czy operacja się powiodła
 */
bool StackOpSub(Stack stack);


/**
 * Wypisuje na stdout czy dwa wielomiany na wierzchu stosu
 * są sobie równe (1 jeśli tak, 0 jeśli nie)
 * @param stack stos wielomianów
 * @return Czy operacja się powiodła
 */
bool StackOpIsEq(Stack stack);


/**
 * Wypisuje na stdout stopień wielomianu na wierzchu stosu
 * @param stack stos wielomianów
 * @return Czy operacja się powiodła
 */
bool StackOpDeg(Stack stack);


/**
 * Wypisuje na stdout stopień wielomianu na wierzchu stosu
 * wg podanej zmiennej
 * @param stack stos wielomianów
 * @param index indeks zmiennej
 * @return Czy operacja się powiodła
 */
bool StackOpDegBy(Stack stack, unsigned index);


/**
 * Zamienia wielomian na wierzchu stosu na wielomian
 * z podstawioną wartością za zmienną x_0
 * @param stack stos wielomianów
 * @param x wartość podstawiana za zmienną
 * @return Czy operacja się powiodła
 */
bool StackOpAt(Stack stack, poly_coeff_t x);


/**
 * Wypisuje wielomian na stdout w standardowej formie
 * @param stack stos wielomianów
 * @return Czy operacja się powiodłą
 */
bool StackOpPrint(Stack stack);


/**
 * Usuwa wielomain z wierzchu stosu
 * @param stack stos wielomianów
 * @return Czy operacja się powiodła
 */
bool StackOpPop(Stack stack);


/**
 * Komponuje pierwszy wielomian ze stosu z następnymi count wielomianami
 * @param stack stos wielomianów
 * @param count liczba wielomianów do skomponowania
 * @return Czy operacja się powiodła
 */
bool StackOpCompose(Stack stack, unsigned count);


#endif //WIELOMIANY_STACK_H
