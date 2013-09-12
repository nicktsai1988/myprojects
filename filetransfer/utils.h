/*
 * =====================================================================================
 *
 *       Filename:  utils.h
 *
 *    Description:  declaration of some base functions
 *
 *        Version:  1.0
 *        Created:  07/30/2013 05:29:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Nick Tsai (hi.baidu.com/nicktsai), nicktsai@163.com
 *        Company:  Xidian University
 *
 * =====================================================================================
 */

#ifndef __UTILS_H
#define __UTILS_H
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/un.h>
#include<stdio.h>
#include<stdlib.h>
#define PORT 9877
#define LEN 1024
#define MAXLINE 1024


const char* filename(const char *path);
const char* sock_address(char* result,const char* serverpath);
const char* sock_port(char* result,const char* server);
int Socket(int domain,int type,int protocal);
int Connect(int sockfd,const struct sockaddr *addr,socklen_t addrlen);
int Listen(int sockfd,int backlog);
ssize_t Read(int fd,void *buf,size_t count);
ssize_t Readn(int fd,void *buf,size_t count);
ssize_t Writen(int fd,const void *buf,size_t count);
ssize_t Readline(int fd,void* buf,size_t maxlen);
int Open_m(const char *pathname,int flags,mode_t mode);
int Open(const char *pathname,int flags);
int Close(int fd);
int Bind(int sockfd,const struct sockaddr *addr,socklen_t addrlen);
int Accept(int sockfd,struct sockaddr *addr,socklen_t *addrlen);
void* Malloc(size_t size);
void* Calloc(size_t nmemb,size_t size);
char *sock_ntop(const struct sockaddr* addr,socklen_t addrlen);
char *Sock_ntop(const struct sockaddr* addr,socklen_t addrlen);

int daemon_init(const char* pname);
int set_work_dir(const char* dir);

int open_echo();
int close_echo();
void progress_bar(int progress);

int file_size_fd(int fd); //the return value maybe overflow,use it carefully
int file_size_path(const char* path);//the return value maybe overflow,use it carefully


//error
void err_ret(const char* fmt,...);
void err_sys(const char* fmt,...);
void err_exit(int error,const char* fmt,...);
void err_dump(const char* fmt,...);
void err_msg(const char* fmt,...);
void err_quit(const char *fmt,...);

//log_error

extern int log_to_stderr;

void log_open(const char* ident,int option,int facility); //open syslog
void log_err(const char* fmt,...); //an error related to system call,just print error message
void log_sys(const char* fmt,...);//an error related to system call,print error message and exit
void log_msg(const char* fmt,...);//not an error related to system call,just print message
void log_quit(const char* fmt,...);//not an error related to system call,print message and exit

#endif
