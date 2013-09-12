/*
 * =====================================================================================
 *
 *       Filename:  transfile.c
 *
 *    Description:  filetransfer's main functions
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
#include"transfile.h"
#include"filetransfer_conf.h"
#include<syslog.h>

const char* error="---\n";
const char* right="+++\n";

void sig_chld(int signo)
{
    pid_t pid;
    int stat;
    while((pid=waitpid(-1,&stat,WNOHANG))>0);
    return;
}

int sendfile(int sockfd,int fd)
{
	char buf[LEN];
    int nread,count=0;
    int size=file_size_fd(fd);

	while((nread=Read(fd,buf,sizeof(buf)))>0)
	{
		Writen(sockfd,buf,nread);
        count+=nread;
        progress_bar((count/size)*100);
	}

	return 0;
}

int recvfile(int sockfd,int fd)
{
	char buf[LEN];
	int nread;
	while((nread=Read(sockfd,buf,sizeof(buf)))>0)
		Writen(fd,buf,nread);
	return 0;
}

int dealrecvfile(int sockfd)
{
	char name[128];
	int nread,fd;
again:
    if((nread=Readline(sockfd,name,sizeof(name)))>0)
        name[nread-1]='\0';//remove the '\n' fo the tail
	else
	{
        log_msg("client terminated early");
        Close(sockfd);
		exit(1);
	}

    if(access(name,F_OK)==0)//file existed
	{
        Writen(sockfd,error,strlen(error));
		goto again;
	}
    Writen(sockfd,right,strlen(right));
	fd=open(name,O_CREAT|O_WRONLY,S_IRWXU);//在这里不使用包装函数，是为了记录信息
	if(fd<0)
	{
        log_err("connot open file %s",name);
        log_msg("receive file %s failed",name);
		return -1;
	}
	recvfile(sockfd,fd);
    log_msg("receive file %s completed",name);
	Close(fd);
	return 0;
}

int dealsendfile(int sockfd,const char* file)
{
	int fd,nread;
	char stat[8],newname[128];
	const char *name=filename(file);
	strcpy(newname,name);
    strcat(newname,"\n");
    Writen(sockfd,newname,strlen(newname));
again:
    nread=Readline(sockfd,stat,sizeof(stat));
    if(nread == 0)
    {
        log_msg("server terminated");
        Close(sockfd);
        exit(1);
    }
	stat[nread]=0;
    if(strcmp(stat,error)==0)
	{
        newname[strlen(newname)-1]='\0';
        fprintf(stderr,"file %s have existed in server,please input new name:",newname);
        while(1)
        {
            fgets(newname,sizeof(newname),stdin);
            if(newname[0] != '\n')
                break;
            else
            {
                fprintf(stderr,"input is empty,please try again:\n");
            }
        }
		Writen(sockfd,newname,strlen(newname));
		goto again;
	}
	else
        if(strcmp(stat,right)==0) //filename is not exsit
		{
			fd=Open(file,O_RDONLY);
			sendfile(sockfd,fd);
			Close(fd);
			return 0;
		}
		else
		{
			fprintf(stderr,"protocal error\n");
			return -1;
		}
}
int check_passwd_inclient(int sockfd)
{
	int nread;
    char passwd[CONF_PASSWD_LEN],stat[8];
    fprintf(stderr,"enter your passwd:");
    while(1)
    {
        while(1)
        {
            close_echo();
            fgets(passwd,sizeof(passwd),stdin);
            open_echo();
            fprintf(stderr,"\n");
            if (passwd[0] == '\n')
            {
                fprintf(stderr,"passwd is empty,please input again:");
            }
            else
                break;
        }
        Writen(sockfd,passwd,strlen(passwd));
        nread=Readline(sockfd,stat,sizeof(stat));
        if(nread == 0)
        {
            fprintf(stderr,"verify passwd too many times\n,please try again later.\n");
            return -1;
        }
        stat[nread]='\0';
        if(strcmp(stat,error)==0)
        {
            fprintf(stderr,"passwd error,please input again:");
            continue;
        }
        else
            if(strcmp(stat,right)==0)
            {
                return 0;
            }
            else
            {
                fprintf(stderr,"protocal error");
                return -1;
            }
    }
	return -1;
}
		
int check_passwd_inserver(int sockfd,const char* secret)
{
    int nread;
    int count=0;
    char passwd[CONF_PASSWD_LEN];
    while(count<NUM_VERIFY_PASSWD)
    {
        count++;
        nread=Readline(sockfd,passwd,sizeof(passwd));
        if(nread == 0)
        {
            log_msg("client terminated early");
            Close(sockfd);
            exit(1);
        }
        passwd[nread-1]='\0';
        if(strcmp(passwd,secret) != 0)
        {
            Writen(sockfd,error,strlen(error));
            continue;
        }
        else
        {
            Writen(sockfd,right,strlen(right));
            return 0;
        }
    }
    log_msg("vefified too many times");
    return -1;
}


	





	
