#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    // 定義 fork()
#include <sys/wait.h>  // 定義 wait()
#include <sys/types.h> // 定義 pid_t

int main(int argc, char *argv[]) {
    // 檢查命令列參數
    if(argc != 2){
        fprintf(stderr, "Please use: %s <int>\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]); // 將字串轉換為整數
    // 檢查參數是否為正整數
    if(n <= 0){
        fprintf(stderr, "please enter an integer greater than 0\n");
        return 1;
    }

    pid_t pid;
    pid = fork();
    if(pid < 0){
        perror("Fork fail");
        return 1;
    } 
    else if(pid == 0){ // child
        printf("The child process begins calculating the sequence (PID: %d): ", getpid());
        printf("%d", n);
        while(n != 1){
            if(n & 1){
                n = 3 * n + 1;
            }else{
                n = n / 2;
            }
            printf(", %d", n);
        }
        printf("\nchild done.\n");
        exit(0);
    } 
    else{ // parent
        printf("Parent (PID: %d) wait for child done...\n", getpid());
        wait(NULL);
        printf("parent done.\n");
    }
}