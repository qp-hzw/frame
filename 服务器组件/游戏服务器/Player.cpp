#include "Player.h"

//���캯��
CPlayer::CPlayer(tagUserInfo & UserInfo, tagUserInfoPlus & UserInfoPlus)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ��Ҫ�ص�鿴
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
		//ԭʼ����
	m_ScoreFormer.lScore=UserInfo.lScore;
	m_ScoreFormer.lOpenRoomCard=UserInfo.lOpenRoomCard;
	m_ScoreFormer.dwWinCount=UserInfo.dwWinCount;
	m_ScoreFormer.dwLostCount=UserInfo.dwLostCount;
	m_ScoreFormer.dwDrawCount=UserInfo.dwDrawCount;
	m_ScoreFormer.dwFleeCount=UserInfo.dwFleeCount;
	m_ScoreFormer.dwExperience=UserInfo.dwExperience;

	//���Ա���
	CopyMemory(&m_UserInfo,&UserInfo,sizeof(UserInfo));
	ZeroMemory(&m_RecordInfo,sizeof(m_RecordInfo));
	ZeroMemory(&m_VariationInfo,sizeof(m_VariationInfo));

	//��¼��Ϣ
	m_dwLogonTime=UserInfoPlus.dwLogonTime;
	m_dwInoutIndex=UserInfoPlus.dwInoutIndex;

	//�û�Ȩ��
	m_dwUserRight=UserInfoPlus.dwUserRight;
	m_dwMasterRight=UserInfoPlus.dwMasterRight;
	m_lRestrictScore=UserInfoPlus.lRestrictScore;

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
		m_lTrusteeScore=UserInfo.lScore-UserInfoPlus.lRestrictScore;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	//��¼��Ϣ
	m_dwDBQuestID=0L;

	//�ͻ�����
	m_bVirtualUser=false;

	//����ӿ�
	//m_pIServerUserItemSink=NULL;

	//���Ա���
	ZeroMemory(&m_UserRule,sizeof(m_UserRule));
	ZeroMemory(&m_UserProperty,sizeof(m_UserProperty));

	//�ֻ�����
	m_wMobileUserRule=0;
	m_wMobileUserRule |= VIEW_MODE_PART;
	m_wMobileUserRule |= VIEW_INFO_LEVEL_1;
	m_wDeskPos=0;
	m_wDeskCount=1;

	m_dwOfflineGameID = 0;

	return;
}

//��������
CPlayer::~CPlayer()
{
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

//�Ա��ʺ�
bool CPlayer::ContrastNickName(LPCTSTR pszNickName)
{
	//Ч�����
	ASSERT(pszNickName!=NULL);
	if (pszNickName==NULL) return false;

	//���ȶԱ�
	INT nContrastLen=lstrlen(pszNickName);
	INT nSourceLen=lstrlen(m_UserInfo.szNickName);

	//�ַ��Ա�
	if (nContrastLen!=nSourceLen) return false;
	if (CompareString(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,pszNickName,nContrastLen,m_UserInfo.szNickName,nSourceLen)!=CSTR_EQUAL) return false;

	return true;
}

//����״̬
bool CPlayer::SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID, bool bNotify)
{
	//��¼��Ϣ
	WORD wOldTableID=m_UserInfo.wTableID;
	WORD wOldChairID=m_UserInfo.wChairID;

	//���ñ���
	m_UserInfo.wTableID=wTableID;
	m_UserInfo.wChairID=wChairID;
	m_UserInfo.cbUserStatus=cbUserStatus;

	//����״̬
	//if (m_pIServerUserItemSink!=NULL && bNotify) m_pIServerUserItemSink->OnEventUserItemStatus(this,wOldTableID,wOldChairID);

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

//��ѯ��¼
bool CPlayer::QueryRecordInfo(tagVariationInfo & RecordInfo)
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//���ñ���
	RecordInfo=m_RecordInfo;

	return true;
}

