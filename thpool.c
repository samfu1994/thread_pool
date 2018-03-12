#include "thpool.h"

#define MAX_NUM_ARGC 1024
#define MAX_LENGTH_ARGV 64
#define MAX_SIZE_RES 1024
queue * init_queue(int cap) {
    queue * q = malloc(sizeof(queue));
    q -> cap = cap;
    q -> funcs = malloc(sizeof(function_ptr) *  cap);
    q -> argc_list = malloc(sizeof(int) * cap);
    q -> argv_list = malloc(sizeof(char**) * cap); 
    q -> res_list = malloc(sizeof(char*) * cap); 
    q -> front = 0;
    q -> rear = 0;
    q -> size = 0;

    sem_init(&(q -> mutex), 0, 1);
    sem_init(&(q -> full), 0, 0);
    sem_init(&(q -> empty), 0, cap);
    return q;
}

int enqueue(queue * q, function_ptr fp, int argc, char ** argv, char * res) {
    sem_wait(&(q -> empty));
    sem_wait(&(q -> mutex));
    q -> size += 1;
    q -> funcs[q -> rear] = fp;
    q -> argc_list[q -> rear] = argc;
    q -> argv_list[q -> rear] = argv;
    q -> res_list[q -> rear] = res;
    q -> rear = (q -> rear + 1) % (q -> cap);
    sem_post(&(q -> mutex));
    sem_post(&(q -> full));
    return 0;
}

function_ptr dequeue(queue * q, int * argc, char *** p_argv, char ** res) {
    int cur;
    sem_wait(&(q -> full));
    sem_wait(&(q -> mutex));
    q -> size -= 1;
    cur = q -> front;
    * argc = q -> argc_list[cur];
    * p_argv = q -> argv_list[cur];
    * res = q -> res_list[cur];
    
    q -> front = (q -> front + 1) % (q -> cap);
    sem_post(&(q -> mutex));
    sem_post(&(q -> empty));
    return q -> funcs[cur];
}

int queue_is_empty(queue * q) {
    int res;
    sem_wait(&(q -> mutex));
    res = q -> size == 0;
    sem_post(&(q -> mutex));
    return res;
}

int queue_is_full(queue * q) {
    int res;
    sem_wait(&(q -> mutex));
    res = q -> size == q -> cap;
    sem_post(&(q -> mutex));
    return res;
}

thpool * init_thpool(int thread_num, int queue_cap) {
    thpool * tp = malloc(sizeof(thpool));
    tp -> num_thread = thread_num;
    tp -> avail_thread = thread_num;
    tp -> q = init_queue(queue_cap);
    tp -> threads = malloc(sizeof(pthread_t) * thread_num);
    for(int i = 0; i < thread_num; i++) {
        pthread_create(&tp -> threads[i], NULL, &routine, (void*) tp);
    }
    return tp;
}

void * routine(void * void_tp) {
    thpool * tp = (thpool *) void_tp;
    int argc;
    char * res = malloc(sizeof(char) * MAX_SIZE_RES);
    char ** argv = malloc(sizeof(char*) * MAX_NUM_ARGC);
    for(int i = 0; i < MAX_NUM_ARGC; i++) {
        argv[i] = malloc(sizeof(char*) * MAX_LENGTH_ARGV);
    }
    while(1) {
        function_ptr fp = dequeue(tp -> q, &argc, &argv, &res);
        (*fp)(argc, argv, res);
    }
    return NULL;
}

int thpool_add_job(thpool * tp, function_ptr fp, int argc, char ** argv, char * res) {
    return enqueue(tp -> q, fp, argc, argv, res);
}

int thpool_is_empty(thpool * tp) {
    return queue_is_empty(tp -> q);
}

void f(int argc, char ** argv, char * res) {
    for(int i = 0; i < argc; i++) {
        strcpy(res, argv[i]);
    }
}

int main(){
    int thread_num = 4;
    int queue_size = 10;

    thpool * tp = init_thpool(thread_num, queue_size);
    function_ptr fp = &f;
    int argc = 1;
    char ** argv;
    char ** res;
    res = malloc(sizeof(char* ) * 2048);
    for(int i = 0; i < 2048; i++) {
        res[i] = malloc(sizeof(char) * MAX_SIZE_RES);
    }

    for(int j = 0; j < 2048; j++){
        argv = malloc(sizeof(char*) * argc);
        for(int i = 0; i < argc; i++) {
            argv[i] = malloc(sizeof(char) * MAX_LENGTH_ARGV);
            sprintf(argv[i], "%d", i + j);
        }
        thpool_add_job(tp, fp, argc, argv, res[j]);
    }
    sleep(1);
    for(int i = 0; i < 2048; i++) {
        printf("now res[%d] is %s\n", i, res[i]);
    }
    for(int i = 0; i < thread_num; i++) {
        pthread_join(tp -> threads[i], NULL);
    }
    return 0;
}
