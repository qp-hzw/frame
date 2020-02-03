#include "Stdafx.h"
#include "Player.h"

//���캯��
CPlayer::CPlayer(tagUserInfo & UserInfo)
{
	//���Ա���
	CopyMemory(&m_UserInfo,&UserInfo,sizeof(UserInfo));

	//��������
	m_bClientReady=false;
	m_bModifyScore=false;
	m_bAndroidUser=false;

	//״̬����
	m_UserInfo.cbUserStatus=US_FREE;
	m_UserInfo.wTableID=INVALID_TABLE;
	m_UserInfo.wChairID=INVALID_CHAIR;

	return;
}

//�û�ʹ�õ���
bool CPlayer::UseProp(DWORD dwGoodsID)
{
	//TODONOW ��ʵ��
	//1. У��
	//2. ֪ͨ���ݿ��޸�

	return true;
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
bool CPlayer::SetUserStatus(BYTE cbUserStatus, DWORD wTableID, WORD wChairID)
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

//��������
VOID CPlayer::ResetUserItem()
{
	//ϵͳ����
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
