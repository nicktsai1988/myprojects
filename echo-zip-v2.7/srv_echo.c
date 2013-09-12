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
#include <sys/types.h>
#include <signal.h>
////////////////////////
#include"echodata.h"
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
int echo_init_service(ci_service_xdata_t * srv_xdata,
                      struct ci_server_conf *server_conf);
int echo_check_preview_handler(char *preview_data, int preview_data_len,
                               ci_request_t *);
int echo_end_of_data_handler(ci_request_t * req);
void *echo_init_request_data(ci_request_t * req);
void echo_close_service();
void echo_release_request_data(void *data);
int echo_io(char *wbuf, int *wlen, char *rbuf, int *rlen, int iseof,
            ci_request_t * req);

//nick add
#define SCRIPT_LEN 2048
#define USER_AGENT_LEN 512
void print_headers(void* data,const char * header_name,const char* header_value);

CI_DECLARE_MOD_DATA ci_service_module_t service = 
{
     "echo",                         /* mod_name, The module name */
     "Echo demo service V2.7",            /* mod_short_descr,  Module short description */
     ICAP_RESPMOD | ICAP_REQMOD,     /* mod_type, The service type is responce or request modification */
     echo_init_service,              /* mod_init_service. Service initialization */
     NULL,                           /* post_init_service. Service initialization after c-icap 
					configured. Not used here */
     echo_close_service,           /* mod_close_service. Called when service shutdowns. */
     echo_init_request_data,         /* mod_init_request_data */
     echo_release_request_data,      /* mod_release_request_data */
     echo_check_preview_handler,     /* mod_check_preview_handler */
     echo_end_of_data_handler,       /* mod_end_of_data_handler */
     echo_io,                        /* mod_service_io */
     NULL,
     NULL
};

/*
  The echo_req_data structure will store the data required to serve an ICAP request.
*/

static char script[SCRIPT_LEN];

//static FILE *page;
/* This function will be called when the service loaded  */
int echo_init_service(ci_service_xdata_t * srv_xdata,
                      struct ci_server_conf *server_conf)
{
     ci_debug_printf(5, "Initialization of echo module......\n");
     
     /*Tell to the icap clients that we can support up to 1024 size of preview data*/
     ci_service_set_preview(srv_xdata, 1024);

     /*Tell to the icap clients that we support 204 responses*/
     ci_service_enable_204(srv_xdata);

     /*Tell to the icap clients to send preview data for all files*/
     ci_service_set_transfer_preview(srv_xdata, "*");

     /*Tell to the icap clients that we want the X-Authenticated-User and X-Authenticated-Groups headers
       which contains the username and the groups in which belongs.  */
     ci_service_set_xopts(srv_xdata,  CI_XAUTHENTICATEDUSER|CI_XAUTHENTICATEDGROUPS);
	 ci_debug_printf(5,"init script\n");
	 memset(script,'\0',SCRIPT_LEN);
	 char conf_path[CI_MAX_PATH];
	 ci_debug_printf(5,"CONF.cfg_file = %s\n",CONF.cfg_file);
	 char *dir_end=strrchr(CONF.cfg_file,'/');
	 //ci_debug_printf(5,"conf_dir is %s\n",dirname(CONF.cfg_file));
	 size_t length=dir_end-CONF.cfg_file;
	 memcpy(conf_path,CONF.cfg_file,length);
	 conf_path[length]='\0';
	 strcat(conf_path,"/wafer.conf");
	 ci_debug_printf(5,"wafer configure file %s\n",conf_path);
     FILE *fs=fopen(conf_path,"r");
	 if(fs == NULL)
	 {
		 ci_debug_printf(2,"cannot open configure file %s\n",conf_path);
		 kill(getpid(),SIGINT);
	 }
	 size_t size=SCRIPT_LEN;
	 char *buf=malloc(size);
	 if(buf == NULL)
	 {
		 ci_debug_printf(2,"allocate buf to store script failed")
			 kill(getpid(),SIGINT);
	 }
	 if(fgets(buf,size,fs) == NULL)
	 {
		 ci_debug_printf(2,"read configure file %s failed\n",conf_path);
		 kill(getpid(),SIGINT);
	 }
	 buf[strlen(buf)-1]='\0';
	 strcpy(script,buf);
	 ci_debug_printf(5,"The script is %s\n",script);
	 free(buf);
	 fclose(fs);
     return CI_OK;
}

