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

int keep_run = 1;
char *args[MAX_ARGS], *history[MAX_ARGS];

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

void free_args(char *args[]) { // 因為strdup是用malloc圈出一塊記憶體，再給history新的值時會重新圈一塊位址，要先把舊的空間釋放掉
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

void handle_redirection(char **args) { // I/O處理
    int in_pos = find_symbol_in_array(args, "<");
    if (in_pos > 0) {
        int fd = open(args[in_pos + 1], O_RDONLY);
        if (fd != -1) {
            dup2(fd, STDIN_FILENO);
            close(fd);
            args[in_pos] = NULL;
        }
    }
    int out_pos = find_symbol_in_array(args, ">");
    if (out_pos > 0) {
        int fd = open(args[out_pos + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd != -1) {
            dup2(fd, STDOUT_FILENO);
            close(fd);
            args[out_pos] = NULL;
        }
    }
}

int handle_built_in() { // 回傳1代表有執行
    // 處理exit
    if (strcmp(args[0], "exit") == 0) {
        keep_run = 0;
        return 1;
    }
    // 處理cd
    if (strcmp(args[0], "cd") == 0) {
        char *target_path = args[1];
        if (target_path == NULL) {
            fprintf(stderr, "expected argument to \"cd\"\n");
        }
        else {
            if (chdir(target_path) != 0) {
                perror("cd failed");
            }
        }
        return 1;
    }
    return 0;
}

int main() {
    signal(SIGCHLD, handler); // 回收背景處理的child
    char cwd[MAX_PATH], input[MAX_LINE];
    char *token;
    history[0] = NULL;
    int pipe_pos = 0;
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

        // 處理內部指令
        if (handle_built_in()) {
            continue;
        }

        // 處理&
        if (index > 0 && strcmp(args[index], "&") == 0) {
            need_wait = 0;
            args[index--] = NULL;
        }

        // 先找出|的位置
        pipe_pos = find_symbol_in_array(args, "|");
        
        // 有|的情況這樣fork(只能處理一個|)
        if (pipe_pos > 0) {
            // 把命令拆成兩個
            args[pipe_pos] = NULL;
            char **args_left = args;
            char **args_right = &args[pipe_pos + 1];
            // 開始呼叫child
            int fd[2];
            if (pipe(fd) == -1) { perror("Pipe failed"); continue; }
            pid_t p1 = fork();
            if (p1 < 0) {
                perror("Fork P1 failed");
                close(fd[0]); close(fd[1]);
                continue;
            }
            if (p1 == 0) {
                handle_redirection(args_left); 
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]); close(fd[1]);
                execvp(args_left[0], args_left);
                exit(1);
            }
            pid_t p2 = fork();
            if (p2 < 0) {
                perror("Fork P2 failed");
                kill(p1, SIGKILL);
                close(fd[0]); close(fd[1]);
                continue;
            }
            if (p2 == 0) {
                handle_redirection(args_right); 
                dup2(fd[0], STDIN_FILENO);
                close(fd[1]); close(fd[0]);
                execvp(args_right[0], args_right);
                exit(1);
            }
            // parent
            close(fd[0]); close(fd[1]);
            if (need_wait) {
                waitpid(p1, NULL, 0);
                waitpid(p2, NULL, 0);
            }
        }
        // 沒|時的fork
        else {
            int pid = fork();
            if (pid < 0) { // error
                fprintf(stderr, "Error: can't call fork().");
                return 1;
            }
            else if (pid == 0) { // child
                handle_redirection(args);
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
}