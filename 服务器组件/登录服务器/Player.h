#ifndef C_PLAYER_H
#define C_PLAYER_H
#include "Stdafx.h"

class  CPlayer
{
private:
	//Socket����
	DWORD                           m_dwSocketID;                       //scoketID

	//���Ա���
private:
	tagUserInfo						m_UserInfo;							//�û���Ϣ

	//
private:
	DWORD							m_dwOfflineGameID;					//����ʱ ���ڵ���Ϸ����־

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

	//�û���ʶ
	DWORD GetUserID() { return m_UserInfo.dwUserID; }


	/**********************************  ��Ϸ��̬���� ****************************************/
public:

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