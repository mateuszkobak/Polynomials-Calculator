#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>
#include <stdbool.h>
#include "cmocka.h"

#define array_length(x) (sizeof(x) / sizeof((x)[0]))

/* Chcemy testować funkcję prywatną modułu calculator.c.
Musimy zadeklarować jej sygnaturę.
extern int divide(int a, int b); */

/* Funkcja main modułu calc_poly.c jest zamieniana na funkcję calc_poly_main. */
extern int calc_poly_main();

extern struct PolyStackElem;
extern struct Poly;
typedef struct Poly Poly;
typedef struct Poly* PolyPtr;
typedef struct PolyStackElem* Stack;
extern Stack EmptyStack();
extern void StackOpZero(Stack s);
extern void StackDestroy(Stack s);
extern void StackOpCompose(Stack s, unsigned count);
extern bool StackOpIsEq(Stack s);
extern PolyPtr PolyPtrConst(int x);
extern void StackPushPtr(Stack s, PolyPtr p);
extern PolyPtr PolyPtrVar();
extern void PolyPtrDestroy(PolyPtr p);
extern PolyPtr StackPopPtr(Stack stack);
extern bool StackIsEq(Stack s);
extern bool StackOpPop(Stack s);


static jmp_buf jmp_at_exit;
static int exit_status;

/**
 * Atrapa funkcji main
 */
int mock_main() {
    if (!setjmp(jmp_at_exit))
        return calc_poly_main();
    return exit_status;
}

/**
 * Atrapa funkcji exit
 */
void mock_exit(int status) {
    exit_status = status;
    longjmp(jmp_at_exit, 1);
}

int mock_fprintf(FILE* const file, const char *format, ...) CMOCKA_PRINTF_ATTRIBUTE(2, 3);
int mock_printf(const char *format, ...) CMOCKA_PRINTF_ATTRIBUTE(1, 2);

/**
 * Pomocnicze bufory, do których piszą atrapy funkcji printf i fprintf oraz
 * pozycje zapisu w tych buforach. Pozycja zapisu wskazuje bajt o wartości 0.
 */
static char fprintf_buffer[256];
static char printf_buffer[256];
static int fprintf_position = 0;
static int printf_position = 0;

/**
 * Atrapa funkcji fprintf sprawdzająca poprawność wypisywania na stderr.
 */
int mock_fprintf(FILE* const file, const char *format, ...) {
    int return_value;
    va_list args;

    assert_true(file == stderr);
    /* Poniższa asercja sprawdza też, czy fprintf_position jest nieujemne.
    W buforze musi zmieścić się kończący bajt o wartości 0. */
    assert_true((size_t)fprintf_position < sizeof(fprintf_buffer));

    va_start(args, format);
    return_value = vsnprintf(fprintf_buffer + fprintf_position,
                             sizeof(fprintf_buffer) - fprintf_position,
                             format,
                             args);
    va_end(args);

    fprintf_position += return_value;
    assert_true((size_t)fprintf_position < sizeof(fprintf_buffer));
    return return_value;
}

/**
 * Atrapa funkcji fprintf sprawdzająca poprawność wypisywania na stdout.
 */
int mock_printf(const char *format, ...) {
    int return_value;
    va_list args;

    /* Poniższa asercja sprawdza też, czy printf_position jest nieujemne.
    W buforze musi zmieścić się kończący bajt o wartości 0. */
    assert_true((size_t)printf_position < sizeof(printf_buffer));

    va_start(args, format);
    return_value = vsnprintf(printf_buffer + printf_position,
                             sizeof(printf_buffer) - printf_position,
                             format,
                             args);
    va_end(args);

    printf_position += return_value;
    assert_true((size_t)printf_position < sizeof(printf_buffer));
    return return_value;
}

/**
 *  Pomocniczy bufor, z którego korzystają atrapy funkcji operujących na stdin.
 */
static char input_stream_buffer[256];
static int input_stream_position = 0;
static int input_stream_end = 0;
int read_char_count;

/**
 * Atrapa funkcji scanf używana do przechwycenia czytania z stdin.
 */
