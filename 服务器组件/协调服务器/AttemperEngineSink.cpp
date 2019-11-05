#include "StdAfx.h"
#include "GameCtrl.h"
#include "AttemperEngineSink.h"
#include "ServerItemManager.h"
#include "HandleFromLobby.h"
#include "HandleFromGame.h"

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	tagServerItem* item = CServerItemManager::FindItemBySocketID(dwSocketID);
	if(item == NULL)
	{
		return DoServerRegister(Command, pData, wDataSize, dwSocketID);
	}

	switch(item->byServerType)
	{
	case LOBBY_TYPE:
		{
			return CHandleFromLobby::HandlePacket(Command, pData, wDataSize, dwSocketID);
		}
	case GAME_TYPE:
		{
			return CHandleFromGame::HandlePacket(Command, pData, wDataSize, dwSocketID);
		}
	}

	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	CServerItemManager::DeleteItem(dwSocketID);

	return true;
}

//服务注册
bool CAttemperEngineSink::DoServerRegister(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//消息号校验
	if( (Command.wMainCmdID != MDM_REGISTER)  ||
		(Command.wSubCmdID != CPR_REGISTER_SERVER))
		return false;

	//数据效验
	if (wDataSize!=sizeof(tagServerItem)) return false;

	//消息定义
	tagServerItem * pItem=(tagServerItem *)pData;

	//有效判断
	if (pItem->szServerAddr[0]==0)
	{
		//notify server
		g_GameCtrl->SendData(dwSocketID,MDM_REGISTER,CPO_REGISTER_FAILURE);

		//中断网络
		g_TCPNetworkEngine->CloseSocket(dwSocketID);
		return false;
	}

	//变量定义
	tagServerItem ServerItem;
	ZeroMemory(&ServerItem,sizeof(ServerItem));

	//构造数据
	ServerItem.dwServerID = pItem->dwServerID;
	ServerItem.dwSocketID = dwSocketID;
	ServerItem.byServerType = pItem->byServerType;
	lstrcpyn(ServerItem.szServerName,pItem->szServerName,CountArray(ServerItem.szServerName));
	lstrcpyn(ServerItem.szServerAddr,pItem->szServerAddr,CountArray(ServerItem.szServerAddr));
	ServerItem.wServerPort = pItem->wServerPort;

	CServerItemManager::InsertItem(ServerItem);

	//发送 注册完成
	g_GameCtrl->SendData(dwSocketID,MDM_REGISTER,CPO_REGISTER_SUCESS);

	return true;
}