/* This function will be called when the service shutdown */
void echo_close_service() 
{
    ci_debug_printf(5,"echo Service shutdown!\n");
    /*Nothing to do*/
}

/*This function will be executed when a new request for echo service arrives. This function will
  initialize the required structures and data to serve the request.
 *
 */
///////////////////////////////////////////////
//nick add
void print_headers(void* data,const char * header_name,const char* header_value)
{
	ci_debug_printf(2,"header_name= %s,header_value= %s\n",header_name,header_value);
}
//////////////////////////////////////////////////
void *echo_init_request_data(ci_request_t * req)
{
    struct EchoData *echo_data;
	int content_len;
	const char* content_type;
    /*Allocate memory fot the echo_data*/
    echo_data = malloc(sizeof(struct EchoData));
    if (!echo_data) 
	{
        ci_debug_printf(1, "Memory allocation failed inside echo_init_request_data!\n");
        return NULL;
    }

    /*If the ICAP request encuspulates a HTTP objects which contains body data 
      and not only headers allocate a ci_cached_file_t object to store the body data.
     */
////////////// /////////////////////////////////////
//nick add it
/*
	if(ci_http_request_headers(req) != NULL)
	{
		ci_debug_printf(5,"print reqmod_http headers\n");
		ci_headers_iterate(ci_http_request_headers(req),NULL,&print_headers);
		
	}
	if(ci_http_response_headers(req) != NULL)
	{
		ci_debug_printf(5,"printf respmod_http headers\n");
		ci_headers_iterate(ci_http_response_headers(req),NULL,&print_headers);
	}
	if(req->request_header!= NULL)
	{
		ci_debug_printf(2,"print request header\n")
		ci_headers_iterate(req->request_header,NULL,&print_headers);
	}
	if(req->response_header!= NULL)
	{
		ci_debug_printf(5,"print response header\n")
		ci_headers_iterate(req->response_header,NULL,&print_headers);
	}
	if(req->xheaders!= NULL)
	{
		ci_debug_printf(5,"print X-header\n")
		ci_headers_iterate(req->xheaders,NULL,&print_headers);

	}
*/
//nick add end
///////////////////////////////////////////////////
	ci_debug_printf(5,"in echo_init_request_data()\n");
	echo_data_init(echo_data);
	
	if(req->type == ICAP_RESPMOD && ci_req_hasbody(req))
	{
		content_type=ci_http_response_get_header(req,"Content-Type");
		if(content_type == NULL)
		{
			echo_data->in_data = NULL;
			return echo_data;
		}

		if(strncmp(content_type,"text/html",9) == 0)//it is a html
		{
			ci_debug_printf(5,"a html response\n");
			content_len = ci_http_content_length(req);
			if(content_len > 0 && content_len < 12)//it can not be a normal html text
			{
				ci_debug_printf(5,"content-length < 12 and content-length= %d\n",content_len);
				echo_data->in_data = NULL;
				return echo_data;
			}
			if(content_len >= 12)//the minimum length of a zip data is 12
			{
				ci_debug_printf(5,"found content-length header,content_len=%d\n",content_len); 
				echo_data->in_data = ci_membuf_new_sized(content_len);
			}
			else
			{
				ci_debug_printf(5,"could not found content-length header\n"); 
				echo_data->in_data = ci_membuf_new_sized(4096*5);
			}
			if(echo_data->in_data == NULL)
				ci_debug_printf(2,"allocate error:echo_data->in_data == NULL\n");
			return echo_data;
		}

	}
     /*Return to the c-icap server the allocated data*/
    return echo_data;  // req->service_data=echo_data;
}

/*This function will be executed after the request served to release allocated data*/
void echo_release_request_data(void *data)
{
    /*The data points to the echo_req_data struct we allocated in function echo_init_service */
    struct EchoData *echo_data = (struct EchoData *)data;
	ci_debug_printf(5,"in echo_release_request_data()\n");
	if(echo_data == NULL)
		return;
    /*if we had body data, release the related allocated data*/
	echo_data_clean(echo_data);
    free(echo_data);
	echo_data = NULL;
}


