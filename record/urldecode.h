#ifndef URL_DECODE_H
#define URL_DECODE_H
#include <stddef.h>
#define NON_NUM '0'
/*****************************
 * 将字符串进行URL解码
 * 输入：
 * str：要解码的字符串
 * strSize:字符串的长度
 * result:结果缓冲区的地址
 * resultSize:结果地址的缓冲区大小，可以<=strSize
 * 返回值:
 * >0:result中实际有效的字符长度
 * -1:解码失败，原因是缓冲区result的长度太小
 *********************************/
int urlDecode(const char* str,char* result,const size_t resultSize);
#endif
