#include "HandleFromCenter.h"
#include "GameCtrl.h"
#include "TableManager.h"

bool CHandleFromCenter::HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, WORD wServiceID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_REGISTER:		//注册服务
		{
			return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
		}
	case MDM_TRANSFER:		//中转服务
		{
			return OnTCPSocketMainTransfer(Command.wSubCmdID,pData,wDataSize);	
		}
	}
}

//注册事件
bool CHandleFromCenter::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case CPO_REGISTER_SUCESS:		//注册完成
		{
			//校验
			if (sizeof(tagServerItem) != wDataSize)
				return false;

			//获取数据
			tagServerItem *pServerItem = (tagServerItem *)pData;

			CLog::Log(log_debug, "port:%d", pServerItem->wServerPort);

			//配置端口
			if (0 != g_GameCtrl->SetNetworkPort(pServerItem->wServerPort))
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

			//如果是本服务器的消息, 则通知子游戏解散游戏
			for(int i = 0; i < CTableManager::TableCount(); i++)
			{
				//校验
				CTableFrame *pTableFrame = CTableManager::FindTableByIndex(i);
				if (NULL == pTableFrame)
					continue;

				//找到相同桌子号的， 然后解散
				if(pTableFrame->GetTableID() == pCPO->dwTableID)
				{
					//设置房间处于解散状态
					pTableFrame->SetDismissState(true);

					//通知客户端弹出大局结算面板
					STR_CMD_GR_FRAME_DISMISS_RESULT DismissResult;
					ZeroMemory(&DismissResult, sizeof(DismissResult));
					DismissResult.cbDismiss = 1;
					DismissResult.cbAgree = 1;					
					if (pTableFrame->GetGameStatus() != GAME_STATUS_FREE)
					{
						DismissResult.cbClubQuit = 1;
					}
					
					pTableFrame->SendTableData(INVALID_CHAIR, CMD_GR_FRAME_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_G_FRAME);
					
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
					break;
				}
			}

			return true;
		}
	}

	return true;
}
