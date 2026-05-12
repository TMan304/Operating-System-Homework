#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// 任務結構體
typedef struct {
    void (*function)(void *);
    void *data;
} pool_task_t;

// 執行緒池結構體
typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t notify;
    pthread_t *threads;
    pool_task_t *queue;
    int thread_count;
    int queue_size;
    int head, tail, count;
    int shutdown;
} thread_pool_t;

// 工作執行緒
void *worker_thread(void *pool_ptr) {
    thread_pool_t *pool = (thread_pool_t *)pool_ptr;
    while (1) {
        pthread_mutex_lock(&(pool->lock));

        // 如果隊列為空且沒有要關閉，則等待
        while ((pool->count == 0) && (!pool->shutdown)) {
            pthread_cond_wait(&(pool->notify), &(pool->lock));
        }

        if (pool->shutdown) {
            pthread_mutex_unlock(&(pool->lock));
            pthread_exit(NULL);
        }

        // 領取任務
        pool_task_t task = pool->queue[pool->head];
        pool->head = (pool->head + 1) % pool->queue_size;
        pool->count--;

        pthread_mutex_unlock(&(pool->lock));

        // 執行任務函數
        (*(task.function))(task.data);
    }
    return NULL;
}

// 初始化執行緒池
thread_pool_t* pool_init(int num_threads, int q_size) {
    thread_pool_t *pool = (thread_pool_t *)malloc(sizeof(thread_pool_t));
    pool->thread_count = num_threads;
    pool->queue_size = q_size;
    pool->head = pool->tail = pool->count = pool->shutdown = 0;
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);
    pool->queue = (pool_task_t *)malloc(sizeof(pool_task_t) * q_size);

    pthread_mutex_init(&(pool->lock), NULL);
    pthread_cond_init(&(pool->notify), NULL);

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&(pool->threads[i]), NULL, worker_thread, (void*)pool);
    }
    return pool;
}

// 提交任務
int pool_submit(thread_pool_t *pool, void (*func)(void *), void *arg) {
    pthread_mutex_lock(&(pool->lock));

    if (pool->count == pool->queue_size) {
        pthread_mutex_unlock(&(pool->lock));
        return -1; // 隊列已滿
    }

    pool->queue[pool->tail].function = func;
    pool->queue[pool->tail].data = arg;
    pool->tail = (pool->tail + 1) % pool->queue_size;
    pool->count++;

    pthread_cond_signal(&(pool->notify)); // 喚醒一個工人
    pthread_mutex_unlock(&(pool->lock));
    return 0;
}


// 印出任務編號並模擬工作時間
void my_task(void *arg) {
    int id = *(int *)arg;
    printf("  [執行中] 任務 %d 由執行緒 0x%lx 處理\n", id, (unsigned long)pthread_self());
    usleep(500000); // 模擬工作 0.5 秒
    printf("  [已完成] 任務 %d\n", id);
    free(arg); // 釋放在 main 中配置的記憶體
}

int main() {
    // 初始化執行緒池：4個執行緒，隊列大小為10
    printf("正在初始化執行緒池 (4 threads, queue size 10)...\n");
    thread_pool_t *pool = pool_init(4, 10);

    // 提交8個任務到池中
    printf("正在提交 8 個任務...\n");
    for (int i = 0; i < 8; i++) {
        int *arg = malloc(sizeof(int));
        *arg = i + 1;
        if (pool_submit(pool, my_task, arg) != 0) {
            printf("提交任務 %d 失敗 (隊列可能已滿)\n", i + 1);
        }
    }
    
    sleep(5);
    printf("測試結束。\n");
}