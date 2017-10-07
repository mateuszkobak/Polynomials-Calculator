/** @file
 *  Interfejs parsowania wielomianów i komend
 */

#ifndef WIELOMIANY_PARSE_H
#define WIELOMIANY_PARSE_H

#include "poly.h"
#include "stack.h"
#include <stdbool.h>


/**
 * Sprawdza czy podany warunek zachodzi
 * @param b warunek
 * @return czy zachodzi
 */
bool CheckFailed(bool b);


/**
 * Wypisuje, błąd wpisanej komendy
 * @param row wiersz z pliku wejścia
 */
void WrongCommand(int row);


/**
 * Posunięcie sie do przodu w parsowaniu wejścia
 * @param curr wskaźnik na numer aktualnej kolumny
 * @param column  wskaźnik na aktualny wczytany znak
 */
void StepForward(int *curr, int *column);


/**
 * Parsuje współczynnik wielomanu
 * @param curr wskaźnik na numer aktualnej kolumny
 * @param column wskaźnik na aktualny wczytany znak
 * @param failed wskaźnik na boola "czy pasowanie się nie powiodło?"
 * @return współczynnik wielomianu
 */
poly_coeff_t ParsePolyCoeff(int *curr, int *column, bool *failed);


/**
 * Parsuje wielomian
 * @param curr wskaźnik na numer aktualnej kolumny
 * @param column wskaźnik na aktualny wczytany znak
 * @param failed wskaźnik na boola "czy pasowanie się nie powiodło?"
 * @return wielomian
 */
Poly ParsePoly(int *curr, int *column, bool *failed);


/**
 * Parsuje unsigned int
 * @param curr wskaźnik na numer aktualnej kolumny
 * @param column wskaźnik na aktualny wczytany znak
 * @param failed wskaźnik na boola "czy pasowanie się nie powiodło?"
 * @return unsigned int
 */
unsigned ParseUnsigned(int *curr, int *column, bool *failed);


/**
 * Sprawdza czy znak jest cyfrą
 * @param x znak w kodzie ASCII
 * @return czy @x jest cyfrą
 */
bool IsDigit(int x);


/**
 * Tłumaczy kod ASCII cyfry na liczbę
 * @param x cyfra w kodzie ASCII
 * @return liczba
 */
poly_exp_t GetDigit(int x);


/**
 * Sprawdza czy znak jest literą
 * @param x znak
 * @return czy @x jest literą
 */
bool IsLetter(int x);


/**
 * Parsuje komendę
 * @param curr wskaźnik na numer aktualnej kolumny
 * @param column wskaźnik na aktualny wczytany znak
 * @param failed wskaźnik na boola "czy pasowanie się nie powiodło?"
 * @return komenda w postaci stringa
 */
char *ParseCommand(int *curr, int *column, int row);


/**
 * Wypisuje na stdout wielomian w postaci standardowej
 * @param p wielomian
 */
void PrintPoly(Poly *p);

#endif //WIELOMIANY_PARSE_H
