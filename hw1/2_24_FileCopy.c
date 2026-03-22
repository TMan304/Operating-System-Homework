#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>    // open()
#include <unistd.h>   // read(), write(), close()
#include <errno.h>    // 錯誤處理

#define BUFFER_SIZE 4096

int main() {
    char src_path[256], dest_path[256];
    char buffer[BUFFER_SIZE];
    int src_fd, dest_fd;
    ssize_t bytes_read, bytes_written;

    // 輸入位置
    printf("Please enter the source file location: ");
    scanf("%s", src_path);
    printf("Please enter the target file location: ");
    scanf("%s", dest_path);

    // 開啟來源檔案
    src_fd = open(src_path, O_RDONLY);
    if(src_fd == -1){
        perror("Unable to open source archive");
        exit(EXIT_FAILURE);
    }

    // 開啟/建立目標檔案 (權限設為0644，擁有者讀寫，其他人唯讀)
    dest_fd = open(dest_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(dest_fd == -1){
        perror("Unable to open/create target file");
        close(src_fd);
        exit(EXIT_FAILURE);
    }

    // 複製
    while((bytes_read = read(src_fd, buffer, BUFFER_SIZE)) > 0){
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("An error occurred during the writing process");
            close(src_fd);
            close(dest_fd);
            exit(EXIT_FAILURE);
        }
    }
    if(bytes_read == -1){
        perror("An error occurred during the reading process");
    }

    close(src_fd);
    close(dest_fd);
}