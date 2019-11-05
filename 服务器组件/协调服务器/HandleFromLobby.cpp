#include "HandleFromLobby.h"
#include "GameCtrl.h"
#include "ServerItemManager.h"

//消息分发处理函数
bool CHandleFromLobby::HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_TRANSFER:	//中转服务
		{
			return OnTCPNetworkMainTransfer(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	}
	return false;
}

#pragma region  一级分发处理
//中转服务
bool CHandleFromLobby::OnTCPNetworkMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case CPR_LP_CLUB_TABLE_DISSOLVE :	//登录服通知协调服, 协调服通知游戏服 解散桌子
		{
			//校验数据
			if (wDataSize!=sizeof(STR_CPR_LP_CLUB_TABLE_DISSOLVE)) return true;

			//通知特定游戏服 以GameID为判断依据
			//TOODNOW added by WangChengQing 因为不知道协调服如何通过gmaeID找到游戏服的socketID
			//所以此处是通知所有游戏服，然后游戏服自我校验
			//之后改掉

			STR_CPR_LP_CLUB_TABLE_DISSOLVE *pCPR = (STR_CPR_LP_CLUB_TABLE_DISSOLVE*) pData;

			STR_CPO_PG_CLUB_TABLE_DISSOLVE CPO;
			CPO.dwGameID = pCPR->dwGameID;
			CPO.dwTableID = pCPR->dwTableID;

			//发送通知 -- 全部游戏服
			g_GameCtrl->SendDataBatch(MDM_TRANSFER,CPO_PG_CLUB_TABLE_DISSOLVE,&CPO,sizeof(CPO));

			return true;
		}
		case CPR_LP_CREATE_TABLE:	//创建桌子 查询可用的GameID 
		{
			//校验数据
			if (wDataSize!=sizeof(STR_CPR_LP_CREATE_TABLE)) return true;
			STR_CPR_LP_CREATE_TABLE *pCPR = (STR_CPR_LP_CREATE_TABLE*) pData;


			DWORD dwMatchGameID = 0;
			DWORD dwMinOnlineCount = 60*4; //每个负载最大人数为240 如果以后修改, 这里也需要修改 added by WangChengQing

			//获取到游戏服KindID匹配的数据
			tagServerItem *pItem = CServerItemManager::FindItemByKindID(pCPR->dwKindID); 

			//CPO构造
			STR_CPO_PL_CREATE_TABLE CPO;
			ZeroMemory(&CPO, sizeof(CPO));

			if(NULL == pItem)
			{
				CPO.dwResultCode = 1;
				memcpy(CPO.szDescribeString, TEXT("未找到合适的游戏服务器"), sizeof(CPO.szDescribeString));
				CPO.dwSocketID = pCPR->dwSocketID;
			}
			else
			{
				CPO.dwResultCode = 0;
				CPO.dwGameID = pItem->dwServerID & 0x00FFFF00;
				CPO.dwSocketID = pCPR->dwSocketID;
			}

			//发送消息
			g_GameCtrl->SendData(dwSocketID,MDM_TRANSFER,CPO_PL_CREATE_TABLE, &CPO, sizeof(CPO));
			return true;
		}

	}

	return false;
}
#pragma endregion 


/*
//发送列表
bool CAttemperEngineSink::SendServerListItem(DWORD dwSocketID)
{
	//变量定义
	WORD wPacketSize=0L;
	POSITION Position=NULL;
	BYTE cbBuffer[SOCKET_TCP_PACKET];

	for(auto item : CServerItemManager::FindAllGameServer())
	{
		//发送数据
		if ((wPacketSize+sizeof(tagServerItem))>sizeof(cbBuffer))
		{
			g_GameCtrl->SendData(dwSocketID,MDM_CPD_LIST,CPO_PL_LIST_INSERT_GAME_LIST,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		tagServerItem * pGameServer=(tagServerItem *)(cbBuffer+wPacketSize);
		if(item != NULL)
		{
			wPacketSize+=sizeof(tagServerItem);
			CopyMemory(pGameServer, item, sizeof(tagServerItem));
		}
	}

	//发送数据
	if (wPacketSize>0) 
		g_GameCtrl->SendData(dwSocketID,MDM_CPD_LIST,CPO_PL_LIST_INSERT_GAME_LIST,cbBuffer,wPacketSize);

	return true;
}
*/