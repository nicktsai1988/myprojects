#include"echodata.h"
#include"httpzip.h"
#include"debug.h"
#include<string.h>
#define min(x,y) (((x)<(y))?(x):(y))
void echo_data_init(struct EchoData* data)
{
	if(data == NULL)
	{
		ci_debug_printf(2,"echo_data_init() error: data == NULL\n");
		return;
	}
	data->in_data = NULL;
	data->out_uncompressed_data = NULL;
	data->out_compressed_data = NULL;
	data->out_data = NULL;
	data->encoding=RAW;
	data->output_compress=0;
}
void echo_data_clean(struct EchoData *data)
{
	if(data == NULL)
	{
		ci_debug_printf(2,"echo_data_clean() error: data == NULL\n");
		return;
	}
	if(data->in_data)
	{
		ci_membuf_free(data->in_data);
		data->in_data = NULL;
	}
	if(data->out_uncompressed_data)
	{
		ci_membuf_free(data->out_uncompressed_data);
		data->out_uncompressed_data = NULL;
	}
	if(data->out_compressed_data)
	{
		ci_membuf_free(data->out_compressed_data);
		data->out_compressed_data = NULL;
	}

	if(data->out_data)
	{
		data->out_data = NULL;
	}
	data->encoding=RAW;
	data->output_compress=0;
}
int echo_data_write(struct EchoData* data,const char *buf,int len,int iseof)
{
	if(data->in_data == NULL)
	{
		ci_debug_printf(2,"echo_data_write() error: data->in_data == NULL\n");
		return 0;
	}
	else
		return ci_membuf_write(data->in_data,buf,len,iseof);
}
int echo_data_read(struct EchoData* data,char* buf,int len)
{
	if(data->out_data == NULL)
	{
		ci_debug_printf(2,"echo_data_read() error: data->out_data == NULL\n");
		return 0;
	}
	else
		return ci_membuf_read(data->out_data,buf,len);
}
void echo_data_set_raw(struct EchoData* data)
{
	data->encoding=RAW;
	data->out_data=data->in_data;
}
void echo_data_set_encoding(struct EchoData *data,enum HttpEncoding encoding)
{
	data->encoding=encoding;
}
int echo_data_do_work(struct EchoData *data,const char *script)
{
	struct HttpZip src,dest;
	int state;
	size_t offset,preview_len,script_len;
	const char *p,*e,*ptr;
	if((data  == NULL) || (data->encoding == RAW) || (data->in_data == NULL))
	{
		ci_debug_printf(2,"echo_data_do_work() error: input parameter error\n");
		return -1;
	}
	http_zip_init(&src);
	http_zip_init(&dest);
	http_zip_set(&src,0,data->in_data->buf,data->in_data->endpos);
	state=http_zip_decompress(&src,&dest,data->encoding);
	http_zip_clean(&src);
	if(state < 0)//decompress failed,send origin data
	{
		ci_debug_printf(2,"echo_data_do_work() error: http_zip_decompress failed,state=%d\n",state);
		echo_data_set_raw(data);//set no compressed,don't remove encoding header
		http_zip_clean(&dest);
		return -1;
	}
	if(script == NULL)
	{
		ci_debug_printf(2,"script is NULL\n");
		script_len=0;
	}
	else
		script_len=strlen(script);

	data->out_uncompressed_data=ci_membuf_new_sized(dest.length+script_len+32);//more 32 bytes to avoid memory overflow
	if(data->out_uncompressed_data == NULL)//allocate error,send origin data
	{
		ci_debug_printf(2,"echo_data_do_work() error: ci_membuf_new_sized() failed\n");
		echo_data_set_raw(data);//set to send origin data
		http_zip_clean(&dest);
		return -1;
	}
	// TODO:insert script here
	ptr=(char*)dest.data;
	preview_len=min(PREVIEW_SIZE,dest.length);
	if(strncasestr(ptr,"<frameset",preview_len) != NULL)//had frameset tag
	{
		ci_debug_printf(5,"echo_data_do_work(): html contain s a frameset tag\n");
		ci_membuf_write(data->out_uncompressed_data,dest.data,dest.length,1);//send uncompressed data
		http_zip_clean(&dest);
		data->out_data = data->out_uncompressed_data;
		return 0;
	}
	if((((p=strnstr(ptr,"<head",preview_len)) != NULL) || ((p=strnstr(ptr,"<HEAD",preview_len)) != NULL)) && (e=strnstr(p,">",preview_len-(p-ptr))) != NULL)
	{
		offset=e-ptr+1;
		ci_membuf_write(data->out_uncompressed_data,ptr,offset,0);
		if(script_len != 0)
		{
			ci_membuf_write(data->out_uncompressed_data,script,script_len,0);
			ci_debug_printf(5,"echo_data_do_work(): insert script success\n");
		}
		ci_membuf_write(data->out_uncompressed_data,e+1,dest.length-(offset),1);
	}
	else//don't insert script
	{
		ci_debug_printf(5,"echo_data_do_work(): could not find head tag\n");
		ci_membuf_write(data->out_uncompressed_data,dest.data,dest.length,1);//send uncompressed data
	}
	http_zip_clean(&dest);
	//TODO:need modified
	data->out_data = data->out_uncompressed_data;
	return 0;
}
int echo_data_content_length(struct EchoData* data)
{
	if((data == NULL) || (data->out_data == NULL))
	{
		ci_debug_printf(2,"echo_data_content_length() error: input parameter error\n");
		return -1;
	}
	return data->out_data->endpos;
}
int echo_data_hasalldata(struct EchoData *data)
{
	if((data == NULL) || (data->in_data == NULL))
	{
		return 1;
	}
	return data->in_data->hasalldata;
}
