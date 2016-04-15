/*
 * This file contains global resources that may be used in completing this
 * assignment, as well as an convenience function that can be used to signal the
 * main thread of execution at important milestones.
 */

#ifndef TRING_H
#define TRING_H

#include <pthread.h>

#include "mailbox.h"
extern int num_threads;
pthread_cond_t main_signal;
pthread_mutex_t main_signal_lock;

pthread_cond_t nid_signal;
pthread_mutex_t nid_signal_lock;

pthread_cond_t probe_signal;
pthread_mutex_t probe_signal_lock;

pthread_cond_t sort_signal;
pthread_mutex_t sort_signal_lock;

pthread_cond_t ping_signal;
pthread_mutex_t ping_signal_lock;

#endif
