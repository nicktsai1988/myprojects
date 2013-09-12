/*
 * =====================================================================================
 *
 *       Filename:  errorlog.c
 *
 *    Description:  some functions about error information
 *
 *        Version:  1.0
 *        Created:  07/31/2013 10:58:09 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Nick Tsai (hi.baidu.com/nicktsai), nicktsai@163.com
 *        Company:  Xidian University
 *
 * =====================================================================================
 */
#include "utils.h"
#include <errno.h>		/* for definition of errno */
#include <stdarg.h>		/* ISO C variable arguments */
#include <syslog.h>

static void	log_doit(int, int, const char *, va_list ap);

/*
 * Caller must define and set this: nonzero if
 * interactive, zero if daemon
 */
int	log_to_stderr=1;

/*
 * Initialize syslog(), if running as daemon.
 */
void
log_open(const char *ident, int option, int facility)
{
	if (log_to_stderr == 0)
		openlog(ident, option, facility);
}

/*
 * Nonfatal error related to a system call.
 * Print a message with the system's errno value and return.
 */
void
log_err(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	log_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
}

/*
 * Fatal error related to a system call.
 * Print a message and terminate.
 */
void
log_sys(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	log_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
    exit(1);
}

/*
 * Nonfatal error unrelated to a system call.
 * Print a message and return.
 */
void
log_msg(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	log_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
}

/*
 * Fatal error unrelated to a system call.
 * Print a message and terminate.
 */
void
log_quit(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	log_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
    exit(2);
}

/*
 * Print a message and return to caller.
 * Caller specifies "errnoflag" and "priority".
 */
static void
log_doit(int errnoflag, int priority, const char *fmt, va_list ap)
{
	int		errno_save;
	char	buf[MAXLINE];

	errno_save = errno;		/* value caller might want printed */
	vsnprintf(buf, MAXLINE, fmt, ap);
	if (errnoflag)
		snprintf(buf+strlen(buf), MAXLINE-strlen(buf), ": %s",
		  strerror(errno_save));
	strcat(buf, "\n");
    if (log_to_stderr)
    {
		fflush(stdout);
		fputs(buf, stderr);
		fflush(stderr);
    }
    else
    {
		syslog(priority, buf);
	}
}
