#include "Stdafx.h"
#include "Player.h"
#include "DataBaseEngineSink.h"

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


//�޸��û��Ƹ�
bool CPlayer::ModifyUserTreasure(BYTE byTreasureType,  SCORE lUserTreasuse, string strPayMsg) 
{
	if( (byTreasureType < TREASURE_FK) || (byTreasureType > TREASURE_DIAMOND) )
	{
		return false;
	}

	//�������
	g_TreasureDB->ResetParameter();
	g_TreasureDB->AddParameter(TEXT("@UserID"), GetUserID());
	g_TreasureDB->AddParameter(TEXT("@TreasureType"),byTreasureType);
	g_TreasureDB->AddParameter(TEXT("@TreasureNum"),lUserTreasuse);
	g_TreasureDB->AddParameter(TEXT("@Comment"), strPayMsg.c_str());

	//ִ�в�ѯ
	LONG lResultCode = g_TreasureDB->ExecuteProcess(TEXT("GSP_TreasureRecord_Insert"), true);

	//TODONOW �޸��û���server�е�����

	return true;
}

//�����û���Ӯ��¼��
bool CPlayer::ModifyPlayerScore(BYTE round, SCORE lUserTreasuse, string Msg)
{
	//�������
	g_TreasureDB->ResetParameter();
	g_TreasureDB->AddParameter(TEXT("@UserID"), GetUserID());
	g_TreasureDB->AddParameter(TEXT("@CurCount"),round);
	g_TreasureDB->AddParameter(TEXT("@Score"),lUserTreasuse);
	g_TreasureDB->AddParameter(TEXT("@OnlyID"), Msg.c_str());

	//ִ�в�ѯ
	LONG lResultCode = g_TreasureDB->ExecuteProcess(TEXT("GSP_PlayerScore_Insert"), true);

	return true;
}

//�����û�����ֵ
bool CPlayer::ModifyPlayerExp()
{
	/*
	//�������
	g_TreasureDB->ResetParameter();
	g_TreasureDB->AddParameter(TEXT("@UserID"), GetUserID());
	g_TreasureDB->AddParameter(TEXT("@CurCount"),round);

	//ִ�в�ѯ
	LONG lResultCode = g_TreasureDB->ExecuteProcess(TEXT("GSP_PlayerScore_Insert"), true);
	*/

	return true;
}
