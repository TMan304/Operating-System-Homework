#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/* 全域變數，用於存儲圓內的點數  */
long points_in_circle = 0;
long total_points;

/* 執行緒函數：產生隨機點並統計  */
void* count_points(void* param) {
    unsigned int seed = time(NULL);
    for (long i = 0; i < total_points; i++) {
        /* 產生 -1.0 到 1.0 之間的隨機座標  */
        double x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
        double y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;

        /* 檢查是否在圓內 (x^2 + y^2 <= 1)  */
        if (x * x + y * y <= 1.0) {
            points_in_circle++;
        }
    }
    pthread_exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "使用方式: %s <總點數>\n", argv[0]);
        return -1;
    }

    total_points = atol(argv[1]);
    pthread_t tid;

    /* 建立子執行緒  */
    pthread_create(&tid, NULL, count_points, NULL);

    /* 等待子執行緒結束  */
    pthread_join(tid, NULL);

    /* 計算並輸出估計值  */
    double pi_estimate = 4.0 * points_in_circle / total_points;
    printf("點數總計: %ld\n", total_points);
    printf("圓內點數: %ld\n", points_in_circle);
    printf("估計的 PI 值 = %f\n", pi_estimate);

    return 0;
}