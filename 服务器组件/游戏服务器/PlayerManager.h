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
	static bool InsertPlayer(CPlayer * * pIServerUserResult, tagUserInfo & UserInfo);
	//ɾ
	static bool DeletePlayer(CPlayer * pPlayer);
	//ɾ
	static bool DeletePlayerByID(DWORD dwUserID);
	//ɾ
	static bool DeletePlayerBySocketID(DWORD dwSocketID);
	//ɾ ����
	static bool DeleteAllPlayer();
	//��
	static CPlayer * FindPlayerByEnum(WORD wEnumIndex);
	//��
	static CPlayer * FindPlayerByID(DWORD dwUserID);
	//��
	static CPlayer * FindPlayerBySocketID(DWORD dwSocketID);

	//ͳ�ƺ���
public:
	//��������
	static DWORD GetPlayerCount() { return (DWORD)s_PlayerArray.size(); }
};

#endif