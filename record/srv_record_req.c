/*
 *  Copyright (C) 2004-2008 Christos Tsantilas
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA.
 */

#include "common.h"
#include "c-icap.h"
#include "service.h"
#include "header.h"
#include "body.h"
#include "simple_api.h"
#include "debug.h"
#include "parse.h"
//#include "dbinfo.h"
#include "record_conf.h"
#include "record_utils.h"
#include "urlrecord.h"
#include <pthread.h>

#include<sys/types.h>
#include<signal.h>
int search_record_init_service(ci_service_xdata_t * srv_xdata,
                      struct ci_server_conf *server_conf);
int search_record_check_preview_handler(char *preview_data, int preview_data_len,
                               ci_request_t *);
int search_record_end_of_data_handler(ci_request_t * req);
void *search_record_init_request_data(ci_request_t * req);
void search_record_close_service();
void search_record_release_request_data(void *data);
int search_record_io(char *wbuf, int *wlen, char *rbuf, int *rlen, int iseof,
            ci_request_t * req);

//nick added
#define USER_AGENT_BUF_SIZE 256
#define PATH_LEN 1024

int search_record_post_init_service(ci_service_xdata_t *srv_data,struct ci_server_conf *server_conf);
static struct Url_wordlist_site_t search_engine; 
static struct RecordConf record_conf;
static pthread_mutex_t url_mutex;
static struct UrlRecordList url_list;

const char* baidu_matchurl="www.baidu.com/s";
const char* baidu_wordlist[]={"word=","wd=",NULL};
const char* baidu_site="百度搜索";

const char* google_matchurl="www.google.com.hk/search";
const char* google_wordlist[]={"q=","oq=",NULL};
const char* google_site="谷歌搜索";

CI_DECLARE_MOD_DATA ci_service_module_t service = 
{
     "record",                         /* mod_name, The module name */
     "Record service V1.0",            /* mod_short_descr,  Module short description */
	 ICAP_REQMOD,     /* mod_type, The service type is responce or request modification */
     search_record_init_service,              /* mod_init_service. Service initialization */
     search_record_post_init_service,                           /* post_init_service. Service initialization after c-icap 
					configured. Not used here */
     search_record_close_service,           /* mod_close_service. Called when service shutdowns. */
     search_record_init_request_data,         /* mod_init_request_data */
     search_record_release_request_data,      /* mod_release_request_data */
     search_record_check_preview_handler,     /* mod_check_preview_handler */
     search_record_end_of_data_handler,       /* mod_end_of_data_handler */
     search_record_io,                        /* mod_service_io */
     NULL,
     NULL
};

static void print_record_conf(const struct RecordConf* conf)
{
	ci_debug_printf(5,"db_host: %s\n",conf->db_host);
	ci_debug_printf(5,"db_name: %s\n",conf->db_name);
	ci_debug_printf(5,"db_user: %s\n",conf->db_user);
	ci_debug_printf(5,"db_passwd: %s\n",conf->db_passwd);
	ci_debug_printf(5,"except_url: %s\n",conf->except_url);
	ci_debug_printf(5,"db_port: %u\n",conf->db_port);
	ci_debug_printf(5,"url_record_size: %u\n",conf->url_record_size);
	ci_debug_printf(5,"record_full_url: %d\n",conf->record_full_url);
}
static void read_conf(struct RecordConf *record_conf,const struct ci_server_conf* server_conf)
{
	char conf_path[PATH_LEN];
	ci_debug_printf(5,"record service read configure file\n");
	char *dir_end=strrchr(server_conf->cfg_file,'/');
	size_t length=dir_end-server_conf->cfg_file;
	memcpy(conf_path,server_conf->cfg_file,length);
	conf_path[length]='\0';
	strcat(conf_path,"/record.conf");
	ci_debug_printf(5,"configure file of record service: %s\n",conf_path);
	if(!record_conf_read(record_conf,conf_path))
	{
		ci_debug_printf(2,"record service read configure file error\n");
		kill(getpid(),SIGINT);
	}
}

