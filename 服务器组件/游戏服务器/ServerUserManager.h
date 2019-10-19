#ifndef SERVER_USER_MANAGER_HEAD_FILE
#define SERVER_USER_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////

//�û���Ϣ
class GAME_SERVICE_CLASS CServerUserItem : public IServerUserItem, public IServerUserService
{
	//��Ԫ����
	friend class CServerUserManager;

	//���Ա���
protected:
	tagUserInfo						m_UserInfo;							//�û���Ϣ
	tagUserRule						m_UserRule;							//�û�����
	tagUserScore					m_ScoreFormer;						//��ǰ����
	tagUserProperty                 m_UserProperty;                     //��ҵ���

	//�����¼
protected:
	tagVariationInfo				m_RecordInfo;						//��¼��Ϣ
	tagVariationInfo				m_VariationInfo;					//�����Ϣ

	//���ӱ���
protected:
	DWORD							m_dwUserRight;						//�û�Ȩ��
	DWORD							m_dwMasterRight;					//����Ȩ��

	//�йܱ���
protected:
	SCORE	 						m_lTrusteeScore;					//�йܻ���
	SCORE							m_lRestrictScore;					//���ƻ���
	SCORE	 						m_lFrozenedScore;					//�������

	//��¼��Ϣ
protected:
	DWORD							m_dwDBQuestID;						//�����ʶ
	DWORD							m_dwLogonTime;						//��¼ʱ��
	DWORD							m_dwInoutIndex;						//��������

	//�ͻ�����
protected:
	bool							m_bAndroidUser;						//�����û�

	//�����û�
protected:
	bool							m_bVirtualUser;						//�����û�

	//��������
protected:
	bool							m_bClientReady;						//���ӱ�־
	bool							m_bModifyScore;						//�޸ı�־
	TCHAR							m_szLogonPass[LEN_PASSWORD];		//�û�����

	//ϵͳ����
protected:
	WORD							m_wBindIndex;						//������
	DWORD							m_dwClientAddr;						//���ӵ�ַ
	TCHAR							m_szMachineID[LEN_MACHINE_ID];		//������ʶ

	//����ӿ�
protected:
	IServerUserItemSink *			m_pIServerUserItemSink;				//�ص��ӿ�

	//�ֻ�����
protected:
	WORD	                        m_wDeskPos;                         //��ǰ��ҳ
	WORD	                        m_wDeskCount;                       //��ҳ����
	WORD                            m_wMobileUserRule;                  //�ֻ�����

	DWORD							m_dwOfflineGameID;					//����������־ 

	BYTE							m_OldGameStatus;					//�û�����֮ǰ��״̬

protected:

	//��������
protected:
	//���캯��
	CServerUserItem();
	//��������
	virtual ~CServerUserItem();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//������Ϣ
public:
	//�������
	virtual WORD GetBindIndex() { return m_wBindIndex; }
	//�û���ַ
	virtual DWORD GetClientAddr() { return m_dwClientAddr; }
	//������ʶ
	virtual LPCTSTR GetMachineID() { return m_szMachineID; }

	//���ö��߱�־ GameID  TODONOW
	virtual void SetOfflineGameID(DWORD dwOfflineGameID){m_dwOfflineGameID = dwOfflineGameID;}
	//��ȡ���߱�־ GameID  TODONOW
	virtual DWORD GetOfflineGameID(){return m_dwOfflineGameID; }

	//��¼��Ϣ
public:
	//�����ʶ
	virtual DWORD GetDBQuestID() { return m_dwDBQuestID++; }
	//��¼ʱ��
	virtual DWORD GetLogonTime() { return m_dwLogonTime; }
	//��¼����
	virtual DWORD GetInoutIndex() { return m_dwInoutIndex; }

	//�û���Ϣ
public:
	//�û���Ϣ
	virtual tagUserInfo * GetUserInfo() { return &m_UserInfo; }
	//�û�����
	virtual tagUserRule * GetUserRule() { return &m_UserRule; }
	//������Ϣ
	virtual tagUserProperty * GetUserProperty() { return &m_UserProperty; };

