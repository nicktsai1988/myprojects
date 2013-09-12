#ifndef URL_RECORD_H
#define URL_RECORD_H
#include "record_conf.h"
#include"record_utils.h"
struct UrlRecordEntry
{
	char url[URL_BUF_SIZE];
	char ip[IP_BUF_SIZE];
	char time[TIME_STR_BUF_SIZE];
};
struct UrlRecordList
{
	struct UrlRecordEntry* array;
	unsigned int count;
	unsigned int size;
};
// TRUE for success ,FALSE for failed
int url_record_list_init(struct UrlRecordList* list,size_t size);
void url_record_list_clean(struct UrlRecordList *list);
void url_record_list_reset(struct UrlRecordList *list);
// TRUE for success ,FALSE for failed
int url_record_list_add(struct UrlRecordList *list,const char* url,const char* ip,const char* time);
// TRUE for success ,FALSE for failed
int url_record_list_copy(struct UrlRecordList *dest,const struct UrlRecordList *src);
// TRUE for success ,FALSE for failed
int url_record_list_write_database(const struct RecordConf* record_conf,struct UrlRecordList* list);
#endif
