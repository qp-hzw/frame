#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H
#include <vector>
#include "Stdafx.h"
#include <map>

class CPlayer;

class  CPlayerManager
{
	//�û�����
private:
	static std::vector<CPlayer*>	            s_PlayerArray;					//�û�����
	static std::map<DWORD, DWORD>               s_OfflinePlayer;				//��������б�

	//���Һ���
public:
	//��
	static bool InsertPlayer(DWORD dwSocketID, tagUserInfo& UserInfoPlus);
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
	//�� ���ֲ�����
	static std::vector<CPlayer*> FindPlayerByClubID(DWORD dwClubID);


	//�������
public:
	//��
	static void AddOfflinePlayer(DWORD userID, DWORD dwServerID);
	//ɾ
	static void DeleteOfflinePlayer(DWORD userID);
	//��
	static DWORD FindOfflineGameID(DWORD userID);


	//ͳ�ƺ���
public:
	//��������
	static DWORD GetPlayerCount() { return (DWORD)s_PlayerArray.size(); }
};

#endif