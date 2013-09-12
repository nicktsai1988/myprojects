/*
 * =====================================================================================
 *
 *       Filename:  record_conf.c
 *
 *    Description:  configure option for c-icap record service
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
#include"record_conf.h"
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include"debug.h"

#define CONF_ENTRY_SIZE 32
#define READ_BUF_SIZE 1024
struct ConfEntry
{
	char *name;
	char *value;
};
struct ConfEntryList
{
	struct ConfEntry entrys[CONF_ENTRY_SIZE];
	size_t count;
};

//function declaration and definition
static void conf_entry_list_init(struct ConfEntryList *list);
static int conf_entry_list_add(struct ConfEntryList *list,const char*name,const char *value);
static void conf_entry_list_clean(struct ConfEntryList *list);
static int record_conf_parse_line(char* buf,char** name,char**value);
static char* remove_blank(char *buf);
static const struct ConfEntry* find_entry(const struct ConfEntryList *list,const char *name);

/*
int record_conf_init(struct RecordConf *conf)
{
	if(!conf)
		return 0;
	conf->db_socket=NULL;
	return 1;
}
void record_conf_clean(struct RecordConf *conf)
{
	free(conf->db_socket);
}
*/
int record_conf_read(struct RecordConf *conf,const char* file)
{
	struct ConfEntryList conf_list;
	char buf[READ_BUF_SIZE];
	FILE *fs=fopen(file,"r");
	char *name,*value;
	const struct ConfEntry *entry;
	if(!fs)
	{
			ci_debug_printf(2,"service record error: open configure file failed\n");
		return 0;
	}
	conf_entry_list_init(&conf_list);
	while(fgets(buf,READ_BUF_SIZE,fs) != NULL)
	{
		buf[strlen(buf)-1]='\0'; //get rid of the '\n' of the end of line
		if(remove_blank(buf) == NULL)
			continue; // a blank line
		if(buf[0] == '#')
			continue; //a comment line
		if(record_conf_parse_line(buf,&name,&value) == 0)
			continue;//parse failed
		if(!conf_entry_list_add(&conf_list,name,value))// add to list
		{
			ci_debug_printf(2,"add to conf list error\n");
			conf_entry_list_clean(&conf_list);//clean it
			return 0;//add error
		}
	}
	fclose(fs);
//	ci_debug_printf(5,"the count = %lu\n",conf_list.count);
	//read configure file end
	//start to configure
	
	entry=find_entry(&conf_list,"db_host");
	if(entry)
	{
		strcpy(conf->db_host,entry->value);
	}
	else
	{
		strcpy(conf->db_host,"localhost");
	}
	entry=find_entry(&conf_list,"db_name");
	if(entry)
	{
		strcpy(conf->db_name,entry->value);
	}
	else
	{
		//TODO:
		strcpy(conf->db_name,"test");
	}
	entry=find_entry(&conf_list,"db_user");
	if(entry)
	{
		strcpy(conf->db_user,entry->value);
	}
	else
	{
	//	TODO:
		strcpy(conf->db_user,"root");
	}
	entry=find_entry(&conf_list,"db_passwd");
	if(entry)
	{
		strcpy(conf->db_passwd,entry->value);
	}
	else
	{
		//TODO:
		strcpy(conf->db_passwd,"wafer");
	}
	entry=find_entry(&conf_list,"except_url");
	if(entry)
	{
		strcpy(conf->except_url,entry->value);
	}
	else
	{
		//TODO:
		strcpy(conf->except_url,"10.0.0.1");
	}
	entry=find_entry(&conf_list,"db_port");
	if(entry)
	{
		conf->db_port = atoi(entry->value);
	}
	else
	{
		conf->db_port = 0;//default
	}
	entry=find_entry(&conf_list,"url_record_size");
	if(entry)
	{
		conf->url_record_size = atoi(entry->value);
	}
	else
	{
		conf->url_record_size = 128;//default
	}
	entry=find_entry(&conf_list,"full_url");
	if(entry)
	{
		conf->full_url = !strcmp(entry->value,"on");
	}
	else
	{
		conf->full_url = 0;//default
	}
	entry=find_entry(&conf_list,"db_socket");
	if(entry)
	{
		strcpy(conf->db_socket,entry->value);
	}
	else
	{
		strcpy(conf->db_socket,"/var/run/mysqld/mysqld.sock");
	}
	entry=find_entry(&conf_list,"record_url");
	if(entry)
	{
		conf->record_url = !strcmp(entry->value,"on");
	}
	else
	{
		conf->record_url=1;
	}
	entry=find_entry(&conf_list,"record_word");
	if(entry)
	{
		conf->record_word = !strcmp(entry->value,"on");
	}
	else
	{
		conf->record_word=1;
	}
	conf_entry_list_clean(&conf_list);
	return 1;
}

static void conf_entry_list_init(struct ConfEntryList *list)
{
	size_t i;
	list->count = 0;
	for(i=0;i<CONF_ENTRY_SIZE;++i)
	{
		list->entrys[i].name=NULL;
		list->entrys[i].value=NULL;
	}
}
static int conf_entry_list_add(struct ConfEntryList *list,const char*name,const char *value)
{
	size_t len;
	char *name_ptr,*value_ptr;
	if(list->count >= CONF_ENTRY_SIZE)
	{
		ci_debug_printf(2,"conf_entry_list_add() error: entrys overflow()\n");
		return 0;
	}
	len=strlen(name)+1;
	name_ptr=malloc(sizeof(char)*len);
	if(!name_ptr)
	{
		ci_debug_printf(2,"conf_entry_list_add() error: allocated memory failed\n");
		return 0;
	}
	strcpy(name_ptr,name);
	len=strlen(value)+1;
	value_ptr=malloc(sizeof(char)*len);
	if(!value_ptr)
	{
		free(name_ptr);
		ci_debug_printf(2,"conf_entry_list_add() error: allocated memory failed\n");
		return 0;
	}
	strcpy(value_ptr,value);
	list->entrys[list->count].name=name_ptr;
	list->entrys[list->count].value=value_ptr;
	list->count++;
	return 1;
}
static void conf_entry_list_clean(struct ConfEntryList *list)
{
	size_t i;
	for(i=0;i<list->count;++i)
	{
		free(list->entrys[i].name);
		list->entrys[i].name=NULL;
		free(list->entrys[i].value);
		list->entrys[i].value=NULL;
	}
	list->count=0;
}
static int record_conf_parse_line(char* buf,char** name,char**value)
{
	char *index;
	index=buf;
	while((*index != '=') && (*index !='\0'))
		index++;
	if(*index == '=')
	{	*index='\0';
		*name=buf;
		index++;
		*value=index;
		return 1;
	}
	else
		return 0;
}
static char* remove_blank(char *buf)
{
	
	char *index,*cur;
	if(buf[0] == '\0')
		return NULL;
	index=cur=buf;
	while(*cur)
	{
		if(!isblank(*cur))
		{
			*index=*cur;
			index++;
		}
		cur++;
	}
	*index='\0';
	if(buf[0] == '\0')
		return NULL;
	else
		return buf;
}
static const struct ConfEntry* find_entry(const struct ConfEntryList *list,const char *name)
{
	size_t count=list->count;
	size_t i;
	for(i=0;i<count;++i)
	{
		if(strcmp(list->entrys[i].name,name) == 0)
			return &(list->entrys[i]);
	}
	return NULL;
}
