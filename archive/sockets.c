#include "sockets.h"

void err_n_die(const char *fmt, ...)
{
    int errno_save;
    va_list ap;

    errno_save = errno;

    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    fprintf(stdout, "\n");
    fflush(stdout);

    if (errno_save != 0)
    {
        fprintf(stdout, "(errno = %d) : %s\n", errno_save, strerror(errno_save));
        fprintf(stdout, "\n");
        fflush(stdout);
    }

    va_end(ap);

    exit(1);
}

bool check(int exception_to_check, const char *err_message)
{
    if (exception_to_check <= SOCKET_ERROR)
    {
        // perror(err_message);
        err_n_die(err_message);
        return false;
    }
    return true;
}