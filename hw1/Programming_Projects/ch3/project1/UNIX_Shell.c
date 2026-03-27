#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_LINE 4096
#define MAX_ARGS 64
#define MAX_PATH 1024

void handler(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void deep_copy_args(char *src[], char *dest[]) { // args指向input的內容，淺拷貝會把history也指向input，但input每次while迴圈都會有新值，所以需要深拷貝給history用strdup建立獨立的記憶體位址儲存
    int i = 0;
    while (src[i] != NULL) {
        dest[i] = strdup(src[i]);
        i++;
    }
    dest[i] = NULL;
}

void free_args(char *args[]) { //因為strdup是用malloc圈出一塊記憶體，再給history新的值時會重新圈一塊位址，要先把舊的空間釋放掉
    for (int i = 0; args[i] != NULL; i++) {
        free(args[i]);
        args[i] = NULL;
    }
}

int find_symbol_in_array(char **array, const char *symbol) {
    int i = 0;
    while (array[i] != NULL) {
        if(strcmp(array[i], symbol) == 0) {
            return i;
        }
        i++;
    }
    return 0;
}

int main() {
    signal(SIGCHLD, handler);
    char cwd[MAX_PATH], input[MAX_LINE], in_file_path[MAX_PATH], out_file_path[MAX_PATH];
    char *args[MAX_ARGS], *token;
    char *history[MAX_ARGS];
    history[0] = NULL;
    char c;
    int keep_run = 1, redirect_in = 0, redirect_out = 0, pipe_pos = 0, pipe_fd[2];
    while (keep_run) {
        int need_wait = 1;

        // 輸出提示字元
        fflush(stdout);
        getcwd(cwd, sizeof(cwd));
        printf("%s> ", cwd);

        // 處理輸入到args
        fgets(input, MAX_LINE, stdin);
        input[strcspn(input, "\n")] = '\0';
        if (strcmp(input, "") == 0) continue;
        int index = 0;
        token = strtok(input, " ");
        while (token != NULL) {
            args[index++] = token;
            token = strtok(NULL, " ");
        }
        args[index--] = NULL;
        
        // 處理exit
        if (strcmp(args[0], "exit") == 0) {
            keep_run = 0;
            continue;
        }

        // 處理!!
        if (strcmp(args[0], "!!") == 0) {
            if (history[0] == NULL) {
                fprintf(stderr, "No commands in history.\n");
                continue;
            }
            int i = 0;
            while (history[i] != NULL) {
                args[i] = history[i];
                i++;
            }
            args[i] = NULL;
            index = i - 1;
        }
        else {
            free_args(history);
            deep_copy_args(args, history);
        }

        // 先找出所有需要特別處理的符號
        redirect_in = find_symbol_in_array(args, "<");
        redirect_out = find_symbol_in_array(args, ">");
        pipe_pos = find_symbol_in_array(args, "|");

        // 處理I/O重定位
        if (redirect_in > 0) {
            strcpy(in_file_path, args[redirect_in+1]);
            args[redirect_in] = NULL;
        }
        if (redirect_out > 0) {
            strcpy(out_file_path, args[redirect_out+1]);
            args[redirect_out] = NULL;
        }

        // 處理&
        if (index > 0 && strcmp(args[index], "&") == 0) {
            need_wait = 0;
            args[index--] = NULL;
        }
        
        // 呼叫child
        int pid = fork();
        if (pid < 0) { // error
            fprintf(stderr, "Error: can't call fork().");
            return 1;
        }
        else if (pid == 0) { // child
            if (redirect_in > 0) { // 重定位I/O
                int fd_in = open(in_file_path, O_RDONLY);
                if (fd_in == -1) {
                    perror("Error opening input file");
                    exit(1); 
                }
                dup2(fd_in, STDIN_FILENO);
                close(fd_in); // 不close也可以，fd_in是區域變數
            }
            if (redirect_out > 0) {
                int fd_out = open(out_file_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_out == -1) { // 這個判斷或許不需要
                    perror("Error opening output file");
                    exit(1); 
                }
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }
            if (execvp(args[0], args) == -1) { // 呼叫對應進程取代自己(child)，執行失敗的話跑以下程式
                fprintf(stderr, "Error: can't run \"");
                for (int j = 0; args[j] != NULL; j++) {
                    fprintf(stderr, "%s%s", args[j], (args[j+1] != NULL) ? " " : "");
                }
                fprintf(stderr, "\"\n");
                perror("Reason");
                exit(1);
            }
        }
        else { //parent
            if (need_wait) {
                waitpid(pid, NULL, 0);
            }
        }
    }
}