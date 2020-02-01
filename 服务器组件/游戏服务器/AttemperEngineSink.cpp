#include "StdAfx.h"
#include "GameCtrl.h"
#include "AttemperEngineSink.h"
#include "TableManager.h"
#include "HandleFromCenter.h"
#include "HandleFromGate.h"
#include "RobotManager.h"
#include <vector>
#include <algorithm>


#include "../../全局定义/Define.h"

CAttemperEngineSink * g_AttemperEngineSink = NULL;

//////////////////////////////////////////////////////////////////////////////////
#define CT_CONNECT_CORRESPOND		100									//连接协调
#define CT_LOAD_SERVICE_CONFIG		101									//加载配置
#define CT_TRY_TO_STOP_SERVICE		102									//停止服务

//////////////////////////////////////////////////////////////////////////////////
//时间标识
#define IDI_LOAD_ANDROID_USER			(IDI_MAIN_MODULE_START+1)			//加载机器人
#define IDI_CONNECT_CORRESPOND			(IDI_MAIN_MODULE_START+3)			//连接时间
#define IDI_GAME_SERVICE_PULSE			(IDI_MAIN_MODULE_START+4)			//服务脉冲
#define IDI_DBCORRESPOND_NOTIFY			(IDI_MAIN_MODULE_START+6)			//缓存通知
#define IDI_LOAD_SENSITIVE_WORD			(IDI_MAIN_MODULE_START+9)			//加载敏感词
#define IDI_WRITE_STOCK_SCORE			(IDI_MAIN_MODULE_START+14)			//写库存记录


#define TIME_CONNECT				30									    //重连时间
//////////////////////////////////////////////////////////////////////////////////
//时间定义 秒

#define TIME_LOAD_ANDROID_USER				3000L								//加载机器
#define TIME_DISTRIBUTE_ANDROID				15L									//分配用户
#define TIME_REPORT_SERVER_INFO				30L									//上报时间
#define TIME_DBCORRESPOND_NOTIFY			3L									//缓存通知时间
#define TIME_LOAD_SYSTEM_MESSAGE			3600L								//系统消息时间
#define TIME_SEND_SYSTEM_MESSAGE			10L									//系统消息时间
#define TIME_CONTROL_ROOM_VALUE_SAVE		120000L								//保存控制值时间yang
#define TIME_READ_ROOM_CONFIG				2000L								//读取房间配置yang
#define TIME_WRITE_STOCK_SCORE				120000L								//写库存记录
#define TIME_WAIT_SERVICE_STOP				5000L								//延迟停止
#define TIME_CONNECT_CORRESPOND				30*1000L									//连接时间

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	if(g_AttemperEngineSink == NULL) g_AttemperEngineSink =this;
}


//启动事件
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	return true;
}

//时间事件
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	//调度时间
	if ((dwTimerID>=IDI_MAIN_MODULE_START)&&(dwTimerID<=IDI_MAIN_MODULE_FINISH))
	{
		//时间处理
		switch (dwTimerID)
		{
		case IDI_CONNECT_CORRESPOND:	//连接协调
			{
				g_TCPSocketEngine->Connect(_CPD_SERVER_ADDR, PORT_CENTER);
				return true;
			}
		case IDI_GAME_SERVICE_PULSE:	//服务维护
			{
				return true;
			}

		case IDI_LOAD_SENSITIVE_WORD:	//加载敏感词
			{
				//投递请求
				//g_GameCtrl->PostDataBaseRequest(DBR_GR_LOAD_SENSITIVE_WORDS,0,NULL,0);				
				return true;
			}
		case IDI_LOAD_ANDROID_USER:		//机器人自动加入房间
			{
				CRobotManager::On_ANDROID_JOIN_GAME();
				return true;
			}
		}
	}

	//桌子时间
	if ((dwTimerID>=IDI_TABLE_MODULE_START)&&(dwTimerID<=IDI_TABLE_MODULE_FINISH))
	{
		//桌子号码
		DWORD dwTableTimerID=dwTimerID-IDI_TABLE_MODULE_START;
		DWORD wTableIndex=(DWORD)(dwTableTimerID/TIME_TABLE_MODULE_RANGE);

		CLog::Log(log_debug, "OnEventTimer  %ld : %ld", wTableIndex, dwTableTimerID%TIME_TABLE_MODULE_RANGE);

		//时间通知
		CTableFrame * pTableFrame= CTableManager::FindTableByTableID(wTableIndex);
		if(pTableFrame != NULL)
			return pTableFrame->OnEventTimer(dwTableTimerID%TIME_TABLE_MODULE_RANGE,wBindParam);
	}

	return false;
}

