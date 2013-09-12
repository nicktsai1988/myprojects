#define _GNU_SOURCE
#include <string.h>
#include <mysql/mysql.h>
#include <time.h>
#include "urldecode.h"
#include "parse.h"
#include "urlrecord.h"
#include "debug.h"


void recordWord(const char* time,const char* ip,const char *site,const char* word,const struct RecordConf* record_conf)
{
	int res;
	const char* format="INSERT INTO wordrecord(time,ip,site,word) VALUES (\"%s\",\"%s\",\"%s\",\"%s\")";
	MYSQL handler;
	ci_debug_printf(2,"<%s><%s><%s><%s>\n",time,ip,site,word);
	mysql_init(&handler);
	if(mysql_real_connect(&handler,record_conf->db_host,record_conf->db_user,record_conf->db_passwd,record_conf->db_name,record_conf->db_port,record_conf->db_socket,0))
	{
		ci_debug_printf(5,"recordWord(): connect to database success\n");
		res=mysql_query(&handler,"set names 'utf8'");//set encoding as utf8
		if(res)
		{
			ci_debug_printf(2,"recordWord(): set encoding error %d: %s\n",mysql_errno(&handler),mysql_error(&handler));
		}
		else//set encoding success
		{
			char buf[MYSQL_BUF_SIZE];
			char esc_word[SEARCH_WORD_BUF_SIZE];
			res=wordEscape(esc_word,SEARCH_WORD_BUF_SIZE,word);
			if(!res)
			{
				ci_debug_printf(2,"wordEscape error\n");
				mysql_close(&handler);
				return;
			}
			snprintf(buf,MYSQL_BUF_SIZE,format,time,ip,site,esc_word);
			res=mysql_query(&handler,buf);
			if(!res)
			{
				ci_debug_printf(5,"recordWord(): insert word <%s><%s> success\n",ip,word);
				ci_debug_printf(5,"sql script = %s\n",buf);
			}
			else
			{
				ci_debug_printf(2,"recordWord(): insert word failed %d: %s\n",mysql_errno(&handler),mysql_error(&handler));
			}
		}
		mysql_close(&handler);
	}
	else
	{
		ci_debug_printf(2,"recordWord(): connect to database failed %d: %s\n",mysql_errno(&handler),mysql_error(&handler));
	}
}
void recordUrl(struct UrlRecordList* url_list,pthread_mutex_t* mutex,const char *timeStr,const char* ip,const char *url,const struct RecordConf* record_conf)
{

	ci_debug_printf(2,"<%s><%s><%s>\n",timeStr,ip,url);
	ci_debug_printf(5,"I will try to get mutex lock\n");
	pthread_mutex_lock(mutex);
	ci_debug_printf(5,"I have got mutex lock\n");
	url_record_list_add(url_list,url,ip,timeStr);
	if(url_list->count >= url_list->size)
	{
		int result;
		ci_debug_printf(5,"url record_list is full,so write them to database\n");
		struct UrlRecordList urls;
		result=url_record_list_init(&urls,record_conf->url_record_size);
		if(!result)
		{
			return;
		}
		if(!url_record_list_copy(&urls,url_list))
		{
			url_record_list_clean(&urls);
			return;
		}
		url_record_list_reset(url_list);
		pthread_mutex_unlock(mutex);
		result = url_record_list_write_database(record_conf,&urls);
		if(!result)
		{
			ci_debug_printf(2,"write database failed\n");
		}
		else
		{
			ci_debug_printf(5,"write database success\n");
		}
		url_record_list_clean(&urls);
	}
	else
	{
		pthread_mutex_unlock(mutex);
	}
	ci_debug_printf(5,"I have free mutex lock\n");
}

