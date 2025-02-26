#include "sys_cfg.h"
#include <unistd.h>
#include <stdarg.h>

int _close(int file) {
    return -1;
}

int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) {
    return 1;
}

int _lseek(int file, int ptr, int dir) {
    return 0;
}

int _read(int file, char *ptr, int len) {
    return 0;
}

int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        UARTCharPut(UART0_BASE, ptr[i]);  
    }
    return len;
}

void *_sbrk(int incr) {
    extern char _end;  // Definido en el linker script
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &_end;
    }

    prev_heap_end = heap_end;
    heap_end += incr;

    return prev_heap_end;
}

void _exit(int status) {
    while(1);  // Loop forever
}

int _kill(pid_t pid, int sig) {
    return -1;  // Fake kill, always return error
}

pid_t _getpid(void) {
    return 1;  // Fake process ID
}
