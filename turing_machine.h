#ifndef TURING_MACHINE_H
#define TURING_MACHINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STATES      64
#define MAX_SYMBOLS     16
#define TAPE_SIZE       1024
#define TAPE_CENTER     512
#define MAX_TRANSITIONS 256
#define MAX_STEPS       10000

/* Direction the head moves */
typedef enum {
    DIR_LEFT  = -1,
    DIR_STAY  =  0,
    DIR_RIGHT =  1
} Direction;

/* A single transition rule:
   (current_state, read_symbol) -> (next_state, write_symbol, direction) */
typedef struct {
    int       from_state;
    char      read_sym;
    int       to_state;
    char      write_sym;
    Direction dir;
} Transition;

/* Snapshot of the machine at one step, for visualisation */
typedef struct {
    int   step;
    int   head;          /* absolute tape index */
    int   state;
    char  tape[TAPE_SIZE];
    int   tape_start;    /* leftmost non-blank used index */
    int   tape_end;      /* rightmost non-blank used index */
    int   halted;
    int   accepted;
} Snapshot;

/* The Turing Machine */
typedef struct {
    /* Configuration */
    int        num_states;
    int        num_symbols;
    char       symbols[MAX_SYMBOLS];
    char       blank;
    int        initial_state;
    int        accept_state;
    int        reject_state;
    Transition transitions[MAX_TRANSITIONS];
    int        num_transitions;

    /* Runtime state */
    char       tape[TAPE_SIZE];
    int        head;
    int        current_state;
    int        step_count;
    int        halted;
    int        accepted;

    /* History for playback */
    Snapshot   history[MAX_STEPS];
    int        history_len;
} TuringMachine;

/* ── API ── */
void tm_init(TuringMachine *tm);
void tm_load_input(TuringMachine *tm, const char *input);
int  tm_add_transition(TuringMachine *tm,
                        int from, char read,
                        int to,   char write, Direction dir);
int  tm_step(TuringMachine *tm);
void tm_run(TuringMachine *tm);
void tm_print_tape(const TuringMachine *tm);
void tm_save_snapshot(TuringMachine *tm);
Snapshot *tm_get_snapshot(TuringMachine *tm, int index);

/* Built-in example machines */
void tm_setup_binary_increment(TuringMachine *tm);
void tm_setup_palindrome_checker(TuringMachine *tm, const char *input);
void tm_setup_unary_addition(TuringMachine *tm, const char *input);

#endif /* TURING_MACHINE_H */
