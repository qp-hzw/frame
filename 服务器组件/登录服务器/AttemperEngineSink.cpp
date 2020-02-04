#include "GameCtrl.h"
#include "AttemperEngineSink.h"
#include "HandleFromCenter.h"
#include "HandleFromGate.h"
#include "PlayerManager.h"


CAttemperEngineSink* g_AttemperEngineSink = NULL;
//////////////////////////////////////////////////////////////////////////////////

#define  IDI_CONNECT_CORRESPOND		2									//重连标识
#define	 TIME_CONNECT_CORRESPOND	30*1000L							//连接时间

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	if(!g_AttemperEngineSink)
		g_AttemperEngineSink = this;

	return;
}

//时间事件
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	switch (dwTimerID)
	{
	case IDI_CONNECT_CORRESPOND:	//连接协调
		{
			g_GameCtrl->KillTimer(IDI_CONNECT_CORRESPOND);
			//发起连接
			g_TCPSocketEngine->Connect(_CPD_SERVER_ADDR, PORT_CENTER);

			return true;
		}
	}

	return false;
}

//数据库事件
bool CAttemperEngineSink::OnEventDataBaseResult(WORD wRequestID, DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//TODOLATER 暂时放到HandleFromGate中, 之后放到HandleFromDB中

	return CHandleFromGate::HandlePacketDB(wRequestID, dwScoketID,  pData, wDataSize);
}

//应答事件
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	return true;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	CPlayerManager::DeletePlayerBySocketID (dwSocketID);
	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	return CHandleFromGate::HandlePacket(Command, pData, wDataSize, dwSocketID);
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
		ServerItem.dwServerID = 0; //TODOLATER lobby.serverid = (0 << 16)  + nodeid;  //lobby.kind = 0
		ServerItem.byServerType = LOBBY_TYPE;
		//lstrcpyn(ServerItem.szServerName,pCPR->szServerName,CountArray(ServerItem.szServerName));
		ServerItem.wServerPort = PORT_LOGON;

		//设置变量
		TCHAR szInernet_ip[32] = TEXT("127.0.0.1");
		//CWHIP::GetInternetIP(szInernet_ip);
		lstrcpyn(ServerItem.szServerAddr,szInernet_ip ,CountArray(ServerItem.szServerAddr));

		//发送数据
		g_TCPSocketEngine->SendData(MDM_REGISTER_CPD,CPR_REGISTER_SERVER,&ServerItem,sizeof(ServerItem));

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
		//设置时间
		g_GameCtrl->SetTimer(IDI_CONNECT_CORRESPOND, TIME_CONNECT_CORRESPOND, 1, 0);
		return true;
	}

	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	return CHandleFromCenter::HandlePacket(Command, pData, wDataSize, wServiceID);
}








