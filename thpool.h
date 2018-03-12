
#include "basic.h"
#ifndef THPOOL_H
#define THPOOL_H

typedef void (*function_ptr)(int argc, char ** argv, char * res);

typedef struct {
    int front, rear, size;
    unsigned cap;
    int * argc_list;
    char ** res_list;
    char *** argv_list;
    function_ptr * funcs;
    sem_t mutex, empty, full;
    // void (void**)(int argc, char ** argv);
} queue;

queue * init_queue(int cap);
int enqueue(queue *, function_ptr, int, char **, char *);
function_ptr dequeue(queue *, int *, char *** , char **);
int queue_is_empty(queue * );
int queue_is_full(queue * );
function_ptr front(queue * );

typedef struct {
    int num_thread;
    int avail_thread;
    pthread_t * threads;
    queue * q;
} thpool;

thpool * init_thpool(int, int);
void * routine(void * void_tp); 
int thpool_add_job(thpool *, function_ptr, int, char **, char * );
int thpool_is_empty(thpool *);

#endif
