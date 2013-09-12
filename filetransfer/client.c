/*
 * =====================================================================================
 *
 *       Filename:  client.c
 *
 *    Description:  client of filetranfer
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

int main(int argc,char *argv[])
{
    int passwdstat,result;
    char address_buf[256];
    char port_buf[16];
	if(argc!=3)
	{
        fprintf(stderr,"usage: %s ip:port file\n",argv[0]);
		exit(1);
	}
	const char *file=filename(argv[2]);
    const char *port=sock_port(port_buf,argv[1]);
    const char *address=sock_address(address_buf,argv[1]);
    fprintf(stderr,"transport %s to %s\n",file,argv[1]);
	int sockfd=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=inet_addr(address);
    servaddr.sin_port=htons(atoi(port));
	if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
	{
		perror("connect error");
		exit(1);
	}
    if((passwdstat=check_passwd_inclient(sockfd))<0)
	{
		Close(sockfd);
		exit(1);
	}
    fprintf(stderr,"you have passwed the verification,now start send file...\n");
    result=dealsendfile(sockfd,argv[2]);
	if(result==0)
        printf("send file %s complete\n",argv[2]);
	else
        printf("send file %s fialed\n",argv[2]);
	Close(sockfd);
	exit(0);
}




	

