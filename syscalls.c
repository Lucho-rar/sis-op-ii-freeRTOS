#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

extern char _end;
static char *heap_end;


caddr_t _sbrk(int incr) {
    if (heap_end == 0) {
        heap_end = &_end;
    }
    char *prev_heap_end = heap_end;
    heap_end += incr;
    return (caddr_t) prev_heap_end;
}

int _close(int file) { return -1; }
int _fstat(int file, struct stat *st) { st->st_mode = S_IFCHR; return 0; }
int _isatty(int file) { return 1; }
int _lseek(int file, int ptr, int dir) { return 0; }
int _read(int file, char *ptr, int len) { return 0; }
void _exit(int status) { while (1); }
int _kill(int pid, int sig) { errno = EINVAL; return -1; }
int _getpid(void) { return 1; }
int _write(int file, char *ptr, int len) {
    // int i;
    // for (i = 0; i < len; i++) {
    //     // Send the character to the UART or any other output device
    //     // For example, using UARTSend(ptr[i]);
    // }
    return len;
}
