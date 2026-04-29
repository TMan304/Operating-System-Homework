#include <pthread.h>
#include <stdio.h> 
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>


#define MINPID 300
#define MAXPID 5000
#define NUM_THREADS 100

const int pid_map_size = MAXPID - MINPID + 1;
char *pid_map;
pthread_mutex_t mutex; // 用於保護 pid_map 的互斥鎖

int allocate_map(void) {
    int nitems = (pid_map_size+8-1) / 8;
    pid_map = (char *)calloc(nitems, sizeof(char));
    if (pid_map == NULL) return -1; // failed
    pthread_mutex_init(&mutex, NULL); // 初始化互斥鎖
    return 1; // success
}

int allocate_pid(void) {
    pthread_mutex_lock(&mutex); // 鎖
    for (int i=0; i<pid_map_size; i++) {
        char curr_bit = 1 << (i%8);
        if (!(pid_map[i/8] & curr_bit)) {
            pid_map[i/8] = pid_map[i/8] | curr_bit;
            pthread_mutex_unlock(&mutex); // 解鎖
            return i + MINPID; // 回傳pid
        }
    }
    pthread_mutex_unlock(&mutex); // 失敗時也解鎖
    return -1; // 沒有pid可用
}

void release_pid(int pid) {
    pthread_mutex_lock(&mutex); //鎖
    pid -= MINPID;
    int curr_char = pid / 8;
    char curr_bit = 1 << (pid%8);
    pid_map[curr_char] = pid_map[curr_char] & ~curr_bit;
    pthread_mutex_unlock(&mutex); // 解鎖
}

void *thread_routine(void *arg) {
    int id = *(int *)arg;
    int pid = allocate_pid();
    if (pid != -1) {
        printf("執行緒 %d: 獲取 PID %d\n", id, pid);
        // 隨機睡眠
        sleep(rand() % 3 + 1); 

        printf("執行緒 %d: 釋放 PID %d\n", id, pid);
        release_pid(pid);
    }
    else {
        printf("執行緒 %d: 無法獲取 PID\n", id);
    }
    
    free(arg);
    pthread_exit(0);
}

int main() {
    srand(time(NULL));
    if (allocate_map() == -1) {
        perror("無法初始化pid_map");
        return -1;
    }

    pthread_t tids[NUM_THREADS];

    int counts_create_pthreads = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        int *id = malloc(sizeof(int));
        if (id == NULL) {
            fprintf(stderr, "無法宣告id: %d\n", i);
            break;
        }
        *id = i;
        pthread_create(&tids[i], NULL, thread_routine, id);
        counts_create_pthreads++;
    }

    for (int i = 0; i < counts_create_pthreads; i++) {
        pthread_join(tids[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    free(pid_map);
    printf("所有測試執行緒已完成。\n");
    return 0;
}