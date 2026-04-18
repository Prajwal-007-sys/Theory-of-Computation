#include "turing_machine.h"

/* ────────────────────────────────────────────────
   Core machine operations
   ──────────────────────────────────────────────── */

void tm_init(TuringMachine *tm) {
    memset(tm, 0, sizeof(TuringMachine));
    tm->blank         = '_';
    tm->initial_state = 0;
    tm->accept_state  = -1;   /* set per machine */
    tm->reject_state  = -2;   /* set per machine */
    /* Fill tape with blanks */
    memset(tm->tape, '_', TAPE_SIZE);
    tm->head          = TAPE_CENTER;
    tm->current_state = tm->initial_state;
    tm->halted        = 0;
    tm->accepted      = 0;
    tm->step_count    = 0;
    tm->history_len   = 0;
}

void tm_load_input(TuringMachine *tm, const char *input) {
    int len = (int)strlen(input);
    for (int i = 0; i < len && (TAPE_CENTER + i) < TAPE_SIZE; i++) {
        tm->tape[TAPE_CENTER + i] = input[i];
    }
    tm->head = TAPE_CENTER;
}

int tm_add_transition(TuringMachine *tm,
                       int from, char read,
                       int to,   char write, Direction dir) {
    if (tm->num_transitions >= MAX_TRANSITIONS) return -1;
    Transition *t = &tm->transitions[tm->num_transitions++];
    t->from_state = from;
    t->read_sym   = read;
    t->to_state   = to;
    t->write_sym  = write;
    t->dir        = dir;
    return 0;
}

void tm_save_snapshot(TuringMachine *tm) {
    if (tm->history_len >= MAX_STEPS) return;
    Snapshot *s = &tm->history[tm->history_len++];
    s->step    = tm->step_count;
    s->head    = tm->head;
    s->state   = tm->current_state;
    s->halted  = tm->halted;
    s->accepted= tm->accepted;
    memcpy(s->tape, tm->tape, TAPE_SIZE);

    /* compute visible tape window */
    s->tape_start = tm->head;
    s->tape_end   = tm->head;
    for (int i = 0; i < TAPE_SIZE; i++) {
        if (tm->tape[i] != '_') {
            if (i < s->tape_start) s->tape_start = i;
            if (i > s->tape_end)   s->tape_end   = i;
        }
    }
    /* ensure head is visible */
    if (tm->head < s->tape_start) s->tape_start = tm->head;
    if (tm->head > s->tape_end)   s->tape_end   = tm->head;
    /* pad by 2 on each side */
    s->tape_start = (s->tape_start > 2) ? s->tape_start - 2 : 0;
    s->tape_end   = (s->tape_end < TAPE_SIZE - 3) ? s->tape_end + 2 : TAPE_SIZE - 1;
}

Snapshot *tm_get_snapshot(TuringMachine *tm, int index) {
    if (index < 0 || index >= tm->history_len) return NULL;
    return &tm->history[index];
}

/* Perform one step; returns 1 if machine continues, 0 if halted */
int tm_step(TuringMachine *tm) {
    if (tm->halted) return 0;

    char current_sym = tm->tape[tm->head];
    Transition *matched = NULL;

    for (int i = 0; i < tm->num_transitions; i++) {
        Transition *t = &tm->transitions[i];
        if (t->from_state == tm->current_state && t->read_sym == current_sym) {
            matched = t;
            break;
        }
    }

    if (!matched) {
        /* No transition → reject / halt */
        tm->halted   = 1;
        tm->accepted = 0;
        tm_save_snapshot(tm);
        return 0;
    }

    /* Apply transition */
    tm->tape[tm->head] = matched->write_sym;
    tm->current_state  = matched->to_state;
    tm->head          += matched->dir;

    /* Clamp head */
    if (tm->head < 0)         tm->head = 0;
    if (tm->head >= TAPE_SIZE) tm->head = TAPE_SIZE - 1;

    tm->step_count++;

    /* Check accept / reject states */
    if (tm->current_state == tm->accept_state) {
        tm->halted   = 1;
        tm->accepted = 1;
    } else if (tm->current_state == tm->reject_state) {
        tm->halted   = 1;
        tm->accepted = 0;
    }

    tm_save_snapshot(tm);
    return !tm->halted;
}

void tm_run(TuringMachine *tm) {
    tm_save_snapshot(tm);   /* step 0 */
    while (!tm->halted && tm->step_count < MAX_STEPS) {
        tm_step(tm);
    }
}

void tm_print_tape(const TuringMachine *tm) {
    /* Find the used range */
    int left = tm->head, right = tm->head;
    for (int i = 0; i < TAPE_SIZE; i++) {
        if (tm->tape[i] != '_') {
            if (i < left)  left  = i;
            if (i > right) right = i;
        }
    }
    left  = (left  > 2) ? left  - 2 : 0;
    right = (right < TAPE_SIZE - 3) ? right + 2 : TAPE_SIZE - 1;

    printf("Step %d | State q%d\nTape: ", tm->step_count, tm->current_state);
    for (int i = left; i <= right; i++) {
        if (i == tm->head) printf("[%c]", tm->tape[i]);
        else               printf(" %c ", tm->tape[i]);
    }
    printf("\n%s\n", tm->accepted ? "ACCEPTED" : (tm->halted ? "REJECTED" : "RUNNING"));
}