	//������Ϣ
public:
	//�û��Ա�
	virtual BYTE GetGender() { return m_UserInfo.cbGender; }
	//�û���ʶ
	virtual DWORD GetUserID() { return m_UserInfo.dwUserID; }
	//�û��ǳ�
	virtual LPCTSTR GetNickName() { return m_UserInfo.szNickName; }

	//�û�����֮ǰ��״̬
	virtual void SetOldGameStatus(BYTE gamestatus) { m_OldGameStatus = gamestatus;}

	//�û�����֮ǰ��״̬
	virtual BYTE GetOldGameStatus() { return m_OldGameStatus;}


	//״̬�ӿ�
public:
	//���Ӻ���
	virtual WORD GetTableID() { return m_UserInfo.wTableID; }
	//���Ӻ���
	virtual WORD GetChairID() { return m_UserInfo.wChairID; }
	//�û�״̬
	virtual BYTE GetUserStatus() { return m_UserInfo.cbUserStatus; }

	//Ȩ����Ϣ
public:
	//�û�Ȩ��
	virtual DWORD GetUserRight() { return m_dwUserRight; } 
	//����Ȩ��
	virtual DWORD GetMasterRight() { return m_dwMasterRight; } 

	//�ȼ���Ϣ
public:
	//�û�Ȩ��
	virtual BYTE GetMemberOrder() { return m_UserInfo.cbMemberOrder; } 
	//����Ȩ��
	virtual BYTE GetMasterOrder() { return m_UserInfo.cbMasterOrder; } 

	//������Ϣ
public:
	//�û�����
	virtual SCORE GetUserScore() { return m_UserInfo.lScore; }
	//�û��ɼ�
	virtual SCORE GetUserGrade() { return m_UserInfo.lGrade; }
	//����û�����
	virtual SCORE GetUserRoomCard() { return m_UserInfo.lOpenRoomCard; }
	//�����û�����
	virtual void SetUserRoomCard(SCORE lRoomCard) { m_UserInfo.lOpenRoomCard = lRoomCard; }

	//�û���ʯ
	virtual SCORE GetUserDiamond(){ return m_UserInfo.lDiamond; };
	//�����û���ʯ
	virtual void SetUserDiamond(SCORE lDiamod) { m_UserInfo.lDiamond = lDiamod; }

	//�û����
	virtual SCORE GetUserGold(){ return m_UserInfo.lGold; };
	//�����û����
	virtual void SetUserGold(SCORE lGold){ m_UserInfo.lGold = lGold; }

	//�û�����ֵ
	virtual SCORE GetUserControlScore() { return m_UserInfo.lControlScore; }
	virtual VOID  SetUserControlScore( SCORE val ) { m_UserInfo.lControlScore = val; }
	//�й���Ϣ
public:
	//�йܻ���
	virtual SCORE GetTrusteeScore() { return m_lTrusteeScore; }
	//�������
	virtual SCORE GetFrozenedScore() { return m_lFrozenedScore; }

	//������Ϣ
public:
	//�û�ʤ��
	virtual WORD GetUserWinRate();
	//�û�����
	virtual WORD GetUserLostRate();
	//�û�����
	virtual WORD GetUserDrawRate();
	//�û�����
	virtual WORD GetUserFleeRate();
	//��Ϸ����
	virtual DWORD GetUserPlayCount() { return m_UserInfo.dwWinCount+m_UserInfo.dwLostCount+m_UserInfo.dwDrawCount+m_UserInfo.dwFleeCount; }

	//Ч��ӿ�
public:
	//�Ա��ʺ�
	virtual bool ContrastNickName(LPCTSTR pszNickName);

	//��Ϸ״̬
public:
	//����״̬
	virtual bool IsClientReady() { return m_bClientReady; };
	//��������
	virtual VOID SetClientReady(bool bClientReady) { m_bClientReady=bClientReady; }

	//�����û�
public:
	//����״̬
	virtual bool IsAndroidUser() { return m_bAndroidUser; }
	//���ÿ���
	virtual VOID SetAndroidUser(bool bAndroidUser) { m_bAndroidUser=bAndroidUser; }
	//��¼�ӿ�
public:
	//����ж�
	virtual bool IsVariation() { return m_bModifyScore; }
	//��ѯ��¼
	virtual bool QueryRecordInfo(tagVariationInfo & RecordInfo);
	//��ȡ���
	virtual bool DistillVariation(tagVariationInfo & VariationInfo);
	//����ӿ�
public:
	//����״̬
	virtual bool SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID, bool bNotify=true);

	//�޸Ľӿ�
