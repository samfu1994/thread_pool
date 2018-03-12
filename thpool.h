
#include "basic.h"
#ifndef THPOOL_H
#define THPOOL_H

typedef void (*function_ptr)(int argc, char ** argv);

typedef struct {
    int front, rear, size;
    unsigned cap;
    int * argc_list;
    char *** argv_list;
    function_ptr * funcs;
    // void (void**)(int argc, char ** argv);
} queue;


queue * init_queue(int cap);
int enqueue(queue *, function_ptr, int, char **);
int dequeue(queue * );
int queue_is_empty(queue * );
int queue_is_full(queue * );
function_ptr front(queue * );


typedef struct {
    int num_thread;
    int avail_thread;
    queue * q;
} thpool;

thpool * init_thpool(int, int);
int thpool_add_job(thpool *, function_ptr, int, char ** );
int thpool_is_empty(thpool *);

#endif
