#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define INIT_SIZE 10 // 測試數列大小


typedef struct {
    int *data;      // 指向實際儲存空間的指標
    int size;       // 當前已使用的元素數量
    int capacity;   // 陣列總容量
} DynamicArray;

DynamicArray test_list, result;

void init_dynamic_array(DynamicArray *array) {
    array->data = (int *)malloc(INIT_SIZE * sizeof(int));
    array->size = 0;
    array->capacity = INIT_SIZE;
}

void dynamic_array_expand(DynamicArray *array) {
    int *newData = (int *)realloc(array->data, array->capacity * 2 * sizeof(int));
    if (newData == NULL) {
        fprintf(stderr, "Memory reallocation failed!\n");
        exit(1);
    }
    array->data = newData;
    array->capacity *= 2;
}

int dynamic_array_append(DynamicArray *array, int num) {
    int is_expand = 0;
    if (array->size == array->capacity) {
        dynamic_array_expand(array);
        is_expand = 1;
    }
    array->data[array->size++] = num;
    return is_expand;
}


/* 傳遞給排序執行緒的參數 */
typedef struct {
    int start;
    int end;
} parameters;

/* 簡單的排序演算法 (Selection Sort) */
void* sorter(void* param) {
    parameters* p = (parameters*)param;
    for (int i = p->start; i <= p->end; i++) {
        for (int j = i + 1; j <= p->end; j++) {
            if (test_list.data[i] > test_list.data[j]) {
                int temp = test_list.data[i];
                test_list.data[i] = test_list.data[j];
                test_list.data[j] = temp;
            }
        }
    }
    pthread_exit(0);
}

/* 合併執行緒：負責將兩個已排序區間合併 */
void* merger(void* param) {
    result.size = test_list.size;

    int mid = result.size / 2;
    int left = 0;
    int right = mid;
    int curr = 0;

    // 雙指針合併
    while (left < mid && right < result.size) {
        if (test_list.data[left] <= test_list.data[right]) {
            result.data[curr++] = test_list.data[left++];
        } else {
            result.data[curr++] = test_list.data[right++];
        }
    }
    while (left < mid) result.data[curr++] = test_list.data[left++];
    while (right < result.size) result.data[curr++] = test_list.data[right++];

    result.size = test_list.size;

    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    // 決定檔名並讀取檔案
    const char* filename = (argc > 1) ? argv[1] : "input.txt";
    int num;
    init_dynamic_array(&result);
    init_dynamic_array(&test_list);
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("無法開啟檔案 %s\n", filename);
        exit(1);
    }
    while (1) {
        if (fscanf(fp, "%d", &num) == EOF) break;
        if (dynamic_array_append(&test_list, num) == 1) {
            dynamic_array_expand(&result);
        }
    }

    pthread_t tid_a, tid_b, tid_merge;
    parameters *p_a, *p_b;

    printf("原始數列: ");
    for (int i = 0; i < test_list.size; i++) printf("%d ", test_list.data[i]);
    printf("\n");

    // 建立兩個排序執行緒
    p_a = (parameters*)malloc(sizeof(parameters));
    p_a->start = 0; p_a->end = (test_list.size / 2) - 1;
    pthread_create(&tid_a, NULL, sorter, p_a);

    p_b = (parameters*)malloc(sizeof(parameters));
    p_b->start = test_list.size / 2; p_b->end = test_list.size - 1;
    pthread_create(&tid_b, NULL, sorter, p_b);

    // 必須等待排序完成才能進行合併
    pthread_join(tid_a, NULL);
    pthread_join(tid_b, NULL);

    // 建立合併執行緒
    pthread_create(&tid_merge, NULL, merger, NULL);
    pthread_join(tid_merge, NULL);

    printf("排序後結果: ");
    for (int i = 0; i < result.size; i++) printf("%d ", result.data[i]);
    printf("\n");

    free(p_a);
    free(p_b);
    free(test_list.data);
    free(result.data);
    return 0;
}