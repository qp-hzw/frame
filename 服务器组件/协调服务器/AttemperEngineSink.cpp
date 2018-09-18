#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"

#include "..\..\依赖项\服务核心\WHEncrypt.h"
#include "..\..\消息定义\STR_CMD_LogonServer.h"
//////////////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//状态变量
	m_wCollectItem=INVALID_WORD;

	//设置变量
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;

	//组件变量
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;

	return;
}

//析构函数
CAttemperEngineSink::~CAttemperEngineSink()
{
}

//接口查询
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//绑定参数
	m_pBindParameter=new tagBindParameter[MAX_CONTENT];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*MAX_CONTENT);

	return true;
}

//停止事件
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{
	//状态变量
	m_wCollectItem=INVALID_WORD;
	m_WaitCollectItemArray.RemoveAll();

	//设置变量
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;

	//删除数据
	SafeDeleteArray(m_pBindParameter);

	//设置组件
	m_GlobalInfoManager.ResetData();

	return true;
}

//应答事件
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	//获取索引
	if (LOWORD(dwSocketID)>=MAX_CONTENT) return false;

	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//设置变量
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientAddr=dwClientAddr;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);

	return true;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_REGISTER:		//服务注册
		{
			return OnTCPNetworkMainRegister(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CPD_LIST:	//服务信息
		{
			return OnTCPNetworkMainServiceInfo(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_TRANSFER:	//中转服务
		{
			return OnTCPNetworkMainTransfer(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CS_USER_COLLECT:	//用户命令
		{
			return OnTCPNetworkMainUserCollect(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_WEB: //管理服务
		{
			return OnTCPNetworkMainManagerService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//游戏服务
	if (pBindParameter->ServiceKind==ServiceKind_Game)
	{
		//变量定义
		WORD wBindIndex=LOWORD(dwSocketID);

		/* 1. 汇总处理 */
		if (wBindIndex==m_wCollectItem)
		{
			//设置变量
			m_wCollectItem=INVALID_WORD;

			//汇总切换
			if (m_WaitCollectItemArray.GetCount()>0)
			{
				//提取变量
				INT_PTR nWaitCount=m_WaitCollectItemArray.GetCount();
				m_wCollectItem=m_WaitCollectItemArray[nWaitCount-1];

				//删除数组
				m_WaitCollectItemArray.RemoveAt(nWaitCount-1);

				//发送消息
				DWORD dwSocketID=(m_pBindParameter+m_wCollectItem)->dwSocketID;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_REQUEST);
			}
		}
		else
		{
			//删除等待
			for (INT_PTR i=0;i<m_WaitCollectItemArray.GetCount();i++)
			{
				if (wBindIndex==m_WaitCollectItemArray[i])
				{
					m_WaitCollectItemArray.RemoveAt(i);
					break;
				}
			}
		}

		/* 2. 通知登录服 */
		//变量定义
		STR_CPO_PL_LIST_REMOVE_GAME ServerRemove;
		ZeroMemory(&ServerRemove,sizeof(ServerRemove));

		//删除通知
		ServerRemove.dwServerID=pBindParameter->dwServiceID;
		m_pITCPNetworkEngine->SendDataBatch(MDM_CPD_LIST,CPO_PL_LIST_REMOVE_GAME,&ServerRemove,sizeof(ServerRemove),0L);

		//注销房间
		m_GlobalInfoManager.DeleteServerItem(pBindParameter->dwServiceID);
	}

	//广场服务
	if (pBindParameter->ServiceKind==ServiceKind_Logon)
	{
		//变量定义
		WORD wBindIndex=LOWORD(dwSocketID);
		tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

		//注销房间
		m_GlobalInfoManager.DeletePlazaItem( static_cast<WORD>(pBindParameter->dwServiceID));
	}

	//清除信息
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

#pragma region  主消息分发处理
//注册模块
bool CAttemperEngineSink::OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case CPR_LP_REGISTER_LOGON:	//注册登录服
		{
			//效验数据
			if (wDataSize!=sizeof(STR_CPR_LP_REGISTER_LOGON)) return false;

			//消息定义
			STR_CPR_LP_REGISTER_LOGON * pCPR=(STR_CPR_LP_REGISTER_LOGON *)pData;

			//有效判断		//lee：登陆服地址和名字，只在这里用到了
			if (pCPR->szServerAddr[0]==0)
			{
				//变量定义
				STR_CPO_PGPL_REGISTER_FAILURE RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//设置变量
				RegisterFailure.lErrorCode=0L;
				lstrcpyn(RegisterFailure.szDescribeString,TEXT("服务器注册失败，“服务地址”为空！"),CountArray(RegisterFailure.szDescribeString));

				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_REGISTER,CPO_PGPL_REGISTER_FAILURE,&RegisterFailure,sizeof(RegisterFailure));

				//中断网络
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}

			//设置绑定
			WORD wBindIndex=LOWORD(dwSocketID);
			(m_pBindParameter+wBindIndex)->dwServiceID=wBindIndex;
			(m_pBindParameter+wBindIndex)->ServiceKind=ServiceKind_Logon;

			//变量定义
			tagGamePlaza GamePlaza;
			ZeroMemory(&GamePlaza,sizeof(GamePlaza));

			//构造数据
			GamePlaza.wPlazaID=wBindIndex;
			lstrcpyn(GamePlaza.szServerAddr,pCPR->szServerAddr,CountArray(GamePlaza.szServerAddr));

			//注册房间
			m_GlobalInfoManager.ActivePlazaItem(wBindIndex,GamePlaza);

			//发送列表
			SendServerListItem(dwSocketID);

			//群发设置
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);

			return true;
		}
	case CPR_GP_REGISTER_GAME:	//注册游戏服
		{
			//效验数据
			if (wDataSize!=sizeof(STR_CPR_GP_REGISTER_GAME)) return false;

			//消息定义
			STR_CPR_GP_REGISTER_GAME * pRegisterServer=(STR_CPR_GP_REGISTER_GAME *)pData;

			//失败处理
			if (m_GlobalInfoManager.SearchServerItem(pRegisterServer->dwServerID)!=NULL)
			{
				//变量定义
				STR_CPO_PGPL_REGISTER_FAILURE RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//设置变量
				RegisterFailure.lErrorCode=0L;
				lstrcpyn(RegisterFailure.szDescribeString,TEXT("已经存在相同标识的游戏房间服务，房间服务注册失败"),CountArray(RegisterFailure.szDescribeString));

				//发送消息
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_REGISTER,CPO_PGPL_REGISTER_FAILURE,&RegisterFailure,sizeof(RegisterFailure));

				//中断网络
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}

			//设置绑定
			WORD wBindIndex=LOWORD(dwSocketID);
			(m_pBindParameter+wBindIndex)->ServiceKind=ServiceKind_Game;
			(m_pBindParameter+wBindIndex)->dwServiceID=pRegisterServer->dwServerID;

			//变量定义
			tagGameServer GameServer;
			ZeroMemory(&GameServer,sizeof(GameServer));

			//构造数据
			GameServer.wServerID=pRegisterServer->dwServerID;
			GameServer.wGameServerPort=pRegisterServer->wGameServerPort;
			GameServer.dwSubGameVersion=pRegisterServer->dwSubGameVersion;
			lstrcpyn(GameServer.szServerName,pRegisterServer->szServerName,CountArray(GameServer.szServerName));
			lstrcpyn(GameServer.szGameServerAddr,pRegisterServer->szGameServerAddr,CountArray(GameServer.szGameServerAddr));

			//发送完成 -- 通知游戏服 开启内核
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_REGISTER,CPO_PGPL_REGISTER_SUCESS);

			//注册房间
			m_GlobalInfoManager.ActiveServerItem(wBindIndex,GameServer);

			//群发房间
			m_pITCPNetworkEngine->SendDataBatch(MDM_CPD_LIST, CPO_PL_LIST_INSERT_GAME, &GameServer, sizeof(GameServer), 0L);

			//群发设置 TODONOW没看懂干什么的
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);

			//汇总通知 TODONOW 没看懂干什么的
			if (m_wCollectItem==INVALID_WORD)
			{
				m_wCollectItem=wBindIndex;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_REQUEST);
			}
			else m_WaitCollectItemArray.Add(wBindIndex);

			return true;
		}
	}

	return false;
}

