#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

// 全域變數與動態陣列指標
int num_customers;
int num_resources;
int *available;
int **maximum;
int **allocation;
int **need;
pthread_mutex_t mutex;

// 函數宣告
bool is_safe();
int request_resources(int customer_num, int request[]);
void release_resources(int customer_num, int release[]);
void *customer_thread(void *param);

// 動態配置 2D 矩陣的輔助函數
int** allocate_matrix(int rows, int cols) {
    int **matrix = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (int *)malloc(cols * sizeof(int));
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = 0;
        }
    }
    return matrix;
}

// 銀行家演算法：安全檢查
bool is_safe() {
    int *work = (int *)malloc(num_resources * sizeof(int));
    bool *finish = (bool *)malloc(num_customers * sizeof(bool));
    
    for (int i = 0; i < num_resources; i++) work[i] = available[i];
    for (int i = 0; i < num_customers; i++) finish[i] = false;

    int count = 0;
    while (count < num_customers) {
        bool found = false;
        for (int i = 0; i < num_customers; i++) {
            if (!finish[i]) {
                bool possible = true;
                for (int j = 0; j < num_resources; j++) {
                    if (need[i][j] > work[j]) {
                        possible = false;
                        break;
                    }
                }
                if (possible) {
                    for (int j = 0; j < num_resources; j++) {
                        work[j] += allocation[i][j];
                    }
                    finish[i] = true;
                    found = true;
                    count++;
                }
            }
        }
        // 如果一整圈都找不到可以滿足的客戶，代表發生死結風險
        if (!found) break; 
    }

    bool result = (count == num_customers);
    free(work);
    free(finish);
    return result;
}

// 處理資源請求
int request_resources(int customer_num, int request[]) {
    pthread_mutex_lock(&mutex);

    // 檢查請求是否合理
    for (int i = 0; i < num_resources; i++) {
        if (request[i] > need[customer_num][i] || request[i] > available[i]) {
            pthread_mutex_unlock(&mutex);
            return -1; // 請求超過需求或系統目前可用量
        }
    }

    // 模擬分配資源
    for (int i = 0; i < num_resources; i++) {
        available[i] -= request[i];
        allocation[customer_num][i] += request[i];
        need[customer_num][i] -= request[i];
    }

    // 執行安全演算法
    if (is_safe()) {
        printf("[核准] Customer %d 請求成功。系統處於安全狀態。\n", customer_num);
        pthread_mutex_unlock(&mutex);
        return 0;
    } else {
        // 若不安全，則回滾狀態
        for (int i = 0; i < num_resources; i++) {
            available[i] += request[i];
            allocation[customer_num][i] -= request[i];
            need[customer_num][i] += request[i];
        }
        printf("[拒絕] Customer %d 請求被拒絕，避免系統進入不安全狀態。\n", customer_num);
        pthread_mutex_unlock(&mutex);
        return -1;
    }
}

// 處理資源釋放
void release_resources(int customer_num, int release[]) {
    pthread_mutex_lock(&mutex);
    
    printf("[釋放] Customer %d 釋放資源。\n", customer_num);
    for (int i = 0; i < num_resources; i++) {
        available[i] += release[i];
        allocation[customer_num][i] -= release[i];
        need[customer_num][i] += release[i]; // 釋放後，未來的需求額度會恢復
    }
    
    pthread_mutex_unlock(&mutex);
}

// 客戶執行緒的運作邏輯
void *customer_thread(void *param) {
    int customer_num = *(int *)param;
    int *request = (int *)malloc(num_resources * sizeof(int));
    int *release = (int *)malloc(num_resources * sizeof(int));

    // 每個客戶循環執行2次以模擬持續的請求與釋放
    for (int loop = 0; loop < 2; loop++) {
        usleep((rand() % 500 + 100) * 1000); // 隨機等待 0.1~0.6 秒

        // 隨機產生請求量
        bool has_request = false;
        printf("Customer %d 嘗試請求資源: [ ", customer_num);
        for (int i = 0; i < num_resources; i++) {
            if (need[customer_num][i] > 0) {
                request[i] = rand() % (need[customer_num][i] + 1);
                if (request[i] > 0) has_request = true;
            } else {
                request[i] = 0;
            }
            printf("%d ", request[i]);
        }
        printf("]\n");

        if (has_request) {
            // 發出請求
            if (request_resources(customer_num, request) == 0) {
                // 請求成功，模擬使用資源的時間
                usleep((rand() % 500 + 100) * 1000); 

                // 隨機釋放已分配的資源
                printf("Customer %d 準備釋放資源: [ ", customer_num);
                for (int i = 0; i < num_resources; i++) {
                    if (allocation[customer_num][i] > 0) {
                        release[i] = rand() % (allocation[customer_num][i] + 1);
                    } else {
                        release[i] = 0;
                    }
                    printf("%d ", release[i]);
                }
                printf("]\n");
                
                release_resources(customer_num, release);
            }
        }
    }

    free(request);
    free(release);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    // 處理輸入參數與預設值
    if (argc >= 3) {
        num_customers = atoi(argv[1]);
        num_resources = argc - 2;
        available = (int *)malloc(num_resources * sizeof(int));
        for (int i = 0; i < num_resources; i++) {
            available[i] = atoi(argv[i + 2]);
        }
        printf("[系統] 使用自訂參數：客戶數 = %d, 資源數 = %d\n", num_customers, num_resources);
    } else {
        num_customers = 5;
        num_resources = 3;
        available = (int *)malloc(num_resources * sizeof(int));
        available[0] = 10; available[1] = 10; available[2] = 10;
        printf("[系統] 未提供足夠參數。使用預設值：客戶數 = 5, 資源數 = 3 (10, 10, 10)\n");
    }

    // 初始化矩陣與互斥鎖
    pthread_mutex_init(&mutex, NULL);
    srand(time(NULL));

    maximum = allocate_matrix(num_customers, num_resources);
    allocation = allocate_matrix(num_customers, num_resources);
    need = allocate_matrix(num_customers, num_resources);

    // 隨機生成每個客戶的最大需求量 (不能超過系統總資源)
    printf("\n--- 初始最大需求矩陣 (Max) ---\n");
    for (int i = 0; i < num_customers; i++) {
        printf("Customer %d: ", i);
        for (int j = 0; j < num_resources; j++) {
            // 確保需求至少為 1，最大為 available[j]
            maximum[i][j] = (rand() % available[j]) + 1; 
            need[i][j] = maximum[i][j];
            printf("%d ", maximum[i][j]);
        }
        printf("\n");
    }
    printf("------------------------------\n\n");

    // 建立並啟動客戶執行緒
    pthread_t *threads = (pthread_t *)malloc(num_customers * sizeof(pthread_t));
    int *customer_ids = (int *)malloc(num_customers * sizeof(int));

    for (int i = 0; i < num_customers; i++) {
        customer_ids[i] = i;
        pthread_create(&threads[i], NULL, customer_thread, &customer_ids[i]);
    }

    // 等待所有執行緒完成
    for (int i = 0; i < num_customers; i++) {
        pthread_join(threads[i], NULL);
    }

    // 釋放記憶體與清理
    pthread_mutex_destroy(&mutex);
    free(available);
    free(threads);
    free(customer_ids);
    for (int i = 0; i < num_customers; i++) {
        free(maximum[i]); free(allocation[i]); free(need[i]);
    }
    free(maximum); free(allocation); free(need);

    printf("[系統] 所有客戶執行緒已結束，系統關閉。\n");
    return 0;
}