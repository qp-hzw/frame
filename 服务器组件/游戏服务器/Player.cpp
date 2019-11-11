#include "Player.h"

//构造函数
CPlayer::CPlayer(tagUserInfo & UserInfo)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 需要重点查看
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	//属性变量
	CopyMemory(&m_UserInfo,&UserInfo,sizeof(UserInfo));


	//登录信息
	m_dwLogonTime=UserInfo.dwLogonTime;
	m_dwInoutIndex=UserInfo.dwInoutIndex;

	//连接信息
	m_wBindIndex=UserInfo.wBindIndex;
	m_dwClientAddr=UserInfo.dwClientAddr;
	lstrcpyn(m_szMachineID,UserInfo.szMachineID,CountArray(m_szMachineID));

	//辅助变量
	m_bClientReady=false;
	m_bModifyScore=false;
	m_bAndroidUser=UserInfo.bAndroidUser;
	lstrcpyn(m_szLogonPass,UserInfo.szPassword,CountArray(m_szLogonPass));

	//托管调整
	if ((UserInfo.lRestrictScore>0L)&&(UserInfo.lScore>UserInfo.lRestrictScore))
	{
		m_UserInfo.lScore=UserInfo.lRestrictScore;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	//登录信息
	m_dwDBQuestID=0L;


	//组件接口
	//m_pIServerUserItemSink=NULL;

	//属性变量
	ZeroMemory(&m_UserProperty,sizeof(m_UserProperty));

	m_dwOfflineGameID = 0;

	return;
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
bool CPlayer::SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID)
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


//设置参数
bool CPlayer::SetUserParameter(DWORD dwClientAddr, TCHAR szMachineID[LEN_MACHINE_ID], bool bAndroidUser, 
									   bool bClientReady, const double &dLongitude, const double &dLatitude)
{
	//效验状态
	if (m_UserInfo.dwUserID==0L) return false;

	//用户属性
	m_bAndroidUser=bAndroidUser;
	m_bClientReady=bClientReady;

	//连接属性
	m_dwClientAddr=dwClientAddr;
	lstrcpyn(m_szMachineID,szMachineID,CountArray(m_szMachineID));

	//位置属性
	m_UserInfo.dLatitude = dLatitude;
	m_UserInfo.dLongitude = dLongitude;

	return true;
}


//重置数据
VOID CPlayer::ResetUserItem()
{
	//登录信息
	m_dwDBQuestID=0L;
	m_dwLogonTime=0L;
	m_dwInoutIndex=INVALID_DWORD;

	//系统属性
	m_wBindIndex=INVALID_WORD;
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
