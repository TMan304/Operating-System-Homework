#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define MAX_RESOURCES 5
int available_resources = MAX_RESOURCES;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // 互斥鎖，確保available_resources不會被同時訪問
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; // 條件變數，資源不夠時可以等待，並在有資源時通知在等待的執行緒

/* 請求資源：若不足則阻塞 */
int decrease_count(int count) {
    pthread_mutex_lock(&mutex);
    
    // 如果資源不夠，就進入等待狀態
    while (available_resources < count) {
        pthread_cond_wait(&cond, &mutex);
    }
    
    available_resources -= count;
    printf("獲取了 %d 個資源，剩餘: %d\n", count, available_resources);
    
    pthread_mutex_unlock(&mutex);
    return 0;
}

/* 歸還資源：並喚醒等待中的執行緒 */
int increase_count(int count) {
    pthread_mutex_lock(&mutex);
    
    available_resources += count;
    printf("歸還了 %d 個資源，剩餘: %d\n", count, available_resources);
    
    // 資源增加後，通知正在等待的執行緒重新檢查
    pthread_cond_broadcast(&cond);
    
    pthread_mutex_unlock(&mutex);
    return 0;
}

void* worker(void* arg) {
    int amount = (rand() % 3) + 1; // 隨機請求 1~3 個資源
    decrease_count(amount);
    sleep(2); // 模擬使用資源
    increase_count(amount);
    return NULL;
}

#define TEST_NUM 3

int main() {
    srand(time(NULL));
    pthread_t threads[TEST_NUM];
    for (int i = 0; i < TEST_NUM; i++) {
        pthread_create(&threads[i], NULL, worker, NULL);
    }
    for (int i = 0; i < TEST_NUM; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}