#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TASKS 100
#define QUANTUM 10 // RR 的時間片段設定為 10

// 任務結構
typedef struct {
    char name[10];
    int priority;
    int burst;
    int remaining_burst; // 用於 RR 排程
} Task;

Task tasks[MAX_TASKS];
int task_count = 0;

// 輔助函式：讀取任務檔案
void load_tasks(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("無法開啟檔案 %s\n", filename);
        exit(1);
    }
    task_count = 0;
    // 假設檔案格式為: P1, 4, 20
    while (fscanf(fp, "%[^,], %d, %d\n", tasks[task_count].name, &tasks[task_count].priority, &tasks[task_count].burst) != EOF) {
        tasks[task_count].remaining_burst = tasks[task_count].burst;
        task_count++;
    }
    fclose(fp);
}

// 輔助函式：重置剩餘時間 (供多次測試使用)
void reset_bursts() {
    for (int i = 0; i < task_count; i++) {
        tasks[i].remaining_burst = tasks[i].burst;
    }
}

// 1. FCFS
void schedule_fcfs() {
    printf("\n=== FCFS Scheduling ===\n");
    for (int i = 0; i < task_count; i++) {
        printf("執行 %s (Burst: %d)\n", tasks[i].name, tasks[i].burst);
    }
}

// 2. SJF (使用簡單的泡泡排序)
void schedule_sjf() {
    printf("\n=== SJF Scheduling ===\n");
    Task temp_tasks[MAX_TASKS];
    memcpy(temp_tasks, tasks, sizeof(Task) * task_count);

    for (int i = 0; i < task_count - 1; i++) {
        for (int j = 0; j < task_count - 1 - i; j++) {
            if (temp_tasks[j].burst > temp_tasks[j+1].burst) {
                Task t = temp_tasks[j];
                temp_tasks[j] = temp_tasks[j+1];
                temp_tasks[j+1] = t;
            }
        }
    }
    for (int i = 0; i < task_count; i++) {
        printf("執行 %s (Burst: %d)\n", temp_tasks[i].name, temp_tasks[i].burst);
    }
}

// 3. Priority (數字越大優先權越高)
void schedule_priority() {
    printf("\n=== Priority Scheduling ===\n");
    Task temp_tasks[MAX_TASKS];
    memcpy(temp_tasks, tasks, sizeof(Task) * task_count);

    for (int i = 0; i < task_count - 1; i++) {
        for (int j = 0; j < task_count - 1 - i; j++) {
            if (temp_tasks[j].priority < temp_tasks[j+1].priority) { // 大到小
                Task t = temp_tasks[j];
                temp_tasks[j] = temp_tasks[j+1];
                temp_tasks[j+1] = t;
            }
        }
    }
    for (int i = 0; i < task_count; i++) {
        printf("執行 %s (Priority: %d, Burst: %d)\n", temp_tasks[i].name, temp_tasks[i].priority, temp_tasks[i].burst);
    }
}

// 4. Round-Robin
void schedule_rr() {
    printf("\n=== Round-Robin Scheduling (Quantum = %d) ===\n", QUANTUM);
    reset_bursts();
    int complete = 0;
    int time = 0;
    
    while (complete < task_count) {
        int idle = 1; // 檢查此輪是否所有行程都已結束
        for (int i = 0; i < task_count; i++) {
            if (tasks[i].remaining_burst > 0) {
                idle = 0;
                int run_time = (tasks[i].remaining_burst > QUANTUM) ? QUANTUM : tasks[i].remaining_burst;
                tasks[i].remaining_burst -= run_time;
                time += run_time;
                printf("[%4d] 執行 %s 歷時 %d (剩餘: %d)\n", time, tasks[i].name, run_time, tasks[i].remaining_burst);
                
                if (tasks[i].remaining_burst == 0) {
                    complete++;
                }
            }
        }
        if (idle) break;
    }
}

// 5. Priority with Round-Robin
void schedule_priority_rr() {
    printf("\n=== Priority with RR Scheduling (Quantum = %d) ===\n", QUANTUM);
    reset_bursts();
    
    // 先依優先級排序
    Task temp_tasks[MAX_TASKS];
    memcpy(temp_tasks, tasks, sizeof(Task) * task_count);
    for (int i = 0; i < task_count - 1; i++) {
        for (int j = 0; j < task_count - 1 - i; j++) {
            if (temp_tasks[j].priority < temp_tasks[j+1].priority) {
                Task t = temp_tasks[j];
                temp_tasks[j] = temp_tasks[j+1];
                temp_tasks[j+1] = t;
            }
        }
    }

    int current_idx = 0;
    int time = 0;
    
    while (current_idx < task_count) {
        int current_priority = temp_tasks[current_idx].priority;
        int group_end = current_idx;
        
        // 找出相同優先級的區間
        while (group_end < task_count && temp_tasks[group_end].priority == current_priority) {
            group_end++;
        }
        
        // 對該區間執行 RR
        int group_complete = 0;
        int group_size = group_end - current_idx;
        
        while (group_complete < group_size) {
            for (int i = current_idx; i < group_end; i++) {
                if (temp_tasks[i].remaining_burst > 0) {
                    int run_time = (temp_tasks[i].remaining_burst > QUANTUM) ? QUANTUM : temp_tasks[i].remaining_burst;
                    temp_tasks[i].remaining_burst -= run_time;
                    time += run_time;
                    printf("[%4d] 執行 %s (Priority: %d) 歷時 %d (剩餘: %d)\n", 
                           time, temp_tasks[i].name, temp_tasks[i].priority, run_time, temp_tasks[i].remaining_burst);
                    
                    if (temp_tasks[i].remaining_burst == 0) {
                        group_complete++;
                    }
                }
            }
        }
        current_idx = group_end; // 移動到下一個優先級群組
    }
}

int main(int argc, char *argv[]) {
    // 決定檔名並讀取檔案
    const char* filename = (argc > 1) ? argv[1] : "task.txt";
    load_tasks(filename);
    
    schedule_fcfs();
    schedule_sjf();
    schedule_priority();
    schedule_rr();
    schedule_priority_rr();

    return 0;
}