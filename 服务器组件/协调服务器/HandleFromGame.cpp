#include "HandleFromGame.h"
#include "GameCtrl.h"
#include "ServerItemManager.h"

//消息分发处理函数
bool CHandleFromGame::HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case CPD_MDM_TRANSFER:	//中转服务
		{
			return OnTCPNetworkMainTransfer(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	}
	return false;
}

#pragma region  一级分发处理
//中转服务
bool CHandleFromGame::OnTCPNetworkMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case CPR_GP_CLUB_TABLE_INFO: //club俱乐部桌子信息更新
		{
			//效验数据
			if (wDataSize!=sizeof(STR_CMD_LC_CLUB_TABLE_LIST)) return true;
			
			//发送通知 -- 全部登录服
			g_GameCtrl->SendDataBatch(CPD_MDM_TRANSFER,CPO_PL_CLUB_TABLE_INFO,pData,wDataSize);
			return true;
		}
	case CPR_GP_CLUB_PLAYER_INFO: //club俱乐部玩家信息更新
		{
			//效验数据
			if (wDataSize!=sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST)) return false;
			
			//发送通知 -- 全部登录服
			g_GameCtrl->SendDataBatch(CPD_MDM_TRANSFER,CPO_PL_CLUB_PLAYER_INFO,pData,wDataSize);
			return true;
		}
	case CPO_GP_OFFLINE_FINISH:
		{
			//校验数据
			if (wDataSize != sizeof(STR_CPO_GP_OFFLINE_FINISH)) return false;
			STR_CPO_GP_OFFLINE_FINISH* pCPO = (STR_CPO_GP_OFFLINE_FINISH *)pData;

			static int recv_num = 0;
			recv_num++;

			if(pCPO->bOffline == 1)
			{
				tagServerItem* item =  CServerItemManager::FindItemBySocketID(dwSocketID);
				if(!item)
				{
					//发送给logon
					STR_CPO_PL_OFFLINE_FiNISH cmd;
					cmd.dwUserID = pCPO->dwUserID;
					cmd.dwGameID = item->dwServerID;
					cmd.dwSocketID = pCPO->dwSocketID;
					
					g_GameCtrl->SendData(pCPO->dwSocketID, CPD_MDM_TRANSFER, CPO_PL_OFFLINE_FiNISH,&cmd,sizeof(cmd));
					return true;
				}
			}

			//所有数据接收完毕
			if(recv_num == CServerItemManager::FindAllGameServer().size())
			{
				recv_num = 0;

				//发送给logon
				STR_CPO_PL_OFFLINE_FiNISH cmd;
				cmd.dwUserID = pCPO->dwUserID;
				cmd.dwGameID = 0;
				cmd.dwSocketID = pCPO->dwSocketID;

				g_GameCtrl->SendData(pCPO->dwSocketID, CPD_MDM_TRANSFER, CPO_PL_OFFLINE_FiNISH,&cmd,sizeof(cmd));
				return true;
			}

			return true;
		}
	}

	return false;
}

#pragma endregion 
