#ifndef _HTTPZIP_H
#define _HTTPZIP_H
#include<stddef.h>
struct HttpZip
{
	void* data;
	int allocate;
	size_t length;
};
enum HttpEncoding{RAW=0,GZIP=1,DEFLATE=2};
void http_zip_init(struct HttpZip *httpzip);
void http_zip_set(struct HttpZip* httpzip,int allocate,void *data,size_t length);
void http_zip_clean(struct HttpZip* httpzip);
int http_zip_compress(struct HttpZip *src,struct HttpZip *dest,enum HttpEncoding encoding);
int http_zip_decompress(struct HttpZip *src,struct HttpZip *dest,enum HttpEncoding encoding);
#endif
