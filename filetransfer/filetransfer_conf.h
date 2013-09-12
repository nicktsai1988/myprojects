/*
 * =====================================================================================
 *
 *       Filename:  filetransfer_conf.h
 *
 *    Description:  configure option for filetransfer server
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
#ifndef FILETRANSFER_CONF_H
#define FILETRANSFER_CONF_H

#define CONF_HOST_LEN 256
#define CONF_PORT_LEN 16
#define CONF_WORKDIR_LEN 256
#define CONF_PASSWD_LEN 128
#include<stddef.h>
struct FileTransferConf
{
	char host[CONF_HOST_LEN];
	char port[CONF_PORT_LEN];
	char workdir[CONF_WORKDIR_LEN];
	char passwd[CONF_PASSWD_LEN];
};

extern struct FileTransferConf conf;
//return 1 表示成功读取配置文件，返回0表示读配置文件失败
int filetransfer_conf_read(struct FileTransferConf *conf,const char *file);
void print_conf_option(struct FileTransferConf *conf);
#endif
