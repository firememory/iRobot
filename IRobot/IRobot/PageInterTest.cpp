// PageInterTest.cpp : implementation file
//

#include "stdafx.h"
#include "IRobot.h"
#include "PageInterTest.h"
#include "loginterface.h"
#include "DBConnect.h"
#include "Cfg.h"

extern CMidConn *g_pMidConn;
extern CKcxpConn *g_pKcxpConn;
extern CParseKcbpLog *g_pParseKcbpLog;
extern CDBConnect *g_pDBConn;
extern CLoginterface *g_pLog;

// CPageInterTest dialog

IMPLEMENT_DYNAMIC(CPageInterTest, CPropertyPage)

CPageInterTest::CPageInterTest()
	: CPropertyPage(CPageInterTest::IDD)
	, m_strCommand(_T("203||||600036|"))	
{	
	m_nTestMode = USE_MID;	
}

CPageInterTest::~CPageInterTest()
{
	m_pKDGateWay = NULL;
	m_pKcxpConn = NULL;
}

void CPageInterTest::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_KCXP, m_ctrlKcxp);
	DDX_Control(pDX, IDC_MID, m_ctrlMid);
	
	DDX_Control(pDX, IDC_EDIT_COMMAND, m_ctrlCommand);
	DDX_Text(pDX, IDC_EDIT_COMMAND, m_strCommand);
}

BEGIN_MESSAGE_MAP(CPageInterTest, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_INVOKE, &CPageInterTest::OnBnClickedButtonInvoke)
	ON_BN_CLICKED(IDC_KCXP, &CPageInterTest::OnBnClickedKcxp)
	ON_BN_CLICKED(IDC_MID, &CPageInterTest::OnBnClickedMid)
END_MESSAGE_MAP()


// CPageInterTest message handlers

void CPageInterTest::OnBnClickedButtonInvoke()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_pKcxpConn = g_pKcxpConn->GetKdMidCli();
	m_pKDGateWay = g_pMidConn->GetKDGateWay();

	BOOL bRet = TRUE;

	if (m_nTestMode == USE_MID)
	{
		// ����MID�ӿ�
		SendMidMsg(m_strCommand.GetBuffer());
	}
	else
	{
		// ����KCXP�ӿ�
		if (TRUE == (bRet = SendKcxpMsg(m_strCommand.GetBuffer())) && m_nRowNum > 0)
		{
			// �����ؽ����ӡ����Ļ��
		}
	}
}

void CPageInterTest::OnBnClickedKcxp()
{
	// TODO: Add your control notification handler code here
	m_nTestMode = USE_KCXP;

	m_strCommand.Format("����:%s", "<  32768122> BEGIN:L0300016:03-07:11:34-687338  [_CA=2.3&_ENDIAN=0&F_OP_ROLE=2&F_OP_USER=10004&F_OP_BRANCH=999&F_CHANNEL=7&F_OP_SITE=3@4000247E030C03@40172018009084&SECU_CODE=600036]");

	UpdateData(FALSE);
}

void CPageInterTest::OnBnClickedMid()
{
	// TODO: Add your control notification handler code here
	m_nTestMode = USE_MID;

	m_strCommand.Format("����:%s", "203||||600036|");

	UpdateData(FALSE);
}

// ��Mid������Ϣ
BOOL CPageInterTest::SendMidMsg(char *pCmd)
{		
	g_pLog->WriteRunLog(MID_MODE, LOG_DEBUG, "Send:%s", pCmd);
	if (m_pKDGateWay->WaitAnswer(pCmd)!=TRUE)
	{
		g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "MID����ʧ��!");		
		return FALSE;
	}

	// �Թ�̨���ص�ֵ���н���
	g_pLog->WriteRunLog(MID_MODE, LOG_DEBUG, "Recv:%s", m_pKDGateWay->m_pReturnData);

	char szTemp[2048*10] = {0};
	strcpy_s(szTemp, m_pKDGateWay->m_pReturnData);

	m_nRowNum = m_pKDGateWay->GetRecNum();

	m_nFieldNum = m_pKDGateWay->GetFieldNum();

	return TRUE;
}

