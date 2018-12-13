#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////////////

#define REG_ITEM_NAME				TEXT("Information")					//ע�����

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CInitParameter::CInitParameter() 
{ 
	InitParameter();
}

//��������
CInitParameter::~CInitParameter()
{
}

//��ʼ��
VOID CInitParameter::InitParameter()
{

	return;
}

//��������
VOID CInitParameter::LoadInitParameter()
{
	//���ò���
	InitParameter();
	//��ȡ���������
	LoadMsgParameter();

	return;
}

//��������(�����)yang
VOID CInitParameter::LoadMsgParameter()
{
	//��ʼ��
	m_wMarqueeMsgPlayCount = 3;
	m_wMarqueeMsgApartTime = 10;

	//��ȡ·��
	TCHAR szWorkDirM[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szWorkDirM,CountArray(szWorkDirM));

	//����·��
	TCHAR szIniFileM[MAX_PATH]=TEXT("");
	_sntprintf_s(szIniFileM,CountArray(szIniFileM),TEXT("%s\\GlobalParameter.ini"),szWorkDirM);

	//��ȡ����
	CWHIniData IniDataM;
	IniDataM.SetIniFilePath(szIniFileM);

	//��ȡ����
	m_wMarqueeMsgPlayCount=IniDataM.ReadInt(TEXT("MarqueeMsgConfig"),TEXT("PlayCount"),m_wMarqueeMsgPlayCount);
	m_wMarqueeMsgApartTime=IniDataM.ReadInt(TEXT("MarqueeMsgConfig"),TEXT("ApartTime"),m_wMarqueeMsgApartTime);
	m_wSYSPlayTime = IniDataM.ReadInt(TEXT("MarqueeMsgConfig"),TEXT("SYSPlayTime "),m_wSYSPlayTime);
	m_wGamePlayTime = IniDataM.ReadInt(TEXT("MarqueeMsgConfig"),TEXT("GamePlayTime "),m_wGamePlayTime);

	return;
}

//////////////////////////////////////////////////////////////////////////////////
