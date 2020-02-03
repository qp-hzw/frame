#include "HandleFromCenter.h"
#include "GameCtrl.h"
#include "TableManager.h"

bool CHandleFromCenter::HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, WORD wServiceID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_REGISTER_CPD:		//注册服务
		{
			return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
		}
	case CPD_MDM_TRANSFER:		//中转服务
		{
			return OnTCPSocketMainTransfer(Command.wSubCmdID,pData,wDataSize);	
		}
	}

	return true;
}

//注册事件
bool CHandleFromCenter::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case CPO_REGISTER_SUCESS:		//注册完成
		{
			//校验
			if (sizeof(WORD) != wDataSize)
				return false;

			//获取数据
			WORD* pServerItem = (WORD* )pData;

			//配置端口
			if (0 != g_GameCtrl->SetNetworkPort(*pServerItem))
			{
				g_GameCtrl->ConcludeService();
				return true;
			}

			//开启socket::server服务
			if(!g_GameCtrl->StartNetworkService())
			{
				g_GameCtrl->ConcludeService();
				return true;
			}

			return true;
		}
	case CPO_REGISTER_FAILURE:		//注册失败
		{
			g_GameCtrl->ConcludeService();

			return true;
		}
	}

	return true;
}

//中转服务
bool CHandleFromCenter::OnTCPSocketMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case CPO_PG_CLUB_TABLE_DISSOLVE: //登录服通知协调服, 协调服通知游戏服 解散桌子
		{
			//校验数据
			if (wDataSize != sizeof(STR_CPO_PG_CLUB_TABLE_DISSOLVE)) return false;
			
			STR_CPO_PG_CLUB_TABLE_DISSOLVE* pCPO = (STR_CPO_PG_CLUB_TABLE_DISSOLVE *)pData;

			//如果不是本服务器的消息, 则过滤掉
			if(pCPO->dwGameID != (g_GameCtrl->GetServerID() & 0xFFFFFF00))
			{
				return true;
			}

			CTableFrame *pTableFrame = CTableManager::FindTableByTableID( pCPO->dwTableID);
			if (NULL == pTableFrame)
					return true;

			//设置房间处于解散状态
			pTableFrame->SetDismissState(true);

			//通知客户端弹出大局结算面板
			STR_CMD_GR_FRAME_DISMISS_RESULT DismissResult;
			ZeroMemory(&DismissResult, sizeof(DismissResult));
			DismissResult.cbDismiss = 1;
			//DismissResult.cbAgree = 1;					
			if (pTableFrame->GetGameStatus() != GAME_STATUS_FREE)
			{
				//DismissResult.cbClubQuit = 1;
			}

			//通知子游戏解散房间
			//空闲状态 直接解散
			if(GAME_STATUS_FREE == pTableFrame->GetGameStatus())
			{
				pTableFrame->HandleDJGameEnd(GAME_CONCLUDE_NORMAL);
			}
			//游戏状态则 通知子游戏
			else
			{
				pTableFrame->OnEventClubDismissRoom();
			}

			return true;
		}

	case CPR_PG_OFFLINE_PLAYERQUERY: //查询断线玩家
		{			
			//校验数据
			if (wDataSize != sizeof(STR_CPR_PG_OFFLINE_PLAYERQUERY)) return false;
			STR_CPR_PG_OFFLINE_PLAYERQUERY* pCPO = (STR_CPR_PG_OFFLINE_PLAYERQUERY *)pData;

			STR_CPO_GP_OFFLINE_FINISH cmd;
			cmd.dwSocketID = pCPO->dwSocketID;
			cmd.dwUserID = pCPO->dwUserID;

			CPlayer *player =  CPlayerManager::FindPlayerByID(pCPO->dwUserID);
			if(!player)
			{
				cmd.bOffline = 0;
				g_TCPSocketEngine->SendData(CPD_MDM_TRANSFER,CPO_GP_OFFLINE_FINISH, &cmd, sizeof(cmd));
				return true;
			}
		
			if(player->GetTableID() != INVALID_TABLE)
			{
				cmd.bOffline = 1;
			}
			else
			{
				cmd.bOffline = 0;
			}
			g_TCPSocketEngine->SendData(CPD_MDM_TRANSFER,CPO_GP_OFFLINE_FINISH, &cmd, sizeof(cmd));
			return true;
		}
	}

	return true;
}