int mock_scanf(const char *format, ...) {
    va_list fmt_args;
    int ret;

    va_start(fmt_args, format);
    ret = vsscanf(input_stream_buffer + input_stream_position, format, fmt_args);
    va_end(fmt_args);

    if (ret < 0) { /* ret == EOF */
        input_stream_position = input_stream_end;
    }
    else {
        assert_true(read_char_count >= 0);
        input_stream_position += read_char_count;
        if (input_stream_position > input_stream_end) {
            input_stream_position = input_stream_end;
        }
    }
    return ret;
}

/**
 * Atrapa funkcji getchar używana do przechwycenia czytania z stdin.
 */
int mock_getchar() {
    if (input_stream_position < input_stream_end)
        return input_stream_buffer[input_stream_position++];
    else
        return EOF;
}

/**
 * Atrapa funkcji ungetc.
 * Obsługiwane jest tylko standardowe wejście.
 */
int mock_ungetc(int c, FILE *stream) {
    assert_true(stream == stdin);
    if (input_stream_position > 0)
        return input_stream_buffer[--input_stream_position] = c;
    else
        return EOF;
}

/**
 * Funkcja inicjująca dane wejściowe dla programu korzystającego ze stdin.
 */
static void init_input_stream(const char *str) {
    memset(input_stream_buffer, 0, sizeof(input_stream_buffer));
    input_stream_position = 0;
    input_stream_end = strlen(str);
    assert_true((size_t)input_stream_end < sizeof(input_stream_buffer));
    strcpy(input_stream_buffer, str);
}



static void test_poly_zero1(void **state) {
    Stack stack = *(Stack *) state;
    StackOpZero(stack);
    StackOpZero(stack);
    StackOpCompose(stack, 0);
    assert_true(StackIsEq(stack));
    StackOpPop(stack);
    StackOpPop(stack);

    *(Stack *)state = stack;
}

static void test_poly_zero2(void **state) {
    Stack stack = *(Stack *) state;

    PolyPtr p = PolyPtrConst(42);
    StackOpZero(stack);
    StackPushPtr(stack, p);
    StackOpZero(stack);
    StackOpCompose(stack, 1);
    assert_true(StackOpIsEq(stack));
    StackOpPop(stack);
    StackOpPop(stack);
    free(p);

    *(Stack *)state = stack;
}


static void test_poly_const1(void **state) {
    Stack stack = *(Stack *) state;

    PolyPtr p1 = PolyPtrConst(42);
    PolyPtr p2 = PolyPtrConst(42);
    StackPushPtr(stack, p1);
    StackOpCompose(stack, 0);
    StackPushPtr(stack, p1);
    assert_true(StackIsEq(stack));
    StackOpPop(stack);
    StackOpPop(stack);
    free(p1);
    free(p2);
}

static void test_poly_const2(void **state) {
    Stack stack = *(Stack *) state;

    PolyPtr p1 = PolyPtrConst(42);
    PolyPtr p2 = PolyPtrConst(24);
    StackPushPtr(stack, p1);
    StackPushPtr(stack, p2);
    StackOpCompose(stack, 1);
    StackPushPtr(stack, p2);
    assert_true(StackIsEq(stack));
    StackOpPop(stack);
    StackOpPop(stack);
    free(p1);
    free(p2);
}

static void test_poly_var1(void **state) {
    Stack stack = *(Stack *) state;

    PolyPtr p1 = PolyPtrVar();
    StackPushPtr(stack, p1);
    StackOpCompose(stack, 0);
    StackOpZero(stack);
    assert_true(StackIsEq(stack));
    StackOpPop(stack);
    StackOpPop(stack);
    free(p1);
}

static void test_poly_var2(void **state) {
    Stack stack = *(Stack *) state;

    PolyPtr p1 = PolyPtrConst(42);
    PolyPtr p2 = PolyPtrVar();
    PolyPtr p3 = PolyPtrConst(42);
    StackPushPtr(stack, p1);
    StackPushPtr(stack, p2);
    StackOpCompose(stack, 1);
    StackPushPtr(stack, p3);
    assert_true(StackIsEq(stack));
    StackOpPop(stack);
    StackOpPop(stack);
    free(p1);
    free(p2);
    free(p3);
}