//��ȡ���
bool CPlayer::DistillVariation(tagVariationInfo & VariationInfo)
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//�޸��ж�
	if (m_bModifyScore==true)
	{
		//���ñ���
		VariationInfo=m_VariationInfo;
	}
	else
	{
		//���ñ���
		ZeroMemory(&VariationInfo,sizeof(VariationInfo));
	}

	//��ԭ����
	m_bModifyScore=false;
	ZeroMemory(&m_VariationInfo,sizeof(m_VariationInfo));

	return true;
}

//�����
bool CPlayer::DetachBindStatus()
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//�û�����
	m_bClientReady=false;

	//��������
	m_wBindIndex=INVALID_WORD;

	return true;
}

//���в���
bool CPlayer::ModifyUserInsure(SCORE lScore, SCORE lInsure, SCORE lRevenue)
{
	//ƽ��Ч��
	/*ASSERT((m_bAndroidUser==true)||(((lScore+lInsure+lRevenue)==0L)&&(lRevenue>=0L)));
	if ((m_bAndroidUser==false)&&(((lScore+lInsure+lRevenue)!=0L)||(lRevenue<0L))) return false;*/

	//Ч��״̬
	/*ASSERT((m_UserInfo.dwUserID!=0L)&&((m_UserInfo.lScore+m_lTrusteeScore+lScore)>=0L));
	if ((m_UserInfo.dwUserID==0L)||((m_UserInfo.lScore+m_lTrusteeScore+lScore)<0L)) return false;*/

	//���û���
	m_UserInfo.lScore+=lScore;

	//��¼��Ϣ
	m_RecordInfo.lScore+=lScore;
	m_RecordInfo.lInsure+=lInsure;
	m_RecordInfo.lRevenue+=lRevenue;

	//�йܻ���
	if (m_lRestrictScore>0L)
	{
		//��������
		SCORE lTotalScore=m_UserInfo.lScore+m_lTrusteeScore;

		//�йܵ���
		if (lTotalScore>m_lRestrictScore)
		{
			m_UserInfo.lScore=m_lRestrictScore;
			m_lTrusteeScore=lTotalScore-m_lRestrictScore;
		}
		else
		{
			m_lTrusteeScore=0L;
			m_UserInfo.lScore=lTotalScore;
		}
	}

	/*
	//����״̬
	ASSERT(m_pIServerUserItemSink!=NULL);
	//if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemScore(this, SCORE_REASON_INSURE);
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

//�޸�Ȩ��
VOID CPlayer::ModifyUserRight( DWORD dwAddRight, DWORD dwRemoveRight, bool bGameRight)
{
	//���Ȩ��
	m_dwUserRight |= dwAddRight;

	//ɾ��Ȩ��
	m_dwUserRight &= ~dwRemoveRight;

	/*
	//����״̬
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemRight(this,dwAddRight,dwRemoveRight,bGameRight);
	*/

	return;
}

//��������
VOID CPlayer::ResetUserItem()
{
	//���ӱ���
	m_dwUserRight=0L;
	m_dwMasterRight=0L;

	//�йܱ���
	m_lTrusteeScore=0L;
	m_lRestrictScore=0L;
	m_lFrozenedScore=0L;
	m_lFrozenedScore=0L;

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
	ZeroMemory(&m_UserRule,sizeof(m_UserRule));
	ZeroMemory(&m_ScoreFormer,sizeof(m_ScoreFormer));
	//ZeroMemory(&m_UserProperty,sizeof(m_UserProperty));
	m_UserInfo.wTableID=INVALID_TABLE;
	m_UserInfo.wChairID=INVALID_CHAIR;

	//��¼��Ϣ
	ZeroMemory(&m_RecordInfo,sizeof(m_RecordInfo));
	ZeroMemory(&m_VariationInfo,sizeof(m_VariationInfo));

	//�ֻ�����
	m_wMobileUserRule=0;
	m_wMobileUserRule |= VIEW_MODE_PART;
	m_wMobileUserRule |= VIEW_INFO_LEVEL_1;
	m_wDeskPos=0;
	m_wDeskCount=1;

	return;
}

//////////////////////////////////////////////////////////////////////////////////