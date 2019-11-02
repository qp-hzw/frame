#include "Player.h"

//���캯��
CPlayer::CPlayer(tagUserInfo & UserInfo, tagUserInfoPlus & UserInfoPlus)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ��Ҫ�ص�鿴
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	//���Ա���
	CopyMemory(&m_UserInfo,&UserInfo,sizeof(UserInfo));


	//��¼��Ϣ
	m_dwLogonTime=UserInfoPlus.dwLogonTime;
	m_dwInoutIndex=UserInfoPlus.dwInoutIndex;

	//������Ϣ
	m_wBindIndex=UserInfoPlus.wBindIndex;
	m_dwClientAddr=UserInfoPlus.dwClientAddr;
	lstrcpyn(m_szMachineID,UserInfoPlus.szMachineID,CountArray(m_szMachineID));

	//��������
	m_bClientReady=false;
	m_bModifyScore=false;
	m_bAndroidUser=UserInfoPlus.bAndroidUser;
	lstrcpyn(m_szLogonPass,UserInfoPlus.szPassword,CountArray(m_szLogonPass));

	//�йܵ���
	if ((UserInfoPlus.lRestrictScore>0L)&&(UserInfo.lScore>UserInfoPlus.lRestrictScore))
	{
		m_UserInfo.lScore=UserInfoPlus.lRestrictScore;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	//��¼��Ϣ
	m_dwDBQuestID=0L;


	//����ӿ�
	//m_pIServerUserItemSink=NULL;

	//���Ա���
	ZeroMemory(&m_UserProperty,sizeof(m_UserProperty));

	m_dwOfflineGameID = 0;

	return;
}

//�û�ʤ��
WORD CPlayer::GetUserWinRate()
{
	//����ʤ��
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwWinCount*10000L/dwPlayCount);

	return 0;
}

//�û�����
WORD CPlayer::GetUserLostRate()
{
	//��������
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwLostCount*10000L/dwPlayCount);

	return 0;
}

//�û�����
WORD CPlayer::GetUserDrawRate()
{
	//�������
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwDrawCount*10000L/dwPlayCount);

	return 0;
}

//�û�����
WORD CPlayer::GetUserFleeRate()
{
	//��������
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwFleeCount*10000L/dwPlayCount);

	return 0;
}

//����״̬
bool CPlayer::SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID)
{
	m_UserInfo.wTableID=wTableID;
	m_UserInfo.wChairID=wChairID;
	m_UserInfo.cbUserStatus=cbUserStatus;

	return true;
}

//�����û��Ƹ���Ϣ
bool CPlayer::ModifyUserTreasure(DWORD dwTableID, BYTE byTableMode, BYTE byRound, SCORE lUserTreasuse, BYTE byWin) 
{
	/*
	//����״̬
	if (m_pIServerUserItemSink!=NULL) 
	{
		m_pIServerUserItemSink->OnEventModifyUserTreasure(this, dwTableID, byTableMode, byRound, lUserTreasuse, byWin);
	}
	*/

	return true;
}


//���ò���
bool CPlayer::SetUserParameter(DWORD dwClientAddr, WORD wBindIndex, TCHAR szMachineID[LEN_MACHINE_ID], bool bAndroidUser, 
									   bool bClientReady, const double &dLongitude, const double &dLatitude)
{
	//Ч��״̬
	if (m_UserInfo.dwUserID==0L) return false;

	//�û�����
	m_bAndroidUser=bAndroidUser;
	m_bClientReady=bClientReady;

	//��������
	m_wBindIndex=wBindIndex;
	m_dwClientAddr=dwClientAddr;
	lstrcpyn(m_szMachineID,szMachineID,CountArray(m_szMachineID));

	//λ������
	m_UserInfo.dLatitude = dLatitude;
	m_UserInfo.dLongitude = dLongitude;

	return true;
}


//��������
VOID CPlayer::ResetUserItem()
{
	//��¼��Ϣ
	m_dwDBQuestID=0L;
	m_dwLogonTime=0L;
	m_dwInoutIndex=INVALID_DWORD;

	//ϵͳ����
	m_wBindIndex=INVALID_WORD;
	m_dwClientAddr=INADDR_ANY;
	ZeroMemory(m_szMachineID,sizeof(m_szMachineID));

	//�ͻ�����

	m_bAndroidUser=false;

	//��������
	m_bClientReady=false;
	m_bModifyScore=false;
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));

	//���Ա���
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	m_UserInfo.wTableID=INVALID_TABLE;
	m_UserInfo.wChairID=INVALID_CHAIR;
	return;
}
