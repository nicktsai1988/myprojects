#include<mysql/mysql.h>
#include<string.h>
#include<stdlib.h>
#include"urlrecord.h"
#include"debug.h"
int url_record_list_init(struct UrlRecordList *list,size_t size)
{
	list->array=malloc(sizeof(struct UrlRecordEntry)*size);
	if(list->array == NULL)
	{
		ci_debug_printf(2,"url_record_list_init() error: allocate failed\n");
		return FALSE;
	}
	list->size = size;
	list->count =0;
	return TRUE;
}
void url_record_list_clean(struct UrlRecordList *list)
{
	free(list->array);
	list->count = 0;
	list->size = 0;
}

void url_record_list_reset(struct UrlRecordList *list)
{
	list->count=0;
}
int url_record_list_add(struct UrlRecordList *list,const char* url,const char* ip,const char* time)
{
	struct UrlRecordEntry *entry = &(list->array[list->count]);
	if((!ip) || (!url) || (!time))
		return FALSE;
	if(list->count >= list->size)
	{
		ci_debug_printf(2,"url_record_list_add() error:it have %u url entries\n",list->count);
		return FALSE;
	}
	strcpy(entry->url,url);
	strcpy(entry->ip,ip);
	strcpy(entry->time,time);
	list->count++;
	return TRUE;
}
int url_record_list_copy(struct UrlRecordList *dest,const struct UrlRecordList *src)
{
	if(!dest || !src)
		return FALSE;
	if(src->count > dest->size)
	{
		ci_debug_printf(2,"url_record_list_copy() error:dest don't have enough space to store data\n");
		return FALSE;
	}
	if(!src->count)
		return TRUE;
	dest->count=src->count;
	memcpy((void*)dest->array,(void*)src->array,sizeof(struct UrlRecordEntry)*src->count);
	return TRUE;
}
int url_record_list_write_database(const struct RecordConf *record_conf,struct UrlRecordList *list)
{
	if(list->count == 0)
		return TRUE;
	size_t count=list->count;
	const char *format="INSERT INTO urlrecord(time,ip,url) VALUES (\"%s\",\"%s\",\"%s\")";
	MYSQL handler;
	int res,ret;
	unsigned int i;
	mysql_init(&handler);
	if(mysql_real_connect(&handler,record_conf->db_host,record_conf->db_user,record_conf->db_passwd,record_conf->db_name,record_conf->db_port,record_conf->db_socket,0))
	{
		ci_debug_printf(5,"connect database success\n");
		char buf[MYSQL_BUF_SIZE];
		res=mysql_query(&handler,"set names 'utf8'");
		if(res)
		{
			ci_debug_printf(2,"url_record_list_write_database() error:set encoding failed\n");
			ret=FALSE;
		}
		else
		{

			for(i=0;i < count;++i)
			{
				snprintf(buf,MYSQL_BUF_SIZE,format,list->array[i].time,list->array[i].ip,list->array[i].url);
				res=mysql_query(&handler,buf);
				if(res)
				{
					ci_debug_printf(1,"insert to database failed in %u : %s\n",i,buf);
					ci_debug_printf(1,"mysql errro %d: %s\n",mysql_errno(&handler),mysql_error(&handler));
		//			mysql_close(&handler);
		//			return FALSE;
		//			continue write urls to database when an error accured
				}
			}
			ret=TRUE;
		}
		mysql_close(&handler);
		return ret;
	}
	ci_debug_printf(2,"connec to databese error %d: %s\n",mysql_errno(&handler),mysql_error(&handler));
	return FALSE;
}
