#define STRNCPY_EX(x,y,z)\
{\
	strncpy_s(x, y, z);\
	x[z-1] = '\0';\
}

#define ZEROMEM(x,y)\
{\
	memset((void*)x, 0x00, y);\
}

#define NEWCLS(x,y)\
{\
	x = NULL;\
	try\
	{\
	x = new y;\
	}\
	catch(...)\
	{\
	if(NULL != x)\
		{\
		delete x;\
		x = NULL;\
		}\
	}\
	if(NULL = x)\
	{\
	TRACE("operation new class failed! file=%s:%d",__FILE__, __LINE__);\
	}\
}

#define NEWCLSARY(x,y,z)\
{\
	x = NULL;\
	try\
	{\
	x = new y[z];\
	}\
	catch(...)\
	{\
	if(NULL != x)\
		{\
		delete [] x;\
		x = NULL;\
		}\
	}\
	if(NULL = x)\
	{\
	TRACE("operation new array failed! file=%s:%d",__FILE__, __LINE__);\
	}\
}

#define DELCLS(x)\
{\
	if(NULL != x)\
	{\
	delete x;\
	x = NULL;\
	}\
}

#define DELARY(x)\
{\
	if(NULL != x)\
	{\
	delete [] x;\
	x = NULL;\
	}\
}

// Float小数点后两位取精度
inline float CutFloatPnt(float fVal)
{
	int nVal = (int)(fVal*100+0.5);
	return (float)nVal / 100;
}