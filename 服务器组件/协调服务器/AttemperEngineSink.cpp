#include "StdAfx.h"
#include "GameCtrl.h"
#include "AttemperEngineSink.h"
#include "ServerItemManager.h"
#include "HandleFromLobby.h"
#include "HandleFromGame.h"

//��ȡ�¼�
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

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	CServerItemManager::DeleteItem(dwSocketID);

	return true;
}

//����ע��
bool CAttemperEngineSink::DoServerRegister(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��Ϣ��У��
	if( (Command.wMainCmdID != MDM_REGISTER)  ||
		(Command.wSubCmdID != CPR_REGISTER_SERVER))
		return false;

	//����Ч��
	if (wDataSize!=sizeof(tagServerItem)) return false;

	//��Ϣ����
	tagServerItem * pItem=(tagServerItem *)pData;

	//��Ч�ж�
	if (pItem->szServerAddr[0]==0)
	{
		//notify server
		g_GameCtrl->SendData(dwSocketID,MDM_REGISTER,CPO_REGISTER_FAILURE);

		//�ж�����
		g_TCPNetworkEngine->CloseSocket(dwSocketID);
		return false;
	}

	//��������
	tagServerItem ServerItem;
	ZeroMemory(&ServerItem,sizeof(ServerItem));

	//��������
	ServerItem.dwServerID = pItem->dwServerID;
	ServerItem.dwSocketID = dwSocketID;
	ServerItem.byServerType = pItem->byServerType;
	lstrcpyn(ServerItem.szServerName,pItem->szServerName,CountArray(ServerItem.szServerName));
	lstrcpyn(ServerItem.szServerAddr,pItem->szServerAddr,CountArray(ServerItem.szServerAddr));
	ServerItem.wServerPort = pItem->wServerPort;

	CServerItemManager::InsertItem(ServerItem);

	//���� ע�����
	g_GameCtrl->SendData(dwSocketID,MDM_REGISTER,CPO_REGISTER_SUCESS);

	return true;
}