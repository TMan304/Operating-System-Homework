#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/* 全域變數 */
int circle_count = 0;           // 落在圓內的總點數
int total_points_per_thread;    // 每個執行緒要生成的點數
pthread_mutex_t mutex;          // 互斥鎖

void* runner(void* param) {
    int local_count = 0;
    unsigned int seed = time(NULL) ^ (unsigned int)pthread_self(); // 確保每個執行緒種子不同

    for (int i = 0; i < total_points_per_thread; i++) {
        // 產生 -1 到 1 之間的隨機座標
        double x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
        double y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;

        // 判斷是否在單位圓內 (x^2 + y^2 <= 1)
        if (x * x + y * y <= 1.0) {
            local_count++;
        }
    }

    /* 進入臨界區更新全域變數 */
    pthread_mutex_lock(&mutex);
    circle_count += local_count;
    pthread_mutex_unlock(&mutex);

    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    int total_points = (argc > 1) ? atoi(argv[1]) : 10000;
    int num_threads = (argc > 2) ? atoi(argv[2]) : 2;
    total_points_per_thread = total_points / num_threads;

    pthread_t workers[num_threads];
    pthread_mutex_init(&mutex, NULL);

    /* 建立執行緒 */
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&workers[i], NULL, runner, NULL);
    }

    /* 等待所有執行緒完成 */
    for (int i = 0; i < num_threads; i++) {
        pthread_join(workers[i], NULL);
    }

    /* 計算並輸出結果 */
    double pi_estimate = 4.0 * circle_count / total_points;
    printf("Estimated Pi = %f (Total points: %d, In circle: %d)\n", 
            pi_estimate, total_points, circle_count);

    pthread_mutex_destroy(&mutex);
    return 0;
}