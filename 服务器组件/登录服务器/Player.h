#ifndef C_PLAYER_H_LOGON
#define C_PLAYER_H_LOGON
#include "Stdafx.h"
#include <list>
#include <vector>
#include <map>

class  CPlayer
{
private:
	DWORD                           m_dwSocketID;                       //scoketID

public:
	tagUserInfo						m_UserInfo;							//�û���Ϣ
	static std::map< DWORD, std::vector<tagUserProperty>>	m_ArrPrp;		//�����Ʒ


	//��������
public:
	//���캯��
	CPlayer(DWORD dwSocketID, tagUserInfo& userinfo);
	//��������
	~CPlayer();

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
	//��ʼ���������� db����->server
	void InitProp(tagUserProperty prop);
	//��ӱ��� client->server
	void AddProp(tagUserProperty prop);
	//��ȡ��ҵ���
	std::vector<tagUserProperty> GetProps();
};

#endif