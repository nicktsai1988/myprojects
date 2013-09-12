/*
 * =====================================================================================
 *
 *       Filename:  filetransfer_conf.c
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
#include"filetransfer_conf.h"
#include"utils.h"
#include<string.h>
#include<ctype.h>
#include<syslog.h>

#define CONF_ENTRY_SIZE 12
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
//global configure option
struct FileTransferConf conf;

//function declaration and definition
static void conf_entry_list_init(struct ConfEntryList *list);
static int conf_entry_list_add(struct ConfEntryList *list,const char*name,const char *value);
static void conf_entry_list_clean(struct ConfEntryList *list);
static int conf_parse_line(char* buf,char** name,char**value);
static char* remove_blank(char *buf);
static const struct ConfEntry* find_entry(const struct ConfEntryList *list,const char *name);

int filetransfer_conf_read(struct FileTransferConf *conf,const char* file)
{
	struct ConfEntryList conf_list;
	char buf[READ_BUF_SIZE];
	char *name,*value;
	const struct ConfEntry *entry;

    FILE *fs=fopen(file,"r");
	if(!fs)
	{
        log_err("filetransfer_conf_read error: open configure file %s failed",file);
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
		if(conf_parse_line(buf,&name,&value) == 0)
			continue;//parse failed
		if(!conf_entry_list_add(&conf_list,name,value))// add to list
		{
            log_msg("add to conf list error");
			conf_entry_list_clean(&conf_list);//clean it
			return 0;//add error
		}
	}
	fclose(fs);
	//read configure file end
	//start to configure
	
	entry=find_entry(&conf_list,"host");
	if(entry)
	{
		strcpy(conf->host,entry->value);
	}
	else
	{
		strcpy(conf->host,"all");
	}
	entry=find_entry(&conf_list,"port");
	if(entry)
	{
		strcpy(conf->port,entry->value);
	}
	else
	{
		//TODO:
		strcpy(conf->port,"9877");
	}
	entry=find_entry(&conf_list,"workdir");
	if(entry)
	{
		strcpy(conf->workdir,entry->value);
	}
	else
	{
	//	TODO:
		strcpy(conf->workdir,"/tmp");
	}
	entry=find_entry(&conf_list,"passwd");
	if(entry)
	{
		strcpy(conf->passwd,entry->value);
	}
	else
	{
		//TODO:
		strcpy(conf->passwd,"passwd");
	}
	conf_entry_list_clean(&conf_list);
	return 1;
}

void print_conf_option(struct FileTransferConf *conf)
{
    log_msg("host=%s",conf->host);
    log_msg("port=%s",conf->port);
    log_msg("workdir=%s",conf->workdir);
    log_msg("passwd=%s",conf->passwd);
}


//in general,the following functions don't need to modify

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
        log_msg("conf_entry_list_add() error: entrys overflow()");
		return 0;
	}
	len=strlen(name)+1;
	name_ptr=malloc(sizeof(char)*len);
	if(!name_ptr)
	{
        log_err("conf_entry_list_add() error: allocated memory failed");
		return 0;
	}
	strcpy(name_ptr,name);
	len=strlen(value)+1;
	value_ptr=malloc(sizeof(char)*len);
	if(!value_ptr)
	{
		free(name_ptr);
        log_err("conf_entry_list_add() error: allocated memory failed");
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
static int conf_parse_line(char* buf,char** name,char**value)
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
