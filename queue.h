/*
 * This is the header file for an implemenation of a simple FIFO queue.
 */

#ifndef QUEUE_H
#define QUEUE_H
extern int num_threads;
extern volatile int mailboxCounter,shutdownCounter,nidcounter,probeCounter,sortCounter,iniprobeCounter,probeCounter,iniSortCounter;
extern pthread_mutex_t probe_counter_lock;
extern pthread_mutex_t ini_sort_lock;
extern pthread_mutex_t mail_counter_lock;
extern pthread_mutex_t shutdown_counter_lock;
extern pthread_mutex_t nid_counter_lock;
extern pthread_mutex_t id_counter_lock;
extern pthread_mutex_t iniprobe_counter_lock;
extern pthread_mutex_t sort_lock;

extern int firstID,lastID,ID1Got;
typedef struct queue_node_s {
	struct queue_node_s* next;
	void* payload;
} queue_node;

typedef struct queue_s {
	queue_node* first;
	queue_node* last;
} queue;

void queue_init(queue* q);
int queue_is_empty(queue* q);
void queue_push(queue* q, void* payload);
void* queue_pop(queue* q);

#endif