static const char* get_client_ip(ci_request_t* req,char* result)
{
	const char* ip;
	if((ip = ci_http_request_get_header(req,"X-Forwarded-For")) == NULL)
	{
		ip = ci_headers_value(req->request_header,"X-Client-IP");
		if(ip == NULL)
		return NULL;
	}
	strcpy(result,ip);
	return result;
}

static int is_html(const char* url)
{
	char extention[7];
	size_t len=strlen(url);
	if(len < 6)
		return 0;
	extention[6]='\0';
	//get the last 6 chars
	strcpy(extention,url+(len-6));
	if(strnstr(extention,".php",6) != NULL)
		return 1;
	if(strnstr(extention,".jsp",6) != NULL)
		return 1;
	if(strnstr(extention,".html",6) != NULL)
		return 1;
	if(strnstr(extention,".aspx",6) != NULL)
		return 1;
	if(strnstr(extention,".shtml",6) != NULL)
		return 1;
	if(strnstr(extention,".htm",6) != NULL)
		return 1;
	if(strncasestr(extention,".jpg",6) != NULL)
		return 0;
	if(strncasestr(extention,".png",6) != NULL)
		return 0;
	if(strncasestr(extention,".gif",6) != NULL)
		return 0;
	if(strncasestr(extention,".js",6) != NULL)
		return 0;
	if(strncasestr(extention,".css",6) != NULL)
		return 0;
	if(strncasestr(extention,".xml",6) != NULL)
		return 0;
	if(strncasestr(extention,".swf",6) != NULL)
		return 0;
	if(strncasestr(extention,".ico",6) != NULL)
		return 0;
	return 1;
}
static int is_except(const char* url,const struct RecordConf *record_conf)
{
	if(strnstr(url,record_conf->except_url,strlen(url)) != NULL)
		return 1;
	return 0;
}
static void flush_url_records(const struct RecordConf* record_conf,struct UrlRecordList *list)
{
	url_record_list_write_database(record_conf,list);
}

/* This function will be called when the service loaded  */
int search_record_init_service(ci_service_xdata_t * srv_xdata,
                      struct ci_server_conf *server_conf)
{
     ci_debug_printf(5, "Initialization of Record module......\n");
     
     /*Tell to the icap clients that we can support up to 1024 size of preview data*/
     ci_service_set_preview(srv_xdata, 128);

     /*Tell to the icap clients that we support 204 responses*/
     ci_service_enable_204(srv_xdata);

     /*Tell to the icap clients to send preview data for all files*/
     ci_service_set_transfer_preview(srv_xdata, "*");

     /*Tell to the icap clients that we want the X-Authenticated-User and X-Authenticated-Groups headers
       which contains the username and the groups in which belongs.  */
     /*ci_service_set_xopts(srv_xdata,  CI_XAUTHENTICATEDUSER|CI_XAUTHENTICATEDGROUPS);*/
     
	 initPairs(&search_engine);
	 addNewPair(&search_engine,baidu_matchurl,baidu_wordlist,baidu_site);
	 addNewPair(&search_engine,google_matchurl,google_wordlist,google_site);
	 read_conf(&record_conf,server_conf);
	 print_record_conf(&record_conf); 
	 if(!url_record_list_init(&url_list,record_conf.url_record_size))
		 return CI_ERROR;
     return CI_OK;
}

int search_record_post_init_service(ci_service_xdata_t *srv_data,struct ci_server_conf *server_conf)
{

	ci_debug_printf(5,"start init url_mutex in process %d\n",getpid());
	int res=pthread_mutex_init(&url_mutex,NULL);
	if(res)
	{
		ci_debug_printf(5,"init url_mutex error\n");
		kill(getpid(),SIGINT);
	}
	ci_debug_printf(5,"finished init url_mutex in process %d\n",getpid());
	return CI_OK;
}

