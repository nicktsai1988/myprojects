#include"httpzip.h"
#include<zlib.h>
#include<string.h>
#include<stdlib.h>
#include"debug.h"
void http_zip_init(struct HttpZip* httpzip)
{
	httpzip->data =  NULL;
	httpzip->allocate = 0;
	httpzip->length = 0;
}
void http_zip_set(struct HttpZip* httpzip,int allocate,void *data,size_t length)
{
	if(httpzip->allocate && (httpzip->data != NULL))
		http_zip_clean(httpzip);
	httpzip->data = data;
	httpzip->length = length;
	httpzip->allocate = allocate;
}
void http_zip_clean(struct HttpZip *httpzip)
{	
	if(httpzip->allocate && (httpzip->data != NULL))
	{
		free(httpzip->data);
	}
	httpzip->allocate=0;
	httpzip->data = NULL;
	httpzip->length =0;
}

int http_zip_compress(struct HttpZip *src,struct HttpZip *dest,enum HttpEncoding encoding)
{
	return 0;

}
int http_zip_decompress(struct HttpZip *src,struct HttpZip *dest,enum HttpEncoding encoding)
{
	char *block = NULL,*temp = NULL;
	z_stream d_stream;
	unsigned int bufsize,bytesgot;
	int err;
	if(dest == NULL)
	{
		ci_debug_printf(2,"http_zip_decompress() error: dest is NULL\n");
		return -1;
	}

#if ZLIB_VERNUM < 0x1210
#warning ************************************
#warning For gzip support you need zlib 1.2.1
#warning or later to be installed.
#warning You can ignore this warning but
#warning internet bandwidth may be wasted.
#warning ************************************
#endif
	if((src->data == NULL ) || (src->length < 12))
	{
		ci_debug_printf(2,"http_zip_decompress() error: it can not be gzip data\n");
		return -2;//it can not be gzip data
	}
	bufsize=src->length*6;
	block = malloc(sizeof(char)*bufsize);
	if(block == NULL)
	{
		ci_debug_printf(2,"http_zip_decompress() error: block=malloc(sizeof(char)*bufsize) failed\n");
		return -3;
	}
	d_stream.zalloc=(alloc_func)0;
	d_stream.zfree=(free_func)0;
	d_stream.opaque=(voidpf)0;
	d_stream.next_in=(Bytef*)(src->data);
	d_stream.avail_in=src->length;
	d_stream.next_out=(Bytef*)block;
	d_stream.avail_out=bufsize;
	switch(encoding)
	{
		case GZIP:
			err = inflateInit2(&d_stream,15+32);//gzip data,the second parameter must be 47
			break;
		case DEFLATE:
			err = inflateInit2(&d_stream,-15);
			break;
		default:
			{
				ci_debug_printf(2,"http_zip_decompress() error: raw data\n");
				return -8;
			}
	}
	if(err != Z_OK)
	{
		free(block);
		ci_debug_printf(2,"http_zip_decompress() error: bad init inflate: %d\n",err);
		return -4;
	}
	while(1)
	{
		ci_debug_printf(5,"http_zip_decompress(): inflate loop\n");
		err = inflate(&d_stream,Z_SYNC_FLUSH);
		bytesgot = d_stream.total_out;
		if(err == Z_STREAM_END)
		{
			err = inflateEnd(&d_stream);
			if(err != Z_OK)
			{
				free(block);
				ci_debug_printf(2,"http_zip_decompress() error: bad infalteEnd: %s\n",d_stream.msg);
				return -5;
			}
			break;//complete and ok
		}
		if(err != Z_OK)
		{
			free(block);
			ci_debug_printf(2,"http_zip_decompress() error: bad inflate: %s\n",d_stream.msg);
			err = inflateEnd(&d_stream);
			if(err != Z_OK)
			{
				ci_debug_printf(2,"http_zip_decompress() error: bad inflate: %s\n",d_stream.msg);
			}
			return -6;
		}
		bufsize = bytesgot*2;
		temp = malloc(sizeof(char)*bufsize);
		if(temp == NULL)
		{
			free(block);
			ci_debug_printf(2,"http_zip_decompress() error: temp=malloc(sizeof(char)*bufsize) failed\n");
			err = inflateEnd(&d_stream);
			if(err != Z_OK)
			{
				ci_debug_printf(2,"http_zip_decompress() error: bad inflate: %s\n",d_stream.msg);
			}
			return -7;
		}
		memcpy(temp,block,bytesgot);
		free(block);
		block=temp;
		temp = NULL;
		d_stream.next_out = (Bytef*)(block+bytesgot);
		d_stream.avail_out = bufsize-bytesgot;
	}
	http_zip_set(dest,1,block,bytesgot);
	return 0;
}