//列表命令
bool CAttemperEngineSink::OnTCPNetworkMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case CPR_GP_LIST_GAME_ONLINE:	//房间人数 通知数据库
		{
			//效验数据
			if (wDataSize!=sizeof(STR_CPR_GP_LIST_GAME_ONLINE)) return false;

			//消息定义
			STR_CPR_GP_LIST_GAME_ONLINE * pServerOnLine=(STR_CPR_GP_LIST_GAME_ONLINE *)pData;

			//获取参数
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//连接效验
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//查找房间
			DWORD wServerID=pBindParameter->dwServiceID;
			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(wServerID);

			//设置人数
			if (pGlobalServerItem!=NULL)
			{
				//设置变量
				pGlobalServerItem->m_GameServer.dwOnlineCount=pServerOnLine->dwOnLineCount;

				//变量定义
				STR_CPR_PL_LIST_GAME_ONLINE ServerOnLine;
				ZeroMemory(&ServerOnLine,sizeof(ServerOnLine));
				ServerOnLine.wServerID=wServerID;
				ServerOnLine.dwOnLineCount=pServerOnLine->dwOnLineCount;

				//发送通知
				m_pITCPNetworkEngine->SendDataBatch(MDM_CPD_LIST,CPO_PL_LIST_GAME_ONLINE,&ServerOnLine,sizeof(ServerOnLine),0L);
			}

			return true;
		}
	}

	return false;
}

