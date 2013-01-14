#include "stdafx.h"
#include "tools.h"

char* strchr(char* src, int c, int num)
{
	char* dest;
	dest = src;
	while(*dest++ != 0 && num >0)
	{
		if (*dest == c)
			num--;
	}
	return  dest;
}

char * GetValue(const char *src,char *dest ,int num,char ch)
{
	char *s0=dest;
	dest[0]=0;
	while (*src!=0 && *src!='\n' && num!=1)
	{
		if (*src==ch)
		  num--;
		src++;
	}
	while (*src!=0 && *src!='\n' && *src!=ch)
		*s0++=*src++;
	*s0=0;
	return dest;
}

char * GetValue(const char *src,char *dest ,int num,char ch, int count)
{
/*
	char *s0=dest;
	dest[0]=0;
	while (*src!=0 && *src!='\n' && num!=1)
	{
		if (*src==ch)
			num--;
		src++;
	}
	int icount =0;
	while (*src!=0 && *src!='\n' && *src!=ch && icount++<count)
		*s0++=*src++;
	*s0=0;
	return dest;
*/
  char *s0=dest;
  dest[0]=0;
  while (*src!=0 && num!=1)
  {
    if (*src==ch)
      num--;
    src++;
  }
  int icount =0;
  while (*src!=0 &&  *src!=ch && icount++<count)
    *s0++=*src++;
  *s0=0;
  return dest;
}


char * ltrim(char *str)
{
	char *s=str;
	while (*s==' '&& *s!=0) s++;
	if (s != str)
		strcpy(str,s);
	return str;
}

char * rtrim(char *str)
{
	char *s=str+strlen(str)-1;
	while (s-str>0 && (*s==' ' || *s=='\n')) s--;
	s++;
	*s=0;
	return str;
}

char * alltrim(char *str)
{
	ltrim(str);
	rtrim(str);
	return str;
}

char * padl(char *str,char ch,int num)
{
	char sBuffer[255];
	memset(sBuffer,ch,num);
	MoveMemory(sBuffer+num-min(num,(int)strlen(str)),str,min(num,(int)strlen(str)));
	sBuffer[num]=0;
	strcpy_s(str,255,sBuffer);
	return str;
}

char * padr(char *str,char ch,int num){
	char sBuffer[255];
	memset(sBuffer,ch,num);
	MoveMemory(sBuffer,str,min(num,(int)strlen(str)));
	sBuffer[num]=0;
	strcpy_s(str,255,sBuffer);
   return str;
}

char * padc(char *str,char ch,int num)
{
	char sBuffer[255];
	memset(sBuffer,ch,num);
	MoveMemory(sBuffer+num-min(num,(int)strlen(str))/2,str,min(num,(int)strlen(str)));
	sBuffer[num]=0;
	strcpy_s(str,255,sBuffer);
	return str;
}

inline unsigned char btoh(char ch)
{
	return  ((ch)>=0x30 && (ch)<=0x39)?(ch)-0x30:  
	((ch)>=0x41 && (ch)<=0x5a)?(ch)-0x41+0xa: 
	((ch)>=0x61 && (ch)<=0x7a)?(ch)-0x61+0xa: 0 ;
}

long ahtol(char* str)
{
	int i = strlen(str);
	long lval = 0;
	
	while (*str)
	{
		lval += ((long)(btoh(*str++)))<<(--i<<2);
	}
	
	return lval;
}

