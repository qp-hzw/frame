#include "HandleFromCenter.h"
#include "GameCtrl.h"
#include "PlayerManager.h"
#include "Player.h"

//消息分发处理函数
bool CHandleFromCenter::HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, WORD wServiceID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_REGISTER_CPD:		//注册模块
		{
			return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
		}
	case CPD_MDM_TRANSFER:	//中转服务
		{
			return OnTCPSocketMainTransfer(Command.wSubCmdID,pData,wDataSize);	
		}
	}

	return false;
}


//注册模块
bool CHandleFromCenter::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case CPO_REGISTER_SUCESS:	//注册成功
		{
			//开启socket::server服务
			if(g_GameCtrl->StartNetworkService() != 0)
			{
				g_GameCtrl->ConcludeService();
				return true;
			}

			return true;
		}

	case CPO_REGISTER_FAILURE:		//注册失败
		{
			//事件通知 
			g_GameCtrl ->ConcludeService();
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
	case CPO_PL_CREATE_TABLE:	//创建桌子 查询可用的GameID
		{
			//效验数据
			if (wDataSize!=sizeof(STR_CPO_PL_CREATE_TABLE)) return true;
			STR_CPO_PL_CREATE_TABLE *pCPO = (STR_CPO_PL_CREATE_TABLE*)pData;
			
			STR_CMD_LC_GAME_QUERY_GAMEID CMD;
			CMD.dwGameID = pCPO->dwGameID;
			CMD.dwResultCode = pCPO->dwResultCode;
			memcpy(CMD.szDescribeString, pCPO->szDescribeString, sizeof(CMD.szDescribeString));
			CopyMemory(CMD.szIPAddr, pCPO->szIPAddr, sizeof(CMD.szIPAddr));
			CMD.dwPort = pCPO->dwPort;

			g_GameCtrl->SendData(pCPO->dwSocketID, MDM_GAME, CMD_LC_GAME_QUERY_GAMEID, &CMD, sizeof(CMD));
			return true;
		}
	case CPO_PL_OFFLINE_FiNISH: //查询返回
		{
			//效验数据
			if (wDataSize!=sizeof(STR_CPO_PL_OFFLINE_FiNISH)) return true;
			STR_CPO_PL_OFFLINE_FiNISH *pCPO = (STR_CPO_PL_OFFLINE_FiNISH*)pData;

			CPlayer* player = CPlayerManager::FindPlayerByID(pCPO->dwUserID);
			if(!player) return true;

			//发送登录成功 
			STR_CMD_LC_LOGON_PLATFORM  cmd;
			cmd.dwResultCode = 0;
			memcpy(&cmd.useInfo, player->GetUserInfo(), sizeof(cmd.useInfo));
			cmd.dwOffLineGameID  = pCPO->dwGameID;
			g_GameCtrl->SendData(player->GetSocketID(), MDM_LOGON, CMD_LC_LOGON_ACCOUNTS, &cmd, sizeof(cmd));


			//查询该玩家背包, 关注等信息
			g_GameCtrl->PostDataBaseRequest(DBR_CL_BAG_QUERY, 0, &(pCPO->dwUserID),sizeof(DWORD));

			return true;
		}
	}

	return true;
}
