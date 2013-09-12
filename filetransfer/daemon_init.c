/*
 * =====================================================================================
 *
 *       Filename:  daemon_init.c
 *
 *    Description:  make process in daemon state
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


#include"utils.h"
#include<syslog.h>
#define MAXFD 64
extern int log_to_stderr;
int daemon_init(const char *pname)
{
	int i;
	pid_t pid;
	if((pid=fork())<0)
		return -1;
	else
		if(pid)
            exit(0); //parent terminated
	//child contimues
	if(setsid()<0)  //become session leader
		return -1;
	signal(SIGHUP,SIG_IGN);
	if((pid=fork())<0)
		return -1;
	else
		if(pid)
            exit(0);    //child 1 terminated
	// child 2 continues
	//daemon_proc=1;
	for(i=0;i<MAXFD;i++)
		close(i);
	open("dev/null",O_RDONLY);//将，0，1，2重定向到/dev/null
	open("dev/null",O_RDWR);
	open("dev/null",O_RDWR);
	if(pname!=NULL)
        log_open(pname,LOG_PID,LOG_USER);
    log_to_stderr=0;
	return 0;
}

