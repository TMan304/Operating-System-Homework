#include <stdio.h> 
#include <stdlib.h>
#include <errno.h>

#define MINPID 300
#define MAXPID 5000

const int pid_map_size = MAXPID - MINPID + 1;
char *pid_map;

int allocate_map(void) {
    int nitems = (pid_map_size+8-1) / 8;
    pid_map = (char *)calloc(nitems, sizeof(char));
    if (pid_map == NULL) return -1; // failed
    return 1; // success
}

int allocate_pid(void) {
    for (int i=0; i<pid_map_size; i++) {
        char curr_bit = 1 << (i%8);
        if (!(pid_map[i/8] & curr_bit)) {
            pid_map[i/8] = pid_map[i/8] | curr_bit;
            return i + MINPID; // 回傳pid
        }
    }
    return -1; // 沒有pid可用
}

void release_pid(int pid) {
    pid -= MINPID;
    int curr_char = pid / 8;
    char curr_bit = 1 << (pid%8);
    pid_map[curr_char] = pid_map[curr_char] & ~curr_bit;
}

int main() {
    if (allocate_map() == -1) {
        perror("Fail to initialize pid map.");
    }

    int p1, p2, p3;
    p1 = allocate_pid();
    p2 = allocate_pid();
    printf("p1: %d\np2: %d\n", p1, p2);
    release_pid(p1);
    p3 = allocate_pid();
    printf("after release p1\np2: %d\np3: %d\n", p2, p3);
    release_pid(p2);
    release_pid(p3);

    int pid[pid_map_size];
    for (int i=0; i<pid_map_size; i++) {
        pid[i] = allocate_pid();
    }
    p1 = allocate_pid();
    printf("after allocate all pid\np1: %d\n", p1);

    free(pid_map);
}