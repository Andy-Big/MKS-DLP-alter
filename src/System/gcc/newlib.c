#include <sys/unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include "cmsis_gcc.h"

void __attribute__ ((weak)) _init(void)  {}

caddr_t _sbrk(int incr)
{
    extern char _ebss;
    static char *heap_end= &_ebss;
    char *prev_heap_end;

    prev_heap_end = heap_end;

    char * stack = (char*) __get_MSP();
    if (heap_end + incr > stack)
    {
        //_write(STDERR_FILENO, "Heap and stack collision\n", 25);
        errno = ENOMEM;
        return (caddr_t) - 1;
        //abort ();
    }

    heap_end += incr;
    return (caddr_t) prev_heap_end;

}

int _read(int file, char *ptr, int len)
{
    errno = EBADF;
    return -1;
}

int _close(int file)
{
    return -1;
}

int _isatty(int file)
{
    switch (file)
    {
    case STDOUT_FILENO:
    case STDERR_FILENO:
    case STDIN_FILENO:
        return 1;
    default:
        //errno = ENOTTY;
        errno = EBADF;
        return 0;
    }
}

int _fstat(int file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int _lseek(int file, int ptr, int dir)
{
    return 0;
}

void console_write(const char* ptr,int len) __attribute__((weak));


int _write(int file, char *ptr, int len)
{
    switch (file)
    {
    case STDOUT_FILENO: /*stdout*/
    case STDERR_FILENO: /* stderr */
        console_write(ptr,len);
        break;

    default:
        errno = EBADF;
        return -1;
    }
    return len;
}

int _getpid() {
    return 1;
}
  
extern void _exit(int status)
{
    printf("Exiting with status %d.\n", status);

    for (;;);
}

extern void _kill(int pid, int sig)
{
    return;
}