public:
	//�����û��Ƹ���Ϣ
	virtual bool ModifyUserTreasure(DWORD dwTableID, BYTE byTableMode, BYTE byRound, SCORE lUserTreasuse, BYTE byWin);

	//�����ӿ�
public:
	//�����
	virtual bool DetachBindStatus();
	//���в���
	virtual bool ModifyUserInsure(SCORE lScore, SCORE lInsure, SCORE lRevenue);
	//���ò���
	virtual bool SetUserParameter(DWORD dwClientAddr, WORD wBindIndex, TCHAR szMachineID[LEN_MACHINE_ID], bool bAndroidUser, 
									bool bClientReady, const double &dLongitude, const double &dLatitude);

	//�޸�Ȩ��
	virtual VOID ModifyUserRight( DWORD dwAddRight, DWORD dwRemoveRight,bool bGameRight=true);

	//��������
private:
	//��������
	VOID ResetUserItem();

	//�ֻ�����
public:
	//�ֻ�����
	virtual WORD GetMobileUserRule(){return m_wMobileUserRule;}
	//���ö���
	virtual VOID SetMobileUserRule(WORD wMobileUserRule) { m_wMobileUserRule=wMobileUserRule; }
	//��ǰ��ҳ
	virtual WORD GetMobileUserDeskPos(){return m_wDeskPos;}
	//��ǰ��ҳ
	virtual VOID SetMobileUserDeskPos(WORD wMobileUserDeskPos){m_wDeskPos=wMobileUserDeskPos;}
	//��ҳ����
	virtual WORD GetMobileUserDeskCount(){return m_wDeskCount;}
	//��ҳ����
	virtual VOID SetMobileUserDeskCount(WORD wMobileUserDeskCount){m_wDeskCount=wMobileUserDeskCount;}

public:
	//����Ϊ�����û�
	virtual VOID SetVirtualUser(bool bVirtual){m_bVirtualUser = bVirtual;}
	//��ȡ�Ƿ�Ϊ�����û�
	virtual bool IsVirtualUser(){return m_bVirtualUser;}
};

//////////////////////////////////////////////////////////////////////////////////

//�û�������
typedef CWHArray<CServerUserItem *> CServerUserItemArray;
typedef CMap<DWORD,DWORD,CServerUserItem *,CServerUserItem *> CServerUserItemMap;

//�û�������
class GAME_SERVICE_CLASS CServerUserManager : public IServerUserManager
{
	//�û�����
protected:
	CServerUserItemMap				m_UserIDMap;						//�û�����
	CServerUserItemArray			m_UserItemArray;					//�û�����
	CServerUserItemArray			m_UserItemStore;					//�洢�û�

	//����ӿ�
protected:
	IServerUserItemSink *			m_pIServerUserItemSink;				//�ص��ӿ�

	//��������
public:
	//���캯��
	CServerUserManager();
	//��������
	virtual ~CServerUserManager();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ú���
public:
	//���ýӿ�
	virtual bool SetServerUserItemSink(IUnknownEx * pIUnknownEx);

	//���Һ���
public:
	//ö���û�
	virtual IServerUserItem * EnumUserItem(WORD wEnumIndex);
	//�����û�
	virtual IServerUserItem * SearchUserItem(DWORD dwUserID);
	//�����û�
	virtual IServerUserItem * SearchUserItem(LPCTSTR pszNickName);

	//ͳ�ƺ���
public:
	//��������
	virtual DWORD GetUserItemCount() { return (DWORD)m_UserItemArray.GetCount(); }

	//������
public:
	//ɾ���û�
	virtual bool DeleteUserItem();
	//ɾ���û�
	virtual bool DeleteUserItem(IServerUserItem * pIServerUserItem);
	//�����û�
	virtual bool InsertUserItem(IServerUserItem * * pIServerUserResult, tagUserInfo & UserInfo, tagUserInfoPlus & UserInfoPlus);
};

//////////////////////////////////////////////////////////////////////////////////

#endif