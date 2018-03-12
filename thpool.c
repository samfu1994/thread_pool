#include "thpool.h"

queue * init_queue(int cap) {
    queue * q = malloc(sizeof(queue));
    q -> cap = cap;
    q -> funcs = malloc(sizeof(function_ptr) *  cap);
    q -> argc_list = malloc(sizeof(int) * cap);
    q -> argv_list = malloc(sizeof(char**) * cap); 
    q -> front = 0;
    q -> rear = 0;
    q -> size = 0;
}

int enqueue(queue * q, function_ptr fp, int argc, char ** argv) {
    //need to call is_full first to make sure the queue is not full
    if(queue_is_full(q)) {
        return -1;
    }
    
    q -> size += 1;
    q -> funcs[q -> rear] = fp;
    q -> argc_list[q -> rear] = argc;
    q -> argv_list[q -> rear] = argv;
    q -> rear = (q -> rear + 1) % (q -> cap);
    return 0;
}

int dequeue(queue * q) {
    //need to call is_empty first to make sure the queue is not empty
    if(queue_is_empty(q)) {
        return -1;
    }

    q -> size -= 1;
    //q -> funcs[q -> front] = NULL;
    q -> front = (q -> front + 1) % (q -> cap);
    return 0;
}

int queue_is_empty(queue * q) {
    return q -> size == 0;
}

int queue_is_full(queue * q) {
    return q -> size == q -> cap;
}

function_ptr front(queue * q) {
    if(q -> size == 0) {
        return NULL;
    }
    return q -> funcs[q -> front];
}

thpool * init_thpool(int thread_num, int queue_cap) {
    thpool * tp = malloc(sizeof(thpool));
    tp -> num_thread = thread_num;
    tp -> avail_thread = thread_num;
    tp -> q = init_queue(queue_cap);

    return tp;
}

void execute(thpool * tp, int i) {
    (* (tp -> q -> funcs[i])) (tp -> q -> argc_list[i], tp -> q -> argv_list[i]);
}

int thpool_add_job(thpool * tp, function_ptr fp, int argc, char ** argv) {
    return enqueue(tp -> q, fp, argc, argv);
}

int thpool_is_empty(thpool * tp) {
    return queue_is_empty(tp -> q);
}

void f(int argc, char ** argv) {
    for(int i = 0; i < argc; i++) {
        printf("argv[%d] is %s\n", i, argv[i]);
    }
}

int main(){
    thpool * tp = init_thpool(4, 10);
    printf("thread num is %d, queue cap is %d\n", tp -> num_thread, tp -> q -> cap);
    function_ptr fp = &f;
    int argc = 2;
    char ** argv = malloc(sizeof(char*) * argc);
    
    for(int i = 0; i < argc; i++) {
        argv[i] = malloc(sizeof(char) * 3);
        sprintf(argv[i], "%d", i * 100);
    }
    thpool_add_job(tp, fp, argc, argv);
    execute(tp, 0);
    return 0;
}