BOOL CPageInterTest::SendKcxpMsg( char *pCmd )
{	
	g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, pCmd);

	int iRetCode = KCBP_MSG_OK;
	char szTemp[512] = {0};

	if (NULL == m_pKcxpConn)
	{
		g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, "��ȡKCXP����ʧ��!");		
		return FALSE;
	}	

	// ������Ϣ
	try
	{
		// ��������
		g_pParseKcbpLog->ParseCmd(pCmd);

		// ��KCXP��������
		BOOL bRet = g_pParseKcbpLog->ExecSingleCmd();

		// �����־������������һ�β���
		g_pParseKcbpLog->Clean();

		if (FALSE != bRet)
		{
			// ��ȡִ�н��
			int nRow = 0;				

			if ((iRetCode = m_pKcxpConn->RsOpen()) == KCBP_MSG_OK)
			{
				// ��ȡ�����������ע�������ǰ�������ģ��������Ҫ��1
				m_pKcxpConn->RsGetRowNum(&nRow);

				if (nRow>1)
				{
					m_nRowNum = nRow - 1;					
				}
				else
				{
					g_pLog->WriteRunLogEx(__FILE__,__LINE__,"��������������쳣!");
					m_nRowNum = 0;					
					return FALSE;
				}

				if ((iRetCode = m_pKcxpConn->RsFetchRow()) == KCBP_MSG_OK)
				{
					if ((iRetCode = m_pKcxpConn->RsGetCol(1, szTemp)) == KCBP_MSG_OK)
					{
						if ((iRetCode = m_pKcxpConn->RsGetCol(2, szTemp)) == KCBP_MSG_OK)
						{
							if(strcmp(szTemp,"0") != 0)
							{
								iRetCode = m_pKcxpConn->RsGetCol(3, szTemp);

								g_pLog->WriteRunLogEx(__FILE__,__LINE__, "��ȡ���������Ϣʧ��,ERRCODE = %ld", iRetCode);								
								return FALSE;
							}
							else
							{
								// ��LBM������Ϣ��ӡ��������
								char szKey[512] = {0};
								char szVal[512] = {0};

								if (iRetCode = m_pKcxpConn->RsMore() == KCBP_MSG_OK)
								{
									int nRow = 0;
									while(nRow < m_nRowNum)
									{
										if(m_pKcxpConn->RsFetchRow() != KCBP_MSG_OK)
										{

											break;
										}

										int nColNum = 0;
										m_pKcxpConn->RsGetColNum(&nColNum);

										for (int nColIdx = 0; nColIdx < nColNum; nColIdx++)
										{
											memset(&szKey, 0x00, sizeof(szKey));
											memset(&szVal, 0x00, sizeof(szVal));

											m_pKcxpConn->RsGetColName(nColIdx, szKey, sizeof(szKey));
											m_pKcxpConn->RsGetColByName(szKey, szVal);

											g_pLog->WriteRunLog(MID_MODE, LOG_DEBUG, "Recv:Key = %s, Val = %s", szKey, szVal);
										}

										nRow++;
									}		
								}
							}
						}
					}
					else
					{
						g_pLog->WriteRunLogEx(__FILE__,__LINE__, "��ȡ���������Ϣʧ��,ERRCODE = %ld", iRetCode);						
						return FALSE;
					}
				}				
			}
			else
			{	
				g_pLog->WriteRunLogEx(__FILE__,__LINE__,"�򿪽����ʧ��,ERRCODE = %ld", iRetCode);				
				return FALSE;
			}
		}
		else
		{			
			return FALSE;
		}
	}
	catch(...)
	{
		g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, "LBM�����쳣��");		
		return FALSE;
	}	
	
	return TRUE;
}