/* This function will be called when the service shutdown */
void search_record_close_service() 
{
	int res=pthread_mutex_destroy(&url_mutex);
	if(res)
	{
		ci_debug_printf(5,"destroy url_mutex error\n");
	}
	flush_url_records(&record_conf,&url_list);
	ci_debug_printf(5,"flush_record success\n");
	cleanupPairs(&search_engine);
	url_record_list_clean(&url_list);
    ci_debug_printf(5,"Search Record Service shutdown!\n");
}

/*This function will be executed when a new request for search_record service arrives. This function will
  initialize the required structures and data to serve the request,if function return NULL,preview_handler will return allow_204
 */
void *search_record_init_request_data(ci_request_t * req)
{
	ci_debug_printf(5,"search_record_init_request_data()\n");
    struct SearchRecord *search_record_data;
	char full_url[FULL_URL_BUF_SIZE];
	char ip[IP_BUF_SIZE];
	char user_agent[USER_AGENT_BUF_SIZE];
	const char* accept;
	int url_state = ci_http_request_full_url(req,full_url,FULL_URL_BUF_SIZE);
	const char* ip_state = get_client_ip(req,ip);
	if((url_state == 0) || (ip_state == NULL))
	{
		ci_debug_printf(2,"could not get full url or client ip\n");
		return NULL;
	}
	//just check it like this for chrome or firefox
	if(ci_headers_copy_value(ci_http_request_headers(req),"User-Agent",user_agent,USER_AGENT_BUF_SIZE -1) != NULL)
	{
		if((strstr(user_agent,"Chrome") != NULL) || (strstr(user_agent,"Firefox") != NULL))
		{
			accept=ci_http_request_get_header(req,"Accept");
			if((accept == NULL) || strncmp(accept,"text/html",9) != 0)
			{
				return NULL;
			}
		}

	}
    /*Allocate memory fot the search_record_data*/
    search_record_data = malloc(sizeof(struct SearchRecord));
    if (!search_record_data) 
	{
		ci_debug_printf(1, "Memory allocation failed inside search_record_init_request_data!\n");
		return NULL;
	}
	//user agent is null or neither chrome nor firefox
	strcpy(search_record_data->full_url,full_url);
	strcpy(search_record_data->ip,ip);
    /*Return to the c-icap server the allocated data*/
    return search_record_data;
}

/*This function will be executed after the request served to release allocated data*/
void search_record_release_request_data(void *data)
{
	ci_debug_printf(5,"search_record_release_request_data()\n");
    /*The data points to the search_record_req_data struct we allocated in function search_record_init_service */
    struct SearchRecord *search_record_data = (struct SearchRecord *)data;
	char url[URL_BUF_SIZE];
	if(search_record_data == NULL)
		return;
	if(getMainUrl(search_record_data->full_url,url,URL_BUF_SIZE) == NULL)
	{
		free(search_record_data);
		return;
	}
	if(is_except(url,&record_conf) || !is_html(url))
	{
		free(search_record_data);
		return;
	}
	parseUrl(&url_list,&url_mutex,search_record_data,&search_engine,&record_conf);
    /*if we had body data, release the related allocated data*/
	free(search_record_data);
}


int search_record_check_preview_handler(char *preview_data, int preview_data_len,
                               ci_request_t * req)
{
	return CI_MOD_ALLOW204;
}

/* This function will called if we returned CI_MOD_CONTINUE in  search_record_check_preview_handler
 function, after we read all the data from the ICAP client*/
int search_record_end_of_data_handler(ci_request_t * req)
{
    /*and return CI_MOD_DONE */
     return CI_MOD_DONE;
}

/* This function will called if we returned CI_MOD_CONTINUE in  search_record_check_preview_handler
   function, when new data arrived from the ICAP client and when the ICAP client is 
   ready to get data.
*/
int search_record_io(char *wbuf, int *wlen, char *rbuf, int *rlen, int iseof,
            ci_request_t * req)
{

     return CI_OK;
}
