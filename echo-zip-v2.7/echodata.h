#ifndef _ECHO_DATA_H
#define _ECHO_DATA_H
#include"body.h"
#include"httpzip.h"
#define PREVIEW_SIZE 1024
struct EchoData
{
	struct ci_membuf *in_data;
	struct ci_membuf *out_uncompressed_data;
	struct ci_membuf *out_compressed_data;
	struct ci_membuf *out_data;
	enum HttpEncoding encoding;
	int output_compress;//flag to send compressed data or not
};
void echo_data_init(struct EchoData* data);
void echo_data_clean(struct EchoData* data);
int echo_data_write(struct EchoData *data,const char* buf,int len,int iseof);
int echo_data_read(struct EchoData *data,char* buf,int len);
int echo_data_do_work(struct EchoData *data,const char *script);
void echo_data_set_encoding(struct EchoData *data,enum HttpEncoding encoding);
void echo_data_set_raw(struct EchoData *data);
int echo_data_content_length(struct EchoData* data);
int echo_data_hasalldata(struct EchoData *data);
#endif