int echo_check_preview_handler(char *preview_data, int preview_data_len,
                               ci_request_t * req)
{
    int content_len;
	char* buf; 
	const char *p,*e;
	int length;
		
	//const char* script="\n<script type=\"text/javascript\">\nalert(\"hello,world\")\n</script>\n";
	//const char* script= "\n<script type=\"text/javascript\" src=\"http://192.168.6.158:8080/manage/js/toolbar.js\" charset=\"utf-8\"></script>\n";
	//const char* script="\n<!--Add by nicktsai in C-icap server-->\n\n";
     /*Get the echo_req_data we allocated using the  echo_init_service  function*/
    struct EchoData *echo_data = ci_service_data(req);
	if(req->type == ICAP_REQMOD )
		return CI_MOD_ALLOW204;
	if(echo_data == NULL)
		return CI_MOD_ALLOW204;
	 if(echo_data->in_data == NULL)
		return CI_MOD_ALLOW204;
     /*If there are is a Content-Length header in encupsulated Http object read it
      and display a debug message (used here only for debuging purposes)*/

     /*If there are not body data in HTTP encapsulated object but only headers
       respond with Allow204 (no modification required) and terminate here the
       ICAP transaction */
     if(!ci_req_hasbody(req))
		return CI_MOD_ALLOW204;

     content_len = ci_http_content_length(req);
     ci_debug_printf(5, "We expect to read :%" PRINTF_OFF_T " body data\n",(CAST_OFF_T) content_len);
     /*Unlock the request body data so the c-icap server can send data before 
       all body data has received */
     //ci_req_unlock_data(req);
     ci_req_lock_data(req);

     /*If there are not preview data tell to the client to continue sending data 
       (http object modification required). */
     if (!preview_data_len)
	 {
		ci_debug_printf(5,"preview_data_len == 0\n");
		echo_data_set_raw(echo_data);	
		return CI_MOD_CONTINUE;
	 }

     /* In most real world services we should decide here if we must modify/process
	or not the encupsulated HTTP object and return CI_MOD_CONTINUE or  
	CI_MOD_ALLOW204 respectively. The decision can be taken examining the http
	object headers or/and the preview_data buffer.

	In this example service we just use the whattodo static variable to decide
	if we want to process or not the HTTP object.
      */
     if (req->type == ICAP_RESPMOD) 
	 {
          ci_debug_printf(8, "Echo RESPMOD service will process the request\n");
	  /*if we have preview data and we want to proceed with the request processing
	    we should store the preview data. There are cases where all the body
	    data of the encapsulated HTTP object included in preview data. Someone can use
	    the ci_req_hasalldata macro to  identify these cases*/
		if (preview_data_len) 
		{
			int offset;
			const char *encoding;
			const char *ptr=ci_http_response_get_header(req,"Content-Type");
			if(ptr == NULL)
				return CI_MOD_ALLOW204;
			if(strncmp(ptr,"text/html",9) !=0)
				return CI_MOD_ALLOW204;
			encoding=ci_http_response_get_header(req,"Content-Encoding");
			if(encoding == NULL) // not encoded data
			{		
				ci_debug_printf(5,"not encoded data\n");
				if(strncasestr(preview_data,"<frameset",preview_data_len) != NULL)
					return CI_MOD_ALLOW204;

		//	ci_debug_printf(5,"**************in echo_check_preview_hanlder()****************\n");
				if((((p=strnstr(preview_data,"<head",preview_data_len)) != NULL) || ((p=strnstr(preview_data,"<HEAD",preview_data_len)) != NULL)) && (e=strnstr(p,">",preview_data_len-(p-preview_data))) != NULL)
				{ 
					int end;
					length=preview_data_len+strlen(script);
					buf=malloc(length);
					offset=0;
					memcpy(buf,preview_data,e-preview_data+1);
					offset=e-preview_data+1;
					memcpy(buf+offset,script,strlen(script));
					offset+=strlen(script);
					memcpy(buf+offset,e+1,preview_data_len+preview_data-e-1);
					end=ci_req_hasalldata(req);
					echo_data_write(echo_data,buf,length,end);
					free(buf);
					ci_debug_printf(5,"add script successful in not encoded data\n");

					echo_data_set_raw(echo_data);
					
					if(content_len)//chaneg content-lengt header
					{
						content_len += strlen(script);
						char head[50];
						ci_http_response_remove_header(req,"Content-Length");
						snprintf(head,50,"Content-Length: %" PRINTF_OFF_T,(CAST_OFF_T) content_len);
						ci_http_response_add_header(req,head);
						ci_debug_printf(5,"change content length header in not encoded data (Content-Length: %d)\n",content_len);
						ci_req_unlock_data(req);//strart send data to ICAP client
					}
					return CI_MOD_CONTINUE;
				}
				else
				{
					return CI_MOD_ALLOW204;
				}
			}
			if(strncmp(encoding,"gzip",4) == 0)// it is gzip data
			{
				ci_debug_printf(5,"it is a gzip data\n");
				echo_data_set_encoding(echo_data,GZIP);
				echo_data_write(echo_data,preview_data,preview_data_len,ci_req_hasalldata(req));
				return CI_MOD_CONTINUE;
			}
			if(strncmp(encoding,"deflate",7) == 0)//not gzip encoded
			{	
				ci_debug_printf(5,"it is a deflate data\n");
				echo_data_set_encoding(echo_data,DEFLATE);
				echo_data_write(echo_data,preview_data,preview_data_len,ci_req_hasalldata(req));
				return CI_MOD_CONTINUE;
			}
			else// it is an unknown encoding
			{
				ci_debug_printf(5,"unknown encoding\n");
				return CI_MOD_ALLOW204;
			}
		}
		else
		{
			ci_debug_printf(5,"preview_data_len == 0\n");
			return CI_MOD_CONTINUE;
		}
     }
     else 
	 {
	  /*Nothing to do just return an allow204 (No modification) to terminate here
	   the ICAP transaction */
          return CI_MOD_ALLOW204;
     }
}

