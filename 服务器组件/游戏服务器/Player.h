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

class  CPlayer
{
	DWORD                           m_dwSocketID;                       //scoketID

	//���Ա���
private:
	tagUserInfo						m_UserInfo;							//�û���Ϣ
	tagUserProperty                 m_UserProperty;                     //��ҵ���

public:
	bool							m_bAndroidUser;						//�����û�

	//��������
private:
	bool							m_bClientReady;						//���ӱ�־
	bool							m_bModifyScore;						//�޸ı�־
	TCHAR							m_szLogonPass[LEN_PASSWORD];		//�û�����

	//ϵͳ����
private:
	DWORD							m_dwClientAddr;						//���ӵ�ַ
	TCHAR							m_szMachineID[LEN_MACHINE_ID];		//������ʶ

	//��������
public:
	//���캯��
	CPlayer(tagUserInfo & UserInfo);
	//��������
	~CPlayer(){}
	//��������
	CPlayer(const CPlayer &player);

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
	BYTE GetSex() { return m_UserInfo.cbGender; }

	 //�Ƿ������
	  bool IsAndroidUser() { return m_bAndroidUser; }
	/**********************************  ��Ϸ��̬���� ****************************************/
public:
	//���Ӻ���
	 DWORD GetTableID() { return m_UserInfo.wTableID; }
	//���Ӻ���
	 virtual BYTE GetChairID() { return static_cast<BYTE>(m_UserInfo.wChairID); }

	//�û�״̬
	 BYTE GetUserStatus() { return m_UserInfo.cbUserStatus; }
	 bool SetUserStatus(BYTE cbUserStatus, DWORD wTableID, WORD wChairID);

	/**********************************  socket��Ϣ ****************************************/
public:
	//�û���ַ
	 DWORD GetClientAddr() { return m_dwClientAddr; }
	//������ʶ
	 LPCTSTR GetMachineID() { return m_szMachineID; }


	//�û�����
public:
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

	//
public:
	//�����û��Ƹ���Ϣ
	bool ModifyUserTreasure(BYTE byTreasureType, SCORE lUserTreasuse, string PayMsg);
	//�����û���Ӯ��¼��
	bool ModifyPlayerScore(BYTE round, WORD wIdentity, SCORE lUserTreasuse, string PayMsg);
	//��Ҿ���
	bool ModifyPlayerExp();

	//��������
private:
	//��������
	VOID ResetUserItem();

};

#endif