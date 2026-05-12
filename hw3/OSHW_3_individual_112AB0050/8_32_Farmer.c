#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 定義互斥鎖，代表橋樑的使用權
pthread_mutex_t bridge_mutex;

// 北向農夫的執行緒函數
void* northbound_farmer(void* param) {
    int id = *(int *)param;

    // 嘗試上橋（取得鎖）
    pthread_mutex_lock(&bridge_mutex);
    
    // 模擬過橋時間：睡眠隨機時間 (1~3 秒)
    printf("北農夫%d過橋\n", id);
    sleep(rand() % 3 + 1); 
    
    // 下橋（釋放鎖）
    pthread_mutex_unlock(&bridge_mutex);

    pthread_exit(0);
}

// 南向農夫的執行緒函數
void* southbound_farmer(void* param) {
    int id = *(int *)param;

    // 嘗試上橋（取得鎖）
    pthread_mutex_lock(&bridge_mutex);
    
    // 模擬過橋時間
    printf("南農夫%d過橋\n", id);
    sleep(rand() % 3 + 1); 
    
    // 下橋
    pthread_mutex_unlock(&bridge_mutex);

    pthread_exit(0);
}

int main() {
    int num_farmers = 3; // 每個方向的農夫數量
    pthread_t north_threads[num_farmers], south_threads[num_farmers];
    int ids[num_farmers];

    pthread_mutex_init(&bridge_mutex, NULL);
    srand(time(NULL));

    // 建立多個北向與南向農夫執行緒 
    for (int i = 0; i < num_farmers; i++) {
        ids[i] = i + 1;
        pthread_create(&north_threads[i], NULL, northbound_farmer, &ids[i]);
        pthread_create(&south_threads[i], NULL, southbound_farmer, &ids[i]);
    }

    // 等待所有執行緒結束
    for (int i = 0; i < num_farmers; i++) {
        pthread_join(north_threads[i], NULL);
        pthread_join(south_threads[i], NULL);
    }

    pthread_mutex_destroy(&bridge_mutex);
    printf("所有農夫均已安全過橋\n");
}