#include "Stdafx.h"
#include "Player.h"
#include "DataBaseEngineSink.h"

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

//拷贝构造
CPlayer::CPlayer(const CPlayer &player)
{
	m_dwSocketID = player.m_dwSocketID;
	memcpy(&m_UserInfo, &player.m_UserInfo, sizeof(tagUserInfo));
	memcpy(&m_UserProperty, &player.m_UserProperty, sizeof(tagUserProperty));
	m_bAndroidUser = player.m_bAndroidUser;
	m_bClientReady = player.m_bClientReady;
	m_bModifyScore = player.m_bModifyScore;
	m_dwClientAddr = player.m_dwClientAddr;
	memcpy(m_szLogonPass, player.m_szLogonPass, sizeof(TCHAR)*LEN_PASSWORD);
	memcpy(m_szMachineID, player.m_szMachineID, sizeof(TCHAR)*LEN_MACHINE_ID);
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


//修改用户财富
bool CPlayer::ModifyUserTreasure(BYTE byTreasureType,  SCORE lUserTreasuse, string strPayMsg) 
{
	if( (byTreasureType < TREASURE_FK) || (byTreasureType > TREASURE_DIAMOND) )
	{
		return false;
	}

	//输入参数
	g_TreasureDB->ResetParameter();
	g_TreasureDB->AddParameter(TEXT("@UserID"), GetUserID());
	g_TreasureDB->AddParameter(TEXT("@TreasureType"),byTreasureType);
	g_TreasureDB->AddParameter(TEXT("@TreasureNum"),lUserTreasuse);
	g_TreasureDB->AddParameter(TEXT("@Comment"), strPayMsg.c_str());

	//执行查询
	LONG lResultCode = g_TreasureDB->ExecuteProcess(TEXT("GSP_TreasureRecord_Insert"), true);

	//TODONOW 修改用户在server中的数据

	return true;
}

//更新用户输赢记录表
bool CPlayer::ModifyPlayerScore(BYTE round, SCORE lUserTreasuse, string Msg)
{
	//输入参数
	g_TreasureDB->ResetParameter();
	g_TreasureDB->AddParameter(TEXT("@UserID"), GetUserID());
	g_TreasureDB->AddParameter(TEXT("@CurCount"),round);
	g_TreasureDB->AddParameter(TEXT("@Score"),lUserTreasuse);
	g_TreasureDB->AddParameter(TEXT("@OnlyID"), Msg.c_str());

	//执行查询
	LONG lResultCode = g_TreasureDB->ExecuteProcess(TEXT("GSP_PlayerScore_Insert"), true);

	return true;
}

//更新用户经验值
bool CPlayer::ModifyPlayerExp()
{
	/*
	//输入参数
	g_TreasureDB->ResetParameter();
	g_TreasureDB->AddParameter(TEXT("@UserID"), GetUserID());
	g_TreasureDB->AddParameter(TEXT("@CurCount"),round);

	//执行查询
	LONG lResultCode = g_TreasureDB->ExecuteProcess(TEXT("GSP_PlayerScore_Insert"), true);
	*/

	return true;
}