//中转服务
bool CAttemperEngineSink::OnTCPNetworkMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case CPR_GP_CLUB_TABLE_INFO: //club俱乐部桌子信息更新
		{
			//效验数据
			if (wDataSize!=sizeof(STR_CMD_LC_CLUB_TABLE_LIST)) return true;
			
			//发送通知 -- 全部登录服
			m_pITCPNetworkEngine->SendDataBatch(MDM_TRANSFER,CPO_PL_CLUB_TABLE_INFO,pData,wDataSize,0L);
			return true;
		}
	case CPR_GP_CLUB_PLAYER_INFO: //club俱乐部玩家信息更新
		{
			//效验数据
			if (wDataSize!=sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST)) return false;
			
			//发送通知 -- 全部登录服
			m_pITCPNetworkEngine->SendDataBatch(MDM_TRANSFER,CPO_PL_CLUB_PLAYER_INFO,pData,wDataSize,0L);
			return true;
		}
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
			m_pITCPNetworkEngine->SendDataBatch(MDM_TRANSFER,CPO_PG_CLUB_TABLE_DISSOLVE,&CPO,sizeof(CPO),0L);

			return true;
		}
		case CPR_LP_CREATE_TABLE:	//创建桌子 查询可用的GameID 
		{
			//校验数据
			if (wDataSize!=sizeof(STR_CPR_LP_CREATE_TABLE)) return true;
			STR_CPR_LP_CREATE_TABLE *pCPR = (STR_CPR_LP_CREATE_TABLE*) pData;


			DWORD dwMatchGameID = 0;
			DWORD dwMinOnlineCount = 60*4; //每个负载最大人数为240 如果以后修改, 这里也需要修改 added by WangChengQing
			//获取可用的, 当前负载最小的GameID -- 负载根据当前人数来判断
			for (int j = 0; j < MAX_CONTENT; j++)
			{
				//获取到游戏服KindID匹配的数据
				tagBindParameter * pBindParameter=(m_pBindParameter+j);
				if ( pBindParameter &&
					(pBindParameter->ServiceKind == ServiceKind_Game) && 
					(((pBindParameter->dwServiceID &0x00FF0000) >>16) == pCPR->dwKindID ))
				{
					CGlobalServerItem * pGlobalServerItem = m_GlobalInfoManager.SearchServerItem(pBindParameter->dwServiceID);
					if(NULL == pGlobalServerItem) continue;

					if(pGlobalServerItem->m_GameServer.dwOnlineCount < dwMinOnlineCount)
					{
						dwMinOnlineCount = pGlobalServerItem->m_GameServer.dwOnlineCount;
						dwMatchGameID = pGlobalServerItem->m_GameServer.wServerID;
					}
				}
			}


			//CPO构造
			STR_CPO_PL_CREATE_TABLE CPO;
			ZeroMemory(&CPO, sizeof(CPO));

			if(0 == dwMatchGameID)
			{
				CPO.dwResultCode = 1;
				memcpy(CPO.szDescribeString, TEXT("未找到合适的游戏服务器"), sizeof(CPO.szDescribeString));
				CPO.dwSocketID = pCPR->dwSocketID;
			}
			else
			{
				CPO.dwResultCode = 0;
				CPO.dwGameID = dwMatchGameID & 0x00FFFF00;
				CPO.dwSocketID = pCPR->dwSocketID;
			}

			//发送消息
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_TRANSFER,CPO_PL_CREATE_TABLE, &CPO, sizeof(CPO));
			return true;
		}

	}

	return false;
}

