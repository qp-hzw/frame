#include "HandleFromCenter.h"
#include "GameCtrl.h"

//消息分发处理函数
bool CHandleFromCenter::HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, WORD wServiceID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_REGISTER:		//注册模块
		{
			return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
		}
	case MDM_USER:	//列表命令
		{
			return OnTCPSocketMainServiceInfo(Command.wSubCmdID,pData,wDataSize);
		}
	case MDM_TRANSFER:	//中转服务
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
//列表命令
bool CHandleFromCenter::OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_USER_OFFLINE_B: //用户断线
		{
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
	case CPO_PL_CLUB_TABLE_INFO:  //俱乐部桌子信息
		{
			//TODONOW 暂时先改为通知所有client, 后面改为通知在该房间内的人
			//added by WangChengQing 2018/5/21
			//效验数据
			if (wDataSize!=sizeof(STR_CMD_LC_CLUB_TABLE_LIST)) return true;

			//发送通知 -- 全部客户端
			g_GameCtrl->SendDataBatch(MDM_CLUB,CMD_LC_CLUB_TABLE_LIST_TABLE,pData,wDataSize);
			return true;
		}
	case CPO_PL_CLUB_PLAYER_INFO: //俱乐部玩家信息
		{
			//TODONOW 暂时先改为通知所有client, 后面改为通知在该房间内的人
			//added by WangChengQing 2018/5/21
			//效验数据
			if (wDataSize!=sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST)) return true;

			//发送通知 -- 全部客户端
			g_GameCtrl->SendDataBatch(MDM_CLUB, CMD_LC_CLUB_TABLE_LIST_USER,pData,wDataSize);
			return true;
		}
	case CPO_PL_CREATE_TABLE:	//创建桌子 查询可用的GameID
		{
			//效验数据
			if (wDataSize!=sizeof(STR_CPO_PL_CREATE_TABLE)) return true;
			STR_CPO_PL_CREATE_TABLE *pCPO = (STR_CPO_PL_CREATE_TABLE*)pData;
			
			STR_CMD_LC_GAME_QUERY_GAMEID CMD;
			CMD.dwGameID = pCPO->dwGameID;
			CMD.dwResultCode = pCPO->dwResultCode;
			memcpy(CMD.szDescribeString, pCPO->szDescribeString, sizeof(CMD.szDescribeString));

			g_GameCtrl->SendData(pCPO->dwSocketID, MDM_GAME, CMD_LC_GAME_QUERY_GAMEID, &CMD, sizeof(CMD));
			return true;
		}
	}

	return true;
}
