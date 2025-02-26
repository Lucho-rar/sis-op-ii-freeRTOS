#ifndef SYS_CONFIG_H
#define SYS_CONFIG_H

#include <sys/stat.h>
#include <unistd.h>
#include "DriverLib.h"
int _close(int file);
int _fstat(int file, struct stat *st);
int _isatty(int file);
int _lseek(int file, int ptr, int dir);
int _read(int file, char *ptr, int len);
int _write(int file, char *ptr, int len);
void *_sbrk(int incr);
void _exit(int status);
int _kill(pid_t pid, int sig);
pid_t _getpid(void);

#endif /* SYS_CONFIG_H */
