#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>    // 定義 fork()
#include <sys/wait.h>  // 定義 wait()
#include <sys/types.h> // 定義 pid_t
#include <sys/mman.h> // 記憶體映射
#include <fcntl.h> // 檔案描述符
#include <sys/stat.h> // 獲取檔案狀態

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

    // 創建共享記憶體
    int shm_fd = shm_open("/my_shm", O_CREAT  | O_RDWR, 0666);
    ftruncate(shm_fd, 4096);
    char *shm_ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // 呼叫child
    pid_t pid;
    pid = fork();
    if(pid < 0){
        perror("Fork fail");
        return 1;
    } 
    else if(pid == 0){ // child
        printf("The child process begins calculating the sequence (PID: %d)...", getpid());
        int pos = 0;
        pos += sprintf(shm_ptr, "%d ", n);
        while (n != 1) {
            if (n & 1) {
                n = 3 * n + 1;
            }
            else {
                n = n / 2;
            }
            pos += sprintf(shm_ptr+pos, "%d ", n);
        }
        printf("\nchild done.\n");
        exit(0);
    } 
    else{ // parent
        printf("Parent (PID: %d) wait for child done...\n", getpid());
        waitpid(pid, NULL, 0);
        printf("%s\n", shm_ptr);
        printf("parent done.\n");
    }

    // 釋放記憶體空間和關閉檔案
    munmap(shm_ptr, 4096);
    shm_unlink("/my_shm");
    close(shm_fd);
}