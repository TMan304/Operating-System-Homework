#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// 記憶體區塊節點
typedef struct Block {
    int start;
    int end;
    char process[16];
    bool is_free;
    struct Block *next;
} Block;

Block *head = NULL;
int MAX_MEMORY = 0;

// 建立新節點
Block* create_block(int start, int end, const char* process, bool is_free) {
    Block *new_block = (Block*)malloc(sizeof(Block));
    new_block->start = start;
    new_block->end = end;
    strcpy(new_block->process, process);
    new_block->is_free = is_free;
    new_block->next = NULL;
    return new_block;
}

// 初始化記憶體
void init_memory(int size) {
    MAX_MEMORY = size;
    head = create_block(0, size - 1, "Unused", true);
}

// 功能 1：請求記憶體 (First-Fit)
void request_memory(const char* process, int size) {
    Block *current = head;
    while (current != NULL) {
        int block_size = current->end - current->start + 1;
        if (current->is_free && block_size >= size) {
            // 找到足夠的空間
            if (block_size == size) {
                // 大小剛好，直接佔用
                strcpy(current->process, process);
                current->is_free = false;
            } else {
                // 空間太大，需要分割
                Block *new_free_block = create_block(current->start + size, current->end, "Unused", true);
                new_free_block->next = current->next;
                
                current->end = current->start + size - 1;
                strcpy(current->process, process);
                current->is_free = false;
                current->next = new_free_block;
            }
            printf("成功為程序 %s 分配 %d 單位記憶體。\n", process, size);
            return;
        }
        current = current->next;
    }
    printf("錯誤：沒有足夠的連續記憶體可分配給 %s。\n", process);
}

// 輔助功能：合併相鄰的空閒區塊
void merge_free_blocks() {
    Block *current = head;
    while (current != NULL && current->next != NULL) {
        if (current->is_free && current->next->is_free) {
            Block *temp = current->next;
            current->end = temp->end;
            current->next = temp->next;
            free(temp);
        } else {
            current = current->next;
        }
    }
}

// 功能 2：釋放記憶體
void release_memory(const char* process) {
    Block *current = head;
    bool found = false;
    while (current != NULL) {
        if (!current->is_free && strcmp(current->process, process) == 0) {
            current->is_free = true;
            strcpy(current->process, "Unused");
            found = true;
        }
        current = current->next;
    }
    if (found) {
        merge_free_blocks(); // 釋放後合併相鄰洞
        printf("成功釋放程序 %s 的記憶體。\n", process);
    } else {
        printf("錯誤：找不到程序 %s。\n", process);
    }
}

// 功能 3：緊縮記憶體
void compact_memory() {
    Block *current = head;
    int current_address = 0;
    
    // 將所有已分配的區塊往前移
    Block *new_head = NULL;
    Block *new_tail = NULL;
    
    while (current != NULL) {
        if (!current->is_free) {
            int size = current->end - current->start + 1;
            Block *moved_block = create_block(current_address, current_address + size - 1, current->process, false);
            
            if (new_head == NULL) {
                new_head = moved_block;
                new_tail = moved_block;
            } else {
                new_tail->next = moved_block;
                new_tail = moved_block;
            }
            current_address += size;
        }
        Block *temp = current;
        current = current->next;
        free(temp); // 釋放舊節點
    }
    
    // 如果還有剩餘空間，在尾端加上一個巨大的空閒區塊
    if (current_address < MAX_MEMORY) {
        Block *free_block = create_block(current_address, MAX_MEMORY - 1, "Unused", true);
        if (new_head == NULL) {
            new_head = free_block;
        } else {
            new_tail->next = free_block;
        }
    }
    
    head = new_head;
    printf("記憶體緊縮完成。\n");
}

// 功能 4：報告記憶體狀態
void report_status() {
    Block *current = head;
    printf("--- 記憶體狀態報告 ---\n");
    while (current != NULL) {
        printf("Addresses [%06d : %06d] Process %s\n", current->start, current->end, current->process);
        current = current->next;
    }
    printf("----------------------\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("用法: %s <初始記憶體大小>\n", argv[0]);
        return 1;
    }
    
    int initial_memory = atoi(argv[1]);
    init_memory(initial_memory);
    printf("系統初始化：總記憶體大小為 %d\n", initial_memory);
    
    char command[10];
    char process[16];
    int size;
    
    // 簡易命令列互動迴圈
    while (1) {
        printf("allocator> ");
        scanf("%s", command);
        
        if (strcmp(command, "RQ") == 0) { // 請求：RQ P0 10000
            scanf("%s %d", process, &size);
            request_memory(process, size);
        } else if (strcmp(command, "RL") == 0) { // 釋放：RL P0
            scanf("%s", process);
            release_memory(process);
        } else if (strcmp(command, "C") == 0) {  // 緊縮：C
            compact_memory();
        } else if (strcmp(command, "STAT") == 0) { // 報告：STAT
            report_status();
        } else if (strcmp(command, "X") == 0 || strcmp(command, "exit") == 0) {
            break;
        } else {
            printf("未知指令。可用指令: RQ <程序> <大小>, RL <程序>, C, STAT, X(離開)\n");
        }
    }
}