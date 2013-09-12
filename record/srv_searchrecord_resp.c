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
#define USER_AGENT_LEN 256
extern struct Url_wordlist_site_t search_engine; 
const char* baidu_matchurl="www.baidu.com/s";
const char* baidu_wordlist[]={"word=","wd=",NULL};
const char* baidu_site="百度搜索";

const char* google_matchurl="www.google.com.hk/search";
const char* google_wordlist[]={"q=","oq=",NULL};
const char* google_site="谷歌搜索";

CI_DECLARE_MOD_DATA ci_service_module_t service = 
{
     "search_record",                         /* mod_name, The module name */
     "Serch record service V1.0",            /* mod_short_descr,  Module short description */
	 ICAP_RESPMOD,     /* mod_type, The service type is responce or request modification */
     search_record_init_service,              /* mod_init_service. Service initialization */
     NULL,                           /* post_init_service. Service initialization after c-icap 
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

/*
  The search_record_req_data structure will store the data required to serve an ICAP request.
*/
/*
static int search_record_data_init(struct search_record_req_data *data)
{
	if((data->full_url = malloc(URL_LEN*sizeof(char))) == NULL)
		return 0;
	if((data->ip = malloc(IP_LEN*sizeof(char))) == NULL)
		return 0;
//	if((data->body = ci_membuf_new()) == NULL)
//		return 0;
	return 1;
}
static void search_record_data_clean(struct search_record_req_data *data)
{
	free(data->full_url);
	free(data->ip);

}
*/
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
	size_t len=strlen(url);
	if(!len)
		return 0;
	if(strnstr(url,".php",len) != NULL)
		return 1;
	if(strnstr(url,".jsp",len) != NULL)
		return 1;
	if(strnstr(url,".html",len) != NULL)
		return 1;
	if(strnstr(url,".aspx",len) != NULL)
		return 1;
	if(strnstr(url,".shtml",len) != NULL)
		return 1;
	if(strnstr(url,".htm",len) != NULL)
		return 1;
	if(strncasestr(url,".jpg",len) != NULL)
		return 0;
	if(strncasestr(url,".png",len) != NULL)
		return 0;
	if(strncasestr(url,".gif",len) != NULL)
		return 0;
	if(strncasestr(url,".js",len) != NULL)
		return 0;
	if(strncasestr(url,".css",len) != NULL)
		return 0;
	if(strncasestr(url,".xml",len) != NULL)
		return 0;
	if(strncasestr(url,".swf",len) != NULL)
		return 0;
	return 1;
}

/* This function will be called when the service loaded  */
int search_record_init_service(ci_service_xdata_t * srv_xdata,
                      struct ci_server_conf *server_conf)
{
     ci_debug_printf(5, "Initialization of Search Record module......\n");
     
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
     return CI_OK;
}

/* This function will be called when the service shutdown */
void search_record_close_service() 
{
	cleanupPairs(&search_engine);
    ci_debug_printf(5,"Search Record Service shutdown!\n");
    /*Nothing to do*/
}

/*This function will be executed when a new request for search_record service arrives. This function will
  initialize the required structures and data to serve the request.
 */
void *search_record_init_request_data(ci_request_t * req)
{
    struct SearchRecord *search_record_data;
	char full_url[URL_LEN];
	char ip[IP_LEN];
	const char* content_type=ci_http_response_get_header(req,"Content-Type");
	int url_state = ci_http_request_full_url(req,full_url,URL_LEN);
	const char* ip_state = get_client_ip(req,ip);
	if((url_state == 0) || (ip_state == NULL))
	{
		ci_debug_printf(2,"could not get full url or client ip\n");
		return NULL;
	}
	if((content_type == NULL) || (strncmp(content_type,"text/html",9) == 0))
	{
		search_record_data = malloc(sizeof(struct SearchRecord));
	    if (!search_record_data) 
		{
			ci_debug_printf(1, "Memory allocation failed inside search_record_init_request_data!\n");
			return NULL;
		}
		strcpy(search_record_data->full_url,full_url);
		strcpy(search_record_data->ip,ip);
		return search_record_data;
	}
	else
		return NULL;
}

		

/*This function will be executed after the request served to release allocated data*/
void search_record_release_request_data(void *data)
{
    /*The data points to the search_record_req_data struct we allocated in function search_record_init_service */
    struct SearchRecord *search_record_data = (struct SearchRecord *)data;
	char url[URL_LEN];
	if(search_record_data == NULL)
		return;
	if(getMainUrl(search_record_data->full_url,url,URL_LEN) == NULL)
	{
		free(search_record_data);
		return;
	}
	ci_debug_printf(5,"request url= %s\n",search_record_data->full_url);
	parseUrl(search_record_data,&search_engine);
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
