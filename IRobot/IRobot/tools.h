#ifndef DEF_H_TOOLS
  #define DEF_H_TOOLS
  
char* strchr(char* src, int c, int num);
char * GetValue(const char *src,char *dest ,int num,char ch);
char * GetValue(const char *src,char *dest ,int num,char ch, int count);
char * ltrim(char *str);
char * rtrim(char *str);
char * alltrim(char *str);
char * padl(char *str,char ch,int num);
char * padr(char *str,char ch,int num);
char * padc(char *str,char ch,int num);
long ahtol(char* str);

#endif