static void test_poly_var3(void **state) {
    Stack stack = *(Stack *) state;

    PolyPtr p1 = PolyPtrVar();
    PolyPtr p2 = PolyPtrVar();
    PolyPtr p3 = PolyPtrVar();
    StackPushPtr(stack, p1);
    StackPushPtr(stack, p2);
    StackOpCompose(stack, 1);
    StackPushPtr(stack, p3);
    assert_true(StackIsEq(stack));
    StackOpPop(stack);
    StackOpPop(stack);
    free(p1);
    free(p2);
    free(p3);
}

static void test_no_parameter(void **state) {
    (void)state;

    init_input_stream("0\nCOMPOSE\n");
    assert_int_equal(mock_main(), 0);
    assert_string_equal(fprintf_buffer, "ERROR 2 WRONG COUNT\n");
}

static void test_min_value(void **state) {
    (void) state;

    init_input_stream("4\n"
                              "(3,0)+(1,1)\n"
                              "COMPOSE 0\n"
                              "PRINT\n");
    assert_int_equal(mock_main(), 0);
    assert_string_equal(fprintf_buffer, "");
}

static void test_max_value(void **state) {
    (void) state;

    init_input_stream("COMPOSE 4294967295\n");
    assert_int_equal(mock_main(), 0);
    assert_string_equal(fprintf_buffer, "ERROR 1 STACK UNDERFLOW\n");
}

static void test_under_min_value(void **state) {
    (void) state;

    init_input_stream("COMPOSE -1\n");
    assert_int_equal(mock_main(), 0);
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

static void test_over_max_value(void **state) {
    (void) state;

    init_input_stream("COMPOSE 4294967296\n");
    assert_int_equal(mock_main(), 0);
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

static void test_very_big_value(void **state) {
    (void) state;

    init_input_stream("COMPOSE 9999999999999999999\n");
    assert_int_equal(mock_main(), 0);
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

static void test_letters(void **state) {
    (void) state;

    init_input_stream("COMPOSE EDVFERVRF\n");
    assert_int_equal(mock_main(), 0);
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}


static void test_letters_and_digits(void **state) {
    (void) state;

    init_input_stream("COMPOSE 9FCWER\n");
    assert_int_equal(mock_main(), 0);
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

static int test_setup_parse(void **state) {
    (void)state;
    memset(fprintf_buffer, 0, sizeof(fprintf_buffer));
    memset(printf_buffer, 0, sizeof(printf_buffer));
    printf_position = 0;
    fprintf_position = 0;

    /* Zwrócenie zera oznacza sukces. */
    return 0;
}

static int test_setup(void **state) {
    (void)state;

    Stack stack = EmptyStack();
    *state = stack;


    memset(fprintf_buffer, 0, sizeof(fprintf_buffer));
    memset(printf_buffer, 0, sizeof(printf_buffer));
    printf_position = 0;
    fprintf_position = 0;

    /* Zwrócenie zera oznacza sukces. */
    return 0;
}

static int test_teardown(void **state) {
    StackDestroy( *(Stack *)state);
}

int main(int args, char** argc) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_poly_zero1),
            cmocka_unit_test(test_poly_zero2),
            cmocka_unit_test(test_poly_const1),
            cmocka_unit_test(test_poly_const2),
            cmocka_unit_test(test_poly_var1),
            cmocka_unit_test(test_poly_var2),
            cmocka_unit_test(test_poly_var3),
            cmocka_unit_test_setup(test_no_parameter, test_setup_parse),
            cmocka_unit_test_setup(test_min_value, test_setup_parse),
            cmocka_unit_test_setup(test_max_value, test_setup_parse),
            cmocka_unit_test_setup(test_under_min_value, test_setup_parse),
            cmocka_unit_test_setup(test_over_max_value, test_setup_parse),
            cmocka_unit_test_setup(test_very_big_value, test_setup_parse),
            cmocka_unit_test_setup(test_letters, test_setup_parse),
            cmocka_unit_test_setup(test_letters_and_digits, test_setup_parse)
    };

    return cmocka_run_group_tests(tests, test_setup, test_teardown);

}