//用户处理
bool CAttemperEngineSink::OnTCPNetworkMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_USER_ENTER:		//用户进入
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_CS_C_UserEnter)) return false;

			//消息处理
			CMD_CS_C_UserEnter * pUserEnter=(CMD_CS_C_UserEnter *)pData;
			pUserEnter->szNickName[CountArray(pUserEnter->szNickName)-1]=0;

			//获取参数
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//连接效验
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//变量定义
			tagGlobalUserInfo GlobalUserInfo;
			ZeroMemory(&GlobalUserInfo,sizeof(GlobalUserInfo));

			//设置变量
			GlobalUserInfo.dwUserID=pUserEnter->dwUserID;
			GlobalUserInfo.dwGameID=pUserEnter->dwGameID;
			lstrcpyn(GlobalUserInfo.szNickName,pUserEnter->szNickName,CountArray(GlobalUserInfo.szNickName));

			//辅助信息
			GlobalUserInfo.cbGender=pUserEnter->cbGender;
			GlobalUserInfo.cbMemberOrder=pUserEnter->cbMemberOrder;
			GlobalUserInfo.cbMasterOrder=pUserEnter->cbMasterOrder;

			//激活用户
			m_GlobalInfoManager.ActiveUserItem(GlobalUserInfo,pBindParameter->dwServiceID);

			return true;
		}
	case SUB_CS_C_USER_LEAVE:		//用户离开
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_CS_C_UserLeave));
			if (wDataSize!=sizeof(CMD_CS_C_UserLeave)) return false;

			//消息处理
			CMD_CS_C_UserLeave * pUserLeave=(CMD_CS_C_UserLeave *)pData;

			//获取参数
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//连接效验
			ASSERT(pBindParameter->ServiceKind==ServiceKind_Game);
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//删除用户
			m_GlobalInfoManager.DeleteUserItem(pUserLeave->dwUserID,pBindParameter->dwServiceID);

			return true;
		}
	case SUB_CS_C_USER_FINISH:		//用户完成
		{
			//获取参数
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//连接效验
			ASSERT((m_wCollectItem==wBindIndex)&&(pBindParameter->ServiceKind==ServiceKind_Game));
			if ((m_wCollectItem!=wBindIndex)||(pBindParameter->ServiceKind!=ServiceKind_Game)) return false;

			//设置变量
			m_wCollectItem=INVALID_WORD;

			//汇总切换
			if (m_WaitCollectItemArray.GetCount()>0)
			{
				//切换汇总
				INT_PTR nWaitCount=m_WaitCollectItemArray.GetCount();
				m_wCollectItem=m_WaitCollectItemArray[nWaitCount-1];

				//删除数组
				m_WaitCollectItemArray.RemoveAt(nWaitCount-1);

				//发送消息
				DWORD dwSocketID=(m_pBindParameter+m_wCollectItem)->dwSocketID;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_REQUEST);
			}

			return true;
		}
	case SUB_CS_C_USER_OFFLINE:	//用户断线
		{
			//效验数据
			if (wDataSize!=sizeof(STR_SUB_CS_C_USER_OFFLINE)) return false;

			//消息处理
			STR_SUB_CS_C_USER_OFFLINE * pUserOffLine=(STR_SUB_CS_C_USER_OFFLINE *)pData;

			STR_SUB_CS_C_USER_OFFLINE data;
			data.dwUserID = pUserOffLine->dwUserID;
			data.dwGameID = pUserOffLine->dwGameID;
			data.byMask = pUserOffLine->byMask;
			
			//发送通知 -- 全部登录服
			m_pITCPNetworkEngine->SendDataBatch(MDM_CPD_LIST,SUB_CS_C_USER_OFFLINE_B,&data,sizeof(STR_SUB_CS_C_USER_OFFLINE),0L);
			
			return true;
		}
	}

	return false;
}