//数据库事件
bool CAttemperEngineSink::OnEventDataBaseResult(WORD wRequestID, DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//TODONOW 暂时放到CHandleFromGate中, 之后扔到CHandleFromDB
	return CHandleFromGate::HandlePacketDB(wRequestID, dwScoketID, pData, wDataSize);
}

//连接事件
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//协调连接
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//错误判断
		if (nErrorCode!=0)
		{
			//设置时间
			g_GameCtrl->SetTimer(IDI_CONNECT_CORRESPOND, TIME_CONNECT_CORRESPOND, 1, 0);
			return false;
		}

		//变量定义
		tagServerItem ServerItem;
		ZeroMemory(&ServerItem,sizeof(ServerItem));

		//构造数据
		ServerItem.dwServerID = g_GameCtrl->GetServerID();
		ServerItem.byServerType = GAME_TYPE;
		//lstrcpyn(RegisterServer.szServerName, m_pGameServiceOption->szServerName, CountArray(RegisterServer.szServerName));

		//设置变量
		TCHAR szInernet_ip[32] = TEXT("0.0.0.0");
		if( ( 0 != CWHIP::GetInternetIP(szInernet_ip)) ||
			(_tcscmp(szInernet_ip, TEXT("0.0.0.0")) == 0))
		{
			memcpy(szInernet_ip, TEXT("127.0.0.1"), CountArray(szInernet_ip));
		}
		memcpy(szInernet_ip, TEXT("127.0.0.1"), CountArray(szInernet_ip));
		lstrcpyn(ServerItem.szServerAddr,szInernet_ip ,CountArray(ServerItem.szServerAddr));

		ServerItem.wServerPort = 0;

		CLog::Log(log_debug, "ServerID:%d", ServerItem.dwServerID);

		//发送数据
		g_TCPSocketEngine->SendData(MDM_REGISTER,CPR_REGISTER_SERVER,&ServerItem,sizeof(ServerItem));

		return true;
	}

	return true;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//协调连接
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//删除时间
		//g_GameCtrl->KillTimer(IDI_REPORT_SERVER_INFO);

		//设置时间
		g_GameCtrl->SetTimer(IDI_CONNECT_CORRESPOND, TIME_CONNECT_CORRESPOND, 1, 0);

		return true;
	}

	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	return CHandleFromCenter::HandlePacket(Command,pData, wDataSize, wServiceID);
	/*
	//协调连接
	if (wServiceID==NETWORK_CORRESPOND)
	{
	}*/
}

//应答事件
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	return true;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//获取用户
	CPlayer * pPlayer=  CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if(pPlayer == NULL)
		return true;

	//桌子处理
	DWORD tableID = pPlayer->GetTableID();
	CTableFrame *pTableFrame = CTableManager::FindTableByTableID(tableID);
	if (pTableFrame != NULL) //如果在桌子里, 则暂不删除用户
	{
		//table 广播
		pTableFrame->PlayerOffline(pPlayer);

		//发送给协调服, 由协调服通知登录服
		tagOfflineUser data;
		data.dwUserID = pPlayer->GetUserID();
		data.dwServerID = g_GameCtrl->GetKindID(); //以后需要改成GameID
		data.byMask = 1; //表示增加断线用户
		g_TCPSocketEngine->SendData(MDM_USER,SUB_CS_C_USER_OFFLINE,&data,sizeof(tagOfflineUser));
	}
	else //没有在桌子里, 则直接删除用户
	{
		//删除用户
		CPlayerManager::DeletePlayer(pPlayer);
	}
	return true;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	return CHandleFromGate::HandlePacket(Command, pData, wDataSize, dwSocketID);
}
