#include "HandleFromGame.h"
#include "GameCtrl.h"

//消息分发处理函数
bool CHandleFromGame::HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case CPD_MDM_TRANSFER:	//中转服务
		{
			return OnTCPNetworkMainTransfer(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case CPD_MDM_USER://用户命令
		{
			return OnTCPNetworkMainUserCollect(Command.wSubCmdID,pData,wDataSize,dwSocketID);
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
	}

	return false;
}

//用户处理
bool CHandleFromGame::OnTCPNetworkMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_USER_OFFLINE:	//用户断线
		{
			//效验数据
			if (wDataSize!=sizeof(tagOfflineUser)) return true;

			//发送通知 -- 全部登录服
			g_GameCtrl->SendDataBatch(CPD_MDM_USER,SUB_CS_C_USER_OFFLINE_B, pData, wDataSize);
			return true;
		}
	}

	return false;
}
#pragma endregion 
