#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H
#include <vector>
#include "Stdafx.h"

class CPlayer;

class  CPlayerManager
{
	//�û�����
private:
	static std::vector<CPlayer*>	            s_PlayerArray;					//�û�����

	//���Һ���
public:
	//��
	static bool InsertPlayer(DWORD dwSocketID, tagUserInfo & UserInfo);
	//ɾ
	static bool DeletePlayer(CPlayer * pPlayer);
	//ɾ
	static bool DeletePlayerByID(DWORD dwUserID);
	//ɾ
	static bool DeletePlayerBySocketID(DWORD dwSocketID);
	//ɾ ����
	static bool DeleteAllPlayer();
	//��
	static CPlayer * FindPlayerByID(DWORD dwUserID);
	//��
	static CPlayer * FindPlayerBySocketID(DWORD dwSocketID);

	//�Ͽ���������
	 static void CloseSocket(CPlayer* player);

	//ͳ�ƺ���
public:
	//��������
	static DWORD GetPlayerCount() { return (DWORD)s_PlayerArray.size(); }
};

#endif