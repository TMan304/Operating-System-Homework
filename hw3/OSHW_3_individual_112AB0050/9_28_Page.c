#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // 檢查命令列參數是否正確
    if (argc < 2) {
        printf("Usage: %s <virtual address>\n", argv[0]);
        return 1;
    }

    // strtoul 可以處理較大的十進制字串轉換
    unsigned int address = (unsigned int)strtoul(argv[1], NULL, 10);

    // 分頁大小為 4 KB = 4096 bytes
    unsigned int page_size = 4096;

    // 計算分頁號碼與偏移量
    unsigned int page_number = address / page_size;
    unsigned int offset = address % page_size;

    // 輸出
    printf("The address %u contains:\n", address);
    printf("page number = %u\n", page_number);
    printf("offset = %u\n", offset);
}