const char* currentTimeStr(char *buf,const size_t bufSize)
{
	time_t cur;
	struct tm mytime;
	const char* format="%04d-%02d-%02d,%02d:%02d:%02d";
	struct tm *tm_ptr;
	if(bufSize < MIN_TIME_STR_SIZE)
		return NULL;
	if(time(&cur) == (time_t)-1)
		return NULL;
	tm_ptr=localtime_r(&cur,&mytime);
	if(tm_ptr == NULL)
		return NULL;
	snprintf(buf,bufSize,format,mytime.tm_year+1900,mytime.tm_mon+1,mytime.tm_mday,mytime.tm_hour,mytime.tm_min,mytime.tm_sec);
		return buf;
}
int parseWord(const char* url,char* result,const size_t resultSize,const char** wordList)
{
	const char *start=NULL;
	char *end=NULL;
	char url_key[SEARCH_WORD_BUF_SIZE];
	size_t len;
	int i=0;
	if((url == NULL) || strlen(url) == 0 || (result == NULL) || (resultSize == 0) || (wordList == NULL) ||(wordList[0] == NULL))
		return -1;
	for(i=0;wordList[i] != NULL;i++)
	{
		if((start=strstr(url,wordList[i])) != NULL)
		{
			start+=strlen(wordList[i]);
			break;
		}
	}
	if(start == NULL)
		return -1;
	ci_debug_printf(5,"in ParseWord begin\n");
	end=strchr(start,'&');//IE中的google可能以'\0'结尾,别的浏览器的都是以&结尾
	if(end)
	{
		len = end-start;
	}
	else
		len=strlen(start);
	memcpy(url_key,start,len);
	url_key[len]='\0';
	ci_debug_printf(5,"in ParseWord end\n");
	return urlDecode(url_key,result,resultSize);
}
int addNewPair(struct Url_wordlist_site_t *obj,const char* matchurl,const char** wordlist,const char *site)
{
	if(obj->count >= SEARCH_ENGINE_ARRAY_SIZE)
		return -1;
	obj->array[obj->count].matchurl = matchurl;
	obj->array[obj->count].wordlist = wordlist;
	obj->array[obj->count].site = site;
	obj->count++;
	return obj->count;
}
size_t pairCount(const struct Url_wordlist_site_t *obj)
{
	return obj->count;
}
int initPairs(struct Url_wordlist_site_t *obj)
{
	obj->count=0;
	return obj->count;
}
int cleanupPairs(struct Url_wordlist_site_t *obj)
{
	obj->count=0;
	return obj->count;
}
int isSearchUrl(const char* url,const char* match_str)
{
	return (strstr(url,match_str) == NULL)?0:1;
}
int parseUrl(struct UrlRecordList* url_list,pthread_mutex_t* mutex,struct SearchRecord* data,const struct Url_wordlist_site_t *obj,const struct RecordConf *record_conf)
{
	size_t i;
	size_t size=pairCount(obj);
	char searchword[SEARCH_WORD_BUF_SIZE];
	char url[URL_BUF_SIZE];
	char timestr[TIME_STR_BUF_SIZE];
	int result;
	ci_debug_printf(5,"in parseUrl\n");
	if(getMainUrl(data->full_url,url,URL_BUF_SIZE) == NULL)
		return 0;
	if(currentTimeStr(timestr,sizeof(timestr)) == NULL)
		return 0;
	if(record_conf->record_word)
	{
		for(i=0;i<size;++i)
		{
			if(isSearchUrl(url,obj->array[i].matchurl))
			{
				result=parseWord(data->full_url,searchword,sizeof(searchword),obj->array[i].wordlist);
				if(result > 0)
				{
					recordWord(timestr,data->ip,obj->array[i].site,searchword,record_conf);
				}
			}
		}
	}
	if(record_conf->record_url)
	{
		if(!record_conf->full_url)
		{
			recordUrl(url_list,mutex,timestr,data->ip,url,record_conf);//record main url
		}
		else
		{
			recordUrl(url_list,mutex,timestr,data->ip,data->full_url,record_conf);//record full url
		}
	}
	return 1;
}
const char* getMainUrl(const char * fullUrl,char* buf,const size_t bufSize)
{
	const char *end;
	size_t len;
	if(fullUrl == NULL)
		return NULL;
	end=strchr(fullUrl,'?');
	if(end == NULL)
		len=strlen(fullUrl);
	else
		len=end-fullUrl;
	if(len >= bufSize)
	{
		memcpy(buf,fullUrl,bufSize-1);
		buf[bufSize -1]='\0';
		return buf;
	}
	memcpy(buf,fullUrl,len);
	buf[len]='\0';
	return buf;
}
int wordEscape(char* result,const size_t result_size,const char* src)
{
	size_t i = 0;
	const char* index=src;
	if((result == NULL) || (result_size == 0) || (src == NULL))
	{
		ci_debug_printf(2,"wordEscape() parameters error\n");
		return 0;
	}
	while((i<result_size)&&(*index))
	{
		if(*index != '\"')
		{
			result[i++]=*index;
		}
		else
		{
			result[i++]='\\';
			result[i++]='\"';
		}
		index++;
	}
	if(i == result_size)
	{
		ci_debug_printf(2,"wordEscape() need more space\n");
		return 0;
	}
	result[i]='\0';
	return 1;
}
