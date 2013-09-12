/*
 * =====================================================================================
 *
 *       Filename:  record_conf.h
 *
 *    Description:  configure option for c-icap record service
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
#ifndef RECORD_CONF_H
#define RECORD_CONF_H
#include<stddef.h>
struct RecordConf
{
	//database option
	char db_host[128+1];//数据库主机
	char db_name[32+1];//数据库名
	char db_user[32+1];//用户名
	char db_passwd[32+1];//用户密码
	char db_socket[256+1]; //数据库socket文件的位置
	unsigned int db_port;//mysql端口号
	//record option
	int full_url; 
	unsigned int url_record_size;
	char except_url[256+1];
	//record switch
	int record_url;
	int record_word;
};
//return 1表示成功读取配置文件，返回0表示读配置文件失败
int record_conf_read(struct RecordConf *conf,const char *file);
#endif
