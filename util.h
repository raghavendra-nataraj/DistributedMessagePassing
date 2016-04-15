/*
 * This file contains declarations of utility functions for the extra credit
 * assignment.  As well, we declare a type for booleans as well as macros for
 * true and false.
 *
 */

#ifndef UTIL_H
#define UTIL_H

#define TRUE (0 == 0)
#define FALSE (!TRUE)

typedef unsigned char bool;

void ids_cleanup(void);
void ids_init(int num);
int ids_next(void);

void tring_print(int from, int to);
void tring_printLast(int from);
void tring_print_finalize(void);
void tring_print_init(void);
void tring_signal(void);
void tring_nid_signal(void);
void tring_probe_signal(void);
void tring_sort_signal(void);
void tring_ping_signal(void);
void pong(int id);
int pong_check(void);
int pong_count(void);
void pong_init(void);

#endif
