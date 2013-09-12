/*
 * =====================================================================================
 *
 *       Filename:  utils.c
 *
 *    Description:  some basic functions
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
#include<termios.h>
int Socket(int domain,int type,int protocal)
{
	int n;
	if((n=socket(domain,type,protocal))<0)
	{
        log_sys("socket error");
	}
	return n;
}
int Connect(int sockfd,const struct sockaddr *addr,socklen_t addrlen)
{
	int n;
	if((n=connect(sockfd,addr,addrlen))<0)
	{
        log_sys("connect error");
	}
	return n;
}
int Listen(int sockfd,int backlog)
{
	int n;
	if((n=listen(sockfd,backlog))<0)
	{
        log_sys("listen error");
	}
	return n;
}
ssize_t Readn(int fd,void *buf,size_t count)
{
	size_t nleft;
	ssize_t nread;
	char *ptr=(char*)buf;
	nleft=count;
	while(nleft>0)
	{
		if((nread=read(fd,ptr,nleft))<0)
		{
			if(errno==EINTR)
				nread=0;
			else
            {
                log_sys("readn error");
            }
		}
		else
			if(nread==0)
				break;
		nleft-=nread;
		ptr+=nread;
	}
	return (count-nleft);
}
ssize_t Read(int fd,void *buf,size_t count)
{
	int n;
again:
	if((n=read(fd,buf,count))<0)
	{
		if(errno==EINTR)
			goto again;
		else
		{
            log_sys("read error");
		}
	}
	return n;
}
ssize_t Writen(int fd,const void *buf,size_t count)
{
	size_t nleft;
	ssize_t nwritten;
	const char* ptr=(char*)buf;
	nleft=count;
	while(nleft>0)
	{	
		if((nwritten=write(fd,ptr,nleft))<=0)
		{
			if(nwritten<0&&errno==EINTR)
				nwritten=0;
			else
            {
                log_sys("writen error");
            }
		}
		nleft-=nwritten;
		ptr+=nwritten;
	}
	return(count);
}
int Open_m(const char *pathname,int flags,mode_t mode)
{
	int fd;
	if((fd=open(pathname,flags,mode))<0)
	{
        log_sys("open file %s error",pathname);
	}
	return fd;
}

int Open(const char *pathname,int flags)
{
	int fd;
	if((fd=open(pathname,flags))<0)
	{
        log_sys("open file %s error",pathname);
	}
	return fd;
}
int Close(int fd)
{
	int n;
	if((n=close(fd))<0)
	{
        log_sys("close error");
	}
	return n;
}
int Bind(int sockfd,const struct sockaddr *addr,socklen_t addrlen)
{
	int n;
	if((n=bind(sockfd,addr,addrlen))<0)
	{
        log_sys("bind error");
	}
	return n;
}
int Accept(int sockfd,struct sockaddr *addr,socklen_t *addrlen)
{
	int n;
	if((n=accept(sockfd,addr,addrlen))<0)
	{
        log_sys("accept error");
	}
	return n;
}
void* Malloc(size_t size)
{
	void* ptr;
	if((ptr=malloc(size))==NULL)
	{
        log_sys("malloc error");
	}
	return(ptr);
}
void* Calloc(size_t nmemb,size_t size)
{
	void* ptr;
	if((ptr=calloc(nmemb,size))==NULL)
	{
        log_sys("calloc error");
	}
	return(ptr);
}
ssize_t Readline(int fd,void* buf,size_t maxlen)//该函数效率很慢，以后可能会改进
{
	int nread,count=0;
    char ch;
    char *buf_ptr=(char*)buf;
    while(1)
    {
        if(count == maxlen)
            return count;
        nread=read(fd,&ch,1);
        if(nread<0) //nread<0
        {
            if(errno == EINTR)
                continue;
            else
                log_sys("readline error");
        }
        else
            if(nread == 0) //nread = 0
                return count;
            else //nread>0
            {
                buf_ptr[count]=ch;
                count++;
                if('\n' == ch)
                    return count;
            }
    }
}
char* sock_ntop(const struct sockaddr* addr,socklen_t addrlen)
{
	char port[8];
	static char address[128];
	switch(addr->sa_family)
	{
		case AF_INET:
			{
				struct sockaddr_in *sin=(struct sockaddr_in*)addr;
				if(inet_ntop(AF_INET,&sin->sin_addr,address,sizeof(address))==NULL)
					return NULL;
				if(ntohs(sin->sin_port)!=0)
				{
					snprintf(port,sizeof(port),":%d",ntohs(sin->sin_port));
					strcat(address,port);
				}
				return address;
			}
#ifdef IPV6
		case AF_INET6:
			{
				struct sockaddr_in6 *sin6=(struct sockadd_in6*)addr;
				address[0]='[';
				if(inet_ntop(AF_INET6,&sin6->sin_addr,address+1,sizeof(address)-1)==NULL)
					return NULL;
				if(ntohs(sin6->sin_port)!=0)
				{
					snprintf(port,sizeof(port),"]:%d",ntohs(sin6_sin_port));
					strcat(address,port);
					return address;
				}
			}
#endif
#ifdef AF_UNIX
		case AF_UNIX:
			{
				struct sockaddr_un *unp=(struct sockaddr_un*)addr;
				if(unp->sun_path[0]==0)
				{
					strcpy(address,"no pathname bound");
				}
				else
					snprintf(address,sizeof(address),"%s",unp->sun_path);
				return address;
			}
#endif
	}
	return NULL;
}
char *Sock_ntop(const struct sockaddr* addr,socklen_t addrlen)
{
	char *ptr;
	if((ptr=sock_ntop(addr,addrlen))==NULL)
	{
        log_sys("sock_ntop error");
	}
	return ptr;
}

const char* filename(const char *path)
{
	char *ptr;
	if((ptr=strrchr(path,'/'))==NULL)
		return path;
	else
		return ++ptr;
}

// serverpath like 192.168.0.8:8080
const char* sock_address(char* result,const char* serverpath)
{
    const char* ptr=strchr(serverpath,':');
	int length=0;
	if(ptr==NULL)
	{
        return NULL;
	}

    length=ptr-serverpath;
    memcpy(result,serverpath,length);
    result[length]='\0';
	return result;
}
//server like 192.168.0.8:8080
const char* sock_port(char *result, const char *server)
{
    const char* ptr=strrchr(server,':');
    if(ptr==NULL)
    {
        return NULL;
    }
    strcpy(result,ptr+1);
    return result;
}


int set_work_dir(const char *dir)
{
    char buf[256];
    int result;
    if(dir == NULL)
        return -1;
    if(access(dir,F_OK)<0)//dir not exist
    {
        log_msg("dir %s not exist,now create it\n",dir);
        result=mkdir(dir,S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH);
        if(result<0)
        {
            snprintf(buf,256,"create dir %s failed",dir);
            log_err(buf);
            return -1;
        }
        else
        {
            log_msg("create dir %s success\n",dir);
        }
    }
    else  // dir exist
    {
        if(access(dir,W_OK|X_OK)<0)
        {
            snprintf(buf,256,"access to dir %s error",dir);
            log_err(buf);
            return -1;
        }
    }
    if(chdir(dir)<0)
    {
        snprintf(buf,256,"change dir to %s failed",dir);
        log_err(buf);
        return -1;
    }
    else
        return 0;
}

int open_echo()
{
    struct  termios ter_conf;
    int ret;
    ret=tcgetattr(0,&ter_conf);
    if(ret<0)
    {
        return -1;
    }
    ter_conf.c_lflag |= ECHO; //open terminal's echo function
    return tcsetattr(0,TCSAFLUSH,&ter_conf);
}
int close_echo()
{
    struct  termios ter_conf;
    int ret;
    ret=tcgetattr(0,&ter_conf);
    if(ret<0)
    {
        return -1;
    }
    ter_conf.c_lflag &= ~ECHO; //close terminal's echo function
    return tcsetattr(0,TCSAFLUSH,&ter_conf);
}

int file_size_fd(int fd)
{
    struct stat file_stat;
    if(fstat(fd,&file_stat)<0)
    {
        log_err("fstat error");
        return -1;
    }
    return file_stat.st_size;
}

int file_size_path(const char *path)
{
    struct stat file_stat;
    if(stat(path,&file_stat)<0)
    {
        log_err("fstat error");
        return -1;
    }
    return file_stat.st_size;
}

void progress_bar(int progress)
{
    int j=0;
    putchar('\r');
    putchar('[');
    for(j=0;j<progress;++j)
        putchar('+');
    for(j=1;j<=(100-progress);++j)
        putchar('-');
    putchar(']');
    fprintf(stdout," %3d%%",progress);
    fflush(stdout);
    if(progress == 100)
    {
        putchar('\n');
        fflush(stdout);
    }
}

		



	


		





