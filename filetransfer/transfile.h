/*
 * =====================================================================================
 *
 *       Filename:  transfile.h
 *
 *    Description:  declaration of some functions about filetransfer
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

#ifndef TRANSFILE_H
#define TRANSFILE_H
#define NUM_VERIFY_PASSWD 5

int sendfile(int sockfd,int fd);
int recvfile(int sockfd,int fd);
int check_passwd_inclient(int sockfd);
int check_passwd_inserver(int sockfd,const char* secret);
int dealrecvfile(int sockfd);
int dealsendfile(int sockfd,const char *file);
void sig_chld(int signo);

#endif // TRANSFILE_H
