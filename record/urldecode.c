#include "urldecode.h"
#include <string.h>
static char char2Num(char ch)
{
	if(ch >= '0' && ch <= '9')
		return (char)(ch-'0');
	if(ch >= 'a' && ch <= 'z')
		return (char)(ch-'a'+10);
	if(ch >= 'A' && ch <= 'Z')
		return (char)(ch-'A'+10);
	return NON_NUM;
}
int urlDecode(const char *str,char* result,const size_t resultSize)
{
	char ch,ch1,ch2;
	int i;
	int j=0;
	size_t strSize=(str == NULL)?0:strlen(str);
	if((strSize == 0) || (result == NULL) || (resultSize == 0))
		return -1;
	for(i=0;(i<strSize)&&(j<(resultSize-1));++i)
	{
		ch= str[i];
		switch(ch)
		{
			case '+':
				result[j++]=' ';
				break;
			case '%':
				if(i+2<strSize)
				{
					ch1=char2Num(str[i+1]);
					ch2=char2Num(str[i+2]);
					if((ch1 != NON_NUM) &&(ch2 != NON_NUM))
					{
						result[j++]=(char)(ch1<<4|ch2);
						i+=2;
						break;
					}
				}
			default:
				result[j++]=ch;
				break;
		}
	}
	result[j]='\0';
	return j;
}

