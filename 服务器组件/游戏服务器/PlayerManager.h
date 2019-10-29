#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include <vector>
#include "Player.h"
#include "Stdafx.h"

/*
** �û�������
** 2019-10-29
** wcq
*/
class  CPlayerManager
{
	//�û�����
private:
	static std::vector<CPlayer*>	            s_PlayerArray;					//�û�����

	//��������
public:
	//���캯��
	CPlayerManager(){}
	//��������
	~CPlayerManager(){}

	//���Һ���
public:
	//��
	static bool InsertPlayer(CPlayer * * pIServerUserResult, tagUserInfo & UserInfo, tagUserInfoPlus & UserInfoPlus);
	//ɾ
	static bool DeletePlayer(CPlayer * pPlayer);
	//ɾ ����
	static bool DeleteAllPlayer();
	//��
	static CPlayer * SearchPlayerByEnum(WORD wEnumIndex);
	//��
	static CPlayer * SearchPlayerByID(DWORD dwUserID);

	//ͳ�ƺ���
public:
	//��������
	static DWORD GetPlayerCount() { return (DWORD)s_PlayerArray.size(); }
};

#endif