#ifndef C_PLAYER_H
#define C_PLAYER_H
#include "Stdafx.h"

class  CPlayer
{
private:
	//Socket����
	DWORD                           m_dwSocketID;                       //scoketID
	//DWORD							m_dwClientAddr;						//���ӵ�ַ
	//DWORD							m_dwLogonTime;						//��¼ʱ��


	//���Ա���
private:
	tagUserInfo						m_UserInfo;							//�û���Ϣ
//	tagUserProperty                 m_UserProperty;                     //��ҵ���
//
//	//��¼��Ϣ
//private:
//	DWORD							m_dwDBQuestID;						//�����ʶ
//	
//	DWORD							m_dwInoutIndex;						//��������
//
//private:
//	bool							m_bAndroidUser;						//�����û�
//
//	//��������
//private:
//	bool							m_bClientReady;						//���ӱ�־
//	bool							m_bModifyScore;						//�޸ı�־
//	TCHAR							m_szLogonPass[LEN_PASSWORD];		//�û�����
//
//	//ϵͳ����
//private:
//	WORD							m_wBindIndex;						//������
//	TCHAR							m_szMachineID[LEN_MACHINE_ID];		//������ʶ
//
//	DWORD							m_dwOfflineGameID;					//����������־ 
//	BYTE							m_OldGameStatus;					//�û�����֮ǰ��״̬

	//��������
public:
	//���캯��
	CPlayer(DWORD dwSocketID, tagUserInfo& userinfo);
	//��������
	~CPlayer(){}

	/**********************************  ��Ϸ��̬���� ****************************************/
public:
	//�û�SocketID
	DWORD GetSocketID() { return m_dwSocketID;}

	//�û���Ϣ
	tagUserInfo * GetUserInfo() { return &m_UserInfo; }
//	//������Ϣ
//	tagUserProperty * GetUserProperty() { return &m_UserProperty; };
//
	//�û���ʶ
	DWORD GetUserID() { return m_UserInfo.dwUserID; }
//	//�û��ǳ�
//	LPCTSTR GetNickName() { return m_UserInfo.szNickName; }
//	//�û��Ա�
//	BYTE GetGender() { return m_UserInfo.cbGender; }
//
//	//�û�Ȩ��
//	 BYTE GetMemberOrder() { return m_UserInfo.cbMemberOrder; } 
//	//����Ȩ��
//	 BYTE GetMasterOrder() { return m_UserInfo.cbMasterOrder; } 
//
//	//��¼ʱ��
//	 DWORD GetLogonTime() { return m_dwLogonTime; }
//
//	 //�Ƿ������
//	  bool IsAndroidUser() { return m_bAndroidUser; }
//	/**********************************  ��Ϸ��̬���� ****************************************/
//public:
//	//���Ӻ���
//	 WORD GetTableID() { return m_UserInfo.wTableID; }
//	//���Ӻ���
//	 WORD GetChairID() { return m_UserInfo.wChairID; }
//
//	//���ö��߱�־ GameID  TODONOW
//	 void SetOfflineGameID(DWORD dwOfflineGameID){m_dwOfflineGameID = dwOfflineGameID;}
//	//��ȡ���߱�־ GameID  TODONOW
//	 DWORD GetOfflineGameID(){return m_dwOfflineGameID; }
//	
//	//�û�����֮ǰ��״̬
//	 BYTE GetOldGameStatus() { return m_OldGameStatus;}
//	 void SetOldGameStatus(BYTE gamestatus) { m_OldGameStatus = gamestatus;}
//
//
//	/**********************************  socket��Ϣ ****************************************/
//public:
//	//�������
//	 WORD GetBindIndex() { return m_wBindIndex; }
//	//�û���ַ
//	// DWORD GetClientAddr() { return m_dwClientAddr; }
//	//������ʶ
//	 LPCTSTR GetMachineID() { return m_szMachineID; }
//
//	//��¼��Ϣ
//public:
//	//�����ʶ
//	virtual DWORD GetDBQuestID() { return m_dwDBQuestID++; }
//
//	//������Ϣ
//public:
//	//�û�����
//	virtual SCORE GetUserScore() { return m_UserInfo.lScore; }
//	//�û��ɼ�
//	virtual SCORE GetUserGrade() { return m_UserInfo.lGrade; }
//	//����û�����
//	virtual SCORE GetUserRoomCard() { return m_UserInfo.lOpenRoomCard; }
//	//�����û�����
//	virtual void SetUserRoomCard(SCORE lRoomCard) { m_UserInfo.lOpenRoomCard = lRoomCard; }
//
//	//�û���ʯ
//	virtual SCORE GetUserDiamond(){ return m_UserInfo.lDiamond; };
//	//�����û���ʯ
//	virtual void SetUserDiamond(SCORE lDiamod) { m_UserInfo.lDiamond = lDiamod; }
//
//	//�û����
//	virtual SCORE GetUserGold(){ return m_UserInfo.lGold; };
//	//�����û����
//	virtual void SetUserGold(SCORE lGold){ m_UserInfo.lGold = lGold; }
};

#endif