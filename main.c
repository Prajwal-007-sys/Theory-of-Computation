#include <stdio.h>
#include <stdlib.h>
#include "turing_machine.h"

int main(void) {
    TuringMachine *tm = malloc(sizeof(TuringMachine));
    if (!tm) { fprintf(stderr, "OOM\n"); return 1; }

    /* ── Test 1: Binary Increment ── */
    printf("=== Binary Increment (1011 + 1) ===\n");
    tm_setup_binary_increment(tm);
    tm_load_input(tm, "1011");
    tm_save_snapshot(tm);
    tm_run(tm);
    tm_print_tape(tm);
    printf("Steps: %d | Snapshots: %d\n\n", tm->step_count, tm->history_len);

    /* ── Test 2: Palindrome ── */
    printf("=== Palindrome: 'abba' ===\n");
    tm_setup_palindrome_checker(tm, "abba");
    tm_run(tm);
    printf("Result: %s | Steps: %d\n\n",
           tm->accepted ? "ACCEPTED (palindrome)" : "REJECTED", tm->step_count);

    printf("=== Palindrome: 'abab' ===\n");
    tm_setup_palindrome_checker(tm, "abab");
    tm_run(tm);
    printf("Result: %s | Steps: %d\n\n",
           tm->accepted ? "ACCEPTED (palindrome)" : "REJECTED", tm->step_count);

    /* ── Test 3: Unary Addition ── */
    printf("=== Unary Addition: 1110 11 (3+2=5) ===\n");
    tm_setup_unary_addition(tm, "111011");
    tm_run(tm);
    tm_print_tape(tm);
    printf("Steps: %d\n\n", tm->step_count);

    free(tm);
    return 0;
}