//管理服务
bool CAttemperEngineSink::OnTCPNetworkMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case CPR_WP_WEB_MARQUEE: //跑马灯消息
		{
			//效验大小
			if (wDataSize!=sizeof(STR_CPR_WP_WEB_MARQUEE)) 
			{
				return false;
			}

			//校验数据
			STR_CPR_WP_WEB_MARQUEE *pSub = (STR_CPR_WP_WEB_MARQUEE*) pData;
			if(1 != pSub->byMask) return false;

			//发送通知到客户端(只发送给大厅)
			SendDataToPlaza(INVALID_WORD, MDM_WEB, CPO_PL_WEB_MARQUEE, pData, wDataSize);
			////群发给所有连接的服务器
			//m_pITCPNetworkEngine->SendDataBatch(MDM_WEB,CPO_PL_WEB_MARQUEE, &pSendLobbyMessage,sizeof(CMD_CS_MarqueeMessage),0L);
			return true;
		}
	case CPR_WP_WEB_SYSTEM_MESSAGE:	//聊天频道 -- 系统消息
		{
			//效验大小
			if (wDataSize!=sizeof(STR_CPR_WP_WEB_SYSTEM_MESSAGE))
			{
				return false;
			}

			//校验数据
			STR_CPR_WP_WEB_SYSTEM_MESSAGE *pSub = (STR_CPR_WP_WEB_SYSTEM_MESSAGE*) pData;
			
			LPCTSTR pszDescribe=pSub->szMessage;
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			//发送通知到客户端(只发送给大厅)
			SendDataToPlaza(INVALID_WORD, MDM_WEB, CPO_PL_WEB_SYSTEM_MESSAGE, pData, wDataSize);
			return true;
		}
	}

	return false;
}
#pragma endregion 

//发送列表
bool CAttemperEngineSink::SendServerListItem(DWORD dwSocketID)
{
	//变量定义
	WORD wPacketSize=0L;
	POSITION Position=NULL;
	BYTE cbBuffer[SOCKET_TCP_PACKET];

	//发送信息
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CPD_LIST,SUB_CS_S_SERVER_INFO);

	//收集数据
	do
	{
		//发送数据
		if ((wPacketSize+sizeof(tagGameServer))>sizeof(cbBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CPD_LIST,CPO_PL_LIST_INSERT_GAME_LIST,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//获取对象
		tagGameServer * pGameServer=(tagGameServer *)(cbBuffer+wPacketSize);
		CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(Position);		//获得每个游戏服信息

		//设置数据
		if (pGlobalServerItem!=NULL)
		{
			wPacketSize+=sizeof(tagGameServer);
			CopyMemory(pGameServer,&pGlobalServerItem->m_GameServer,sizeof(tagGameServer));
		}

	} while (Position!=NULL);

	//发送数据
	if (wPacketSize>0) 
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CPD_LIST,CPO_PL_LIST_INSERT_GAME_LIST,cbBuffer,wPacketSize);

	//发送完成
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_REGISTER,CPO_PGPL_REGISTER_SUCESS);

	return true;
}

//房间发送
bool CAttemperEngineSink::SendDataToGame(DWORD wServerID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//查找房间
	CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(wServerID);
	if (pGlobalServerItem==NULL) return false;

	//获取参数
	WORD wBindIndex=pGlobalServerItem->GetIndex();
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//发送数据
	DWORD dwSocketID=pBindParameter->dwSocketID;
	m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);

	return true;
}

//用户发送
bool CAttemperEngineSink::SendDataToUser(DWORD wServerID, DWORD dwUserID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//大厅发送
bool CAttemperEngineSink::SendDataToPlaza( WORD wSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize )
{
	if ( wSocketID != INVALID_WORD)
	{
		return m_pITCPNetworkEngine->SendData(wSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	POSITION Position = NULL;

	do
	{
		CGlobalPlazaItem * pGlobalPlazaItem = m_GlobalInfoManager.EnumPlazaItem(Position);
		if (pGlobalPlazaItem != NULL)
		{
			WORD wBindIndex = pGlobalPlazaItem->m_wIndex;
			if(wBindIndex < MAX_CONTENT)
			{
				tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);
				DWORD dwSocketID=pBindParameter->dwSocketID;
				if(!m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize))
				{
					return false;
				}
			}
		}

	} while (Position != NULL);

	return true;
}

//游戏发送
bool CAttemperEngineSink::SendDataToGameAllRoom( WORD wKindID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize )
{
	//变量定义
	POSITION Position=NULL;
	//查找房间
	do
	{
		CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(Position);
		if (pGlobalServerItem != NULL)
		{
			if (pGlobalServerItem->GetKindID() == wKindID)
			{
				SendDataToGame(pGlobalServerItem->GetServerID(),wMainCmdID,wSubCmdID,pData,wDataSize);
			}
		}
	} while (Position != NULL);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