/* ────────────────────────────────────────────────
   Built-in Machine 1: Binary Increment
   Increments a binary number written on the tape.
   States: 0=scan-right, 1=increment, 2=accept
   ──────────────────────────────────────────────── */
void tm_setup_binary_increment(TuringMachine *tm) {
    tm_init(tm);
    tm->accept_state = 2;
    tm->reject_state = -2;  /* never rejects */

    /* State 0: move right to end of number */
    tm_add_transition(tm, 0,'0', 0,'0', DIR_RIGHT);
    tm_add_transition(tm, 0,'1', 0,'1', DIR_RIGHT);
    /* hit blank → start incrementing from right */
    tm_add_transition(tm, 0,'_', 1,'_', DIR_LEFT);

    /* State 1: carry propagation */
    tm_add_transition(tm, 1,'1', 1,'0', DIR_LEFT);   /* 1+carry → 0, carry continues */
    tm_add_transition(tm, 1,'0', 2,'1', DIR_RIGHT);  /* 0+carry → 1, done */
    tm_add_transition(tm, 1,'_', 2,'1', DIR_RIGHT);  /* overflow → write 1 */
}

/* ────────────────────────────────────────────────
   Built-in Machine 2: Palindrome Checker
   Accepts strings over {a,b} that are palindromes.
   ──────────────────────────────────────────────── */
void tm_setup_palindrome_checker(TuringMachine *tm, const char *input) {
    tm_init(tm);
    tm->accept_state = 10;
    tm->reject_state = 11;

    tm_load_input(tm, input);

    /* State 0: read & erase leftmost symbol */
    tm_add_transition(tm, 0,'a', 1,'_', DIR_RIGHT);   /* erase 'a', go find right 'a' */
    tm_add_transition(tm, 0,'b', 3,'_', DIR_RIGHT);   /* erase 'b', go find right 'b' */
    tm_add_transition(tm, 0,'_', 10,'_', DIR_STAY);   /* empty → accept */

    /* State 1: scan right looking for matching 'a' */
    tm_add_transition(tm, 1,'a', 1,'a', DIR_RIGHT);
    tm_add_transition(tm, 1,'b', 1,'b', DIR_RIGHT);
    tm_add_transition(tm, 1,'_', 2,'_', DIR_LEFT);    /* hit end */

    /* State 2: at rightmost; check for 'a' */
    tm_add_transition(tm, 2,'a', 5,'_', DIR_LEFT);    /* match → erase, scan left */
    tm_add_transition(tm, 2,'b', 11,'b', DIR_STAY);   /* mismatch → reject */
    tm_add_transition(tm, 2,'_', 10,'_', DIR_STAY);   /* single char left → accept */

    /* State 3: scan right looking for matching 'b' */
    tm_add_transition(tm, 3,'a', 3,'a', DIR_RIGHT);
    tm_add_transition(tm, 3,'b', 3,'b', DIR_RIGHT);
    tm_add_transition(tm, 3,'_', 4,'_', DIR_LEFT);

    /* State 4: at rightmost; check for 'b' */
    tm_add_transition(tm, 4,'b', 5,'_', DIR_LEFT);
    tm_add_transition(tm, 4,'a', 11,'a', DIR_STAY);
    tm_add_transition(tm, 4,'_', 10,'_', DIR_STAY);

    /* State 5: scan back to leftmost blank boundary */
    tm_add_transition(tm, 5,'a', 5,'a', DIR_LEFT);
    tm_add_transition(tm, 5,'b', 5,'b', DIR_LEFT);
    tm_add_transition(tm, 5,'_', 0,'_', DIR_RIGHT);   /* back to start state */
}

/* ────────────────────────────────────────────────
   Built-in Machine 3: Unary Addition
   Input: 1^m 0 1^n  → output: 1^(m+n)
   ──────────────────────────────────────────────── */
void tm_setup_unary_addition(TuringMachine *tm, const char *input) {
    tm_init(tm);
    tm->accept_state = 3;
    tm->reject_state = -2;

    tm_load_input(tm, input);

    /* State 0: scan right to the separator '0' */
    tm_add_transition(tm, 0,'1', 0,'1', DIR_RIGHT);
    tm_add_transition(tm, 0,'0', 1,'1', DIR_RIGHT);   /* overwrite separator with 1 */

    /* State 1: scan right to the end */
    tm_add_transition(tm, 1,'1', 1,'1', DIR_RIGHT);
    tm_add_transition(tm, 1,'_', 2,'_', DIR_LEFT);

    /* State 2: erase the last 1 */
    tm_add_transition(tm, 2,'1', 3,'_', DIR_LEFT);
}
