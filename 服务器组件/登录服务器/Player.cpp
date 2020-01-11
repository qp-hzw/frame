#include "Player.h"

//¹¹Ôìº¯Êý
CPlayer::CPlayer(DWORD dwSocketID, tagUserInfo& UserInfo)
{
	ZeroMemory(&m_UserInfo, sizeof(tagUserInfo));
	CopyMemory(&m_UserInfo, &UserInfo, sizeof(tagUserInfo));

	m_dwSocketID = dwSocketID;
}