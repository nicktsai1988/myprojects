#ifndef PARSE_H
#define PARSE_H

#include <stddef.h>
#include <pthread.h>
#include "record_conf.h"
#include "record_utils.h"
#include "urlrecord.h"
struct Pair_t
{
	const char *matchurl;
	const char **wordlist;
	const char *site;
};
struct Url_wordlist_site_t
{
	struct Pair_t array[SEARCH_ENGINE_ARRAY_SIZE];
	size_t count;
};
struct SearchRecord
{
	char full_url[FULL_URL_BUF_SIZE];
	char ip[IP_BUF_SIZE];
};
int addNewPair(struct Url_wordlist_site_t *obj,const char* matchurl,const char** wordlist,const char *site);
size_t pairCount(const struct Url_wordlist_site_t *obj);
int initPairs(struct Url_wordlist_site_t *obj);
int cleanupPairs(struct Url_wordlist_site_t *obj);

void recordWord(const char* time,const char* ip,const char* site,const char* word,const struct RecordConf* record_conf);
void recordUrl(struct UrlRecordList* url_list,pthread_mutex_t* mutex,const char* timeStr,const char* ip,const char* url,const struct RecordConf *record_conf);
/*************************
 * 返回当前时间的字符串表示
 * 返回值：
 * NULL表示失败
 * 否则为当前时间的字符串
 ************************/
const char* currentTimeStr(char* buf,const size_t bufSize);
/***********************
 * wordList为每个URL里面表示搜索关键字的那么字符串
 * 返回值：
 * -1表示解析失败
 *  >0表示成功
 ***********************/
int parseWord(const char* url,char* result,const size_t resultSize,const char** wordList);
/***********************
 * 解析URL,并提取搜索关键字
 * 成功返回1,失败返回0
 * url为要解析的url,obj为url,wordlist和site的集合，ip为客户端的ip
 * *********************/
int parseUrl(struct UrlRecordList* url_list,pthread_mutex_t* mutex,struct SearchRecord* data,const struct Url_wordlist_site_t *obj,const struct RecordConf *record_conf);
/***********************
 * 判断是否是一个搜索的URL
 * 是返回1,不是返回0
 * ********************/
int isSearchUrl(const char* url,const char* match_str);

const char* getMainUrl(const char* fullUrl,char* buf,const size_t bufSize);
int wordEscape(char* result,const size_t result_size,const char *src);
#endif
