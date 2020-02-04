#include "Player.h"
#include "DataBaseEngineSink.h"

//���캯��
CPlayer::CPlayer(DWORD dwSocketID, tagUserInfo& UserInfo)
{
	ZeroMemory(&m_UserInfo, sizeof(tagUserInfo));
	CopyMemory(&m_UserInfo, &UserInfo, sizeof(tagUserInfo));

	m_dwSocketID = dwSocketID;

	m_ArrPrp = new list<tagUserProperty*>();
	m_ArrPrp->clear();
}

//��������
CPlayer::~CPlayer()
{
}

//��ʼ������
void CPlayer::InitProp(tagUserProperty prop)
{
	m_ArrPrp->push_back(pProp);
}
//��ӵ���
void CPlayer::AddProp(tagUserProperty prop)
{
	tagUserProperty* pProp = new tagUserProperty();
	pProp->dwPropID = prop.dwPropID;
	pProp->dwPropCount = prop.dwPropCount;
	m_ArrPrp->push_back(pProp);

	//���浽���ݿ�
	g_AccountsDB->ResetParameter();
	g_AccountsDB->AddParameter(TEXT("@UserID"), GetUserID());
	g_AccountsDB->AddParameter(TEXT("@GoodsID"), prop.dwPropID);
	g_AccountsDB->AddParameter(TEXT("@GoodsNum"), prop.dwPropCount);
	LONG lResultCode = g_AccountsDB->ExecuteProcess(TEXT("GSP_CL_Service_RequestLottery"), true);	
	if(lResultCode != DB_SUCCESS)
	{
		CLog::Log(log_error, "[%ld] insert prop failed  [%ld:%ld]", GetUserID(), prop.dwPropID, prop.dwPropCount);
	}
}
