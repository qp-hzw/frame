#include "Stdafx.h"
#include "Player.h"

//构造函数
CPlayer::CPlayer(tagUserInfo & UserInfo)
{
	//属性变量
	CopyMemory(&m_UserInfo,&UserInfo,sizeof(UserInfo));

	//辅助变量
	m_bClientReady=false;
	m_bModifyScore=false;
	m_bAndroidUser=false;

	//状态设置
	m_UserInfo.cbUserStatus=US_FREE;
	m_UserInfo.wTableID=INVALID_TABLE;
	m_UserInfo.wChairID=INVALID_CHAIR;

	return;
}

//用户使用道具
bool CPlayer::UseProp(DWORD dwGoodsID)
{
	//TODONOW 待实现
	//1. 校验
	//2. 通知数据库修改

	return true;
}
//用户胜率
WORD CPlayer::GetUserWinRate()
{
	//计算胜率
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwWinCount*10000L/dwPlayCount);

	return 0;
}

//用户输率
WORD CPlayer::GetUserLostRate()
{
	//计算输率
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwLostCount*10000L/dwPlayCount);

	return 0;
}

//用户和率
WORD CPlayer::GetUserDrawRate()
{
	//计算和率
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwDrawCount*10000L/dwPlayCount);

	return 0;
}

//用户逃率
WORD CPlayer::GetUserFleeRate()
{
	//计算逃率
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwFleeCount*10000L/dwPlayCount);

	return 0;
}

//设置状态
bool CPlayer::SetUserStatus(BYTE cbUserStatus, DWORD wTableID, WORD wChairID)
{
	m_UserInfo.wTableID=wTableID;
	m_UserInfo.wChairID=wChairID;
	m_UserInfo.cbUserStatus=cbUserStatus;

	return true;
}

//更新用户财富信息
bool CPlayer::ModifyUserTreasure(DWORD dwTableID, BYTE byTableMode, BYTE byRound, SCORE lUserTreasuse, BYTE byWin) 
{
	/*
	//发送状态
	if (m_pIServerUserItemSink!=NULL) 
	{
		m_pIServerUserItemSink->OnEventModifyUserTreasure(this, dwTableID, byTableMode, byRound, lUserTreasuse, byWin);
	}
	*/

	return true;
}

//重置数据
VOID CPlayer::ResetUserItem()
{
	//系统属性
	m_dwClientAddr=INADDR_ANY;
	ZeroMemory(m_szMachineID,sizeof(m_szMachineID));

	//客户类型
	m_bAndroidUser=false;

	//辅助变量
	m_bClientReady=false;
	m_bModifyScore=false;
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));

	//属性变量
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	m_UserInfo.wTableID=INVALID_TABLE;
	m_UserInfo.wChairID=INVALID_CHAIR;
	return;
}