/* This function will called if we returned CI_MOD_CONTINUE in  echo_check_preview_handler
 function, after we read all the data from the ICAP client*/
int echo_end_of_data_handler(ci_request_t * req)
{
	int content_len;
    struct EchoData *echo_data = ci_service_data(req);
	if(echo_data == NULL)
		return CI_MOD_DONE;
	if(echo_data->in_data == NULL)
		return CI_MOD_DONE;
    /*mark the eof*/
    echo_data->in_data->hasalldata = 1;
	ci_debug_printf(5,"echo_end_of_data_handler()\n");
	if(echo_data->encoding != RAW)
	{
		ci_debug_printf(5,"it is a compressed data,start do work\n");
		if(echo_data_do_work(echo_data,script) == 0)//work well
		{
			ci_http_response_remove_header(req,"Content-Encoding");
		}
	}
	
	content_len=echo_data_content_length(echo_data);
	if((req->data_locked == 1) && (content_len > 0))// 
	{
		char head[64];
		ci_http_response_remove_header(req,"Content-Length");
		snprintf(head,50,"Content-Length: %" PRINTF_OFF_T,(CAST_OFF_T) content_len);
		ci_http_response_add_header(req,head);
		ci_debug_printf(5,"add new content length header (Content-Length: %d)\n",content_len);
	}
	
    ci_req_unlock_data(req);
    return CI_MOD_DONE;
}

/* This function will called if we returned CI_MOD_CONTINUE in  echo_check_preview_handler
   function, when new data arrived from the ICAP client and when the ICAP client is 
   ready to get data.
*/
int echo_io(char *wbuf, int *wlen, char *rbuf, int *rlen, int iseof,
            ci_request_t * req)
{
     int ret;
     struct EchoData *echo_data = ci_service_data(req);
     ret = CI_OK;

     /*write the data read from icap_client to the echo_data->body*/
     if(rlen && rbuf) 
	 {
         *rlen = echo_data_write(echo_data, rbuf, *rlen,ci_req_hasalldata(req));
         if (*rlen < 0)
		 ret = CI_ERROR;
     }

     /*read some data from the echo_data->body and put them to the write buffer to be send
      to the ICAP client*/
     if (wbuf && wlen) 
	 {
          *wlen = echo_data_read(echo_data, wbuf, *wlen);
     }
     if(*wlen == 0 && echo_data_hasalldata(echo_data) == 1)
	 *wlen = CI_EOF;

     return ret;
}
