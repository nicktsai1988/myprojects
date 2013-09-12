/*
 * =====================================================================================
 *
 *       Filename:  transfileserver.c
 *
 *    Description:  server of filetranfer
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
#include "filetransfer_conf.h"
#include "transfile.h"
#include <syslog.h>
#include <stddef.h>

extern struct FileTransferConf conf;

int main()
{
    const char *conf_file="./filetransfer.conf";
    int listenfd,connfd,passwdstat;
	struct sockaddr_in servaddr,cliaddr;
    //struct FileTransferConf conf;
    socklen_t clilen;
    pid_t pid;
    uint32_t host;
    uint16_t port;
    if(!filetransfer_conf_read(&conf,conf_file))
    {
        log_quit("read configure file error");
    }
    print_conf_option(&conf);


    //change workdir
    if(set_work_dir(conf.workdir)<0)
    {
       log_quit("change workdir to %s failed",conf.workdir);
    }

    if(strcmp(conf.host,"all")) // not "all"
        host=inet_addr(conf.host);
    else
        host=htonl(INADDR_ANY);
    port=(uint16_t)atoi(conf.port);

    if(daemon_init("filetransfer")!=0)
    {
        log_quit("daemon error");
    }


	signal(SIGCHLD,sig_chld);

    listenfd=Socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=host;
    servaddr.sin_port=htons(port);
    Bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));



    Listen(listenfd,10);
    clilen=sizeof(cliaddr);

	while(1)
	{
        connfd=Accept(listenfd,(struct sockaddr*)&cliaddr,&clilen);
		if((pid=fork())==0)  //child
		{
            Close(listenfd); //child close listenfd

            if((passwdstat=check_passwd_inserver(connfd,conf.passwd))<0)
			{
				Close(connfd);
				exit(1);
			}
            if(set_work_dir(conf.workdir)<0)//recheck word dir if it is been deleted or moved
            {
                log_msg("recheck workdir %s failed",conf.workdir);
                Close(connfd);
                exit(1);
            }
			dealrecvfile(connfd);
			Close(connfd);
			return 0;
		}
		else //parent
            Close(connfd); //parent close connfd
	}
}
