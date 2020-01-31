#ifndef C_PLAYER_H
#define C_PLAYER_H
#include "Stdafx.h"
#include "TableFrameBase.h"

//�û�״̬
enum User_Action
{
	US_SIT,        //���¶���
	US_IN_TABLE,   //��table�� -- վ��(�Թ�)����
	US_READY,
	US_LEAVE,		//����뿪
	US_OFFLINE,    //���߶���
	US_PLAYING,    //��Ϸ״̬
	US_NULL,
	US_FREE,       //û��״̬ -- û����Table��
};

class  CPlayer : public IServerUserItem
{
	DWORD                           m_dwSocketID;                       //scoketID

	//���Ա���
private:
	tagUserInfo						m_UserInfo;							//�û���Ϣ
	tagUserProperty                 m_UserProperty;                     //��ҵ���

	//��¼��Ϣ
private:
	DWORD							m_dwDBQuestID;						//�����ʶ
	DWORD							m_dwLogonTime;						//��¼ʱ��
	DWORD							m_dwInoutIndex;						//��������

private:
	bool							m_bAndroidUser;						//�����û�

	//��������
private:
	bool							m_bClientReady;						//���ӱ�־
	bool							m_bModifyScore;						//�޸ı�־
	TCHAR							m_szLogonPass[LEN_PASSWORD];		//�û�����

	//ϵͳ����
private:
	WORD							m_wBindIndex;						//������
	DWORD							m_dwClientAddr;						//���ӵ�ַ
	TCHAR							m_szMachineID[LEN_MACHINE_ID];		//������ʶ

	BYTE							m_OldGameStatus;					//�û�����֮ǰ��״̬

	//��������
public:
	//���캯��
	CPlayer(tagUserInfo & UserInfo);
	//��������
	~CPlayer(){}

	/**********************************  ��Ϸ��̬���� ****************************************/
public:
	//�û�SocketID
	DWORD GetSocketID() { return m_dwSocketID;}
	void SetSocketID(DWORD socketID){m_dwSocketID = socketID;}

	//�û���Ϣ
	tagUserInfo * GetUserInfo() { return &m_UserInfo; }
	//������Ϣ
	tagUserProperty * GetUserProperty() { return &m_UserProperty; };

	//�û���ʶ
	DWORD GetUserID() { return m_UserInfo.dwUserID; }
	//�û��ǳ�
	LPCTSTR GetNickName() { return m_UserInfo.szNickName; }
	//�û��Ա�
	BYTE GetGender() { return m_UserInfo.cbGender; }

	//�û�Ȩ��
	 BYTE GetMemberOrder() { return m_UserInfo.cbMemberOrder; } 
	//����Ȩ��
	 BYTE GetMasterOrder() { return m_UserInfo.cbMasterOrder; } 

	//��¼ʱ��
	 DWORD GetLogonTime() { return m_dwLogonTime; }

	 //�Ƿ������
	  bool IsAndroidUser() { return m_bAndroidUser; }
	/**********************************  ��Ϸ��̬���� ****************************************/
public:
	//���Ӻ���
	 DWORD GetTableID() { return m_UserInfo.wTableID; }
	//���Ӻ���
	 virtual WORD GetChairID() { return m_UserInfo.wChairID; }

	//�û�״̬
	 BYTE GetUserStatus() { return m_UserInfo.cbUserStatus; }
	 bool SetUserStatus(BYTE cbUserStatus, DWORD wTableID, WORD wChairID);

	 //�û�����֮ǰ��״̬
	 BYTE GetOldGameStatus() { return m_OldGameStatus;}
	 void SetOldGameStatus(BYTE gamestatus) { m_OldGameStatus = gamestatus;}


	/**********************************  socket��Ϣ ****************************************/
public:
	//�������
	 WORD GetBindIndex() { return m_wBindIndex; }
	//�û���ַ
	 DWORD GetClientAddr() { return m_dwClientAddr; }
	//������ʶ
	 LPCTSTR GetMachineID() { return m_szMachineID; }


	//��¼��Ϣ
public:
	//�����ʶ
	virtual DWORD GetDBQuestID() { return m_dwDBQuestID++; }
	

	//�û�����
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

	//�û�ʹ�õ��� 
	bool UseProp(DWORD dwGoodsID);

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

	//�޸Ľӿ�
public:
	//�����û��Ƹ���Ϣ
	virtual bool ModifyUserTreasure(DWORD dwTableID, BYTE byTableMode, BYTE byRound, SCORE lUserTreasuse, BYTE byWin);

	//�����ӿ�
public:
	//���ò���
	virtual bool SetUserParameter(DWORD dwClientAddr, TCHAR szMachineID[LEN_MACHINE_ID], bool bAndroidUser, 
		bool bClientReady, const double &dLongitude, const double &dLatitude);

	//��������
private:
	//��������
	VOID ResetUserItem();

};

#endif