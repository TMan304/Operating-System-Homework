#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* 定義傳遞給執行緒的參數結構 */
typedef struct {
    int row;
    int column;
} parameters;

/* 全域變數 */
int sudoku[9][9];
int results[11] = {0}; // 存儲驗證結果：1 代表有效，0 代表無效

/* 驗證數字 1-9 是否不重複出現的通用函數 */
int is_valid_set(int data[9]) {
    int check[10] = {0};
    for (int i = 0; i < 9; i++) {
        int num = data[i];
        if (num < 1 || num > 9 || check[num]) return 0;
        check[num] = 1;
    }
    return 1;
}

/* 執行緒 1：檢查所有列 */
void* check_rows(void* param) {
    for (int i = 0; i < 9; i++) {
        if (!is_valid_set(sudoku[i])) { pthread_exit(NULL); }
    }
    results[0] = 1;
    pthread_exit(NULL);
}

/* 執行緒 2：檢查所有行 */
void* check_cols(void* param) {
    for (int j = 0; j < 9; j++) {
        int col_data[9];
        for (int i = 0; i < 9; i++) col_data[i] = sudoku[i][j];
        if (!is_valid_set(col_data)) { pthread_exit(NULL); }
    }
    results[1] = 1;
    pthread_exit(NULL);
}

/* 執行緒 3~11：檢查單一個 3x3 子九宮格 */
void* check_subgrid(void* param) {
    parameters* p = (parameters*)param;
    int data[9], idx = 0;
    for (int i = p->row; i < p->row + 3; i++) {
        for (int j = p->column; j < p->column + 3; j++) {
            data[idx++] = sudoku[i][j];
        }
    }
    if (is_valid_set(data)) {
        // 根據子九宮格位置決定在 results 陣列的索引 (2-10)
        results[2 + (p->row / 3) * 3 + (p->column / 3)] = 1;
    }
    free(p); // 記得釋放動態分配的記憶體
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    // 決定檔名並讀取檔案
    const char* filename = (argc > 1) ? argv[1] : "input.txt";
    FILE* fp = fopen(filename, "r");
    
    if (fp == NULL) {
        fprintf(stderr, "錯誤：找不到檔案 '%s'\n", filename);
        return EXIT_FAILURE;
    }

    printf("正在讀取檔案 '%s'...\n", filename);
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (fscanf(fp, "%d", &sudoku[i][j]) != 1) {
                fprintf(stderr, "錯誤：檔案格式不正確或資料不足 81 個數字\n");
                fclose(fp);
                return EXIT_FAILURE;
            }
        }
    }
    fclose(fp);

    // 建立執行緒
    pthread_t threads[11];
    
    pthread_create(&threads[0], NULL, check_rows, NULL);
    pthread_create(&threads[1], NULL, check_cols, NULL);

    int t_idx = 2;
    for (int i = 0; i < 9; i += 3) {
        for (int j = 0; j < 9; j += 3) {
            parameters* p = (parameters*)malloc(sizeof(parameters));
            p->row = i; p->column = j;
            pthread_create(&threads[t_idx++], NULL, check_subgrid, p);
        }
    }

    // 等待所有執行緒完成
    for (int i = 0; i < 11; i++) {
        pthread_join(threads[i], NULL);
    }

    // 檢查結果
    int valid = 1;
    for (int i = 0; i < 11; i++) {
        if (results[i] == 0) {
            valid = 0;
            break;
        }
    }

    // 輸出結論
    printf("\n=== 驗證結果 ===\n");
    printf("該數獨解法是否有效: %s\n", valid ? "YES (有效)" : "NO (無效)");
    
    return 0;
}