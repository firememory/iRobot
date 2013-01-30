#include "StdAfx.h"
#include "ParseKcbpLog.h"

CParseKcbpLog::CParseKcbpLog(void)
{
}

CParseKcbpLog::~CParseKcbpLog(void)
{
}

void CParseKcbpLog::ParseLog()
{
	// 打开log、
	FILE *fp = NULL;
	fopen_s(&fp, "d:/kcbp.log", "r");

	if (NULL == fp)
	{		
		return;
	}

	char szBuf[1024] = {0};

	// 逐行读取日志文件
	while(NULL != fgets(&szBuf[0], 1024, fp))
	{
		int pos = strlen(szBuf);

		// 将行尾变为行结束符
		if (szBuf[pos - 1] == '\n')
		{
			szBuf[pos - 1] = '\0';
		}		

		LBM_PARAM_INFO *pLbmInfo = new LBM_PARAM_INFO;

		// 取LBM编号
		char szLbmId[10] = {0};
		char *pFind = strstr(szBuf, "BEGIN:");
		if (NULL != pFind)
		{
			// 如果找到BEGIN，说明是执行行，则继续查找
			// 将行尾的] 符号 删除
			szBuf[pos - 2] = '\0';

			// 查找 LBM ID
			pFind += 6;

			char *pColon = strstr(pFind, ":");

			*pColon = '\0';

			strcpy_s(szLbmId, pFind);			
			printf("LBM=%s\n", szLbmId);
			strcpy_s(pLbmInfo->szLbmId, szLbmId);

			// 查找参数, 默认参数是以F_型参数开始
			pColon++;
			pFind = strstr(pColon, "&F_");

			char arr[50][50];
			memset(&arr, 0x00, sizeof(arr));

			int i = 0;

			char *token = strtok(pFind, "&");
			while( token != NULL )
			{
				// While there are tokens in "string"
				printf( " %s\n", token );				

				strcpy_s(arr[i], token);
				i++;

				// Get next token: 
				token = strtok( NULL, "&" ); 		
			}

			for (int idx=0;idx<i;idx++)
			{
				pFind = arr[idx];

				char *pName = strtok(pFind, "=");
				char *pValue = strtok(NULL, "=");

				printf("Name = %s, Value = %s \n", pName, pValue);
				
				PARAM_UNIT *pParamUnit = new PARAM_UNIT;

				strcpy_s(pParamUnit->szName, pName);

				// 如果Value为空
				if (NULL == pValue)
				{
					strcpy_s(pParamUnit->szValue, "");
				}
				else
				{
					strcpy_s(pParamUnit->szValue, pValue);
				}				
				
				pLbmInfo->arrParams.Add(pParamUnit);
			}

			m_arrCmds.Add(pLbmInfo);
		}
	}
}

void CParseKcbpLog::ReadRlt()
{
	int n = m_arrCmds.GetCount();

	for (int i=0; i<n; i++)
	{
		LBM_PARAM_INFO *pLBM = m_arrCmds.GetAt(i);

		int nLBMParamCnt = pLBM->arrParams.GetCount();
		printf("LBM:%s has %d params \n", pLBM->szLbmId, nLBMParamCnt);
	
		for (int idx = 0; idx<nLBMParamCnt; idx++)
		{
			PARAM_UNIT *pParamUnit = pLBM->arrParams.GetAt(idx);
			printf("Param %d, Name:%s, Value%s \n", pParamUnit->szName, pParamUnit->szValue);
		}
	}
}

