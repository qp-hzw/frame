#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"
#include <vector>
#include <algorithm>
#include <iostream>

#include "../../全局定义/Define.h"

//////////////////////////////////////////////////////////////////////////////////
//时间标识

#define IDI_LOAD_ANDROID_USER			(IDI_MAIN_MODULE_START+1)			//机器信息
#define IDI_REPORT_SERVER_INFO			(IDI_MAIN_MODULE_START+2)			//房间人数
#define IDI_CONNECT_CORRESPOND			(IDI_MAIN_MODULE_START+3)			//连接时间
#define IDI_GAME_SERVICE_PULSE			(IDI_MAIN_MODULE_START+4)			//服务脉冲
#define IDI_DISTRIBUTE_ANDROID			(IDI_MAIN_MODULE_START+5)			//分配机器
#define IDI_DBCORRESPOND_NOTIFY			(IDI_MAIN_MODULE_START+6)			//缓存通知

#define IDI_LOAD_SENSITIVE_WORD			(IDI_MAIN_MODULE_START+9)			//加载敏感词

#define IDI_WRITE_STOCK_SCORE			(IDI_MAIN_MODULE_START+14)			//写库存记录
#define IDI_WAIT_SERVICE_STOP			(IDI_MAIN_MODULE_START+15)			//延迟停止

#define TIME_CONNECT				30									//重连时间
//////////////////////////////////////////////////////////////////////////////////
//时间定义 秒

#define TIME_LOAD_ANDROID_USER				600L								//加载机器
#define TIME_DISTRIBUTE_ANDROID				15L									//分配用户
#define TIME_REPORT_SERVER_INFO				30L									//上报时间
#define TIME_DBCORRESPOND_NOTIFY			3L									//缓存通知时间
#define TIME_LOAD_SYSTEM_MESSAGE			3600L								//系统消息时间
#define TIME_SEND_SYSTEM_MESSAGE			10L									//系统消息时间
#define TIME_CONTROL_ROOM_VALUE_SAVE		120000L								//保存控制值时间yang
#define TIME_READ_ROOM_CONFIG				2000L								//读取房间配置yang
#define TIME_WRITE_STOCK_SCORE				120000L								//写库存记录
#define TIME_WAIT_SERVICE_STOP				5000L								//延迟停止
#define TIME_CONNECT_CORRESPOND				30*1000L									//连接时间

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	srand((unsigned)time(NULL));

	//状态变量
	m_bCollectUser=false;
	m_bNeekCorrespond=true;
	m_bStopping = false;

	//绑定数据
	m_pNormalParameter=NULL;
	m_pAndroidParameter=NULL;

	//状态变量
	m_pGameParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//组件变量
	m_pITimerEngine=NULL;
	m_pIAttemperEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pIGameServiceManager=NULL;
	m_WaitDistributeList.RemoveAll();

	//数据引擎
	m_pIRecordDataBaseEngine=NULL;
	m_pIKernelDataBaseEngine=NULL;
	m_pIDBCorrespondManager=NULL;

	//比赛变量
	m_pIGameMatchServiceManager=NULL;

	return;
}

//析构函数
CAttemperEngineSink::~CAttemperEngineSink()
{
	//删除数据
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pAndroidParameter);

	//删除桌子
	for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
	{
		SafeRelease(m_TableFrameArray[i]);
	}

	//清理数据
	m_WaitDistributeList.RemoveAll();

	return;
}

//接口查询
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IMainServiceFrame,Guid,dwQueryVer);
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//绑定信息
	m_pAndroidParameter=new tagBindParameter[MAX_ANDROID + MAX_VIRTUAL];
	ZeroMemory(m_pAndroidParameter,sizeof(tagBindParameter)*(MAX_ANDROID +MAX_VIRTUAL));

	//绑定信息
	m_pNormalParameter=new tagBindParameter[MAX_TABLE];
	ZeroMemory(m_pNormalParameter,sizeof(tagBindParameter)*MAX_TABLE);

	//m_GameControlManager.Reset();

	//配置机器
	if (InitAndroidUser()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//配置桌子
	if (InitTableFrameArray()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置接口
	if (m_ServerUserManager.SetServerUserItemSink(QUERY_ME_INTERFACE(IServerUserItemSink))==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//启动机器
	if (m_AndroidUserManager.StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置时间
	m_pITimerEngine->SetTimer(IDI_GAME_SERVICE_PULSE,1000L,TIMES_INFINITY,NULL);

#ifdef _DEBUG
	m_pITimerEngine->SetTimer(IDI_LOAD_ANDROID_USER,10000L,TIMES_INFINITY,NULL);
#else
	m_pITimerEngine->SetTimer(IDI_LOAD_ANDROID_USER,TIME_LOAD_ANDROID_USER*1000L,TIMES_INFINITY,NULL);
#endif

	m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_ANDROID,TIME_DISTRIBUTE_ANDROID*1000L,TIMES_INFINITY,NULL);

	m_pITimerEngine->SetTimer(IDI_DBCORRESPOND_NOTIFY,TIME_DBCORRESPOND_NOTIFY*1000L,TIMES_INFINITY,NULL);
	return true;
}

//停止事件
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{
	//状态变量
	m_bCollectUser=false;
	m_bNeekCorrespond=true;
	m_bStopping=false;

	//配置信息
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//组件变量
	m_pITimerEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;

	//数据引擎
	m_pIRecordDataBaseEngine=NULL;
	m_pIKernelDataBaseEngine=NULL;

	//绑定数据
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pAndroidParameter);

	//删除桌子
	for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
	{
		SafeRelease(m_TableFrameArray[i]);
	}

	//删除用户
	m_TableFrameArray.RemoveAll();
	m_ServerUserManager.DeleteUserItem();
	m_WaitDistributeList.RemoveAll();

	//停止服务
	m_AndroidUserManager.ConcludeService();

	if(m_pIGameMatchServiceManager!=NULL)
		m_pIGameMatchServiceManager->StopService();

	return true;
}

//控制事件
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{
	case CT_CONNECT_CORRESPOND:		//连接协调
		{
			//发起连接
			m_pITCPSocketService->Connect(_CPD_SERVER_ADDR, PORT_CENTER);

			//构造提示
			TCHAR szString[512]=TEXT("");
			_sntprintf_s(szString,CountArray(szString),TEXT("正在连接协调服务器"));

			//提示消息
			CTraceService::TraceString(szString,TraceLevel_Normal);

			return true;
		}
	case CT_LOAD_SERVICE_CONFIG:	//加载配置
		{
			//加载机器
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_ANDROID_USER,0L,NULL,0L);

			//对m_GameProgress 初始化
			m_GameProgress = 0;

			//加载断线重连数据 && 数据库置空对应的桌子的所有数据
			STR_DBR_GR_LOAD_OFFLINE  dbr;
			dbr.byMystery = _MYSTERY;
			dbr.dwGameID = (m_pGameServiceOption->dwServerID) & 0xFFFFFF00;
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_OFFLINE,0L,&dbr,sizeof(dbr));

			//事件通知 -- 在UI_SERVICE_CONFIG_RESULT的case函数中已经对ServiceStatus做了处理, 因此这里没有问题
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_SERVICE_CONFIG_RESULT,&ControlResult,sizeof(ControlResult));
			return true;
		}
	case CT_TRY_TO_STOP_SERVICE:	//停止服务
		{
			StopService();
			break;
		}
	}

	return false;
}


//时间事件
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	//调度时间
	if ((dwTimerID>=IDI_MAIN_MODULE_START)&&(dwTimerID<=IDI_MAIN_MODULE_FINISH))
	{
		//时间处理
		switch (dwTimerID)
		{
		case IDI_LOAD_ANDROID_USER:		//加载机器
			{
				//加载机器
				m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_ANDROID_USER,0L,NULL,0L);

				return true;
			}
		case IDI_REPORT_SERVER_INFO:	//房间人数
			{
				//变量定义
				STR_CPR_GP_LIST_GAME_ONLINE ServerOnLine;
				ZeroMemory(&ServerOnLine,sizeof(ServerOnLine));

				//设置变量
				ServerOnLine.dwOnLineCount=m_ServerUserManager.GetUserItemCount();

				//发送数据
				m_pITCPSocketService->SendData(MDM_CPD_LIST,CPR_GP_LIST_GAME_ONLINE,&ServerOnLine,sizeof(ServerOnLine));
				return true;
			}
		case IDI_CONNECT_CORRESPOND:	//连接协调
			{
				//发起连接 TODONOWW
                m_pITCPSocketService->Connect(_CPD_SERVER_ADDR, PORT_CENTER);

				//构造提示
				TCHAR szString[512]=TEXT("");
				_sntprintf_s(szString,CountArray(szString),TEXT("正在连接协调服务器"));

				//提示消息
				CTraceService::TraceString(szString,TraceLevel_Normal);

				return true;
			}
		case IDI_GAME_SERVICE_PULSE:	//服务维护
			{
				return true;
			}
		case IDI_DISTRIBUTE_ANDROID:	//分配机器
			{
				DistributeAndroid();
				return true;
			}
		case IDI_DBCORRESPOND_NOTIFY: //缓存定时处理
			{
				if(m_pIDBCorrespondManager) m_pIDBCorrespondManager->OnTimerNotify();
				return true;
			}

		case IDI_LOAD_SENSITIVE_WORD:	//加载敏感词
			{
				//投递请求
				//m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_SENSITIVE_WORDS,0,NULL,0);				
				return true;
			}
		case IDI_WAIT_SERVICE_STOP:
			{
				SendServiceStopped();
				return true;
			}

		}
	}

	//机器时器
	if ((dwTimerID>=IDI_REBOT_MODULE_START)&&(dwTimerID<=IDI_REBOT_MODULE_FINISH))
	{
		//时间处理
		m_AndroidUserManager.OnEventTimerPulse(dwTimerID,wBindParam);

		return true;
	}

	//比赛定时器
	if((dwTimerID>=IDI_MATCH_MODULE_START)&&(dwTimerID<IDI_MATCH_MODULE_FINISH))
	{
		if(m_pIGameMatchServiceManager!=NULL) m_pIGameMatchServiceManager->OnEventTimer(dwTimerID,wBindParam);
		return true;
	}

	//桌子时间
	if ((dwTimerID>=IDI_TABLE_MODULE_START)&&(dwTimerID<=IDI_TABLE_MODULE_FINISH))
	{
		//桌子号码
		DWORD dwTableTimerID=dwTimerID-IDI_TABLE_MODULE_START;
		WORD wTableID=(WORD)(dwTableTimerID/TIME_TABLE_MODULE_RANGE);

		//时间效验
		if (wTableID>=(WORD)m_TableFrameArray.GetCount()) 
		{
			ASSERT(FALSE);
			return false;
		}

		//时间通知
		CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
		return pTableFrame->OnEventTimer(dwTableTimerID%TIME_TABLE_MODULE_RANGE,wBindParam);
	}

	return false;
}

//数据库事件
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_CG_LOGON_USERID:			//ID登录返回
		{
			return On_CMD_GC_Logon_UserID(dwContextID, pData, wDataSize);
		}
	case DBO_CG_USER_CREATE_GROUP_ROOM:	//创建牌友圈房间返回
		{
			return On_CMD_GC_User_CreateGroupRoom(dwContextID, pData, wDataSize);
		}
	case DBO_CG_USER_JOIN_GROUP_ROOM:	//加入牌友圈房间返回
		{
			return On_CMD_GC_User_JoinGroupRoom(dwContextID, pData, wDataSize);
		}
	case DBO_SG_MODIFY_USER_TREASURE:	//修改用户财富信息返回
		{
			return On_CMD_GC_User_ModifyUserTreasure(dwContextID, pData, wDataSize);
		}
	case DBR_GR_GAME_ANDROID_INFO:		//机器信息
		{
			return OnDBGameAndroidInfo(dwContextID,pData,wDataSize);
		}

	case DBO_GR_LOAD_OFFLINE:		//加载断线重连返回
		{
			return On_DBO_GR_LOAD_OFFLINE(dwContextID,pData,wDataSize);
		}

	case DBO_GR_ROOMLEVELMODIFY:	//房间难度调整
		{
			return OnDBRoomLevelModify(dwContextID,pData,wDataSize);
		}
	case DBO_GR_ROOMCONTROLVALMODIFY:
		{
			return OnDBRoomControlValModify(dwContextID,pData,wDataSize);
		}
	case DBO_GR_LOTTERY_RESULT:
		{
			return OnDBLotteryResult(dwContextID,pData,wDataSize);
		}
	case DBO_GC_CLUB_CREATE_TABLE:	//club牌友圈创建桌子
		{
			return On_CMD_CG_CLUB_CREATE_TABLE(dwContextID,pData,wDataSize);
		}
	case DBO_GC_USER_JOIN_TABLE_NO_PASS://加入桌子 不需要密码
		{
			return On_CMD_GC_USER_JOIN_TABLE_NO_PASS(dwContextID,pData,wDataSize);
		}
	case DBO_GC_CLUB_UPDATE_TABLE_INFO:// 更新桌子信息
		{
			if(sizeof(STR_CMD_LC_CLUB_TABLE_LIST) != wDataSize) return false;
			m_pITCPSocketService->SendData(MDM_TRANSFER, CPR_GP_CLUB_TABLE_INFO, pData, wDataSize);
			return true;;
		}
	case DBO_GC_CLUB_UPDATE_PLAYER_INFO://更新玩家信息
		{
			if(sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST) != wDataSize) return false;
			m_pITCPSocketService->SendData(MDM_TRANSFER, CPR_GP_CLUB_PLAYER_INFO, pData, wDataSize);
			return true;
		}
	case DBO_GC_JOIN_TABLE://加入桌子 数据库校验
		{
			return On_CMD_GC_JOIN_TABLE(dwContextID,pData,wDataSize);
		}
	case DBO_GC_USER_JOIN_TABLE_HALL_GOLD: //加入桌子 -- 金币大厅桌子
		{
			return On_CMD_GC_USER_JOIN_TABLE_HALL_GOLD(dwContextID,pData,wDataSize);
		}
	}


	if(wRequestID>=DBO_GR_MATCH_EVENT_START && wRequestID<=DBO_GR_MATCH_EVENT_END)
	{
		//参数效验
		if(m_pIGameMatchServiceManager==NULL) return false;

		tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
		IServerUserItem *pIServerUserItem=pBindParameter!=NULL?pBindParameter->pIServerUserItem:NULL;

		//废弃判断
		if ((pBindParameter->pIServerUserItem==NULL)||(pBindParameter->dwSocketID!=dwContextID))
		{
			//错误断言
			ASSERT(FALSE);
			return true;
		}

		return m_pIGameMatchServiceManager->OnEventDataBase(wRequestID,pIServerUserItem,pData,wDataSize);
	}

	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//协调连接
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//设置变量
		m_bCollectUser=false;

		//删除时间
		m_pITimerEngine->KillTimer(IDI_REPORT_SERVER_INFO);

		//重连判断
		if (m_bNeekCorrespond==true)
		{
			//构造提示
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("与协调服务器的连接关闭了，%ld 秒后将重新连接"),TIME_CONNECT);

			//提示消息
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//设置时间
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND, TIME_CONNECT_CORRESPOND, 1, 0);
		}

		return true;
	}

	return false;
}

//连接事件
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//协调连接
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//错误判断
		if (nErrorCode!=0)
		{
			//构造提示
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("协调服务器连接失败 [ %ld ]，%ld 秒后将重新连接"),
				nErrorCode,TIME_CONNECT_CORRESPOND);

			//提示消息
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//设置时间
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND, TIME_CONNECT_CORRESPOND, 1, 0);

			return false;
		}

		//提示消息
		CTraceService::TraceString(TEXT("正在发送游戏房间注册信息..."),TraceLevel_Normal);

		//变量定义
		STR_CPR_GP_REGISTER_GAME RegisterServer;
		ZeroMemory(&RegisterServer,sizeof(RegisterServer));

		//构造数据
		RegisterServer.dwServerID=m_pGameServiceOption->dwServerID;
		lstrcpyn(RegisterServer.szServerName, m_pGameServiceOption->szServerName, CountArray(RegisterServer.szServerName));
		lstrcpyn(RegisterServer.szGameServerAddr, m_pGameServiceOption->szGameServerAddr,CountArray(RegisterServer.szGameServerAddr));
		RegisterServer.wGameServerPort=m_pGameServiceOption->wGameServerPort;
		RegisterServer.dwSubGameVersion = m_pGameServiceAttrib->dwSubGameVersion;

		//发送数据
		m_pITCPSocketService->SendData(MDM_REGISTER,CPR_GP_REGISTER_GAME,&RegisterServer,sizeof(RegisterServer));

		//设置时间
		m_pITimerEngine->SetTimer(IDI_REPORT_SERVER_INFO,TIME_REPORT_SERVER_INFO*1000L,TIMES_INFINITY,0);

		return true;
	}

	return true;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//协调连接
	if (wServiceID==NETWORK_CORRESPOND)
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
		case MDM_CS_USER_COLLECT:	//用户汇总
			{
				return OnTCPSocketMainUserCollect(Command.wSubCmdID,pData,wDataSize);
			}
		}
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//应答事件
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//设置变量
	if (pBindParameter!=NULL)
	{
		pBindParameter->dwSocketID=dwSocketID;
		pBindParameter->dwClientAddr=dwClientAddr;
		pBindParameter->dwActiveTime=(DWORD)time(NULL);
		return true;
	}

	//错误断言
	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//获取用户
	IServerUserItem * pIServerUserItem=pBindParameter->pIServerUserItem;

	//Socket连接关闭 数据库处理
	if((pIServerUserItem != NULL) && (pIServerUserItem->GetUserID() != 0))
	{
		//Club俱乐部 玩家掉线的处理
		if(pIServerUserItem->GetTableID() != 0) //如果用户在桌子中
		{
			ClubPlayerOffline(pIServerUserItem->GetTableID(), \
				pIServerUserItem->GetUserID(), static_cast<BYTE>(pIServerUserItem->GetChairID()), 1);
		}
	}

	//用户处理
	if (pIServerUserItem!=NULL)
	{
		bool bIsExsit = false; //是否属于断线

		//变量定义
		WORD wTableID=pIServerUserItem->GetTableID();

		//断线处理
		if (wTableID!=INVALID_TABLE)
		{
			//解除绑定
			pIServerUserItem->DetachBindStatus();

			//是否需要处理断线重连
			bool bRet = m_TableFrameArray[wTableID]->OnEventUserOffLine(pIServerUserItem);

			if(bRet &&  (m_GameProgress == 3)) //只有3期工程才会有断线重连
			{
				DWORD dwServerID = (m_pGameServiceOption->dwServerID) &0xFFFFFF00;

				//游戏服直接修改 TODONOW 暂时屏蔽
				pIServerUserItem->SetOfflineGameID(dwServerID);

				//发送给协调服, 由协调服通知登录服
				tagOfflineUser data;
				data.dwUserID = pIServerUserItem->GetUserID();
				data.dwServerID = dwServerID;
				data.byMask = 1; //表示增加断线用户
				m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_OFFLINE,&data,sizeof(tagOfflineUser));

				bIsExsit = true;
			}
		}
		else
		{
			pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);
		}

		//玩家状态变化
		/* 情形三 游戏服 socket断掉的时候
		**	1. 判断游戏列表中  是否存在
		**	如果不在, 则登录服肯定连接了，应该设置为大厅在线
		**	如果在, 则设置为游戏断线
		*/
		DBR_GP_UserQuitInfo quitInfo;
		quitInfo.dwUserID = pIServerUserItem->GetUserID();
		if(bIsExsit)
		{
			quitInfo.byOnlineMask = 3;
		}
		else
		{
			quitInfo.byOnlineMask = 1;
		}
		m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GP_GAME_USER_STATE,dwSocketID, &quitInfo,sizeof(quitInfo));
	}


	//清除信息
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
    switch (Command.wMainCmdID)
    {
    case MDM_GR_LOGON:		//登录命令
        {
            return OnTCPNetworkMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
        }
    case MDM_USER:			//用户命令
        {
            return OnTCPNetworkMainUser(Command.wSubCmdID,pData,wDataSize,dwSocketID);
        }
    case MDM_G_FRAME:		//框架命令
        {
            return OnTCPNetworkMainFrame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
        }
    case MDM_GF_GAME:		//游戏命令
        {
            return OnTCPNetworkMainGame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
        }
    case MDM_GR_MATCH:		//比赛命令
        {
            return OnTCPNetworkMainMatch(Command.wSubCmdID,pData,wDataSize,dwSocketID);
        }
    }

	//游戏服是长连接, 心跳包有效
	return true;
}

//房间消息
bool CAttemperEngineSink::SendRoomMessage(LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//构造数据
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//数据属性
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//发送数据
	m_AndroidUserManager.SendDataToClient(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	m_pITCPNetworkEngine->SendDataBatch(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize,BG_COMPUTER);

	return true;
}

//游戏消息
bool CAttemperEngineSink::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//构造数据
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//数据属性
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//发送数据
	m_AndroidUserManager.SendDataToClient(MDM_G_FRAME,CMD_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	m_pITCPNetworkEngine->SendDataBatch(MDM_G_FRAME,CMD_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize,BG_COMPUTER);

	return true;
}

//房间消息
bool CAttemperEngineSink::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//效验参数
	if (pIServerUserItem==NULL) return false;

	//发送数据
	if (pIServerUserItem->GetBindIndex()!=INVALID_WORD)
	{
		//变量定义
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//构造数据
		SystemMessage.wType=wType;
		SystemMessage.wLength=lstrlen(lpszMessage)+1;
		lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

		//变量定义
		WORD dwUserIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(dwUserIndex);

		//数据属性
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//发送数据
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//机器用户
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_AndroidUserManager.SendDataToClient(pBindParameter->dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

			//关闭处理
			if ((wType&(SMT_CLOSE_ROOM|SMT_CLOSE_LINK))!=0) m_AndroidUserManager.DeleteAndroidUserItem(pBindParameter->dwSocketID);
		}
		else
		{
			//常规用户
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
		}

		return true;
	}

	return false;
}

//游戏消息
bool CAttemperEngineSink::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	if(pIServerUserItem->IsVirtualUser())
		return true;

	//发送数据
	if ((pIServerUserItem->GetBindIndex()!=INVALID_WORD)&&(pIServerUserItem->IsClientReady()==true))
	{
		//变量定义
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//构造数据
		SystemMessage.wType=wType;
		SystemMessage.wLength=lstrlen(lpszMessage)+1;
		lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

		//变量定义
		WORD dwUserIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(dwUserIndex);

		//数据属性
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//发送数据
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//机器用户
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_AndroidUserManager.SendDataToClient(pBindParameter->dwSocketID,MDM_G_FRAME,CMD_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

			//关闭处理
			if ((wType&(SMT_CLOSE_ROOM|SMT_CLOSE_LINK))!=0) m_AndroidUserManager.DeleteAndroidUserItem(pBindParameter->dwSocketID);
		}
		else
		{
			//常规用户
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,MDM_G_FRAME,CMD_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
		}

		return true;
	}

	return false;
}

//房间消息
bool CAttemperEngineSink::SendRoomMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType, bool bAndroid)
{
	//变量定义
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//构造数据
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//数据属性
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//发送数据
	if (bAndroid)
	{
		//机器用户
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	}
	else
	{
		//常规用户
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	}

	return true;
}

//发送跑马灯消息
bool CAttemperEngineSink::SendMessageLobbyAndAllGame( LPCTSTR lpszMessage, WORD wType ,WORD MsgID)
{
	/*
	//构造消息
	CMD_CS_C_SendMarquee pSendLobbyMessage;
	ZeroMemory(&pSendLobbyMessage,sizeof(pSendLobbyMessage));
	lstrcpyn(pSendLobbyMessage.szMessage,lpszMessage,CountArray(pSendLobbyMessage.szMessage));
	pSendLobbyMessage.MsgType=wType;
	pSendLobbyMessage.MsgID = MsgID;
	//发送给协调服务器
	m_pITCPSocketService->SendData(MDM_WEB,CPR_WP_WEB_MARQUEE,&pSendLobbyMessage,sizeof(CMD_CS_C_SendMarquee));
	*/
	return true;
}

//发送数据
bool CAttemperEngineSink::SendData(BYTE cbSendMask, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//机器数据
	if ((cbSendMask&BG_COMPUTER)!=0)
	{
		m_AndroidUserManager.SendDataToClient(wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	//用户数据
	m_pITCPNetworkEngine->SendDataBatch(wMainCmdID,wSubCmdID,pData,wDataSize,cbSendMask);

	return true;
}

//发送数据
bool CAttemperEngineSink::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//发送数据
	if (LOWORD(dwSocketID)!=INVALID_WORD)
	{
		if (LOWORD(dwSocketID)>=INDEX_ANDROID)
		{
			//机器用户
			m_AndroidUserManager.SendDataToClient(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
		else 
		{
			//网络用户
			m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
	}

	return true;
}

//发送数据
bool CAttemperEngineSink::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;
	if(pIServerUserItem->IsVirtualUser())
	{
		return true;
	}

	//发送数据
	if (pIServerUserItem->GetBindIndex()!=INVALID_WORD)
	{
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//机器用户
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_AndroidUserManager.SendDataToClient(pBindParameter->dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
		else
		{
			//常规用户
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
		return true;
	}

	return false;
}

//更新用户财富信息
bool CAttemperEngineSink::OnEventModifyUserTreasure(IServerUserItem *pIServerUserItem, DWORD dwTableID, BYTE byTableMode, BYTE byRound, SCORE lUserTreasuse, BYTE byWin)
{
	//效验参数
	if (pIServerUserItem==NULL) return false;

	//构造数据
	STR_DBR_GR_MODIFY_USER_TREASURE DBR;
	ZeroMemory(&DBR, sizeof(DBR));
	DBR.dwUserId  =  pIServerUserItem->GetUserID(); //pUserInfo->dwUserID;
	DBR.dwKind = ((m_pGameServiceOption->dwServerID) >> 16) & 0xFF;
	DBR.dwTableID = dwTableID;
	DBR.byTableMode = byTableMode;
	DBR.byRound = byRound;
	DBR.lUserTreasure = lUserTreasuse;
	DBR.byWin = byWin;

	//发送数据
	DWORD dwBindIndex = pIServerUserItem->GetBindIndex();
	tagBindParameter *pBindParameter = GetBindParameter(static_cast<WORD>(dwBindIndex));
	 
	//构造提示
		TCHAR szString[512]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),TEXT("Step2 Round = %d, UserID = %ld, pBindParameter = %d"),
			byRound,
			pIServerUserItem->GetUserID(),
			(pBindParameter == NULL)
			);
		CTraceService::TraceString(szString,TraceLevel_Normal);

	if (NULL != pBindParameter)
	{
		m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_MODIFY_USER_TREASURE, pBindParameter->dwSocketID, &DBR, sizeof(DBR));
	}
	else
		m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_MODIFY_USER_TREASURE, 0, &DBR, sizeof(DBR));
	
	return true;
}

//用户状态
bool CAttemperEngineSink::OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID, WORD wOldChairID)
{
	//效验参数
	if (pIServerUserItem==NULL) return false;

	//变量定义
	CMD_GR_UserStatus UserStatus;
	ZeroMemory(&UserStatus,sizeof(UserStatus));

	//构造数据
	UserStatus.dwUserID=pIServerUserItem->GetUserID();
	UserStatus.UserStatus.wTableID=pIServerUserItem->GetTableID();
	UserStatus.UserStatus.wChairID=pIServerUserItem->GetChairID();
	UserStatus.UserStatus.cbUserStatus=pIServerUserItem->GetUserStatus();

	//发送数据
	SendData(BG_COMPUTER,MDM_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));

	if(pIServerUserItem->GetUserStatus()==US_SIT)
		SendDataBatchToMobileUser(pIServerUserItem->GetTableID(),MDM_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));
	else
		SendDataBatchToMobileUser(wOldTableID,MDM_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));


	//离开判断
	if (pIServerUserItem->GetUserStatus()==US_NULL)
	{
		//获取绑定
		WORD wBindIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

		//绑带处理
		if (pBindParameter!=NULL)
		{
			//绑定处理
			if (pBindParameter->pIServerUserItem==pIServerUserItem)
			{
				pBindParameter->pIServerUserItem=NULL;
			}

			//中断网络
			if (pBindParameter->dwSocketID!=0L)
			{
				if (LOWORD(pBindParameter->dwSocketID)>=INDEX_ANDROID)
				{
					m_AndroidUserManager.DeleteAndroidUserItem(pBindParameter->dwSocketID);
				}
				else
				{
					m_pITCPNetworkEngine->ShutDownSocket(pBindParameter->dwSocketID);
				}
			}
		}

		//离开处理
		OnEventUserLogout(pIServerUserItem,LER_NORMAL);
	}

	//开始判断
	if (pIServerUserItem->GetUserStatus()==US_READY)
	{
		//状态校验
		CTableFrame *pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];

		//开始判断
		if (pTableFrame->EfficacyStartGame(pIServerUserItem->GetChairID())==true)
		{
			pTableFrame->StartGame(); 
		}
	}

	return true;
}

//用户权限
bool CAttemperEngineSink::OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,bool bGameRight)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();

	DBR_GR_ManageUserRight ManageUserRight= {0};
	ManageUserRight.dwUserID = pUserInfo->dwUserID;
	ManageUserRight.dwAddRight = dwAddRight;
	ManageUserRight.dwRemoveRight = dwRemoveRight;
	ManageUserRight.bGameRight=bGameRight;

	//发送请求
	m_pIDBCorrespondManager->PostDataBaseRequest(ManageUserRight.dwUserID,DBR_GR_MANAGE_USER_RIGHT,0,&ManageUserRight,sizeof(ManageUserRight));

	return true;
}

//机器信息
bool CAttemperEngineSink::OnDBGameAndroidInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	DBO_GR_GameAndroidInfo * pGameAndroidInfo=(DBO_GR_GameAndroidInfo *)pData;
	WORD wHeadSize=sizeof(DBO_GR_GameAndroidInfo)-sizeof(pGameAndroidInfo->AndroidParameter);

	//效验参数
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGameAndroidInfo->wAndroidCount*sizeof(pGameAndroidInfo->AndroidParameter[0]))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGameAndroidInfo->wAndroidCount*sizeof(pGameAndroidInfo->AndroidParameter[0])))) return false;

	//设置机器
	if (pGameAndroidInfo->lResultCode==DB_SUCCESS)
	{
		m_AndroidUserManager.SetAndroidStock(pGameAndroidInfo->AndroidParameter,pGameAndroidInfo->wAndroidCount);
	}

	return true;
}

//加载断线重连返回
bool CAttemperEngineSink::On_DBO_GR_LOAD_OFFLINE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	if(sizeof(BYTE ) != wDataSize) return false;

	m_GameProgress =  *((BYTE*)pData);

	return true;
}

//修改难易度yang
bool CAttemperEngineSink::OnDBRoomLevelModify(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize==sizeof(DBR_GR_RoomLevelModify));
	if(wDataSize!=sizeof(DBR_GR_RoomLevelModify)) return false;

	//数据库修改成功 通知游戏服务器
	DBR_GR_RoomLevelModify * pRoomLevelModify = (DBR_GR_RoomLevelModify *)pData;
	/*if(pRoomLevelModify==NULL) return false;
	m_GameControlManager.SetControlLevel(pRoomLevelModify->wGameLevel);
	for ( int i = 0; i < m_TableFrameArray.GetCount(); i++ )
	{
	m_TableFrameArray[i]->WebControlGameLevel(pRoomLevelModify->wSeverID,pRoomLevelModify->wGameLevel);
	}*/

	return true;
}

//修改房间控制值yang
bool CAttemperEngineSink::OnDBRoomControlValModify(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize==sizeof(DBR_GR_RoomControlValModify));
	if(wDataSize!=sizeof(DBR_GR_RoomControlValModify)) return false;

	//数据库修改成功 通知游戏服务器
	DBR_GR_RoomControlValModify * pRoomControlValModify = (DBR_GR_RoomControlValModify *)pData;
	if(pRoomControlValModify==NULL) return false;

	InitRoomControlVal( pRoomControlValModify->wControlVal );

	return true;
}

#pragma region 主消息号分发处理
//注册事件
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case CPO_PGPL_REGISTER_SUCESS:		//注册完成
		{
			//事件处理
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_CORRESPOND_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	case CPO_PGPL_REGISTER_FAILURE:		//注册失败
		{
			//效验参数
			ASSERT(wDataSize ! = sizeof(STR_CPO_PGPL_REGISTER_FAILURE));
			if (wDataSize != sizeof(STR_CPO_PGPL_REGISTER_FAILURE)) return false;

			//变量定义
			STR_CPO_PGPL_REGISTER_FAILURE * pRegisterFailure=(STR_CPO_PGPL_REGISTER_FAILURE *)pData;

			//关闭处理
			m_bNeekCorrespond=false;
			m_pITCPSocketService->CloseSocket();

			//显示消息
			if (lstrlen(pRegisterFailure->szDescribeString)>0)
			{
				CTraceService::TraceString(pRegisterFailure->szDescribeString,TraceLevel_Exception);
			}

			//事件通知
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_FAILURE;
			SendUIControlPacket(UI_CORRESPOND_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	}

	return true;
}

//中转服务
bool CAttemperEngineSink::OnTCPSocketMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case CPO_PG_CLUB_TABLE_DISSOLVE: //登录服通知协调服, 协调服通知游戏服 解散桌子
		{
			//校验数据
			if (wDataSize != sizeof(STR_CPO_PG_CLUB_TABLE_DISSOLVE)) return false;
			
			STR_CPO_PG_CLUB_TABLE_DISSOLVE* pCPO = (STR_CPO_PG_CLUB_TABLE_DISSOLVE *)pData;

			//如果不是本服务器的消息, 则过滤掉
			if(pCPO->dwGameID != (m_pGameServiceOption->dwServerID & 0xFFFFFF00))
			{
				return true;
			}

			//如果是本服务器的消息, 则通知子游戏解散游戏
			for(int i = 0; i < m_TableFrameArray.GetCount(); i++)
			{
				//校验
				CTableFrame *pTableFrame = m_TableFrameArray.GetAt(i);
				if (NULL == pTableFrame)
					continue;

				//找到相同桌子号的， 然后解散
				if(pTableFrame->GetPassword() == pCPO->dwTableID)
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
						pTableFrame->ConcludeGame(GAME_CONCLUDE_NORMAL);
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

//汇总事件
bool CAttemperEngineSink::OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_COLLECT_REQUEST:	//用户汇总
		{
			//变量定义
			CMD_CS_C_UserEnter UserEnter;
			ZeroMemory(&UserEnter,sizeof(UserEnter));

			//发送用户
			WORD wIndex=0;
			do
			{
				//获取用户
				IServerUserItem * pIServerUserItem=m_ServerUserManager.EnumUserItem(wIndex++);
				if (pIServerUserItem==NULL) break;

				//设置变量
				UserEnter.dwUserID=pIServerUserItem->GetUserID();
				lstrcpyn(UserEnter.szNickName,pIServerUserItem->GetNickName(),CountArray(UserEnter.szNickName));

				//辅助信息
				UserEnter.cbGender=pIServerUserItem->GetGender();
				UserEnter.cbMemberOrder=pIServerUserItem->GetMemberOrder();
				UserEnter.cbMasterOrder=pIServerUserItem->GetMasterOrder();

				//发送数据
				m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_ENTER,&UserEnter,sizeof(UserEnter));
			} while (true);

			//汇报完成
			m_bCollectUser=true;
			m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_FINISH);

			return true;
		}
	}

	return true;
}

#pragma endregion

//登录处理
bool CAttemperEngineSink::OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CG_LOGON_USERID:		//I D 登录
		{
			return On_SUB_CG_Logon_UserID(pData,wDataSize,dwSocketID);
		}
	}

	return true;
}

//用户处理
bool CAttemperEngineSink::OnTCPNetworkMainUser(WORD wSubCmdID, VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CG_USER_RULE:			//用户规则
		{
			return On_SUB_CG_User_Rule(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_LOOKON:		//用户旁观
		{
			return On_SUB_CG_User_Lookon(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_SITDOWN:		//用户坐下
		{
			return On_SUB_CG_User_SitDown(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_STANDUP:		//用户起立
		{
			return On_SUB_CG_User_StandUp(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_INVITE_USER:	//邀请用户
		{
			return On_SUB_CG_User_InviteUser(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_REFUSE_SIT:	//拒绝玩家坐下
		{
			return On_SUB_CG_User_RefuseSit(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_KICK_USER:		//踢出用户
		{
			return On_SUB_CG_User_KickUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_CHAIR_REQ:    //请求更换位置
		{
			return OnTCPNetworkSubUserChairReq(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_CREATE_ROOM:	//创建房间
		{
			return On_SUB_CG_USER_CREATE_ROOM(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_CLUB_CREATE_TABLE:	//创建桌子
		{
			return On_SUB_CG_USER_CREATE_TABLE(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_JOIN_FK_ROOM:		//加入桌子 需要密码
		{
			return On_SUB_User_JoinFkRoom(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_JOIN_TABLE_NO_PASS://加入桌子 不需要密码
		{
			return On_SUB_CG_USER_JOIN_TABLE_NO_PASS(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_JOIN_GOLD_HALL_ROOM:	//加入金币大厅桌子
		{
			return On_SUB_CG_USER_JOIN_GOLD_HALL_ROOM(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_RECONNECT_ROOM:	//断线重连
		{
			return On_SUB_User_ReconnectRoom(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_CREATE_GROUP_ROOM:	//创建牌友圈房间
		{
			return On_Sub_CG_User_CreateGroupRoom(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_JOIN_GROUP_ROOM:	//加入牌友圈房间
		{
			return On_Sub_CG_User_JoinGroupRoom(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_JOIN_GOLD_ROOM:	//加入金币房
		{
			return On_SUB_User_JoinGoldRoom(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_GET_TABLELIST:			//获得开房列表
		{
			return OnTCPNetworkSubGetTableList(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_QUERY_LOTTERY:			//请求抽奖
		{
			return OnTcpNetworkQueryLottery(pData, wDataSize, dwSocketID);
		}
	}

	return false;
}

//比赛命令
bool CAttemperEngineSink::OnTCPNetworkMainMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//效验接口
	ASSERT(m_pIGameMatchServiceManager!=NULL);
	if (m_pIGameMatchServiceManager==NULL) return false;

	//消息处理
	return m_pIGameMatchServiceManager->OnEventSocketMatch(wSubCmdID,pData,wDataSize,pIServerUserItem,dwSocketID);
}

//游戏处理
bool CAttemperEngineSink::OnTCPNetworkMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//处理过虑
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//消息处理 
	CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
	return pTableFrame->OnEventSocketGame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//框架处理
bool CAttemperEngineSink::OnTCPNetworkMainFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//处理过虑
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//消息处理 
	CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
	return pTableFrame->OnEventSocketFrame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//I D 登录
bool CAttemperEngineSink::On_SUB_CG_Logon_UserID(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	if (wDataSize != sizeof(STR_SUB_CG_LOGON_USERID)) return false;
	STR_SUB_CG_LOGON_USERID *pLogonUserID = (STR_SUB_CG_LOGON_USERID *)pData;

	//版本校验   校验client子游戏  与  server子游戏版本是否匹配
	DWORD clientSubGameVersion = pLogonUserID->dwSubGameVersion;
	DWORD serverSubGameVersion = m_pGameServiceAttrib->dwSubGameVersion;
	if(0 != Compate_clientSubGame_ServerSubGame(clientSubGameVersion, serverSubGameVersion))
	{
		//printf("\n【客户端】：USERID登录，版本不匹配\n");
	}

	//提示消息
	TCHAR szString[512]=TEXT("");

	//重复判断
	WORD wBindIndex = LOWORD(dwSocketID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex); //有空闲socket
	IServerUserItem *pIBindUserItem = GetBindUserItem(wBindIndex);   //该空闲socket没有绑定用户
	if ((pBindParameter==NULL)||(pIBindUserItem!=NULL))
	{ 
		_sntprintf_s(szString,CountArray(szString),
			TEXT("【ID登录】【%ld重复登录1】"), 
			pLogonUserID->dwUserID);
		CTraceService::TraceString(szString,TraceLevel_Info);

		return false;
	}

	//在用户列表获取用户
	IServerUserItem *pIServerUserItem = m_ServerUserManager.SearchUserItem(pLogonUserID->dwUserID);

	//非正常登录的用户不允许踢出
	if (pIServerUserItem!=NULL)
	{
		//非正常登录的两种情况
		//1. 该用户如果是机器用户, 但是从外地登录
		//2. 该机器非机器用户, 但是在该进程上登录
		if((pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr!=0L))
			|| (!pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr==0L)))
		{
			_sntprintf_s(szString,CountArray(szString),
				TEXT("【ID登录】【%ld登录失败】非正常登录, 不允许踢出"), 
				pLogonUserID->dwUserID);
			CTraceService::TraceString(szString,TraceLevel_Info);

			return false;
		}
	}

	
	//切换判断 TODONOW 重点查看
	if( NULL != pIServerUserItem )
	{
		_sntprintf_s(szString,CountArray(szString),
			TEXT("【ID登录】【%ld重复登录】切换连接1"), 
			pLogonUserID->dwUserID);
		CTraceService::TraceString(szString,TraceLevel_Info);

		return SwitchUserItemConnect(pIServerUserItem, pLogonUserID->szMachineID, 
			wBindIndex,
			pLogonUserID->dLongitude, pLogonUserID->dLatitude);
	}
	

	//变量定义
	STR_DBR_CG_LOGON_USERID LogonUserID;
	ZeroMemory(&LogonUserID,sizeof(LogonUserID));

	//构造数据
	LogonUserID.dwUserID = pLogonUserID->dwUserID;
	LogonUserID.dwClientAddr = pBindParameter->dwClientAddr;
	LogonUserID.dLatitude = pLogonUserID->dLatitude;
	LogonUserID.dLongitude = pLogonUserID->dLongitude;
	lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
	lstrcpyn(LogonUserID.szMachineID,pLogonUserID->szMachineID,CountArray(LogonUserID.szMachineID));

	//投递请求
	return m_pIDBCorrespondManager->PostDataBaseRequest(LogonUserID.dwUserID, DBR_CG_LOGON_USERID, dwSocketID, &LogonUserID,sizeof(LogonUserID));
}

//ID登录返回
bool CAttemperEngineSink::On_CMD_GC_Logon_UserID(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	if (wDataSize != sizeof(STR_DBO_CG_LOGON_USERID)) return false;
	STR_DBO_CG_LOGON_USERID *pDBOLogon = (STR_DBO_CG_LOGON_USERID *)pData;

	//判断在线
	if ((m_pNormalParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//用户登录失败，直接返回
	if ( DB_SUCCESS != pDBOLogon->lResultCode )
	{
		//构造返回数据
		STR_CMD_GC_LOGON_USERID logonFail;
		ZeroMemory(&logonFail, sizeof(STR_CMD_GC_LOGON_USERID));
		logonFail.lResultCode = 0;
		lstrcpyn(logonFail.szDescribeString, pDBOLogon->szDescribeString, CountArray(logonFail.szDescribeString));

		//发送数据
		m_pITCPNetworkEngine->SendData(dwContextID, MDM_GR_LOGON, CMD_GC_LOGON_USERID, &logonFail, sizeof(STR_CMD_GC_LOGON_USERID));
		return true;
	}

	//变量定义
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex); //寻找空闲socket

	//无空闲socket退出
	if ( (NULL != pBindParameter->pIServerUserItem)||
		 (pBindParameter->dwSocketID != dwContextID) )
	{
		//提示消息
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),
			TEXT("【ID登录】【%ld登录失败】无空闲socket"),
			pDBOLogon->dwUserID);
		CTraceService::TraceString(szDescribe,TraceLevel_Warning);

		return true;
	}

	//在用户列表中获取 用户
	IServerUserItem *pIServerUserItem = m_ServerUserManager.SearchUserItem(pDBOLogon->dwUserID);

	/*
	//重复登录判断 TODONOW
	if (pIServerUserItem!=NULL)
	{
		//切换用户
		SwitchUserItemConnect(pIServerUserItem, pDBOLogon->szMachineID, wBindIndex, pDBOLogon->dLongitude, pDBOLogon->dLatitude, 
								pDBOLogon->cbDeviceType, pDBOLogon->wBehaviorFlags, pDBOLogon->wPageTableCount);

		//提示消息
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),
			TEXT("【ID登录】【%ld重复登录2】"),
			pIServerUserItem->GetUserID());
		CTraceService::TraceString(szDescribe,TraceLevel_Warning);

		return true;
	}
	*/

	//激活用户
	ActiveUserItem(&pIServerUserItem, dwContextID, pBindParameter, pDBOLogon, wBindIndex);

	return true;
}

//ID登录成功，激活用户
void CAttemperEngineSink::ActiveUserItem(IServerUserItem **pIServerUserItem, DWORD dwContextID, 
		tagBindParameter *pBindParameter, STR_DBO_CG_LOGON_USERID *pDBOLogon, WORD wBindIndex)
{
	if ( NULL == pBindParameter || NULL == pDBOLogon )
	{
		return;
	}

	//用户变量
	tagUserInfo UserInfo;
	tagUserInfoPlus UserInfoPlus;
	ZeroMemory(&UserInfo,sizeof(UserInfo));
	ZeroMemory(&UserInfoPlus,sizeof(UserInfoPlus));

	//属性资料
	UserInfo.dwUserID = pDBOLogon->dwUserID;
	UserInfo.dwGroupID = pDBOLogon->dwGroupID;
	UserInfo.dwCustomID = pDBOLogon->dwCustomID;
	lstrcpyn(UserInfo.szNickName,pDBOLogon->szNickName,CountArray(UserInfo.szNickName));
	lstrcpyn(UserInfo.szHeadUrl,pDBOLogon->szHeadUrl,CountArray(UserInfo.szHeadUrl));


	//用户资料
	UserInfo.cbGender=pDBOLogon->cbGender;
	UserInfo.cbMemberOrder=pDBOLogon->cbMemberOrder;
	UserInfo.cbMasterOrder=pDBOLogon->cbMasterOrder;
	lstrcpyn(UserInfo.szGroupName,pDBOLogon->szGroupName,CountArray(UserInfo.szGroupName));
	lstrcpyn(UserInfo.szUnderWrite,pDBOLogon->szUnderWrite,CountArray(UserInfo.szUnderWrite));

	//状态设置
	UserInfo.cbUserStatus=US_FREE;
	UserInfo.wTableID=INVALID_TABLE;
	UserInfo.wChairID=INVALID_CHAIR;

	//积分信息
	UserInfo.lScore=pDBOLogon->lScore;
	UserInfo.lGrade=pDBOLogon->lGrade;
	UserInfo.lOpenRoomCard=pDBOLogon->lOpenRoomCard;
	UserInfo.lDiamond=pDBOLogon->lDiamond;
	UserInfo.lGold=pDBOLogon->lGold;
	UserInfo.lControlScore=pDBOLogon->lControlScore;
	UserInfo.dwWinCount=pDBOLogon->dwWinCount;
	UserInfo.dwLostCount=pDBOLogon->dwLostCount;
	UserInfo.dwDrawCount=pDBOLogon->dwDrawCount;
	UserInfo.dwFleeCount=pDBOLogon->dwFleeCount;
	UserInfo.dwUserMedal=pDBOLogon->dwUserMedal;
	UserInfo.dwExperience=pDBOLogon->dwExperience;
	UserInfo.lLoveLiness=pDBOLogon->lLoveLiness;

	//位置信息
	UserInfo.dLatitude = pDBOLogon->dLatitude;
	UserInfo.dLongitude = pDBOLogon->dLongitude;

	//登录信息
	UserInfoPlus.dwLogonTime=(DWORD)time(NULL);
	UserInfoPlus.dwInoutIndex=pDBOLogon->dwInoutIndex;

	//用户权限
	UserInfoPlus.dwUserRight=pDBOLogon->dwUserRight;
	UserInfoPlus.dwMasterRight=pDBOLogon->dwMasterRight;

	//辅助变量
	bool bAndroidUser = (wBindIndex>=INDEX_ANDROID);						//用户类型，真人/机器人
	UserInfoPlus.bAndroidUser = bAndroidUser;
	UserInfoPlus.lRestrictScore=0L;//屏蔽每局封顶
	lstrcpyn(UserInfoPlus.szPassword,pDBOLogon->szPassword,CountArray(UserInfoPlus.szPassword));

	//连接信息
	UserInfoPlus.wBindIndex=wBindIndex;
	UserInfoPlus.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(UserInfoPlus.szMachineID,pDBOLogon->szMachineID,CountArray(UserInfoPlus.szMachineID));

	//激活用户 -- 设置用户信息
	m_ServerUserManager.InsertUserItem(pIServerUserItem, UserInfo, UserInfoPlus);

	//错误判断 -- 设置用户pIServerUserItem信息失败
	if (pIServerUserItem == NULL)
	{
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),
			TEXT("【ID登录】【玩家%ld登录失败】 pIServerUserItem设置信息失败"),
			UserInfo.dwUserID);
		CTraceService::TraceString(szDescribe,TraceLevel_Warning);

		//断开用户
		if (bAndroidUser==true)
		{
			m_AndroidUserManager.DeleteAndroidUserItem(dwContextID);
		}
		else
		{
			m_pITCPNetworkEngine->ShutDownSocket(dwContextID);
		}
		return;
	}

	//设置用户
	pBindParameter->pIServerUserItem = *pIServerUserItem;
	
	//登录事件
	OnEventUserLogon(*pIServerUserItem, false);
}

//用户登录
VOID CAttemperEngineSink::OnEventUserLogon(IServerUserItem * pIServerUserItem, bool bAlreadyOnLine)
{
	//获取参数
	WORD wBindIndex = pIServerUserItem->GetBindIndex();
	bool bAndroidUser = pIServerUserItem->IsAndroidUser();
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	/* 1. 构造返回数据 */
	STR_CMD_GC_LOGON_USERID logon;
	ZeroMemory(&logon, sizeof(STR_CMD_GC_LOGON_USERID));
	logon.lResultCode = 0;
	lstrcpyn(logon.szDescribeString, TEXT("用户登录成功"), CountArray(logon.szDescribeString));
	
	//TODONOW
	logon.dwOffLineGameID = pIServerUserItem -> GetOfflineGameID();

	CString strTrace;
	strTrace.Format(TEXT("【ID登录】【%ld断线重连】 GetOfflineGameID = %ld"),
		(pIServerUserItem->GetUserInfo())->dwUserID, 
		logon.dwOffLineGameID);
	CTraceService::TraceString(strTrace, TraceLevel_Debug);

	//发送数据
	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GR_LOGON, CMD_GC_LOGON_USERID, &logon, sizeof(STR_CMD_GC_LOGON_USERID));
	
	/* 2. 发送玩家自己的信息给客户端 */
	SendUserInfoPacket(pIServerUserItem, pBindParameter->dwSocketID);

	/* 3. 发送登录游戏服的所有用户信息给该用户 */
	WORD wUserIndex = 0;
	IServerUserItem *pIServerUserItemSend = NULL;
	while (true)
	{
		pIServerUserItemSend = m_ServerUserManager.EnumUserItem(wUserIndex++);
		if (pIServerUserItemSend == NULL) break;
		if (pIServerUserItemSend == pIServerUserItem) continue;
		SendUserInfoPacket(pIServerUserItemSend, pBindParameter->dwSocketID);
	}

	/* 4. 桌子信息 */
	CMD_GR_TableInfo TableInfo;
	ZeroMemory(&TableInfo, sizeof(CMD_GR_TableInfo));

	//赋值
	TableInfo.wTableCount = (WORD)m_TableFrameArray.GetCount();
	for (WORD i=0; i<TableInfo.wTableCount; i++)
	{
		CTableFrame *pTableFrame = m_TableFrameArray[i];
		TableInfo.TableStatusArray[i].cbTableLock = pTableFrame->IsTableLocked()?TRUE:FALSE;
		TableInfo.TableStatusArray[i].cbPlayStatus = pTableFrame->IsTableStarted()?TRUE:FALSE;
	}

	//桌子状态
	WORD wHeadSize=sizeof(TableInfo)-sizeof(TableInfo.TableStatusArray);
	WORD wSendSize=wHeadSize+TableInfo.wTableCount*sizeof(TableInfo.TableStatusArray[0]);
	SendData(pBindParameter->dwSocketID, MDM_GR_STATUS, CMD_GR_TABLE_INFO, &TableInfo, wSendSize);

	/* 5. 发送玩家信息		TODO  */
	if (!bAlreadyOnLine)
	{
		SendUserInfoPacket(pIServerUserItem,INVALID_DWORD);
	}

	/* 6. 非机器人 */
	if (!bAndroidUser)
	{
		//设置允许群发
		m_pITCPNetworkEngine->AllowBatchSend(pBindParameter->dwSocketID, true, BG_COMPUTER);
	}

	/* 7. 广播在线人数		TODONOW 客户端没用到该消息号 */
	CMD_GF_OnlinePlayers OnlinePlayers;
	ZeroMemory(&OnlinePlayers, sizeof(CMD_GF_OnlinePlayers));

	//赋值
	for(WORD i=0;static_cast<DWORD>(i) < m_ServerUserManager.GetUserItemCount();i++)
	{
		if( m_ServerUserManager.EnumUserItem(i)->GetUserStatus() != US_OFFLINE 
			&& !m_ServerUserManager.EnumUserItem(i)->IsVirtualUser())
			OnlinePlayers.wOnlinePlayers++;
	}

	//发送数据
	for(WORD i=0;static_cast<DWORD>(i) < m_ServerUserManager.GetUserItemCount();i++)
	{
		IServerUserItem* pItem = m_ServerUserManager.EnumUserItem(i);
		if( pItem->GetUserStatus() != US_OFFLINE && !pItem->IsVirtualUser())
		{
			SendData(pItem,MDM_USER,SUB_GF_ONLINE_PLAYERS,&OnlinePlayers,sizeof(CMD_GF_OnlinePlayers));
		}
	}

	/* 10. 比赛信息 */
	if(m_pIGameMatchServiceManager!=NULL)
	{
		m_pIGameMatchServiceManager->SendMatchInfo(pIServerUserItem);
	}

	/* 11. 通知数据库改变 玩家状态 */
	//TODONOWW
	DBR_GP_UserQuitInfo quitInfo;
	quitInfo.dwUserID = pIServerUserItem->GetUserID();
	quitInfo.byOnlineMask = 2;
	m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GP_GAME_USER_STATE,0, &quitInfo,sizeof(quitInfo));
}

//切换连接
bool CAttemperEngineSink::SwitchUserItemConnect(IServerUserItem * pIServerUserItem, TCHAR szMachineID[LEN_MACHINE_ID], WORD wTargetIndex,
												const double &dLongitude, const double &dLatitude,
												BYTE cbDeviceType, WORD wBehaviorFlags, WORD wPageTableCount)
{
	//效验参数
	if ((NULL == pIServerUserItem)||(INVALID_WORD == wTargetIndex)) 
		return false;

	//用户有效，断开已绑定的用户
	if (INVALID_WORD != pIServerUserItem->GetBindIndex())	
	{
		//绑定参数
		WORD wSourceIndex = pIServerUserItem->GetBindIndex();
		tagBindParameter *pSourceParameter = GetBindParameter(wSourceIndex);

		//解除绑定
		if ( (NULL != pSourceParameter) && (pSourceParameter->pIServerUserItem==pIServerUserItem) ) 
			pSourceParameter->pIServerUserItem=NULL;

		//断开用户
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			m_AndroidUserManager.DeleteAndroidUserItem(pSourceParameter->dwSocketID);
		}
		else
		{
			m_pITCPNetworkEngine->ShutDownSocket(pSourceParameter->dwSocketID);
		}
	}

	//机器判断
	LPCTSTR pszMachineID = pIServerUserItem->GetMachineID();
	bool bSameMachineID = (lstrcmp(pszMachineID, szMachineID)==0);

	//变量定义
	bool bAndroidUser = (wTargetIndex>=INDEX_ANDROID);
	tagBindParameter *pTargetParameter = GetBindParameter(wTargetIndex);

	//重新激活用户
	pTargetParameter->pIServerUserItem = pIServerUserItem;
	pIServerUserItem->SetUserParameter(pTargetParameter->dwClientAddr, wTargetIndex, szMachineID, bAndroidUser, false, dLongitude, dLatitude);

	//登录事件
	OnEventUserLogon(pIServerUserItem,true);

	return true;
}

/*********************************************【主消息 3 用户命令】*********************************************************/
//用户规则
bool CAttemperEngineSink::On_SUB_CG_User_Rule(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize>=sizeof(CMD_GR_UserRule));
	if (wDataSize<sizeof(CMD_GR_UserRule)) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//规则判断
	//ASSERT(CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==false);
	//if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	//消息处理
	tagUserRule * pUserRule=pIServerUserItem->GetUserRule();
	CMD_GR_UserRule * pCMDUserRule=(CMD_GR_UserRule *)pData;

	//规则标志
	pUserRule->bLimitSameIP=((pCMDUserRule->cbRuleMask&UR_LIMIT_SAME_IP)>0);
	pUserRule->bLimitWinRate=((pCMDUserRule->cbRuleMask&UR_LIMIT_WIN_RATE)>0);
	pUserRule->bLimitFleeRate=((pCMDUserRule->cbRuleMask&UR_LIMIT_FLEE_RATE)>0);
	pUserRule->bLimitGameScore=((pCMDUserRule->cbRuleMask&UR_LIMIT_GAME_SCORE)>0);

	//规则属性
	pUserRule->szPassword[0]=0;
	pUserRule->wMinWinRate=pCMDUserRule->wMinWinRate;
	pUserRule->wMaxFleeRate=pCMDUserRule->wMaxFleeRate;
	pUserRule->lMaxGameScore=pCMDUserRule->lMaxGameScore;
	pUserRule->lMinGameScore=pCMDUserRule->lMinGameScore;

	//桌子密码
	if (wDataSize>sizeof(CMD_GR_UserRule))
	{
		//变量定义
		VOID * pDataBuffer=NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pCMDUserRule+1,wDataSize-sizeof(CMD_GR_UserRule));

		//提取处理
		while (true)
		{
			//提取数据
			pDataBuffer=RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe==DTP_NULL) break;

			//数据分析
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_GR_TABLE_PASSWORD:		//桌子密码
				{
					//效验数据
					ASSERT(pDataBuffer!=NULL);
					ASSERT(DataDescribe.wDataSize<=sizeof(pUserRule->szPassword));

					//规则判断
					ASSERT(CServerRule::IsForfendLockTable(m_pGameServiceOption->dwServerRule)==false);
					//if (CServerRule::IsForfendLockTable(m_pGameServiceOption->dwServerRule)==true) break;

					//设置数据
					if (DataDescribe.wDataSize<=sizeof(pUserRule->szPassword))
					{
						CopyMemory(&pUserRule->szPassword,pDataBuffer,DataDescribe.wDataSize);
						pUserRule->szPassword[CountArray(pUserRule->szPassword)-1]=0;
					}

					break;
				}
			}
		}
	}

	return true;
}

//用户旁观
bool CAttemperEngineSink::On_SUB_CG_User_Lookon(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_UserLookon));
	if (wDataSize!=sizeof(CMD_GR_UserLookon)) return false;

	//效验数据
	CMD_GR_UserLookon * pUserLookon=(CMD_GR_UserLookon *)pData;
	if (pUserLookon->wChairID>=m_pGameServiceAttrib->wChairCount) return false;
	if (pUserLookon->wTableID>=(WORD)m_TableFrameArray.GetCount()) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//消息处理
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
	if ((wTableID==pUserLookon->wTableID)&&(wChairID==pUserLookon->wChairID)&&(cbUserStatus==US_LOOKON)) return true;

	//用户判断
	if (cbUserStatus==US_PLAYING)
	{
		SendRequestFailure(pIServerUserItem,TEXT("您正在游戏中，暂时不能离开，请先结束当前游戏！"),0);
		return true;
	}

	//离开处理
	if (wTableID!=INVALID_TABLE)
	{
		CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
		if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
	}

	//坐下处理
	CTableFrame * pTableFrame=m_TableFrameArray[pUserLookon->wTableID];
	pTableFrame->PerformLookonAction(pUserLookon->wChairID,pIServerUserItem);

	return true;
}

//用户坐下
bool CAttemperEngineSink::On_SUB_CG_User_SitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_UserSitDown));
	if (wDataSize!=sizeof(CMD_GR_UserSitDown)) return false;

	//效验数据
	CMD_GR_UserSitDown * pUserSitDown=(CMD_GR_UserSitDown *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//消息处理
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

	//重复判断
	if ((pUserSitDown->wTableID < MAX_TABLE)&&(pUserSitDown->wChairID<m_pGameServiceAttrib->wChairCount))
	{
		CTableFrame * pTableFrame=m_TableFrameArray[pUserSitDown->wTableID];
		if (pTableFrame->GetTableUserItem(pUserSitDown->wChairID)==pIServerUserItem) return true;
	}

	//用户判断
	if (cbUserStatus==US_PLAYING)
	{
		SendRequestFailure(pIServerUserItem,TEXT("您正在游戏中，暂时不能离开，请先结束当前游戏！"),0);
		return true;
	}

	//房间判断
	if(m_bStopping)
	{
		//发送失败
		//SendLogonFailure(TEXT("很抱歉，此游戏房间已经关闭了，不允许继续进入！"),LOGON_FAIL_SERVER_INVALIDATION,dwSocketID);
		return true;
	}


	//离开处理
	if (wTableID!=INVALID_TABLE)
	{
		CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
		if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
	}

	////防作弊
	//if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR))
	//{
	//}

	//请求调整
	WORD wRequestTableID=pUserSitDown->wTableID;
	WORD wRequestChairID=pUserSitDown->wChairID;

	//桌子调整
	if (wRequestTableID>=m_TableFrameArray.GetCount())
	{
		//起始桌子
		WORD wStartTableID=0;
//		DWORD dwServerRule=m_pGameServiceOption->dwServerRule;
	//	if ((CServerRule::IsAllowAvertCheatMode(dwServerRule)==true)&&(m_pGameServiceAttrib->wChairCount<MAX_CHAIR)) wStartTableID=1;

		//动态加入
		bool bDynamicJoin=true;
		//if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
	//	if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;

		//寻找位置
		for (WORD i=wStartTableID;i<m_TableFrameArray.GetCount();i++)
		{
			//游戏状态
			if ((m_TableFrameArray[i]->IsGameStarted()==true)&&(bDynamicJoin==false))continue;

			//获取空位
			WORD wNullChairID=m_TableFrameArray[i]->GetNullChairID();

			//调整结果
			if (wNullChairID!=INVALID_CHAIR)
			{
				//设置变量
				wRequestTableID=i;
				wRequestChairID=wNullChairID;

				break;
			}
		}

		//结果判断
		if ((wRequestTableID==INVALID_CHAIR)||(wRequestChairID==INVALID_CHAIR))
		{
			SendRequestFailure(pIServerUserItem,TEXT("当前游戏房间已经人满为患了，暂时没有可以让您加入的位置，请稍后再试！"),0);
			return true;
		}
	}

	//椅子调整
	if (wRequestChairID>=m_pGameServiceAttrib->wChairCount)
	{
		//效验参数
		ASSERT(wRequestTableID<m_TableFrameArray.GetCount());
		if (wRequestTableID>=m_TableFrameArray.GetCount()) return false;

		//查找空位
		wRequestChairID=m_TableFrameArray[wRequestTableID]->GetNullChairID();

		//结果判断
		if (wRequestChairID==INVALID_CHAIR)
		{
			SendRequestFailure(pIServerUserItem,TEXT("由于此游戏桌暂时没有可以让您加入的位置了，请选择另外的游戏桌！"),0);
			return true;
		}
	}

	//坐下处理
	CTableFrame * pTableFrame=m_TableFrameArray[wRequestTableID];
	pTableFrame->PerformSitDownAction(wRequestChairID,pIServerUserItem,pUserSitDown->szPassword);

	return true;
}

//用户起立
bool CAttemperEngineSink::On_SUB_CG_User_StandUp(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//数据包校验
	ASSERT(wDataSize == sizeof(STR_SUB_CG_USER_STANDUP));
	if (wDataSize != sizeof(STR_SUB_CG_USER_STANDUP)) 
		return false;

	//获取用户
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);

	//用户效验
	if (pIServerUserItem == NULL) return false;

	//起立数据
	STR_SUB_CG_USER_STANDUP *pUserStandUp = (STR_SUB_CG_USER_STANDUP *)pData;

	//取消分组		TODO 这是在干嘛？
	if( //CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule) && 
		(m_pGameServiceAttrib->wChairCount < MAX_CHAIR) )
	{
		POSITION pos = m_WaitDistributeList.GetHeadPosition();
		while(pos != NULL)
		{
			POSITION tempPos = pos;
			IServerUserItem *pUserItem = m_WaitDistributeList.GetNext(pos);
			if(pUserItem->GetUserID() == pIServerUserItem->GetUserID())
			{
				m_WaitDistributeList.RemoveAt(tempPos);
				break;
			}
		}

		if(pUserStandUp->wTableID == INVALID_TABLE) return true;
	}

	//效验桌子和椅子
	if (pUserStandUp->wChairID >= m_pGameServiceAttrib->wChairCount) 
		return false;
	if (pUserStandUp->wTableID >= (WORD)m_TableFrameArray.GetCount()) 
		return false;

	//桌子号和椅子号校验
	WORD wTableID = pIServerUserItem->GetTableID();
	WORD wChairID = pIServerUserItem->GetChairID();
	if ((wTableID !=pUserStandUp->wTableID) || (wChairID != pUserStandUp->wChairID)) 
		return true;
		
	//动态加入处理
	bool bDynamicJoin = false;
	if (wTableID != INVALID_TABLE)
	{
		//校验桌子
		CTableFrame *pTableFrame = m_TableFrameArray[wTableID];
		if ( NULL == pTableFrame )
			return false;

		//房间规则：金币或电玩模式则动态加入
		tagTableRule *pTableCfg = (tagTableRule*)pTableFrame->GetCustomRule();
		if(pTableCfg != NULL && (pTableCfg->GameMode == TABLE_MODE_DW || pTableCfg->GameMode == TABLE_MODE_GOLD))
			bDynamicJoin = true;
	}

	//用户判断
	if (!bDynamicJoin && (pIServerUserItem->GetUserStatus()==US_PLAYING))
	{
		SendRequestFailure(pIServerUserItem, TEXT("您正在游戏中，暂时不能离开，请先结束当前游戏！"), REQUEST_FAILURE_NORMAL);
		return true;
	}

	//离开处理
	if (wTableID != INVALID_TABLE)
	{
		//获得桌子
		CTableFrame *pTableFrame = m_TableFrameArray[wTableID];
		if (NULL == pTableFrame) 
			return true;
		//用户站起
		if ( !pTableFrame->PerformStandUpAction(pIServerUserItem) ) 
			return true;
	}

	return true;
}

//邀请用户
bool CAttemperEngineSink::On_SUB_CG_User_InviteUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_GR_UserInviteReq));
	if (wDataSize!=sizeof(CMD_GR_UserInviteReq)) return false;

	//消息处理
	CMD_GR_UserInviteReq * pUserInviteReq=(CMD_GR_UserInviteReq *)pData;

	//获取用户
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//效验状态
	if (pUserInviteReq->wTableID==INVALID_TABLE) return true;
	if (pIServerUserItem->GetTableID()!=pUserInviteReq->wTableID) return true;
	if (pIServerUserItem->GetUserStatus()==US_PLAYING) return true;
	if (pIServerUserItem->GetUserStatus()==US_OFFLINE) return true;

	//目标用户
	IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchUserItem(pUserInviteReq->dwUserID);
	if (pITargetUserItem==NULL) return true;
	if (pITargetUserItem->GetUserStatus()==US_PLAYING) return true;

	//发送消息
	CMD_GR_UserInvite UserInvite;
	memset(&UserInvite,0,sizeof(UserInvite));
	UserInvite.wTableID=pUserInviteReq->wTableID;
	UserInvite.dwUserID=pIServerUserItem->GetUserID();
	SendData(pITargetUserItem, MDM_USER, CMD_GC_USER_INVITE_USER, &UserInvite, sizeof(UserInvite));

	return true;
}

//拒绝玩家坐下
bool CAttemperEngineSink::On_SUB_CG_User_RefuseSit(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_GR_UserRepulseSit));
	if (wDataSize!=sizeof(CMD_GR_UserRepulseSit)) return false;

	//消息处理
	CMD_GR_UserRepulseSit * pUserRepulseSit=(CMD_GR_UserRepulseSit *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//获取桌子
	CTableFrame * pTableFrame=m_TableFrameArray[pUserRepulseSit->wTableID];
	if (pTableFrame->IsGameStarted()==true) return true;

	//获取用户
	IServerUserItem * pRepulseIServerUserItem = pTableFrame->GetTableUserItem(pUserRepulseSit->wChairID);
	if (pRepulseIServerUserItem==NULL) return true;
	if(pRepulseIServerUserItem->GetUserID() != pUserRepulseSit->dwRepulseUserID)return true;

	//发送消息
	TCHAR szDescribe[256]=TEXT("");
	lstrcpyn(szDescribe,TEXT("此桌有玩家设置了不与您同桌游戏！"),CountArray(szDescribe));
	SendRoomMessage(pRepulseIServerUserItem,szDescribe,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);

	//弹起玩家
	pTableFrame->PerformStandUpAction(pRepulseIServerUserItem);

	return true;
}

//踢出用户
bool CAttemperEngineSink::On_SUB_CG_User_KickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_KickUser));
	if (wDataSize!=sizeof(CMD_GR_KickUser)) return false;

	//变量定义
	CMD_GR_KickUser * pKickUser=(CMD_GR_KickUser *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//目标用户
	IServerUserItem * pITargetUserItem = m_ServerUserManager.SearchUserItem(pKickUser->dwTargetUserID);
	if(pITargetUserItem==NULL) return true;

	//用户效验
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMemberOrder()>pITargetUserItem->GetMemberOrder()));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMemberOrder()<=pITargetUserItem->GetMemberOrder())) return false;

	//权限判断
	ASSERT(CUserRight::CanKillOutUser(pIServerUserItem->GetUserRight())==true);
	if (CUserRight::CanKillOutUser(pIServerUserItem->GetUserRight())==false) return false;

	//百人游戏
	if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
	{
		//发送消息
		SendRoomMessage(pIServerUserItem,TEXT("很抱歉，百人游戏不许踢人！"),SMT_EJECT);
		return true;
	}

	//用户状态
	if(pITargetUserItem->GetUserStatus()==US_PLAYING)
	{
		//变量定义
		TCHAR szMessage[256]=TEXT("");
		_sntprintf_s(szMessage,CountArray(szMessage),TEXT("由于玩家 [ %s ] 正在游戏中,您不能将它踢出游戏！"),pITargetUserItem->GetNickName());

		//发送消息
		SendRoomMessage(pIServerUserItem,szMessage,SMT_EJECT);
		return true;
	}

	//防踢判断
	if((pITargetUserItem->GetUserProperty()->wPropertyUseMark&PT_USE_MARK_GUARDKICK_CARD)!=0)
	{
		//变量定义
		DWORD dwCurrentTime=(DWORD)time(NULL);
		tagUserProperty * pUserProperty = pITargetUserItem->GetUserProperty();

		//时效判断
		DWORD dwValidTime=pUserProperty->PropertyInfo[2].wPropertyCount*pUserProperty->PropertyInfo[2].dwValidNum;
		if(pUserProperty->PropertyInfo[2].dwEffectTime+dwValidTime>dwCurrentTime)
		{
			//变量定义
			TCHAR szMessage[256]=TEXT("");
			_sntprintf_s(szMessage,CountArray(szMessage),TEXT("由于玩家 [ %s ] 正在使用防踢卡,您无法将它踢出游戏！"),pITargetUserItem->GetNickName());

			//发送消息
			SendRoomMessage(pIServerUserItem,szMessage,SMT_EJECT);

			return true; 
		}
		else
			pUserProperty->wPropertyUseMark &= ~PT_USE_MARK_GUARDKICK_CARD;
	}

	//请离桌子
	WORD wTargerTableID = pITargetUserItem->GetTableID();
	if(wTargerTableID != INVALID_TABLE)
	{
		//定义变量
		TCHAR szMessage[64]=TEXT("");
		_sntprintf_s(szMessage,CountArray(szMessage),TEXT("你已被%s请离桌子！"),pIServerUserItem->GetNickName());

		//发送消息
		SendGameMessage(pITargetUserItem,szMessage,SMT_CHAT|SMT_CLOSE_GAME);

		CTableFrame * pTableFrame=m_TableFrameArray[wTargerTableID];
		if (pTableFrame->PerformStandUpAction(pITargetUserItem)==false) return true;
	}

	return true;
}

//请求更换位置  TODONOW 切换桌子逻辑  WangChengQing
bool CAttemperEngineSink::OnTCPNetworkSubUserChairReq(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//用户状态
	if(pIServerUserItem->GetUserStatus() == US_PLAYING)
	{
		//失败
		m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem,TEXT("您正在游戏中，暂时不能离开，请先结束当前游戏！"),REQUEST_FAILURE_NORMAL);
		return true;
	}

	//查找桌子
	INT nStartIndex=rand()%MAX_TABLE;
	for (int i=0;i< MAX_TABLE; i++)
	{
		//定义变量
		INT nTableIndex=(i+nStartIndex)%MAX_TABLE;

		//过滤同桌
		if(nTableIndex == pIServerUserItem->GetTableID())continue;

		//获取桌子
		CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
		if ((pTableFrame->IsGameStarted()==true)/*&&(bDynamicJoin==false)*/) continue;
		if(pTableFrame->IsTableLocked()) continue;
		if(pTableFrame->GetChairCount()==pTableFrame->GetNullChairCount()) continue;

		//无效过滤
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID==INVALID_CHAIR) continue;

		//离开处理
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
			if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
		}

		//定义变量
		WORD wTagerDeskPos = pIServerUserItem->GetMobileUserDeskPos();
		WORD wTagerDeskCount = pIServerUserItem->GetMobileUserDeskCount();

		//更新信息
		if((nTableIndex < wTagerDeskPos) ||(nTableIndex > (wTagerDeskPos+wTagerDeskCount-1)))
		{
			pIServerUserItem->SetMobileUserDeskPos(nTableIndex/wTagerDeskCount);
		}

		//用户坐下
		pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);
		return true;
	}

	//查找桌子
	nStartIndex=rand()%MAX_TABLE;
	for (INT_PTR i=0;i<MAX_TABLE;i++)
	{
		//定义变量
		INT nTableIndex=(i+nStartIndex)%MAX_TABLE;

		//过滤同桌
		if(nTableIndex == pIServerUserItem->GetTableID())continue;

		//获取桌子
		CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
		if ((pTableFrame->IsGameStarted()==true)/*&&(bDynamicJoin==false)*/) continue;
		if(pTableFrame->IsTableLocked()) continue;

		//无效过滤
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID==INVALID_CHAIR) continue;

		//离开处理
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
			if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
		}

		//定义变量
		WORD wTagerDeskPos = pIServerUserItem->GetMobileUserDeskPos();
		WORD wTagerDeskCount = pIServerUserItem->GetMobileUserDeskCount();

		//更新信息
		if((nTableIndex < wTagerDeskPos) ||(nTableIndex > (wTagerDeskPos+wTagerDeskCount-1)))
		{
			pIServerUserItem->SetMobileUserDeskPos(nTableIndex/wTagerDeskCount);
		}

		//用户坐下
		pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);
		return true;
	}

	//查找同桌
	if(pIServerUserItem->GetTableID() != INVALID_TABLE)
	{
		//获取桌子
		CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
		if (pTableFrame->IsGameStarted()==false && pTableFrame->IsTableLocked()==false)
		{
			//无效过滤
			WORD wChairID=pTableFrame->GetRandNullChairID();
			if (wChairID!=INVALID_CHAIR)
			{
				//离开处理
				if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
					if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
				}

				//用户坐下
				pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);
				return true;
			}
		}
	}

	//失败
	m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem,TEXT("没找到可进入的游戏桌！"),REQUEST_FAILURE_NORMAL);
	return true;
}

//请求房间列表
bool CAttemperEngineSink::OnTCPNetworkSubGetTableList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize>=sizeof(CMD_GR_GetTableList));
	if (wDataSize<sizeof(CMD_GR_GetTableList)) return false;

	//处理消息
	CMD_GR_GetTableList * pGetTableList = (CMD_GR_GetTableList *)pData;

	CMD_GR_TableListResult pTableList;
	ZeroMemory(&pTableList,sizeof(CMD_GR_TableListResult));


	int iCount = 20;
	for(int i=0;i < m_TableFrameArray.GetCount(),i < iCount;i++)
	{
		CTableFrame* pTableFrame = m_TableFrameArray.GetAt(i);

		if( pTableFrame->GetTableMode() == pGetTableList->GameMode)
		{
			tagTableRule* pCfg = (tagTableRule*)pTableFrame->GetCustomRule();

			//不允许陌生人加入则不发送
			if(pCfg->bAllowStranger == 0)
				continue;

			if((pGetTableList->GoldRank == 0  && pCfg->CellScore <= 100)
				||(pGetTableList->GoldRank == 1  && pCfg->CellScore <= 500 && pCfg->CellScore > 100 )
				||(pGetTableList->GoldRank == 2  && pCfg->CellScore <= 5000 && pCfg->CellScore > 500 ))
			{
				pTableList.byCount++;
				pTableList.tableInfoList[i].byCurrPlayerCount =  static_cast<BYTE>(pTableFrame->GetSitUserCount());
				pTableList.tableInfoList[i].byMaxPlayerCount = pCfg->PlayerCount;
				pTableList.tableInfoList[i].dwJoinScore = pCfg->JoinScore;
				pTableList.tableInfoList[i].dwLeaveScore = pCfg->LeaveScore;

				//pTableList.tableInfoList[i].dwTablePassword = pCfg->KaiHuaGang;
				//pTableList.tableInfoList[i].QingYiSe = pCfg->QingYiSe;
				//pTableList.tableInfoList[i].QiXiaoDui = pCfg->QiXiaoDui;
				pTableList.tableInfoList[i].wCellScore = pCfg->CellScore;
				//pTableList.tableInfoList[i].XuanFengGang = pCfg->XuanFengGang;
				//pTableList.tableInfoList[i].ZhangMao = pCfg->ZhangMao;
			}
		}
	}

	if(pTableList.byCount > 0)
	{
		pTableList.GoldRank = pGetTableList->GoldRank;
		//发送房间列表
		SendData(dwSocketID,MDM_USER,SUB_GR_GET_TABLELIST_RESULT,&pTableList,sizeof(CMD_GR_TableListResult));	
	}

	return true;
}

//抽奖请求
bool CAttemperEngineSink::OnTcpNetworkQueryLottery(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	ASSERT(wDataSize==sizeof(CMD_GR_QueryLottery));
	if(wDataSize!=sizeof(CMD_GR_QueryLottery))
		return false;

	CMD_GR_QueryLottery* pGetRankReward = (CMD_GR_QueryLottery*)pData;

	DBR_GR_QueryLottery QueryLottery;
	memcpy(&QueryLottery,pGetRankReward,sizeof(DBR_GR_QueryLottery));

	return m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_GR_QUERY_LOTTERY,dwSocketID,&QueryLottery,sizeof(DBR_GR_QueryLottery));

}

/***************************************【主消息 3】*******************************************************/
//创建房间
bool CAttemperEngineSink::On_SUB_CG_USER_CREATE_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取用户
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);

	//用户校验
	if (pIServerUserItem==NULL) 
	{
		return false;
	}

	//校验数据包
	if(wDataSize != sizeof(STR_SUB_CG_USER_CREATE_ROOM))
	{
		SendRequestFailure(pIServerUserItem,TEXT("创建房间数据大小不匹配！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//房间规则
	STR_SUB_CG_USER_CREATE_ROOM *pCreateRoom = (STR_SUB_CG_USER_CREATE_ROOM *)pData;

	//转为桌子规则
	tagTableRule *pCfg = (tagTableRule*)pCreateRoom->CommonRule;
	if ( NULL == pCfg )
	{
		return false;
	}
	
	//桌子类型
	bool bRet = true;
	if(0 == pCfg->byClubCreate)//创建普通桌子
	{
		bRet = CreateTableNormal(pCfg, pIServerUserItem, pCreateRoom);
	}
	else if(1 == pCfg->byClubCreate || 2 == pCfg->byClubCreate)//创建牌友圈房间
	{
		bRet = CreateRoomClub(pCfg, pIServerUserItem, pCreateRoom);
	}
	else
	{
		TCHAR szString[512]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),
			TEXT("【创建房间】不存在该类型的桌子"), 
			pCfg->byClubCreate);
		CTraceService::TraceString(szString,TraceLevel_Info);
	}

	return bRet;
}

//创建桌子
bool CAttemperEngineSink::On_SUB_CG_USER_CREATE_TABLE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取用户
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);

	//用户校验
	if (pIServerUserItem==NULL) return false;

	//校验数据包
	if(wDataSize != sizeof(STR_SUB_CG_USER_CREATE_TABLE))
	{
		SendRequestFailure(pIServerUserItem,TEXT("创建桌子数据大小不匹配！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//结构体勾走
	STR_SUB_CG_USER_CREATE_TABLE *pCreateRoom = (STR_SUB_CG_USER_CREATE_TABLE *)pData;

	//投递请求
	return m_pIDBCorrespondManager->PostDataBaseRequest(pCreateRoom->dwUserID, DBR_CG_CLUB_CREATE_TABLE, dwSocketID, pData, wDataSize);
}

//创建普通桌子
bool CAttemperEngineSink::CreateTableNormal(tagTableRule * pCfg, IServerUserItem *pIServerUserItem, STR_SUB_CG_USER_CREATE_ROOM* pCreateRoom)
{
	//检查加入门票
	if(!CheckCreateTableTicket(pCfg, pIServerUserItem))
	{
		return true; //TODONOW 如果为false 客户端就断线重连了， 之后修改掉
	} 

	//用户效验
	if (INVALID_CHAIR != pIServerUserItem->GetChairID()) 
	{
		SendRequestFailure(pIServerUserItem, TEXT("正在游戏中,无法创建房间！"), REQUEST_FAILURE_NORMAL);
		return true;//TODONOW 如果为false 客户端就断线重连了， 之后修改掉
	}
	if(INVALID_TABLE != pIServerUserItem->GetTableID())
	{
		SendRequestFailure(pIServerUserItem, TEXT("正在房间中,无法创建房间！"), REQUEST_FAILURE_NORMAL);
		return true;//TODONOW 如果为false 客户端就断线重连了， 之后修改掉
	}

	//寻找空闲房间
	CTableFrame *pCurrTableFrame = GetNextEmptyTable();

	//桌子判断
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem,TEXT("服务器桌子已满,请稍后重试！"),REQUEST_FAILURE_NORMAL);
		return true ; //TODONOW 如果为false 客户端就断线重连了， 之后修改掉
	}

	//设置通用房间规则  
	pCurrTableFrame->SetCommonRule(pCfg);

	//设置子游戏房间规则
	tagTableSubGameRule *pSubGameCfg = (tagTableSubGameRule*)pCreateRoom->SubGameRule;
	if(pSubGameCfg != NULL)
	{
		pCurrTableFrame->SetSubGameRule(pSubGameCfg);
	}

	//替他人开房
	if(1 == pCfg->bCreateToOther)
	{	
		//TODO 替他人开房写数据库流程在这里处理
		HandleCreateTableForOthers(pCurrTableFrame, pIServerUserItem, pCfg);
	}
	else	//正常开房
	{
		//开房失败, 则return false
		if( !HandleCreateTable(pCurrTableFrame, pIServerUserItem, pCfg) )
		{
			return false;
		}
	}

	//发送房间消息
	STR_CMD_GC_USER_CREATE_ROOM_SUCCESS nCreate;
	nCreate.dwPassword = pCurrTableFrame->GetPassword();
	nCreate.wChairID = pIServerUserItem->GetChairID();
	nCreate.byAllGameCount = pCfg->GameCount;
	nCreate.byGameMode = pCfg->GameMode;
	nCreate.byZhuangType = pCfg->RobBankType;
	//nCreate.byMask = 0;

	//发送创建房间成功消息
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_ROOM_SUCCESS, &nCreate, sizeof(STR_CMD_GC_USER_CREATE_ROOM_SUCCESS));	

	return true;
}

//创建牌友圈房间
bool CAttemperEngineSink::CreateRoomClub(tagTableRule * pCfg, IServerUserItem *pIServerUserItem, STR_SUB_CG_USER_CREATE_ROOM* pCreateRoom)
{
	/* 第二步  校验用户 1.未在其他桌子中  2.未在椅子上 */
	/* added by WangChengQing 2018/5/11  
	** 牌友圈创建房间的时候, 因为创建者不需要进入桌子中, 所以不需要处理该判断
	//用户效验
	if (INVALID_CHAIR != pIServerUserItem->GetChairID()) 
	{
		SendRequestFailure(pIServerUserItem, TEXT("正在游戏中,无法创建房间！"), REQUEST_FAILURE_NORMAL);
		return false;
	}
	if(INVALID_TABLE != pIServerUserItem->GetTableID())
	{
		SendRequestFailure(pIServerUserItem, TEXT("正在房间中,无法创建房间！"), REQUEST_FAILURE_NORMAL);
		return false;
	}
	*/

	/* 第三步  寻找空闲桌子 */
	CTableFrame *pCurrTableFrame = GetNextEmptyTable();
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem,TEXT("服务器桌子已满,请稍后重试"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 第四步 处理创建桌子流程 */
	//生成桌子密码，房间号 = 前面 + TABLEID
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwPassword = GenerateTablePassword();

	//设置桌子属性
	//pCurTableFrame->SetTableOwner(pIServerUserItem->GetUserID());
	pCurrTableFrame->SetPassword(dwPassword);

	//设置房间自动解散，默认一分钟 -- 这里是指不开始游戏 自动一分钟后解散
	pCurrTableFrame->SetTableAutoDismiss();
	
	//获得空椅子
	WORD wChairID = pCurrTableFrame->GetNullChairID(); 

	/* 第五步 设置房间规则 */
	//设置通用房间规则  
	pCurrTableFrame->SetCommonRule(pCfg);

	//设置子游戏房间规则
	tagTableSubGameRule *pSubGameCfg = (tagTableSubGameRule*)pCreateRoom->SubGameRule;
	if(pSubGameCfg != NULL)
	{
		pCurrTableFrame->SetSubGameRule(pSubGameCfg);
	}

	/* 第六步 房间信息保存到数据库 */
	STR_DBR_CLUB_ROOM_INFO Dbr;
	Dbr.dwUserID = pCfg->dwUserID;
	Dbr.dwClubID = pCfg->dwClubID;
	Dbr.dwGameID = (pCfg->dwKindID) << 16;
	Dbr.byModeID = 0;

	if(1 == pCfg->byGoldOrFK)//俱乐部的房卡场 就是普通房卡场
	{
		Dbr.byModeID  = 0;
	}
	else if( 2 == pCfg->byGoldOrFK)//俱乐部的金币场 其实是 房卡金币场
	{
		Dbr.byModeID  = 3;
	}

	//Dbr.szKindName ; 数据库自己查询
	Dbr.dwServiceGold = pCfg->dwAmount;
	Dbr.dwRevenue = pCfg->dwOwnerPercentage;
	Dbr.byMask = pCfg->byMask;
	Dbr.dwDizhu = pCfg->dwDizhu;
	Dbr.dwGold = pCfg->dwLevelGold;
	Dbr.byAllRound = pCfg->GameCount;
	Dbr.byChairNum = pCfg->PlayerCount;
	Dbr.DissolveMinute = pCfg->bDissolve;
	memcpy(Dbr.szRealRoomRule, pCfg, 256);

	WriteClubRoomToDB(&Dbr);


	//发送房间消息
	STR_CMD_GC_USER_CREATE_ROOM_SUCCESS nCreate;

	//added by WangChengQing 2018/9/11
	//客户端会根据tableID(即password) 自动进入房间 -- 一键入局
	//因此此处 不返回tableID
	nCreate.dwPassword = 0;
	nCreate.wChairID = pIServerUserItem->GetChairID();
	nCreate.byAllGameCount = pCfg->GameCount;
	nCreate.byGameMode = pCfg->GameMode;
	nCreate.byZhuangType = pCfg->RobBankType;
	//nCreate.byMask = 0;

	//发送创建房间成功消息
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_ROOM_SUCCESS, &nCreate, sizeof(STR_CMD_GC_USER_CREATE_ROOM_SUCCESS));	

	//TODONOWW 需要发送给协调服, 然后协调服 发送给登录服.  登录服通知客户端实时刷新俱乐部房间
	STR_CMD_LC_CLUB_ROOM_RE RECMD;
	RECMD.dwClubID = pCfg->dwClubID;
	SendData(pIServerUserItem, MDM_CLUB, CMD_LC_CLUB_ROOM_RE, &RECMD, sizeof(STR_CMD_LC_CLUB_ROOM_RE));	
	
	//added by WangChengQing 2018/7/2   牌友圈创建房间完成之后, 应该断开与client的连接
	//added by WangChengQing 2018/8/31  此处由客户端主动断开socket连接
	return true; 
}

//创建牌友圈桌子
bool CAttemperEngineSink::CreateTableClub(STR_DBO_GC_CLUB_CREATE_TABLE * pDbo, IServerUserItem *pIServerUserItem)
{
	//用户效验
	if (INVALID_CHAIR != pIServerUserItem->GetChairID()) 
	{
		SendRequestFailure(pIServerUserItem, TEXT("椅子号不为空,无法创建牌友圈桌子"), REQUEST_FAILURE_NORMAL);
		return false;
	}
	if(INVALID_TABLE != pIServerUserItem->GetTableID())
	{
		SendRequestFailure(pIServerUserItem, TEXT("桌子号不为空,无法创建牌友圈桌子"), REQUEST_FAILURE_NORMAL);
		return false;
	}
	
	/* 第0步  判断公会房卡是否足够 */
	if(pDbo->lResultCode != 0)
	{
		SendRequestFailure(pIServerUserItem,TEXT("工会房卡不足, 请联系管理员！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	STR_SUB_CG_USER_CREATE_ROOM CreateRoom = pDbo->strCreateRoom;
	tagTableRule *pCfg = (tagTableRule*)CreateRoom.CommonRule;

	/* 第一步 寻找空闲房间 */
	CTableFrame *pCurrTableFrame = GetNextEmptyTable();

	//桌子判断
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem,TEXT("服务器已满,请稍后重试！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 第二步 生成桌子 */
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwPassword = GenerateTablePassword();

	//设置桌子属性
	//pCurTableFrame->SetTableOwner(pIServerUserItem->GetUserID());
	pCurrTableFrame->SetPassword(dwPassword);

	//设置房间自动解散，默认一分钟 -- 这里是指不开始游戏 自动一分钟后解散
	pCurrTableFrame->SetTableAutoDismiss();

	/* 桌创建子信息写入数据库 && 发送给客户端(在消息号的后续环节中发送) */
	DWORD dwClubRoomID = pDbo->dwClubRoomID;
	DWORD dwUserID = pIServerUserItem->GetUserID();
	DWORD dwTableID = pCurrTableFrame->GetPassword();
    DWORD dwLockState = (pDbo->byTableMode == 2) ? 0 : 1;
	ClubTableCreate(dwClubRoomID, dwUserID, dwTableID, dwLockState);

	/* 设置房间规则 */
	//设置通用房间规则  
	pCurrTableFrame->SetCommonRule(pCfg);

	//设置子游戏房间规则
	tagTableSubGameRule *pSubGameCfg = (tagTableSubGameRule*)(CreateRoom.SubGameRule);
	if(pSubGameCfg != NULL)
	{
		pCurrTableFrame->SetSubGameRule(pSubGameCfg);
	}

	/* 用户坐下 */
	WORD wChairID = pCurrTableFrame->GetNullChairID(); 
	//椅子号有效
	if (INVALID_CHAIR != wChairID)
	{
		//用户坐下		TODO 后面让客户端主动发送
		if(pCurrTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, 0, true)==false)
		{
			SendRequestFailure(pIServerUserItem, TEXT("用户坐下失败!"), REQUEST_FAILURE_NORMAL);
			return false;
		}		
	}
	else
	{
		SendRequestFailure(pIServerUserItem, TEXT("房间已满,无法加入房间!"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	
	/* 发送给client创建房间成功消息 */
	STR_CMD_GC_USER_CREATE_ROOM_SUCCESS nCreate;
	nCreate.dwPassword = pCurrTableFrame->GetPassword();
	nCreate.wChairID = pIServerUserItem->GetChairID();
	nCreate.byAllGameCount = pCfg->GameCount;
	nCreate.byGameMode = pCfg->GameMode;
	nCreate.byZhuangType = pCfg->RobBankType;

	//发送创建房间成功消息
	SendData(pIServerUserItem, MDM_USER, CMD_GC_CLUB_CREATE_TABKE, &nCreate, sizeof(STR_CMD_GC_USER_CREATE_ROOM_SUCCESS));	

	return true;
}

//创建桌子 金币大厅桌子
bool CAttemperEngineSink::CreateTableHallGold(STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD * pDbo, IServerUserItem *pIServerUserItem)
{
	//内部使用, 不校验指针
	tagTableRule *pCfg = (tagTableRule*)pDbo->strCreateRoom.CommonRule;


	//检查加入门票
	if(!CheckCreateTableTicket(pCfg, pIServerUserItem))
	{
		return true; //TODONOW 如果为false 客户端就断线重连了， 之后修改掉
	}

	/* 第一步 寻找空闲房间 */
	CTableFrame *pCurrTableFrame = GetNextEmptyTable();

	//桌子判断
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem,TEXT("服务器已满,请稍后重试！"),REQUEST_FAILURE_NORMAL);
		return true; //TODONOW 如果为false 客户端就断线重连了， 之后修改掉
	}

	/* 第二步 生成桌子 */
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwPassword = GenerateTablePassword();

	//设置桌子属性
	//pCurTableFrame->SetTableOwner(pIServerUserItem->GetUserID());
	pCurrTableFrame->SetPassword(dwPassword);

	//设置房间自动解散，默认一分钟 -- 这里是指不开始游戏 自动一分钟后解散
	pCurrTableFrame->SetTableAutoDismiss();

	/* 桌创建子信息写入数据库 && 发送给客户端(在消息号的后续环节中发送) */
	DWORD dwUserID = pIServerUserItem->GetUserID();
	DWORD dwKindID = pDbo->dwKindID;
	BYTE  byGameType = pDbo->byGameType;
	DWORD dwTableID = pCurrTableFrame->GetPassword();
	HallTableCreate(dwUserID, dwKindID, byGameType, dwTableID);

	/* 设置房间规则 */
	//设置通用房间规则  
	pCurrTableFrame->SetCommonRule(pCfg);

	//设置子游戏房间规则
	tagTableSubGameRule *pSubGameCfg = (tagTableSubGameRule*)(pDbo->strCreateRoom.SubGameRule);
	if(pSubGameCfg != NULL)
	{
		pCurrTableFrame->SetSubGameRule(pSubGameCfg);
	}

	/* 用户坐下 */
	WORD wChairID = pCurrTableFrame->GetNullChairID(); 
	//椅子号有效
	if (INVALID_CHAIR != wChairID)
	{
		//用户坐下		TODO 后面让客户端主动发送
		if(pCurrTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, 0, true)==false)
		{
			SendRequestFailure(pIServerUserItem, TEXT("用户坐下失败!"), REQUEST_FAILURE_NORMAL);
			return true; //TODONOW 如果为false 客户端就断线重连了， 之后修改掉
		}		
	}
	else
	{
		SendRequestFailure(pIServerUserItem, TEXT("房间已满,无法加入房间!"), REQUEST_FAILURE_NORMAL);
		return true ;//TODONOW 如果为false 客户端就断线重连了， 之后修改掉
	}
	
	/* 发送给client创建房间成功消息 
	TODONOW 因为客户端需要跳转场景, 创建成功并不会跳转, 所以此处发送加入成功的消息 added by WangChengQing
	*/
	STR_CMD_GC_USER_JOIN_ROOM_SUCCESS CMD;
	memcpy(&CMD.strTableRule, pDbo->strCreateRoom.CommonRule, sizeof(CMD.strTableRule));

	CMD.dwRoomID = pCurrTableFrame->GetPassword();
	CMD.wChairID = pIServerUserItem->GetChairID();
	CMD.byAllGameCount = pCfg->GameCount;
	CMD.byGameMode = pCfg->GameMode;
	CMD.byZhuangType = pCfg->RobBankType;
	CMD.wPlayerCount = pCfg->PlayerCount;

	//发送加入房卡房间成功
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, &CMD, sizeof(CMD));

	
	return true;
}


//创建桌子 俱乐部桌子
bool CAttemperEngineSink::CreateTableAutoClub(STR_DBO_CG_USER_JOIN_TABLE_NO_PASS * pDbo, IServerUserItem *pIServerUserItem)
{
	//内部使用, 不校验指针
	tagTableRule *pCfg = (tagTableRule*)pDbo->strCreateRoom.CommonRule;

	/* 第一步 寻找空闲房间 */
	CTableFrame *pCurrTableFrame = GetNextEmptyTable();

	//桌子判断
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem,TEXT("服务器已满,请稍后重试！"),REQUEST_FAILURE_NORMAL);
		return true; //TODONOW 如果为false 客户端就断线重连了， 之后修改掉
	}

	/* 第二步 生成桌子 */
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwPassword = GenerateTablePassword();

	//设置桌子属性
	//pCurTableFrame->SetTableOwner(pIServerUserItem->GetUserID());
	pCurrTableFrame->SetPassword(dwPassword);

	//设置房间自动解散，默认一分钟 -- 这里是指不开始游戏 自动一分钟后解散
	pCurrTableFrame->SetTableAutoDismiss();

	/* 桌创建子信息写入数据库 && 发送给客户端(在消息号的后续环节中发送) */
	DWORD dwClubRoomID = pDbo->dwClubRoomID;
	DWORD dwUserID = pIServerUserItem->GetUserID();
	DWORD dwTableID = pCurrTableFrame->GetPassword();
    DWORD dwLockState =  1; //默认为普通桌子(而不是私密桌子)
	ClubTableCreate(dwClubRoomID, dwUserID, dwTableID, dwLockState);


	/* 设置房间规则 */
	//设置通用房间规则  
	pCurrTableFrame->SetCommonRule(pCfg);

	//设置子游戏房间规则
	tagTableSubGameRule *pSubGameCfg = (tagTableSubGameRule*)(pDbo->strCreateRoom.SubGameRule);
	if(pSubGameCfg != NULL)
	{
		pCurrTableFrame->SetSubGameRule(pSubGameCfg);
	}

	/* 用户坐下 */
	WORD wChairID = pCurrTableFrame->GetNullChairID(); 
	//椅子号有效
	if (INVALID_CHAIR != wChairID)
	{
		//用户坐下		TODO 后面让客户端主动发送
		if(pCurrTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, 0, true)==false)
		{
			SendRequestFailure(pIServerUserItem, TEXT("用户坐下失败!"), REQUEST_FAILURE_NORMAL);
			return true; //TODONOW 如果为false 客户端就断线重连了， 之后修改掉
		}		
	}
	else
	{
		SendRequestFailure(pIServerUserItem, TEXT("房间已满,无法加入房间!"), REQUEST_FAILURE_NORMAL);
		return true; //TODONOW 如果为false 客户端就断线重连了， 之后修改掉
	}
	
	/* 发送给client创建房间成功消息 
	TODONOW 因为客户端需要跳转场景, 创建成功并不会跳转, 所以此处发送加入成功的消息 added by WangChengQing
	*/
	STR_CMD_GC_USER_JOIN_ROOM_SUCCESS CMD;
	memcpy(&CMD.strTableRule, pDbo->strCreateRoom.CommonRule, sizeof(CMD.strTableRule));

	CMD.dwRoomID = pCurrTableFrame->GetPassword();
	CMD.wChairID = pIServerUserItem->GetChairID();
	CMD.byAllGameCount = pCfg->GameCount;
	CMD.byGameMode = pCfg->GameMode;
	CMD.byZhuangType = pCfg->RobBankType;
	CMD.wPlayerCount = pCfg->PlayerCount;

	//发送加入房卡房间成功
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, &CMD, sizeof(CMD));

	
	return true;
}


//Club牌友圈房间信息 写入数据库
bool CAttemperEngineSink::WriteClubRoomToDB(STR_DBR_CLUB_ROOM_INFO* pTableInfo)
{
	//无效过滤
	if (pTableInfo==NULL) return false;

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR, DBR_CLUB_ROOM_INFO, 0, pTableInfo, sizeof(STR_DBR_CLUB_ROOM_INFO));

	return true;
}

//加入桌子 -- 需要密码
bool CAttemperEngineSink::On_SUB_User_JoinFkRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验用户
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);
	if (NULL == pIServerUserItem) return false;

	//校验数据包
	if(wDataSize != sizeof(STR_SUB_CG_USER_JOIN_FK_ROOM))
	{
		SendRequestFailure(pIServerUserItem,TEXT("加入牌友圈桌子数据错误"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//数据定义
	STR_SUB_CG_USER_JOIN_FK_ROOM *pJoin = (STR_SUB_CG_USER_JOIN_FK_ROOM *)pData;

	//向数据库查询
	STR_DBR_JOIN_ROOM DBR;
	DBR.dwTableID = pJoin->dwPassword;
	DBR.dwUserID = pIServerUserItem->GetUserID();

	//投递请求
	return m_pIDBCorrespondManager->PostDataBaseRequest(DBR.dwUserID, DBR_CG_JOIN_TABLE, dwSocketID, &DBR,sizeof(DBR));
}

//加入桌子 返回 -- 需要密码
bool CAttemperEngineSink::On_CMD_GC_JOIN_TABLE( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	/* 1. 校验用户 */
	WORD wBindIndex = LOWORD(dwContextID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);
	if (NULL == pIServerUserItem) return false;

	/* 2. 校验数据包 */
	if(wDataSize != sizeof(STR_DBR_JOIN_ROOM))
	{
		SendRequestFailure(pIServerUserItem,TEXT("加入房间数据错误！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 3. 数据库校验 */
	STR_DBR_JOIN_ROOM *pJoin = (STR_DBR_JOIN_ROOM *)pData;
	if(pJoin->lResultCode != 0)
	{
		SendRequestFailure(pIServerUserItem,TEXT("非工会成员, 不能进入工会桌子"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 4. 桌子校验 */
	DWORD dwPassword = pJoin->dwTableID;
	if(dwPassword == 0)
	{
		SendRequestFailure(pIServerUserItem, TEXT("房间号错误,请重新尝试"), REQUEST_FAILURE_PASSWORD);
		return false;
	}

	/* 5. 校验是否在之前桌子中 */	
	WORD wOldTableID = pIServerUserItem->GetTableID();	
	if(wOldTableID != INVALID_TABLE)
	{
		if(wOldTableID > m_TableFrameArray.GetCount())
		{
			wOldTableID = INVALID_TABLE;
		}
		else
		{
			CTableFrame* pOldTable = m_TableFrameArray.GetAt(wOldTableID);
			if(pOldTable == NULL || pOldTable->GetGameStatus() != GAME_STATUS_PLAY)
			{
				wOldTableID = INVALID_TABLE;
			}
		}	
	}

	//还在之前的房间中(这里应该不会走进来, 因为有断线重连的存在)
	if(INVALID_TABLE != wOldTableID)
	{
		SendRequestFailure(pIServerUserItem,TEXT("您已经在游戏中,不能进入其他房间!"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 6. 寻找指定桌子 */
	CTableFrame *pCurrTableFrame = GetDesignatedTable(dwPassword);
	//桌子判断
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem, TEXT("桌子不存在"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 7. 房间规则校验 */
	tagTableRule *pCfg = (tagTableRule*)pCurrTableFrame->GetCustomRule();
	if (NULL == pCfg)
	{
		SendRequestFailure(pIServerUserItem, TEXT("加入失败, 房间规则不存在"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 8. 桌子游戏状态 判断 */
	if(GAME_STATUS_FREE != pCurrTableFrame->GetGameStatus())
	{
		SendRequestFailure(pIServerUserItem, TEXT("房间正在游戏中,无法加入"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 9. 门票校验 */
	if(!CheckJoinTableTicket(pCfg, pIServerUserItem))
	{
		return false;
	}

	/* 10. 玩家坐下判断 */
	WORD wChairID = pCurrTableFrame->GetNullChairID();//寻找空椅子
	if (wChairID != INVALID_CHAIR)
	{
		if(pCurrTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, 0, true)==false)
		{
			SendRequestFailure(pIServerUserItem,TEXT("加入房间失败, 坐下失败"),REQUEST_FAILURE_PASSWORD);
			return false;
		}	
	}
	else
	{
		SendRequestFailure(pIServerUserItem,TEXT("房间已满,请更换其他房间"),REQUEST_FAILURE_NORMAL);
		return false;
	}
	
	/* 11. 发送给client */
	STR_CMD_GC_USER_JOIN_ROOM_SUCCESS CMD;
	memcpy(&CMD.strTableRule, pCfg, sizeof(CMD.strTableRule));
	CMD.dwRoomID = dwPassword;
	CMD.wChairID = wChairID;
	CMD.byAllGameCount = pCfg->GameCount;
	CMD.byGameMode = pCfg->GameMode;
	CMD.byZhuangType = pCfg->RobBankType;
	CMD.wPlayerCount = pCfg->PlayerCount;

	//发送加入房卡房间成功
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, &CMD, sizeof(CMD));

	return true;
}

//加入桌子 -- 不需要密码, 即快速开始
bool CAttemperEngineSink::On_SUB_CG_USER_JOIN_TABLE_NO_PASS(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验用户
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);
	if (NULL == pIServerUserItem) return false;

	//校验数据包
	if(wDataSize != sizeof(STR_SUB_CG_USER_JOIN_TABLE_NO_PASS))
	{
		SendRequestFailure(pIServerUserItem,TEXT("加入房间 结构体数据错误！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//数据定义
	STR_SUB_CG_USER_JOIN_TABLE_NO_PASS *pJoin = (STR_SUB_CG_USER_JOIN_TABLE_NO_PASS *)pData;

	//投递请求
	return m_pIDBCorrespondManager->PostDataBaseRequest(pJoin->dwUserID, DBR_CG_USER_JOIN_TABLE_NO_PASS, dwSocketID, pData,wDataSize);
}

//加入桌子 -- 不需要密码, 即快速开始
bool CAttemperEngineSink::On_CMD_GC_USER_JOIN_TABLE_NO_PASS( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	/* 1. 校验用户 */
	WORD wBindIndex = LOWORD(dwContextID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);
	if (NULL == pIServerUserItem) return false;

	/* 2. 校验数据包 */
	if(wDataSize != sizeof(STR_DBO_CG_USER_JOIN_TABLE_NO_PASS))
	{
		SendRequestFailure(pIServerUserItem,TEXT("加入房间数据错误！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 3. 数据库校验 */

	STR_DBO_CG_USER_JOIN_TABLE_NO_PASS *pJoin = (STR_DBO_CG_USER_JOIN_TABLE_NO_PASS *)pData;
	if( pJoin->lResultCode != DB_SUCCESS)//返回值不为0, 则认为身上金币不足 或者 不是本公司的人
	{
		SendRequestFailure(pIServerUserItem,TEXT("未找到合适房间, 请稍后重试"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	if( pJoin->lResultCode2 != DB_SUCCESS)//返回值不为0, 则工会房卡不足
	{
		SendRequestFailure(pIServerUserItem,TEXT("工会房卡不足, 请联系管理员充值"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	if( pJoin->dwPassword == 0)//桌子号为空, 则认为没有找到对应的桌子
	{
		//如果没有找到俱乐部的桌子, 则直接创建一个桌子
		return CreateTableAutoClub(pJoin ,  pIServerUserItem);
	}


	/* 4. 桌子校验 */
	DWORD dwPassword = pJoin->dwPassword;
	if(dwPassword == 0)
	{
		SendRequestFailure(pIServerUserItem, TEXT("桌子号错误,请重新尝试"), REQUEST_FAILURE_PASSWORD);
		return false;
	}

	/* 5. 校验是否在之前的游戏中 */
	WORD wOldTableID = pIServerUserItem->GetTableID(); //旧桌子号	
	if(wOldTableID != INVALID_TABLE)
	{
		if(wOldTableID > m_TableFrameArray.GetCount())
		{
			wOldTableID = INVALID_TABLE;
		}
		else
		{
			CTableFrame* pOldTable = m_TableFrameArray.GetAt(wOldTableID);
			if(pOldTable == NULL || pOldTable->GetGameStatus() != GAME_STATUS_PLAY)
			{
				wOldTableID = INVALID_TABLE;
			}
		}	
	}

	if (INVALID_TABLE != wOldTableID)
	{
		SendRequestFailure(pIServerUserItem,TEXT("您已经在游戏中,不能进入其他房间"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 6. 寻找指定桌子 */
	CTableFrame *pCurrTableFrame = GetDesignatedTable(dwPassword);
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem, TEXT("加入失败, 桌子不存在"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 7. 房间规则 */
	tagTableRule * pCfg = (tagTableRule*)pCurrTableFrame->GetCustomRule();
	if (NULL == pCfg)
	{
		SendRequestFailure(pIServerUserItem, TEXT("加入失败, 房间规则不存在!"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 8. 判断桌子是否在游戏中 */
	if(GAME_STATUS_FREE != pCurrTableFrame->GetGameStatus())
	{
		SendRequestFailure(pIServerUserItem, TEXT("房间正在游戏中,无法加入！"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 9. 门票校验 */
	if(!CheckJoinTableTicket(pCfg, pIServerUserItem))
	{
		return false;
	}

	/* 10. 用户坐下 */
	WORD wChairID = pCurrTableFrame->GetNullChairID();//寻找空椅子
	if (wChairID != INVALID_CHAIR)
	{
		if(pCurrTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, 0, true)==false)
		{
			SendRequestFailure(pIServerUserItem,TEXT("加入失败, 坐下失败!"),REQUEST_FAILURE_PASSWORD);
			return false;
		}
	}
	else
	{
		SendRequestFailure(pIServerUserItem,TEXT("房间已满,请更换其他房间!"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 11. 发送加入房间成功消息给客户端 */
	STR_CMD_GC_USER_JOIN_ROOM_SUCCESS nJoin;
	ZeroMemory(&nJoin, sizeof(STR_CMD_GC_USER_JOIN_ROOM_SUCCESS));
	memcpy(&nJoin.strTableRule, pCfg, sizeof(nJoin.strTableRule));
	//赋值
	nJoin.dwRoomID = dwPassword;
	nJoin.wChairID = wChairID;
	nJoin.byAllGameCount = pCfg->GameCount;
	nJoin.byGameMode = pCfg->GameMode;
	nJoin.byZhuangType = pCfg->RobBankType;
	nJoin.wPlayerCount = pCfg->PlayerCount;

	//发送加入房卡房间成功
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, &nJoin, sizeof(STR_CMD_GC_USER_JOIN_ROOM_SUCCESS));

	return true;
}

//加入桌子 -- 金币大厅桌子
bool CAttemperEngineSink::On_SUB_CG_USER_JOIN_GOLD_HALL_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验用户
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);
	if (NULL == pIServerUserItem) return true;

	//校验数据包
	if(wDataSize != sizeof(STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM))
	{
		SendRequestFailure(pIServerUserItem,TEXT("加入房间 结构体数据错误！"),REQUEST_FAILURE_NORMAL);
		return true;
	}

	//数据定义
	STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM *pJoin = (STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM *)pData;

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pJoin->dwUserID, DBR_CG_USER_JOIN_TABLE_HALL_GOLD, dwSocketID, pData,wDataSize);

	return true;
}

//加入桌子 -- 金币大厅桌子 返回
bool CAttemperEngineSink::On_CMD_GC_USER_JOIN_TABLE_HALL_GOLD( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	/* 1. 校验用户 */
	WORD wBindIndex = LOWORD(dwContextID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);
	if (NULL == pIServerUserItem) return false;

	/* 2. 校验数据包 */
	if(wDataSize != sizeof(STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD))
	{
		SendRequestFailure(pIServerUserItem,TEXT("加入房间数据错误！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 3. 数据库校验 */
	STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD *pJoin = (STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD *)pData;

	if( pJoin->lResultCode != 0)//返回值不为0, 则认为身上金币不足 或者 不是本公司的人
	{
		SendRequestFailure(pIServerUserItem,TEXT("身上金币不足, 无法加入"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	if( pJoin->dwPassword == 0)//桌子号为空, 则认为没有找到对应的桌子
	{
		//如果没有找到金币大厅的桌子, 则直接创建一个金币大厅的桌子
		return CreateTableHallGold(pJoin ,  pIServerUserItem);
	}

	/* 4. 桌子校验 */
	DWORD dwPassword = pJoin->dwPassword;
	if(dwPassword == 0)
	{
		SendRequestFailure(pIServerUserItem, TEXT("桌子号错误,请重新尝试"), REQUEST_FAILURE_PASSWORD);
		return false;
	}

	/* 5. 校验是否在之前的游戏中 */
	WORD wOldTableID = pIServerUserItem->GetTableID(); //旧桌子号	
	if(wOldTableID != INVALID_TABLE)
	{
		if(wOldTableID > m_TableFrameArray.GetCount())
		{
			wOldTableID = INVALID_TABLE;
		}
		else
		{
			CTableFrame* pOldTable = m_TableFrameArray.GetAt(wOldTableID);
			if(pOldTable == NULL || pOldTable->GetGameStatus() != GAME_STATUS_PLAY)
			{
				wOldTableID = INVALID_TABLE;
			}
		}	
	}

	if (INVALID_TABLE != wOldTableID)
	{
		SendRequestFailure(pIServerUserItem,TEXT("您已经在游戏中,不能进入其他房间"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 6. 寻找指定桌子 */
	CTableFrame *pCurrTableFrame = GetDesignatedTable(dwPassword);
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem, TEXT("加入失败, 桌子不存在"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 7. 房间规则 */
	tagTableRule * pCfg = (tagTableRule*)pCurrTableFrame->GetCustomRule();
	if (NULL == pCfg)
	{
		SendRequestFailure(pIServerUserItem, TEXT("加入失败, 房间规则不存在!"), REQUEST_FAILURE_NORMAL);
		return false;
	}
	//构造提示 TODONOW
	TCHAR szString[512]=TEXT("");
	_sntprintf_s(szString,CountArray(szString),TEXT("加入 房间号: %ld, 进场金币：%ld, 局数: %d"), 
		dwPassword,
		pCfg->dwLevelGold,
		pCfg->GameCount
		);
	CTraceService::TraceString(szString,TraceLevel_Normal);

	/* 8. 判断桌子是否在游戏中 */
	if(GAME_STATUS_FREE != pCurrTableFrame->GetGameStatus())
	{
		SendRequestFailure(pIServerUserItem, TEXT("房间正在游戏中,无法加入！"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 9. 门票校验 */
	if(!CheckJoinTableTicket(pCfg, pIServerUserItem))
	{
		return false;
	}

	/* 10. 用户坐下 */
	WORD wChairID = pCurrTableFrame->GetNullChairID();//寻找空椅子
	if (wChairID != INVALID_CHAIR)
	{
		if(pCurrTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, 0, true)==false)
		{
			SendRequestFailure(pIServerUserItem,TEXT("加入失败, 坐下失败!"),REQUEST_FAILURE_PASSWORD);
			return false;
		}
	}
	else
	{
		SendRequestFailure(pIServerUserItem,TEXT("房间已满,请更换其他房间!"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 11. 发送加入房间成功消息给客户端 */
	STR_CMD_GC_USER_JOIN_ROOM_SUCCESS nJoin;
	ZeroMemory(&nJoin, sizeof(STR_CMD_GC_USER_JOIN_ROOM_SUCCESS));
	memcpy(&nJoin.strTableRule, pCfg, sizeof(nJoin.strTableRule));
	//赋值
	nJoin.dwRoomID = dwPassword;
	nJoin.wChairID = wChairID;
	nJoin.byAllGameCount = pCfg->GameCount;
	nJoin.byGameMode = pCfg->GameMode;
	nJoin.byZhuangType = pCfg->RobBankType;
	nJoin.wPlayerCount = pCfg->PlayerCount;

	//发送加入房间成功
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, &nJoin, sizeof(STR_CMD_GC_USER_JOIN_ROOM_SUCCESS));
	return true;
}

//断线重连
bool CAttemperEngineSink::On_SUB_User_ReconnectRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{	
	//获校验户
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);
	if (NULL == pIServerUserItem)
	{
		return true;
	}

	//校验数据包
	if ( wDataSize != sizeof(STR_SUB_CG_USER_RECONNECT_ROOM))
	{
		return true;
	}

	//校验用户桌子号和椅子号
	WORD wChairID = pIServerUserItem->GetChairID();
	WORD wOldTableID = pIServerUserItem->GetTableID();

	//判断历史桌子是否还在游戏
	if(wOldTableID != INVALID_TABLE)
	{
		if(wOldTableID > m_TableFrameArray.GetCount()) //无效桌子
		{
			wOldTableID = INVALID_TABLE;
		}
		else //桌子已经结束, 则将wOldTableID设置为无效
		{
			CTableFrame* pOldTable = m_TableFrameArray.GetAt(wOldTableID);
			if(pOldTable == NULL)
			{
				wOldTableID = INVALID_TABLE;
			}
		}	
	}

	//椅子号或桌子号无效, 直接退出
	if (INVALID_TABLE == wOldTableID || INVALID_CHAIR == wChairID)
	{
		return true;
	}

	//构造断线重连返回数据
	STR_CMD_GC_USER_RECONNECT_ROOM ReJoinResult;
	ZeroMemory(&ReJoinResult, sizeof(STR_CMD_GC_USER_RECONNECT_ROOM));

	//用户坐下
	if( m_TableFrameArray[wOldTableID]->PerformSitDownAction(wChairID, pIServerUserItem))
	{
		BYTE OldGameStatus = pIServerUserItem->GetOldGameStatus();

		if(OldGameStatus == US_READY)
		{
			pIServerUserItem->SetUserStatus(US_SIT, wOldTableID, wChairID);
		}
		else
		{
			pIServerUserItem->SetUserStatus(OldGameStatus, wOldTableID, wChairID);	
		}

		//发送房间消息
		tagTableRule* pRule = (tagTableRule* )m_TableFrameArray[wOldTableID]->GetCustomRule();
		ReJoinResult.retCode = 0;
		memcpy(&ReJoinResult.strTableRule, pRule, sizeof(ReJoinResult.strTableRule));
		ReJoinResult.wChairID = wChairID;
		ReJoinResult.dwPassWord = m_TableFrameArray[wOldTableID]->GetPassword();
		ReJoinResult.cbGameCount = pRule->GameCount;
		ReJoinResult.cbPlayerCount = pRule->PlayerCount;
		ReJoinResult.cbPayType = pRule->cbPayType;
		ReJoinResult.lSinglePayCost = pRule->lSinglePayCost;
		ReJoinResult.gameMod = pRule->GameMode;
	}
	else
	{
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),
			TEXT("【断线重连】【%ld重连失败】玩家坐下失败"),
			pIServerUserItem->GetUserID());
		CTraceService::TraceString(szDescribe,TraceLevel_Warning);

		return true;
	}

	//发送数据
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_RECONNECT_ROOM, &ReJoinResult, sizeof(STR_CMD_GC_USER_RECONNECT_ROOM));

	//断线重连成功，删除list
	if (ReJoinResult.retCode == 0)
	{
		//游戏服处理  TODONOW
		pIServerUserItem->SetOfflineGameID(0);

		//发送给协调服, 再转发给登录服
		tagOfflineUser data;
		data.dwUserID = pIServerUserItem->GetUserID();
		//data.dwGameID = m_pGameServiceAttrib->dwGameID; 删除的时候不需要改字段
		data.byMask = 2; //表示删除断线用户
		m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_OFFLINE,&data,sizeof(tagOfflineUser));
	}
	return true;
}

//创建牌友圈房间
bool CAttemperEngineSink::On_Sub_CG_User_CreateGroupRoom(VOID *pData, WORD wDataSize, DWORD dwSocketID)
{	
	//用户校验
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);	
	if (pIServerUserItem==NULL) return true;

	//数据包校验
	if(wDataSize != sizeof(STR_SUB_CG_USER_CREATE_GROUP_ROOM))
		return true;

	//SUB数据
	STR_SUB_CG_USER_CREATE_GROUP_ROOM *pCreateRoom = (STR_SUB_CG_USER_CREATE_GROUP_ROOM*)pData;

	//校验圈主是不是已经创建过牌友圈房间，创建过且房间都未坐满人，就不能再次创建房间
	for (WORD i=0; i<m_TableFrameArray.GetCount(); i++)
	{
		//获取对象
		CTableFrame *pGroupTable = m_TableFrameArray[i];

		//是牌友圈房间，并且有一个桌子未开始游戏，则说明还有空桌子加入，不需要再创建房间
		if ( (pGroupTable->GetGroupID() == pCreateRoom->dwGroupID) && 
			 (!pGroupTable->IsGameStarted()) )
		{
			//返回创建成功
			STR_CMD_GC_USER_CREATE_GROUP_ROOM CMDCreate;
			ZeroMemory(&CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));
			CMDCreate.dwResultCode = DB_SUCCESS;
			lstrcpyn(CMDCreate.szDescribeString, TEXT("牌友圈还有空房间"), sizeof(CMDCreate.szDescribeString));

			//发送数据
			SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_GROUP_ROOM, &CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));	

			return true;
		}
	}

	//构造DBR数据
	STR_DBR_CG_USER_CREATE_GROUP_ROOM CreateGroupRoom;
	ZeroMemory(&CreateGroupRoom, sizeof(STR_DBR_CG_USER_CREATE_GROUP_ROOM));

	//赋值
	memcpy_s(&CreateGroupRoom, sizeof(STR_DBR_CG_USER_CREATE_GROUP_ROOM), pCreateRoom, sizeof(STR_DBR_CG_USER_CREATE_GROUP_ROOM));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(CreateGroupRoom.dwUserID, DBR_CG_USER_CREATE_GROUP_ROOM, dwSocketID, &CreateGroupRoom, sizeof(STR_DBR_CG_USER_CREATE_GROUP_ROOM));
	
	return true;
}

//创建牌友圈房间返回
bool CAttemperEngineSink::On_CMD_GC_User_CreateGroupRoom(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	if ((m_pNormalParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//数据校验
	ASSERT(wDataSize == sizeof(STR_DBO_CG_USER_CREATE_GROUP_ROOM));
	if(wDataSize != sizeof(STR_DBO_CG_USER_CREATE_GROUP_ROOM))
		return false;

	//获取用户
	WORD wBindIndex = LOWORD(dwContextID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);

	//定义
	STR_DBO_CG_USER_CREATE_GROUP_ROOM *pDBOGroup = (STR_DBO_CG_USER_CREATE_GROUP_ROOM *)pData;

	//构造返回数据
	STR_CMD_GC_USER_CREATE_GROUP_ROOM CMDCreate;
	ZeroMemory(&CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));

	//赋值
	CMDCreate.dwResultCode = pDBOGroup->dwResultCode;
	lstrcpyn(CMDCreate.szDescribeString, pDBOGroup->szDescribeString, CountArray(CMDCreate.szDescribeString));
	
	//执行失败
	if (DB_SUCCESS != pDBOGroup->dwResultCode )
	{
		SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_GROUP_ROOM, &CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));	
		return true;
	}

	//房间规则
	tagTableRule *pCfg = (tagTableRule* )pDBOGroup->rule;

	//校验用户房卡
	if ( pIServerUserItem->GetUserRoomCard() < pCfg->lSinglePayCost)
	{
		CMDCreate.dwResultCode = 1;
		lstrcpyn(CMDCreate.szDescribeString, TEXT("房卡不足，请充值"), CountArray(CMDCreate.szDescribeString));

		SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_GROUP_ROOM, &CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));	
		return true;
	}

	//用户最多能创建的牌友圈房间数
	WORD wCreateTableNum = static_cast<WORD> (pIServerUserItem->GetUserRoomCard() / pCfg->lSinglePayCost);
	
	//寻找空闲桌子，为牌友圈分配固定的桌子数
	srand(static_cast<unsigned int >(time(NULL)));
	int nTableNum = 0;
	for (WORD i=0; i<m_TableFrameArray.GetCount(); i++)
	{
		//获取对象
		CTableFrame *pGroupTable = m_TableFrameArray[i];

		//桌子无人坐下，并且不是替别人开的房间
		if ( (pGroupTable->GetSitUserCount()==0) && 
			 (pGroupTable->GetCreateTableUser() == 0))
		{
			//牌友圈桌子数限制
			if ( (nTableNum >= m_TableFrameArray.GetCount()) || 
				 (NUM_MAX_GOURP_CREATE_ROOM == nTableNum) ||
				 nTableNum >= wCreateTableNum)
			{
				break;
			}

			//设置桌子属性	
			DWORD dwPassword = GenerateTablePassword();		
			pGroupTable->SetPassword(dwPassword);
			pGroupTable->SetGroupID(pDBOGroup->dwGroupID);			
			pGroupTable->SetCommonRule(pCfg);
			pGroupTable->SetCreateTableUser(pIServerUserItem->GetUserID());		//设置创建玩家ID

			nTableNum++;

			//TODO 需要添加开房信息
		}
	}

	//发送创建成功消息
	_sntprintf_s(CMDCreate.szDescribeString, CountArray(CMDCreate.szDescribeString), TEXT("您已经创建[%d]个牌友圈房间，扣除房卡[%ld]"), 
		      nTableNum, nTableNum*pCfg->lSinglePayCost);
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_GROUP_ROOM, &CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));	

	//扣除创建用户房卡
	SCORE lDeductRoomCard = (-nTableNum)*((int)pCfg->lSinglePayCost);
	
	//
	//OnEventUserOpenRoomCard(pIServerUserItem, pIServerUserItem->GetUserID(), lDeductRoomCard);

	return true;
}

//加入牌友圈房间
bool CAttemperEngineSink::On_Sub_CG_User_JoinGroupRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize == sizeof(STR_SUB_CG_USER_JOIN_GROUP_ROOM));
	if (wDataSize != sizeof(STR_SUB_CG_USER_JOIN_GROUP_ROOM)) return true;

	//处理消息
	STR_SUB_CG_USER_JOIN_GROUP_ROOM *pJoinGroupRoom = (STR_SUB_CG_USER_JOIN_GROUP_ROOM *)pData;

	//绑定信息
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	//校验用户
	if ( (NULL == pBindParameter) || (NULL == pIServerUserItem) )
		return true;

	//校验用户是不是在游戏中
	WORD wOldTableID = pIServerUserItem->GetTableID();
	WORD wOldChairID = pIServerUserItem->GetChairID();
	BYTE cbUserStatus = pIServerUserItem->GetUserInfo()->cbUserStatus;

	//用户在游戏中
	if(wOldTableID != INVALID_TABLE || 
		wOldChairID != INVALID_CHAIR ||
		cbUserStatus == US_PLAYING )
	{
		SendRequestFailure(pIServerUserItem, TEXT("您已经在游戏中,不能进入其他房间!"), REQUEST_FAILURE_NORMAL);
		return true;
	}

	//构造DBR数据
	STR_DBR_CG_USER_JOIN_GROUP_ROOM JoinRoom;
	ZeroMemory(&JoinRoom, sizeof(STR_DBR_CG_USER_JOIN_GROUP_ROOM));

	//赋值
	memcpy_s(&JoinRoom, sizeof(STR_DBR_CG_USER_JOIN_GROUP_ROOM), pJoinGroupRoom, sizeof(STR_DBR_CG_USER_JOIN_GROUP_ROOM));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(JoinRoom.dwUserID, DBR_CG_USER_JOIN_GROUP_ROOM, dwSocketID, &JoinRoom, sizeof(STR_DBR_CG_USER_JOIN_GROUP_ROOM));

	return true;
}

//加入牌友圈房间返回
bool CAttemperEngineSink::On_CMD_GC_User_JoinGroupRoom(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	if ((m_pNormalParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//数据校验
	ASSERT(wDataSize == sizeof(STR_DBO_CG_USER_JOIN_GROUP_ROOM));
	if(wDataSize != sizeof(STR_DBO_CG_USER_JOIN_GROUP_ROOM))
		return false;

	//获取用户
	WORD wBindIndex = LOWORD(dwContextID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);
	if ( NULL == pIServerUserItem )
	{
		return false;
	}

	//定义
	STR_DBO_CG_USER_JOIN_GROUP_ROOM *pDBOGroup = (STR_DBO_CG_USER_JOIN_GROUP_ROOM *)pData;

	//失败返回
	if ( DB_SUCCESS != pDBOGroup->dwResultCode )
	{
		SendRequestFailure(pIServerUserItem, pDBOGroup->szDescribeString, pDBOGroup->dwResultCode);
		return true;
	}

	//判断该圈子的房间是否有空椅子的桌子
	bool bHaveRoom = false;
	for(int i= 0;i < m_TableFrameArray.GetCount();i++)
	{
		CTableFrame *pTableFrame = m_TableFrameArray.GetAt(i);
		//校验桌子
		if( (NULL != pTableFrame) && 
			(pTableFrame->GetGroupID() == pDBOGroup->dwGroupID) && 
			(GAME_STATUS_FREE == pTableFrame->GetGameStatus()) && 
			(0 != pTableFrame->GetPassword()) )
		{
			//找到房间，判断是否有空位置
			WORD wChairID = pTableFrame->GetNullChairID();
			if(wChairID != INVALID_CHAIR)
			{
				//用户坐下
				if(pTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, 0, true)==false)
				{
					SendRequestFailure(pIServerUserItem, TEXT("房间号不对!"), REQUEST_FAILURE_PASSWORD);
					return true;
				}	

				//用户可以加入牌友圈房间
				bHaveRoom = true;

				//获取房间规则
				tagTableRule *pSelfOption = (tagTableRule* )pTableFrame->GetCustomRule();

				//构造加入成功消息
				STR_CMD_GC_USER_JOIN_ROOM_SUCCESS nJoin;
				ZeroMemory(&nJoin, sizeof(STR_CMD_GC_USER_JOIN_ROOM_SUCCESS));
				memcpy(&nJoin.strTableRule, pSelfOption, sizeof(nJoin.strTableRule));
				//赋值
				nJoin.dwRoomID = pTableFrame->GetPassword();
				nJoin.wChairID = wChairID;
				nJoin.byAllGameCount = pSelfOption->GameCount;
				nJoin.byGameMode = pSelfOption->GameMode;
				nJoin.byZhuangType = pSelfOption->RobBankType;
				nJoin.wPlayerCount = pSelfOption->PlayerCount;

				//发送加入成功消息
				SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, &nJoin, sizeof(STR_CMD_GC_USER_JOIN_ROOM_SUCCESS));
				break;
			}
		}
	}

	//用户无法加入，发送错误提示
	if ( !bHaveRoom )
	{
		SendRequestFailure(pIServerUserItem, TEXT("牌友圈房间已满或不存在已开房间，请联系管理"), REQUEST_FAILURE_PASSWORD);
	}

	return true;
}

//加入金币房间
bool CAttemperEngineSink::On_SUB_User_JoinGoldRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//用户效验
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);		
	if (NULL == pIServerUserItem) return true;

	//数据包校验
	if ( wDataSize != sizeof(CMD_GR_Create_Gold_Room) )
		return false;

	//数据定义
	CMD_GR_Create_Gold_Room *pGoldRoom = (CMD_GR_Create_Gold_Room*)pData;

	//校验用户桌子和椅子
	if (pIServerUserItem->GetChairID()!=INVALID_CHAIR) 
	{
		SendRequestFailure(pIServerUserItem,TEXT("正在游戏中,无法加入！"),REQUEST_FAILURE_NORMAL);
		return true;
	}
	if(pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		SendRequestFailure(pIServerUserItem,TEXT("正在房间中,无法加入！"),REQUEST_FAILURE_NORMAL);
		return true;
	}
	//校验用户金币	Modified by lizhihu 只和游戏底分校验
	if( pIServerUserItem->GetUserGold() < pGoldRoom->dwCellScore )
	{
		SendRequestFailure(pIServerUserItem,TEXT("金币不足,无法加入！"),REQUEST_FAILURE_NORMAL);
		return true;
	}

	//寻找金币房空椅子
	WORD wChairID;
	CTableFrame *pTableFrame = GetGlodRoomEmptyChair(wChairID);

	//找到金币房空椅子
	if ( (INVALID_CHAIR != wChairID) && (NULL != pTableFrame) )
	{
		//已有空椅子，用户坐下
		if(pTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, 0, true))
		{
			//获得金币房间规则
			tagTableRule *pTableCfg = (tagTableRule*)pTableFrame->GetCustomRule();
			if (NULL != pTableCfg)
			{
				//返回加入成功
				STR_CMD_GC_USER_JOIN_ROOM_SUCCESS JoinSuccess;
				ZeroMemory(&JoinSuccess, sizeof(JoinSuccess));
				memcpy(&JoinSuccess.strTableRule, pTableCfg, sizeof(JoinSuccess.strTableRule));

				//赋值
				JoinSuccess.byGameMode = TABLE_MODE_GOLD;
				JoinSuccess.wChairID = wChairID;
				JoinSuccess.dwRoomID = pTableFrame->GetPassword();
				JoinSuccess.byAllGameCount = pTableCfg->GameCount;
				JoinSuccess.wPlayerCount = pTableCfg->PlayerCount;
				JoinSuccess.byZhuangType = pTableCfg->RobBankType;

				//发送数据
				SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, &JoinSuccess ,sizeof(STR_CMD_GC_USER_JOIN_ROOM_SUCCESS));
				return true;
			}
		}	
	}

	//没有金币场空位置，重新创建一个金币房间
	CTableFrame *pCurrTableFrame = GetNextEmptyTable();

	//桌子校验
	if(pCurrTableFrame == NULL)
	{
		SendRequestFailure(pIServerUserItem, TEXT("服务器已满,请稍后重试！"), REQUEST_FAILURE_NORMAL);
		return true;
	}

	//配置金币房规则	写在这里没有通用性，最好的是在客户端配置，规则由每个客户端的子游戏决定，传过来
	tagTableRule cfg;
	ZeroMemory(&cfg, sizeof(tagTableRule));

	cfg.GameMode = TABLE_MODE_GOLD;
	cfg.GameCount = 0;
	cfg.PlayerCount = 4;
	cfg.RobBankType = 3;
	cfg.CellScore = pGoldRoom->dwCellScore;
	cfg.JoinScore = 500;
	cfg.LeaveScore = 500;
	cfg.MaxFan = 4;
	/* 注释掉子游戏特有数据
	cfg.ZhangMao = 1;					//长毛
	cfg.XuanFengGang = 1;				//旋风杠
	cfg.QiXiaoDui = 1;					//七小对
	cfg.QingYiSe = 1;					//清一色
	cfg.KaiHuaGang = 1;					//杠开
	*/

	//pCurrTableFrame->SetTableOwner(pIServerUserItem->GetUserID());
	pCurrTableFrame->SetCommonRule(&cfg);

	//寻找空椅子
	wChairID = pCurrTableFrame->GetNullChairID();
	if (wChairID != INVALID_CHAIR)
	{
		//用户坐下
		if(!pCurrTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, 0, true))
		{
			SendRequestFailure(pIServerUserItem,TEXT("服务器已满,请稍后重试!"),REQUEST_FAILURE_NORMAL);
			return true;
		}		
	}
	else
	{
		SendRequestFailure(pIServerUserItem,TEXT("您已经在游戏中,无法加入!"),REQUEST_FAILURE_NORMAL);
		return true;
	}

	//返回加入成功
	STR_CMD_GC_USER_JOIN_ROOM_SUCCESS JoinSuccess;
	ZeroMemory(&JoinSuccess, sizeof(JoinSuccess));
	memcpy(&JoinSuccess.strTableRule, &cfg, sizeof(JoinSuccess.strTableRule));

	//赋值
	JoinSuccess.byGameMode = 2;
	JoinSuccess.wChairID = wChairID;
	JoinSuccess.dwRoomID = pCurrTableFrame->GetPassword();
	JoinSuccess.byAllGameCount = 0;
	JoinSuccess.wPlayerCount = 4;
	JoinSuccess.byZhuangType = 3;

	//发送数据
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, &JoinSuccess ,sizeof(STR_CMD_GC_USER_JOIN_ROOM_SUCCESS));

	return true;
}

//修改用户财富信息
bool CAttemperEngineSink::On_CMD_GC_User_ModifyUserTreasure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//这里不能校验socketID,因为传入的就是0
	//数据校验
	ASSERT(wDataSize == sizeof(STR_DBO_GR_MODIFY_USER_TREASURE));
	if(wDataSize != sizeof(STR_DBO_GR_MODIFY_USER_TREASURE))
		return false;

	//定义
	STR_DBO_GR_MODIFY_USER_TREASURE *pDBOModify = (STR_DBO_GR_MODIFY_USER_TREASURE *)pData;

	//修改失败，直接返回
	if ( DB_SUCCESS != pDBOModify->dwResultCode )
	{
		return true;
	}

	//获取用户
	IServerUserItem *pIServerUserItem = m_ServerUserManager.SearchUserItem(pDBOModify->dwUserID);

	//用户检验
	if ( NULL == pIServerUserItem )
	{
		return true;
	}
	
	//构造返回数据
	STR_CMD_GC_USER_MODIFY_TREASURE CMDModify;
	ZeroMemory(&CMDModify, sizeof(STR_CMD_GC_USER_MODIFY_TREASURE));

	//赋值
	CMDModify.lOpenRoomCard = pDBOModify->lOpenRoomCard;
	CMDModify.lUserDiamond = pDBOModify->lUserDiamond;
	CMDModify.lUserGold = pDBOModify->lUserGold;

	//发送修改成功消息
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_MODIFY_TREASURE, &CMDModify, sizeof(STR_CMD_GC_USER_MODIFY_TREASURE));

	//修改成功后，修改内存中的用户数据
	pIServerUserItem->SetUserRoomCard(CMDModify.lOpenRoomCard);
	pIServerUserItem->SetUserGold(CMDModify.lUserGold);
	pIServerUserItem->SetUserDiamond(CMDModify.lUserDiamond);

	return true;
}

//用户离开
VOID CAttemperEngineSink::OnEventUserLogout(IServerUserItem * pIServerUserItem, DWORD dwLeaveReason)
{
	//变量定义
	DBR_GR_LeaveGameServer LeaveGameServer;
	ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));

	//提取成绩
	pIServerUserItem->QueryRecordInfo(LeaveGameServer.RecordInfo);
	pIServerUserItem->DistillVariation(LeaveGameServer.VariationInfo);

	//LeaveGameServer.lControlScore = pIServerUserItem->GetUserControlScore();

	//用户信息
	LeaveGameServer.dwLeaveReason=dwLeaveReason;
	LeaveGameServer.dwUserID=pIServerUserItem->GetUserID();
	LeaveGameServer.dwInoutIndex=pIServerUserItem->GetInoutIndex();
	LeaveGameServer.dwOnLineTimeCount=(DWORD)(time(NULL))-pIServerUserItem->GetLogonTime();

	//连接信息
	LeaveGameServer.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(LeaveGameServer.szMachineID,pIServerUserItem->GetMachineID(),CountArray(LeaveGameServer.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_LEAVE_GAME_SERVER,0L,&LeaveGameServer,sizeof(LeaveGameServer), TRUE);

	//汇总用户
	if (m_bCollectUser==true)
	{
		//变量定义
		CMD_CS_C_UserLeave UserLeave;
		ZeroMemory(&UserLeave,sizeof(UserLeave));

		//设置变量
		UserLeave.dwUserID=pIServerUserItem->GetUserID();

		//发送消息
		m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_LEAVE,&UserLeave,sizeof(UserLeave));
	}

	//知道比赛服务退出游戏
	if(m_pIGameMatchServiceManager!=NULL)m_pIGameMatchServiceManager->OnUserQuitGame(pIServerUserItem, 0);

	//分组删除
	DeleteWaitDistribute(pIServerUserItem);

	//广播在线人数
	CMD_GF_OnlinePlayers OnlinePlayers;
	ZeroMemory(&OnlinePlayers, sizeof(CMD_GF_OnlinePlayers));

	for(WORD i=0;i < static_cast<WORD>(m_ServerUserManager.GetUserItemCount());i++)
	{
		if( m_ServerUserManager.EnumUserItem(i)->GetUserStatus() != US_OFFLINE 
			&& !m_ServerUserManager.EnumUserItem(i)->IsVirtualUser())
			OnlinePlayers.wOnlinePlayers++;
	}

	for(WORD i=0;i < static_cast<WORD>(m_ServerUserManager.GetUserItemCount());i++)
	{
		IServerUserItem* pItem = m_ServerUserManager.EnumUserItem(i);
		if( pItem->GetUserStatus() != US_OFFLINE && !pItem->IsVirtualUser())
		{
			SendData(pItem,MDM_USER,SUB_GF_ONLINE_PLAYERS,&OnlinePlayers,sizeof(CMD_GF_OnlinePlayers));
		}
	}


	return;
}

//请求失败
bool CAttemperEngineSink::SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode)
{
	//变量定义
	CMD_GR_RequestFailure RequestFailure;
	ZeroMemory(&RequestFailure,sizeof(RequestFailure));

	//设置变量
	RequestFailure.lErrorCode=lErrorCode;
	lstrcpyn(RequestFailure.szDescribeString,pszDescribe,CountArray(RequestFailure.szDescribeString));

	SendData(pIServerUserItem, MDM_USER, SUB_GR_REQUEST_FAILURE, &RequestFailure, sizeof(CMD_GR_RequestFailure));

	return true;
}

//发送用户数据
bool CAttemperEngineSink::SendUserInfoPacket(IServerUserItem *pIServerUserItem, DWORD dwSocketID)
{
	//效验参数
	if (pIServerUserItem==NULL) return false;

	//变量定义
	tagUserInfo *pUserInfo = pIServerUserItem->GetUserInfo();
	if(pUserInfo == NULL)
		return false;

	//发送数据
	if (dwSocketID == INVALID_DWORD)
	{
		//TODO 电脑群发是什么意思？？？
		SendData(BG_COMPUTER, MDM_GR_LOGON, CMD_GC_LOGON_GET_USER_INFO, pUserInfo, sizeof(tagUserInfo));
	}
	else
	{
		SendData(dwSocketID, MDM_GR_LOGON, CMD_GC_LOGON_GET_USER_INFO, pUserInfo, sizeof(tagUserInfo));
	}

	return true;
}

//绑定用户
IServerUserItem * CAttemperEngineSink::GetBindUserItem(WORD wBindIndex)
{
	//获取参数
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//获取用户
	if (pBindParameter!=NULL)
	{
		return pBindParameter->pIServerUserItem;
	}

	return NULL;
}

//绑定参数
tagBindParameter * CAttemperEngineSink::GetBindParameter(WORD wBindIndex)
{
	//无效连接
	if (wBindIndex==INVALID_WORD) return NULL;

	//常规连接
	if (wBindIndex<256
		&&(m_pNormalParameter != NULL))
	{
		return m_pNormalParameter+wBindIndex;
	}

	//机器连接
	if ((wBindIndex>=INDEX_ANDROID) &&(wBindIndex<(INDEX_ANDROID+MAX_ANDROID))
		&& (m_pAndroidParameter != NULL))
	{
		return m_pAndroidParameter+(wBindIndex-INDEX_ANDROID);
	}

	return NULL;
}

//获取空闲虚拟用户 
IServerUserItem * CAttemperEngineSink::GetVirtualUserItem()
{
	for(int i= INDEX_ANDROID+MAX_ANDROID;i < INDEX_ANDROID+MAX_ANDROID + MAX_VIRTUAL;i++)
	{
		tagBindParameter* pBindParameter = m_pAndroidParameter+(i-INDEX_ANDROID);
		if(NULL != pBindParameter && pBindParameter->pIServerUserItem == NULL)
		{
			//初始化虚拟用户信息
			//用户变量
			tagUserInfo UserInfo;
			tagUserInfoPlus UserInfoPlus;
			ZeroMemory(&UserInfo,sizeof(UserInfo));
			ZeroMemory(&UserInfoPlus,sizeof(UserInfoPlus));

			//属性资料
			UserInfo.dwUserID=i;
			UserInfo.dwGroupID=0;
			UserInfo.dwCustomID=0;
			lstrcpyn(UserInfo.szNickName,TEXT("降龙十八掌"),CountArray(UserInfo.szNickName));
			lstrcpyn(UserInfo.szHeadUrl,TEXT(""),CountArray(UserInfo.szHeadUrl));

			//用户资料
			UserInfo.cbGender=rand()%2;
			UserInfo.cbMemberOrder=0;
			UserInfo.cbMasterOrder=0;
			lstrcpyn(UserInfo.szGroupName,TEXT(""),CountArray(UserInfo.szGroupName));
			lstrcpyn(UserInfo.szUnderWrite,TEXT("这个人很懒，没有个性签名"),CountArray(UserInfo.szUnderWrite));

			//状态设置
			UserInfo.cbUserStatus=US_FREE;
			UserInfo.wTableID=INVALID_TABLE;
			UserInfo.wChairID=INVALID_CHAIR;

			srand((DWORD)time(NULL));

			//积分信息
			UserInfo.lScore= 123456 + rand()%793421169;
			UserInfo.lGrade= 114235 + rand()%524242198;
			UserInfo.lOpenRoomCard= 4235 + rand()%164242198;
			UserInfo.lDiamond= 53222 +rand()%1123143523;
			UserInfo.lGold= 879111 + rand() % 218901000;
			UserInfo.lControlScore=0;
			UserInfo.dwWinCount= 23 + rand() % 15611;
			UserInfo.dwLostCount= 12 + rand() % 13611;
			UserInfo.dwDrawCount= rand() % 1611;
			UserInfo.dwFleeCount= rand() % 11142;
			UserInfo.dwUserMedal= rand() % 112314;
			UserInfo.dwExperience= rand() % 1111;
			UserInfo.lLoveLiness= rand() % 175432;

			//登录信息
			UserInfoPlus.dwLogonTime=(DWORD)time(NULL);
			UserInfoPlus.dwInoutIndex=0;

			//用户权限
			UserInfoPlus.dwUserRight=0;
			UserInfoPlus.dwMasterRight=0;

			//辅助变量
			UserInfoPlus.bAndroidUser=0;

			//UserInfoPlus.lRestrictScore=m_pGameServiceOption->lRestrictScore;
			UserInfoPlus.lRestrictScore=0L;//屏蔽每局封顶
			lstrcpyn(UserInfoPlus.szPassword,TEXT(""),CountArray(UserInfoPlus.szPassword));

			//连接信息
			UserInfoPlus.wBindIndex=i;
			UserInfoPlus.dwClientAddr=pBindParameter->dwClientAddr;
			lstrcpyn(UserInfoPlus.szMachineID,TEXT(""),CountArray(UserInfoPlus.szMachineID));

			//激活用户
			m_ServerUserManager.InsertUserItem(&(pBindParameter->pIServerUserItem),UserInfo,UserInfoPlus);
			//设置为虚拟用户
			pBindParameter->pIServerUserItem->SetVirtualUser(true);

			return pBindParameter->pIServerUserItem;
		}
	}

	return NULL;
}

//道具类型
WORD CAttemperEngineSink::GetPropertyType(WORD wPropertyIndex)
{
	switch(wPropertyIndex)
	{
	case PROPERTY_ID_CAR:	case PROPERTY_ID_EGG: 	case PROPERTY_ID_CLAP: 	case PROPERTY_ID_KISS: 	case PROPERTY_ID_BEER:
	case PROPERTY_ID_CAKE: 	case PROPERTY_ID_RING:  case PROPERTY_ID_BEAT: 	case PROPERTY_ID_BOMB:  case PROPERTY_ID_SMOKE:
	case PROPERTY_ID_VILLA: case PROPERTY_ID_BRICK: case PROPERTY_ID_FLOWER: 
		{
			return PT_TYPE_PRESENT;
		};
	case PROPERTY_ID_TWO_CARD: 	case PROPERTY_ID_FOUR_CARD:  case PROPERTY_ID_SCORE_CLEAR:     case PROPERTY_ID_ESCAPE_CLEAR:
	case PROPERTY_ID_TRUMPET:	case PROPERTY_ID_TYPHON:     case PROPERTY_ID_GUARDKICK_CARD:  case PROPERTY_ID_POSSESS:
	case PROPERTY_ID_BLUERING_CARD: case PROPERTY_ID_YELLOWRING_CARD: case PROPERTY_ID_WHITERING_CARD: case PROPERTY_ID_REDRING_CARD:
	case PROPERTY_ID_VIPROOM_CARD: 
		{
			return PT_TYPE_PROPERTY;
		};
	}

	ASSERT(false);

	return PT_TYPE_ERROR;
}


//配置机器
bool CAttemperEngineSink::InitAndroidUser()
{
	//机器参数
	tagAndroidUserParameter AndroidUserParameter;
	ZeroMemory(&AndroidUserParameter,sizeof(AndroidUserParameter));

	//配置参数
	AndroidUserParameter.pGameParameter=m_pGameParameter;
	AndroidUserParameter.pGameServiceAttrib=m_pGameServiceAttrib;
	AndroidUserParameter.pGameServiceOption=m_pGameServiceOption;

	//服务组件
	AndroidUserParameter.pITimerEngine=m_pITimerEngine;
	AndroidUserParameter.pIServerUserManager=&m_ServerUserManager;
	AndroidUserParameter.pIGameServiceManager=m_pIGameServiceManager;
	AndroidUserParameter.pITCPNetworkEngineEvent=QUERY_OBJECT_PTR_INTERFACE(m_pIAttemperEngine,ITCPNetworkEngineEvent);

	//设置对象
	if (m_AndroidUserManager.InitAndroidUser(AndroidUserParameter)==false)
	{
		return false;
	}

	return true;
}

//配置桌子
bool CAttemperEngineSink::InitTableFrameArray()
{
	//桌子参数
	tagTableFrameParameter TableFrameParameter;
	ZeroMemory(&TableFrameParameter,sizeof(TableFrameParameter));

	//内核组件
	TableFrameParameter.pITimerEngine=m_pITimerEngine;
	TableFrameParameter.pIKernelDataBaseEngine=m_pIKernelDataBaseEngine;
	TableFrameParameter.pIRecordDataBaseEngine=m_pIRecordDataBaseEngine;

	//服务组件
	TableFrameParameter.pIMainServiceFrame=this;
	TableFrameParameter.pIAndroidUserManager=&m_AndroidUserManager;
	TableFrameParameter.pIGameServiceManager=m_pIGameServiceManager;

	//组件接口
	TableFrameParameter.PITCPSocketService = m_pITCPSocketService;

	//配置参数
	TableFrameParameter.pGameParameter=m_pGameParameter;
	TableFrameParameter.pGameServiceAttrib=m_pGameServiceAttrib;
	TableFrameParameter.pGameServiceOption=m_pGameServiceOption;

	if(m_pIGameMatchServiceManager!=NULL)
		TableFrameParameter.pIGameMatchServiceManager=QUERY_OBJECT_PTR_INTERFACE(m_pIGameMatchServiceManager,IUnknownEx);

	//桌子容器
	m_TableFrameArray.SetSize(MAX_TABLE);
	ZeroMemory(m_TableFrameArray.GetData(),MAX_TABLE*sizeof(CTableFrame *));

	//创建桌子
	for (WORD i=0;i< MAX_TABLE; i++)
	{
		//创建对象
		m_TableFrameArray[i]=new CTableFrame;

		//配置桌子
		if (m_TableFrameArray[i]->InitializationFrame(i,TableFrameParameter)==false)
		{
			return false;
		}

		if(m_pIGameMatchServiceManager!=NULL)
			m_pIGameMatchServiceManager->InitTableFrame(QUERY_OBJECT_PTR_INTERFACE((m_TableFrameArray[i]),ITableFrame),i);
	}

	if(m_pIGameMatchServiceManager!=NULL)
	{
		if (m_pIGameMatchServiceManager->InitMatchInterface(QUERY_OBJECT_PTR_INTERFACE(m_pIAttemperEngine,ITCPNetworkEngineEvent),m_pIKernelDataBaseEngine,
			(IServerUserManager*)QUERY_OBJECT_INTERFACE(m_ServerUserManager,IServerUserManager),this,m_pITimerEngine,&m_AndroidUserManager)==false)
		{
			ASSERT(FALSE);
			return false;
		}
		// 		if (m_pIGameMatchServiceManager->InitServerUserManager()==false)
		// 		{
		// 			ASSERT(FALSE);
		// 			return false;
		// 		}
		// 
		// 		if(m_pIGameMatchServiceManager->InitMainServiceFrame(QUERY_ME_INTERFACE(IMainServiceFrame))==false)
		// 		{
		// 			ASSERT(FALSE);
		// 			return false;
		// 		}

	}

	return true;
}

//发送请求
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//发送数据
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//插入分配
bool CAttemperEngineSink::InsertDistribute(IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//状态判断
	ASSERT(pIServerUserItem->GetUserStatus()<US_PLAYING);
	if (pIServerUserItem->GetUserStatus()>=US_PLAYING) return false;

	if(m_pIGameMatchServiceManager!=NULL) m_pIGameMatchServiceManager->OnUserJoinGame(pIServerUserItem,0);
	return false;
}

//插入用户
bool CAttemperEngineSink::InsertWaitDistribute(IServerUserItem * pIServerUserItem)
{
	//查找
	bool bAdd=true;
	POSITION pos=m_WaitDistributeList.GetHeadPosition();
	while(pos != NULL)
	{
		IServerUserItem *pUserItem = m_WaitDistributeList.GetNext(pos);
		if(pUserItem->GetUserID()==pIServerUserItem->GetUserID())
		{
			bAdd=false;
			break;
		}
	}

	//加入
	if(bAdd)
	{
		int listCount = m_WaitDistributeList.GetCount();
		if (listCount > 0 )
		{
			POSITION InsertPos = m_WaitDistributeList.FindIndex( rand() % listCount);
			m_WaitDistributeList.InsertAfter(InsertPos,pIServerUserItem);
		}
		else
		{
			m_WaitDistributeList.AddTail(pIServerUserItem);
		}
	}

	//通知
	SendData(pIServerUserItem,MDM_USER,SUB_GR_USER_WAIT_DISTRIBUTE,NULL,0);

	return bAdd;
}

//删除用户
bool CAttemperEngineSink::DeleteWaitDistribute(IServerUserItem * pIServerUserItem)
{
	//查找
	POSITION pos=m_WaitDistributeList.GetHeadPosition();
	while(pos != NULL)
	{
		POSITION tempPos = pos;
		IServerUserItem *pUserItem = m_WaitDistributeList.GetNext(pos);
		if(pUserItem->GetUserID()==pIServerUserItem->GetUserID())
		{
			m_WaitDistributeList.RemoveAt(tempPos);
			break;
		}
	}

	//删除用户
	m_ServerUserManager.DeleteUserItem(pIServerUserItem);

	return true;
}



bool  CAttemperEngineSink::Distribute(CTableFrame* pTableFrame, bool bChoiceAndroid, WORD wWantUserCount, WORD& wHandleCount )
{
	//坐下处理
	bool bAndroidFound = false;
	POSITION pos=m_WaitDistributeList.GetHeadPosition();
	while(pos!=NULL)
	{
		//变量定义
		WORD wChairID=pTableFrame->GetRandNullChairID();

		//无效过滤
		if (wChairID==INVALID_CHAIR) continue;

		//坐下处理
		POSITION tempPos=pos;
		IServerUserItem *pUserItem = m_WaitDistributeList.GetNext(pos);

		if (bChoiceAndroid && pUserItem->IsAndroidUser())
		{
			continue;
		}

		bAndroidFound = true;

		if(pUserItem && pUserItem->GetUserStatus()==US_FREE && (pTableFrame->PerformSitDownAction(wChairID,pUserItem)==true))
		{
			//删除排队
			m_WaitDistributeList.RemoveAt(tempPos);

			//设置变量
			wHandleCount++;

			//完成判断
			if (wHandleCount>=wWantUserCount) 
			{
				break;
			}
		}
	}

	return bAndroidFound;
}

//设置参数
void CAttemperEngineSink::SetMobileUserParameter(IServerUserItem * pIServerUserItem,BYTE cbDeviceType,WORD wBehaviorFlags,WORD wPageTableCount)
{
	if(wPageTableCount > MAX_TABLE)wPageTableCount=MAX_TABLE;
	pIServerUserItem->SetMobileUserDeskCount(wPageTableCount);
	pIServerUserItem->SetMobileUserRule(wBehaviorFlags);
}

//群发数据
bool CAttemperEngineSink::SendDataBatchToMobileUser(WORD wCmdTable, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//枚举用户
	WORD wEnumIndex=0;
	while(wEnumIndex<m_ServerUserManager.GetUserItemCount())
	{
		//过滤用户
		IServerUserItem *pIServerUserItem=m_ServerUserManager.EnumUserItem(wEnumIndex++);
		if(pIServerUserItem==NULL) continue;


		//定义变量
		WORD wMobileUserRule = pIServerUserItem->GetMobileUserRule();
		WORD wTagerTableID = pIServerUserItem->GetTableID();
		bool bViewModeAll = ((wMobileUserRule&VIEW_MODE_ALL)!=0);

		//状态过滤
		if(pIServerUserItem->GetUserStatus() >= US_SIT)
		{
			if(wTagerTableID != wCmdTable)continue;
		}

		//部分可视
		if(!bViewModeAll)
		{
			//消息过滤
			if(wSubCmdID==CMD_GC_LOGON_GET_USER_INFO && wCmdTable==INVALID_TABLE) continue;
			if(wSubCmdID==SUB_GR_USER_SCORE && pIServerUserItem->GetUserStatus() < US_SIT) continue;

			//定义变量
			WORD wTagerDeskPos = pIServerUserItem->GetMobileUserDeskPos();
			WORD wTagerDeskCount = pIServerUserItem->GetMobileUserDeskCount();

			//状态消息过滤
			if(wCmdTable < wTagerDeskPos) continue;
			if(wCmdTable > (wTagerDeskPos+wTagerDeskCount-1)) continue;
		}

		//发送消息
		SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	return true;
}

//库存计算是否控制yang
bool CAttemperEngineSink::IsControl( SCORE expectStock )
{
	bool isControl = false;
	//isControl = m_GameControlManager.IsControl(expectStock);
	return isControl;
}

//修改当前库存值yang
bool CAttemperEngineSink::SetStockScore( SCORE currentStock )
{
	//m_GameControlManager.SetStockScore(currentStock);
	return true;
}

//设置库存值yang
SCORE CAttemperEngineSink::GetChangeStockScore()
{
	return 0/*m_GameControlManager.GetChangeStockScore()*/;
}

void CAttemperEngineSink::StopService()
{
	m_bStopping = true;

	tagBindParameter *pBindParameter = m_pNormalParameter;
	for( INT i = 0; i < MAX_TABLE; i++ )
	{
		//目录用户
		IServerUserItem * pITargerUserItem= pBindParameter->pIServerUserItem;
		if (pITargerUserItem==NULL) 
		{
			pBindParameter++;
			continue;
		}

		//发送消息
		SendGameMessage(pITargerUserItem,TEXT("服务器即将关闭维护，请稍后回来"),SMT_CHAT|SMT_EJECT|SMT_CLOSE_LINK|SMT_CLOSE_GAME);
		pBindParameter++;
	}

	//解散所有游戏
	for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
	{
		//获取桌子
		CTableFrame * pTableFrame=m_TableFrameArray[i];
		if ( !pTableFrame->IsGameStarted() ) continue;

		pTableFrame->DismissGame();
	}

	//停止加载机器人、分配用户机器人等定时器
	m_pITimerEngine->KillTimer(IDI_LOAD_ANDROID_USER);
	m_pITimerEngine->KillTimer(IDI_DISTRIBUTE_ANDROID);

	//延迟关闭，确保写分等数据库存储过程执行
	m_pITimerEngine->SetTimer(IDI_WAIT_SERVICE_STOP, TIME_WAIT_SERVICE_STOP, 1, NULL);
}

void CAttemperEngineSink::SendServiceStopped()
{
	m_pITimerEngine->KillTimer(IDI_WAIT_SERVICE_STOP);

	CP_ControlResult ControlResult;
	ControlResult.cbSuccess = ER_SUCCESS;
	SendUIControlPacket(UI_SERVICE_STOP_SERVICE_FINISH, &ControlResult, sizeof(ControlResult));
}


bool CAttemperEngineSink::DistributeAndroid()
{
//	//动作处理
//	if (m_AndroidUserManager.GetAndroidCount()>0 )
//	{
//		//变量定义
////		bool bAllowDynamicJoin=CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule);
//	//	bool bAllowAndroidAttend=CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule);
//		//bool bAllowAndroidSimulate=CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule);
//		//bool bAllowAvertCheatMode=(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR));
//
//		//模拟处理
//		if (bAllowAndroidSimulate==true && bAllowAvertCheatMode==false)
//		{
//			//机器状态
//			tagAndroidUserInfo AndroidSimulate; 
//			m_AndroidUserManager.GetAndroidUserInfo(AndroidSimulate,ANDROID_SIMULATE);
//
//			//机器处理
//			if (AndroidSimulate.wFreeUserCount>0)
//			{
//				for (WORD i=0;i<8;i++)
//				{
//					//随机桌子
//					//WORD wTableID=rand()%(__max(m_pGameServiceOption->wTableCount/3,1));
//					WORD wTableID=rand()%(__max(MAX_TABLE,1));
//
//					//获取桌子
//					CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
//					if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;
//
//					//桌子状况
//					tagTableUserInfo TableUserInfo;
//					WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);
//
//					//分配判断
//					if (TableUserInfo.wTableUserCount>0) continue;
//					if ((wUserSitCount>=TableUserInfo.wMinUserCount)&&(m_pGameServiceAttrib->wChairCount<MAX_CHAIR)) continue;
//
//					//坐下判断
//					if (AndroidSimulate.wFreeUserCount>=TableUserInfo.wMinUserCount)
//					{
//						//变量定义
//						WORD wHandleCount=0;
//						WORD wWantAndroidCount=TableUserInfo.wMinUserCount;
//
//						//数据调整
//						if (m_pGameServiceAttrib->wChairCount>TableUserInfo.wMinUserCount)
//						{
//							WORD wChairCount=m_pGameServiceAttrib->wChairCount;
//							WORD wFreeUserCount=AndroidSimulate.wFreeUserCount;
//							WORD wOffUserCount=__min(wChairCount,wFreeUserCount)-TableUserInfo.wMinUserCount;
//							wWantAndroidCount+=((wOffUserCount > 0) ? (rand()%(wOffUserCount+1)) : 0);
//						}
//
//						//坐下处理
//						for (WORD j=0;j<AndroidSimulate.wFreeUserCount;j++)
//						{
//							//变量定义
//							WORD wChairID=pTableFrame->GetRandNullChairID();
//
//							//无效过滤
//							//ASSERT(wChairID!=INVALID_CHAIR);
//							if (wChairID==INVALID_CHAIR) continue;
//
//							//用户坐下
//							IAndroidUserItem * pIAndroidUserItem=AndroidSimulate.pIAndroidUserFree[j];
//							if (pTableFrame->PerformSitDownAction(wChairID,pIAndroidUserItem->GetMeUserItem())==true)
//							{
//								//设置变量
//								wHandleCount++;
//
//								//完成判断
//								if (wHandleCount>=wWantAndroidCount) 
//								{
//									return true;
//								}
//							}
//						}
//
//						if(wHandleCount > 0) return true;
//					}
//				}
//			}
//		}
//
//		//陪打处理
//		if (bAllowAndroidAttend==true)
//		{
//			//被动状态
//			tagAndroidUserInfo AndroidPassivity;
//			m_AndroidUserManager.GetAndroidUserInfo(AndroidPassivity,ANDROID_PASSIVITY);
//
//			if(bAllowAvertCheatMode)
//			{
//				//坐下处理
//				for (WORD j=0;j<AndroidPassivity.wFreeUserCount;j++)
//				{
//					IAndroidUserItem * pIAndroidUserItem=AndroidPassivity.pIAndroidUserFree[j];
//					if (InsertWaitDistribute(pIAndroidUserItem->GetMeUserItem())==true) return true;
//				}
//			}
//			else
//			{
//				//被动处理
//				if (AndroidPassivity.wFreeUserCount>0)
//				{
//					//百人游戏
//					if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
//					{
//						for (INT_PTR i=0;i<MAX_TABLE;i++)
//						{
//							//获取桌子
//							CTableFrame * pTableFrame=m_TableFrameArray[i];
//							if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;
//
//							//桌子状况
//							tagTableUserInfo TableUserInfo;
//							WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);
//
//							//分配判断
//							if (wUserSitCount>m_pGameServiceAttrib->wChairCount*2/3) continue;
//
//							//变量定义
//							IServerUserItem * pIServerUserItem=NULL;
//							WORD wChairID=pTableFrame->GetRandNullChairID();
//
//							//无效过滤
//							ASSERT(wChairID!=INVALID_CHAIR);
//							if (wChairID==INVALID_CHAIR) continue;
//
//							//坐下处理
//							for (WORD j=0;j<AndroidPassivity.wFreeUserCount;j++)
//							{
//								IAndroidUserItem * pIAndroidUserItem=AndroidPassivity.pIAndroidUserFree[j];
//								if (pTableFrame->PerformSitDownAction(wChairID,pIAndroidUserItem->GetMeUserItem())==true) return true;
//							}
//						}
//					}
//					else
//					{
//						for (INT_PTR i=0;i<MAX_TABLE;i++)
//						{
//							//获取桌子
//							CTableFrame * pTableFrame=m_TableFrameArray[i];
//							if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;
//
//							//桌子状况
//							tagTableUserInfo TableUserInfo;
//							WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);
//
//							//分配判断
//							if (wUserSitCount==0) continue;
//							if (TableUserInfo.wTableUserCount==0) continue;
//							if ((wUserSitCount>=TableUserInfo.wMinUserCount)&&(rand()%10>5)) continue;
//							if (pTableFrame->GetNullChairCount()==0) continue;
//
//							//变量定义
//							IServerUserItem * pIServerUserItem=NULL;
//							WORD wChairID=pTableFrame->GetRandNullChairID();
//
//							//无效过滤										
//							ASSERT(wChairID!=INVALID_CHAIR);
//							if (wChairID==INVALID_CHAIR) continue;
//
//							//坐下处理
//							for (WORD j=0;j<AndroidPassivity.wFreeUserCount;j++)
//							{
//								IAndroidUserItem * pIAndroidUserItem=AndroidPassivity.pIAndroidUserFree[j];
//								if (pTableFrame->PerformSitDownAction(wChairID,pIAndroidUserItem->GetMeUserItem())==true) return true;
//							}
//						}
//					}
//				}
//			}
//		}
//
//		//陪打处理
//		if (bAllowAndroidAttend==true)
//		{
//			//主动状态
//			tagAndroidUserInfo AndroidInitiative;
//			m_AndroidUserManager.GetAndroidUserInfo(AndroidInitiative,ANDROID_INITIATIVE);
//			WORD wAllAndroidCount = AndroidInitiative.wFreeUserCount+AndroidInitiative.wPlayUserCount+AndroidInitiative.wSitdownUserCount;
//
//			if(bAllowAvertCheatMode)
//			{
//				//坐下处理
//				for (WORD j=0;j<AndroidInitiative.wFreeUserCount;j++)
//				{
//					IAndroidUserItem * pIAndroidUserItem=AndroidInitiative.pIAndroidUserFree[j];
//					if (InsertWaitDistribute(pIAndroidUserItem->GetMeUserItem())==true) return true;
//				}
//			}
//			else
//			{
//				//主动处理
//				if (AndroidInitiative.wFreeUserCount>0)
//				{
//					for (INT_PTR i=0;i<MAX_TABLE;i++)
//					{
//						//获取桌子
//						CTableFrame * pTableFrame=m_TableFrameArray[i];
//						if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;
//
//						//桌子状况
//						tagTableUserInfo TableUserInfo;
//						WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);
//
//						//分配判断
//						if((m_pGameServiceAttrib->wChairCount<MAX_CHAIR) && wUserSitCount>(TableUserInfo.wMinUserCount-1)) continue;
//
//						//变量定义
//						IServerUserItem * pIServerUserItem=NULL;
//						WORD wChairID=pTableFrame->GetRandNullChairID();
//
//						//无效过滤
//						ASSERT(wChairID!=INVALID_CHAIR);
//						if (wChairID==INVALID_CHAIR) continue;
//
//						//坐下处理
//						for (WORD j=0;j<AndroidInitiative.wFreeUserCount;j++)
//						{
//							IAndroidUserItem * pIAndroidUserItem=AndroidInitiative.pIAndroidUserFree[j];
//							if (pTableFrame->PerformSitDownAction(wChairID,pIAndroidUserItem->GetMeUserItem())==true) return true;
//						}
//					}
//				}
//			}
//		}
//
//		//起立处理
//		WORD wStandUpCount=0;
//		WORD wRandCount=((rand()%3)+1);
//		INT_PTR nIndex = rand()%(__max(MAX_TABLE,1));
//		for (INT_PTR i=nIndex;i< MAX_TABLE+nIndex;++i)
//		{
//			//获取桌子
//			INT_PTR nTableIndex=i% MAX_TABLE;
//			CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
//			if (pTableFrame->IsGameStarted()==true) continue;
//
//			//桌子状况
//			tagTableUserInfo TableUserInfo;
//			WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);
//
//			//用户过虑
//			bool bRand = ((rand()%100)>50);
//			if (TableUserInfo.wTableAndroidCount==0) continue;
//			if ((TableUserInfo.wTableUserCount>0)&&(bAllowAndroidAttend==true) && bRand) continue;
//			if (TableUserInfo.wTableAndroidCount>=TableUserInfo.wMinUserCount && bRand) continue;
//
//			//起立处理
//			for (WORD j=0;j<pTableFrame->GetChairCount();j++)
//			{
//				//获取用户
//				IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(j);
//				if (pIServerUserItem==NULL) continue;
//
//				//用户起立
//				if ((pIServerUserItem->IsAndroidUser()==true)&&(pTableFrame->PerformStandUpAction(pIServerUserItem)==true))
//				{
//					wStandUpCount++;
//					if(wStandUpCount>=wRandCount)
//						return true;
//					else
//						break;
//				}
//			}
//		}
//
//		//起立处理
//		nIndex = rand()%(__max(MAX_TABLE,1));
//		for (INT_PTR i=nIndex;i< MAX_TABLE+nIndex; ++i)
//		{
//			//获取桌子
//			INT_PTR nTableIndex=i%MAX_TABLE;
//			CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
//			if (pTableFrame->IsGameStarted()==true) continue;
//
//			//桌子状况
//			tagTableUserInfo TableUserInfo;
//			WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);
//
//			//用户过虑
//			bool bRand = ((rand()%100)>50);
//			if (TableUserInfo.wTableAndroidCount==0) continue;
//			if ((TableUserInfo.wTableUserCount>0)&&(bAllowAndroidAttend==true) && bRand) continue;
//
//			//起立处理
//			for (WORD j=0;j<pTableFrame->GetChairCount();j++)
//			{
//				//获取用户
//				IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(j);
//				if (pIServerUserItem==NULL) continue;
//
//				//用户起立
//				if ((pIServerUserItem->IsAndroidUser()==true)&&(pTableFrame->PerformStandUpAction(pIServerUserItem)==true))
//				{
//					wStandUpCount++;
//					if(wStandUpCount>=wRandCount)
//						return true;
//					else
//						break;
//				}
//			}
//		}
//	}

	return true;
}

void CAttemperEngineSink::InitRoomControlVal( SCORE val )
{
	m_ControlValueForRoomOrgi = m_ControlValueForRoom = val;
}

SCORE CAttemperEngineSink::CheckAndSaveRoomControlValue()
{
	if ( m_ControlValueForRoom != m_ControlValueForRoomOrgi )
	{
		m_ControlValueForRoomOrgi = m_ControlValueForRoom;

		return m_ControlValueForRoom;
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////////////
//添加替他人开房
bool CAttemperEngineSink::AddOtherRoomInfo(DBR_GR_UpdateTableInfo* pTableInfo)
{
	//无效过滤
	if (pTableInfo==NULL) return false;

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR, DBR_GR_ADD_TABLE_INFO, 0, pTableInfo, sizeof(DBR_GR_UpdateTableInfo));

	return true;
}

//开始替他人开房
void CAttemperEngineSink::StartTable(DWORD dwTableID)
{
	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_GR_START_TABLE,0,&dwTableID,sizeof(dwTableID));
}

//结束替他人开房
void CAttemperEngineSink::ConcludeTable(DWORD dwTableID)
{
	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_GR_END_TABLE,0,&dwTableID,sizeof(dwTableID));
}

#pragma region Club牌友圈2 事件通知(1.桌子信息 2.玩家信息)
//club 创建桌子
void CAttemperEngineSink::ClubTableCreate(DWORD dwClubRoomID, DWORD dwUserID, DWORD dwTableID, DWORD dwLockState)
{
    STR_DBR_CLUB_TABLE_INFO Dbr;
	ZeroMemory(&Dbr, sizeof(Dbr));
	Dbr.dwClubRoomID = dwClubRoomID;
	Dbr.dwUserID = dwUserID;
	Dbr.dwTableID =dwTableID;
    Dbr.dwLockState =dwLockState;
	Dbr.dwTableState = 0;
	Dbr.byCurrentRound = 0;
	Dbr.byMask =1;

	Dbr.byCompanyID = _MYSTERY;
	Dbr.dwGameID = (m_pGameServiceOption->dwServerID) & 0xFFFFFF00;

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR, DBR_CLUB_TABLE_INFO, 0, &Dbr, sizeof(Dbr));
}

//club 桌子开始游戏
void CAttemperEngineSink::ClubTableStart(DWORD dwTableID, BYTE byClubOrHallGold)
{
	//俱乐部桌子信息  备注: 桌子开始游戏后, 更新当前局数为 (0 + 1)
	if(1 == byClubOrHallGold)
	{
		STR_DBR_CLUB_TABLE_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.dwTableState = 1;
		Dbr.byCurrentRound = 1;//这是一个增量 TODONOW 后面修改
		Dbr.byMask = 2;

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
	}

	//金币大厅桌子信息
	if(2 == byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_TABLE_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.dwTableState = 1;
		HallDbr.byMask = 2;

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_HALL_GOLD_TABLE_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

//club 桌子已经满员
void CAttemperEngineSink::ClubTableMax(DWORD dwTableID, BYTE byClubOrHallGold)
{
	//备注: 桌子开始游戏后, 更新当前局数为 (0 + 1)
	if(1 == byClubOrHallGold)
	{
		STR_DBR_CLUB_TABLE_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.dwTableState = 2;
		Dbr.byMask = 2;

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
	}

	//金币大厅桌子信息
	if(2 == byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_TABLE_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.dwTableState = 2;
		HallDbr.byMask = 2;

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_HALL_GOLD_TABLE_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

//club 桌子没有满员
void CAttemperEngineSink::ClubTableNotMax(DWORD dwTableID, BYTE byClubOrHallGold)
{
	//备注: 桌子开始游戏后, 更新当前局数为 (0 + 1)
	if(1 ==  byClubOrHallGold)
	{
		STR_DBR_CLUB_TABLE_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.dwTableState = 0;
		Dbr.byMask = 2;

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
	}

	//金币大厅桌子信息
	if( 2 ==  byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_TABLE_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.dwTableState = 0;
		HallDbr.byMask = 2;

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_HALL_GOLD_TABLE_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

//club 桌子小局结束
void CAttemperEngineSink::ClubTableXJ(DWORD dwTableID)
{
	//备注: 桌子小局结束后, 更新当前局数为 (数据库 + 1)
	STR_DBR_CLUB_TABLE_INFO Dbr;
	ZeroMemory(&Dbr, sizeof(Dbr));
	Dbr.dwTableID = dwTableID;
	Dbr.dwTableState = 1;
	Dbr.byCurrentRound = 1;//这是一个增量 TODONOW 后面修改
	Dbr.byMask = 2;
	
	m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
}

//club 桌子大局结束
void CAttemperEngineSink::ClubTableDJ(DWORD dwTableID)
{
    //备注: 桌子大局结束, 解散桌子
    STR_DBR_CLUB_TABLE_INFO Dbr;
	ZeroMemory(&Dbr, sizeof(Dbr));
	Dbr.dwTableID = dwTableID;
	Dbr.byMask = 3;
	
	m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
}

#pragma endregion

//用户加入替他人开房
void CAttemperEngineSink::JoinTable(DWORD dwTableID, DWORD dwUserID)
{
	
	DBR_GP_AddTableUser AddTableUser;
	AddTableUser.dwTableID = dwTableID;
	AddTableUser.dwUserID = dwUserID;
	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_GR_ADD_TABLEUSER,0,&AddTableUser,sizeof(DBR_GP_AddTableUser));
}

//用户离开替他人开房
void CAttemperEngineSink::LeaveTable(DWORD dwTableID, DWORD dwUserID)
{
	DBR_GP_DeleteTableUser DeleteTableUser;
	DeleteTableUser.dwTableID = dwTableID;
	DeleteTableUser.dwUserID = dwUserID;

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_GR_DELETE_TABLEUSER,0,&DeleteTableUser,sizeof(DBR_GP_DeleteTableUser));
}

//抽奖结果
bool CAttemperEngineSink::OnDBLotteryResult( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	ASSERT(wDataSize==sizeof(DBO_GR_LotteryResult));
	if(wDataSize!=sizeof(DBO_GR_LotteryResult))
		return false;

	DBO_GR_LotteryResult *pLotteryResult = (DBO_GR_LotteryResult*)pData;

	CMD_GR_LotteryResult LotteryResult;
	ZeroMemory(&LotteryResult,sizeof(CMD_GR_LotteryResult));

	LotteryResult.byIndex = pLotteryResult->byIndex;
	LotteryResult.byType = pLotteryResult->byType;
	LotteryResult.dwRewardCount = pLotteryResult->dwRewardCount;
	lstrcpyn(LotteryResult.szDescribe,pLotteryResult->szDescribe,CountArray(LotteryResult.szDescribe));
	lstrcpyn(LotteryResult.szPacketID,pLotteryResult->szPacketID,CountArray(LotteryResult.szPacketID));
	//memcpy(&LotteryResult,pLotteryResult,sizeof(CMD_GP_LotteryResult));

	wprintf(TEXT("pLottery:%d,%d,%d,%s,%s\n"),pLotteryResult->byType,pLotteryResult->byIndex,pLotteryResult->dwRewardCount,pLotteryResult->szPacketID,pLotteryResult->szDescribe);
	wprintf(TEXT("Lottery:%d,%d,%d,%s,%s\n"),LotteryResult.byType,LotteryResult.byIndex,LotteryResult.dwRewardCount,LotteryResult.szPacketID,LotteryResult.szDescribe);

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_G_FRAME,SUB_GR_LOTTERY_RESULT,&LotteryResult,sizeof(CMD_GR_LotteryResult));

	return true;
}

//Club牌友圈创建桌子
bool CAttemperEngineSink::On_CMD_CG_CLUB_CREATE_TABLE( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//获取用户
	WORD wBindIndex = LOWORD(dwContextID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);
	//用户校验
	if (pIServerUserItem==NULL) return false;

	//校验参数
	if(wDataSize!=sizeof(STR_DBO_GC_CLUB_CREATE_TABLE))
		return false;

	STR_DBO_GC_CLUB_CREATE_TABLE *pDbo = (STR_DBO_GC_CLUB_CREATE_TABLE*)pData;

	//流程处理
	return CreateTableClub(pDbo, pIServerUserItem);
}

//用户坐下
void CAttemperEngineSink::ClubPlayerSitDown(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold)
{
	//俱乐部桌子
	if(1 == byClubOrHallGold)
	{
		STR_DBR_CLUB_PLAYER_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.dwUserID = dwUserID;
		Dbr.byChairID = byChairID;
		Dbr.byMask = 1; 

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_CLUB_PLAYER_INFO,0,&Dbr,sizeof(Dbr));
	}

	//金币大厅桌子
	if(2 ==  byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_PLAYER_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.dwUserID = dwUserID;
		HallDbr.byChairID = byChairID;
		HallDbr.byMask = 1; 

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_HALL_GOLD_PLAYER_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

//用户起立
void CAttemperEngineSink::ClubPlayerSitUp(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold)
{
	//俱乐部桌子
	if( 1 == byClubOrHallGold)
	{
		STR_DBR_CLUB_PLAYER_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.dwUserID = dwUserID;
		Dbr.byMask = 2; 

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_CLUB_PLAYER_INFO,0,&Dbr,sizeof(Dbr));
	}

	//金币大厅
	if( 2 == byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_PLAYER_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.dwUserID = dwUserID;
		HallDbr.byChairID = byChairID;
		HallDbr.byMask = 2; 

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_HALL_GOLD_PLAYER_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

//最后一个用户起立
void CAttemperEngineSink::ClubLastPlayerSitUp(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold)
{
	//俱乐部桌子
	if( 1 == byClubOrHallGold)
	{
		STR_DBR_CLUB_TABLE_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.byMask = 3;

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
	}

	//金币大厅桌子
	if( 2 == byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_TABLE_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.byMask = 3;

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_HALL_GOLD_TABLE_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

#pragma region 辅助函数
//********************************************辅助函数*******************************************//

//生成桌子ID，即加入房间密码
DWORD CAttemperEngineSink::GenerateTablePassword()
{	
	//生成密码
	srand(static_cast<unsigned int >(time(NULL)));
	BYTE byHigh1 = 1+  rand() % 219 + 24;
	BYTE byHigh2 = ((m_pGameServiceAttrib->dwGameID) >> 16) &  0xFF;
	BYTE byHign3 = ((m_pGameServiceAttrib->dwGameID) >> 8) & 0xF;
	DWORD dwPassword = (byHigh1 << 12) + (byHigh2 <<4) + byHign3;

	//判断桌子密码是否存在
	while(true)
	{
		bool bFind = false;
		for (WORD i=0; i<m_TableFrameArray.GetCount(); i++)
		{
			//获取对象
			ASSERT(m_TableFrameArray[i] != NULL);
			CTableFrame *pTableFrame = m_TableFrameArray[i];

			//桌子状态判断
			if ( (NULL != pTableFrame) && 
				 (0 < pTableFrame->GetSitUserCount() ) && 
				 (dwPassword == pTableFrame->GetPassword()) )
			{
				bFind = true;
				break;
			}
		}
		//若有桌子已经用到该密码，重置密码	TODONOW 不能再用原来的生成方式
		if(bFind) 
		{
			//added by lizhihu
			byHigh1 = 1+  rand() % 219 + 24;
			dwPassword = (byHigh1 << 12) + (byHigh2 <<4) + byHign3;

			/*dwPassword = ((1 + rand() % 5) << 17) + ((rand() % 512) << 8) + (BYTE)(m_pGameServiceOption->dwServerID & 0XFF);*/
		}
		else
			break;
	}

	return dwPassword;
}

//寻找下一个空桌子
CTableFrame* CAttemperEngineSink::GetNextEmptyTable()
{
	//寻找下一个空桌子
	for (WORD i=0;i<m_TableFrameArray.GetCount();i++)
	{
		//获取对象
		ASSERT(m_TableFrameArray[i] != NULL);
		CTableFrame *pTableFrame = m_TableFrameArray[i];

		//桌子判断
		if ( (NULL != pTableFrame) && 
			 (0 == pTableFrame->GetSitUserCount()) && 
			 (0 == pTableFrame->GetCreateTableUser()) ) 
		{
			return pTableFrame;
		}
	}

	return NULL;
}

//根据密码寻找指定桌子
CTableFrame* CAttemperEngineSink::GetDesignatedTable(const DWORD &dwPassword)
{
	//寻找指定桌子
	for (WORD i=0; i<m_TableFrameArray.GetCount(); i++)
	{
		//获取对象
		ASSERT(m_TableFrameArray[i]!=NULL);
		CTableFrame *pTableFrame = m_TableFrameArray[i];

		//桌子判断
		if ( (NULL != pTableFrame) && 
			 (dwPassword == pTableFrame->GetPassword()) )
		{
			return pTableFrame;
		}
	}

	return NULL;
}

//查找金币房空椅子
CTableFrame* CAttemperEngineSink::GetGlodRoomEmptyChair(WORD &wChairID)
{
	//变量定义
	CTableFrame *pTableFrame = NULL;
	wChairID = INVALID_CHAIR;

	//寻找金币房空椅子
	for (WORD i=0; i<m_TableFrameArray.GetCount(); i++)
	{
		//获取对象
		ASSERT(m_TableFrameArray[i] != NULL);
		CTableFrame *pTableFrame = m_TableFrameArray[i];

		//桌子校验
		if ( (NULL == pTableFrame) || 
			 (pTableFrame->GetGameStatus() != GAME_STATUS_FREE) || 
			 (pTableFrame->GetTableMode() != TABLE_MODE_GOLD))
			continue;

		//获取空椅子
		wChairID = pTableFrame->GetNullChairID();
		if(wChairID == INVALID_CHAIR)
			continue;

		return pTableFrame;
	}

	return NULL;
}

//处理替他人开房流程
void CAttemperEngineSink::HandleCreateTableForOthers(CTableFrame *pCurTableFrame, IServerUserItem *pIServerUserItem, tagTableRule *pCfg)
{
	//校验
	if ( NULL == pCurTableFrame || 
		 NULL == pIServerUserItem ||
		 NULL == pCfg)
	{
		return;
	}

	//生成桌子密码
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwPassword = GenerateTablePassword();

	//设置房间密码和规则
	pCurTableFrame->SetPassword(dwPassword);

	//设置房间自动解散,替他人开房，房间暂时保留十分钟
	pCurTableFrame->SetTableAutoDismiss(10);
	pCurTableFrame->SetCreateTableUser(pIServerUserItem->GetUserID()); //设置创建玩家ID

	//添加开房信息
	DBR_GR_UpdateTableInfo TableInfo;
	TableInfo.byCost =  static_cast<BYTE>(pCfg->lSinglePayCost);
	TableInfo.byIsStart = 0;
	TableInfo.byMaxPlayerCount = 5;
	TableInfo.byMode = pCfg->GameMode;
	TableInfo.byPlayerCount = 0;
	TableInfo.byZhuangType = pCfg->RobBankType;
	TableInfo.dwCreaterID = pIServerUserItem->GetUserID();
	TableInfo.dwPassword = dwPassword;
	TableInfo.dwTableID = pCurTableFrame->GetTableID();
	TableInfo.wJuShu = pCfg->GameCount;

	//创建房间时间
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	_sntprintf_s(TableInfo.szTime, sizeof(TableInfo.szTime), TEXT("%04d-%02d-%02d %02d:%02d:%02d"), SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond);

	//添加房间信息进数据库  TODO 流程没有DBO函数，看看是否需要修改
	AddOtherRoomInfo(&TableInfo);

	//发送房间消息
	STR_CMD_GC_USER_CREATE_ROOM_SUCCESS nCreate;
	nCreate.dwPassword = dwPassword;
	nCreate.wChairID = INVALID_CHAIR;
	nCreate.byAllGameCount = pCfg->GameCount;
	nCreate.byGameMode = pCfg->GameMode;
	nCreate.byZhuangType = pCfg->RobBankType;

	//发送创建房间成功消息
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_ROOM_SUCCESS, &nCreate, sizeof(STR_CMD_GC_USER_CREATE_ROOM_SUCCESS));	

	//替他人开房，首先扣除创建桌子用户的房卡 替他人开房只支持房卡模式
	OnEventModifyUserTreasure(pIServerUserItem, dwPassword ,  TABLE_MODE_FK, 0,  -pCfg->lSinglePayCost, 0);
}

//处理正常开房流程
bool CAttemperEngineSink::HandleCreateTable(CTableFrame *pCurTableFrame, IServerUserItem *pIServerUserItem, tagTableRule *pCfg)
{
	//校验
	if ( NULL == pCurTableFrame || 
		 NULL == pIServerUserItem ||
		 NULL == pCfg)
	{
		return false;
	}

	//生成桌子密码，房间号 = 前面 + TABLEID
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwPassword = GenerateTablePassword();

	//设置桌子属性
	//pCurTableFrame->SetTableOwner(pIServerUserItem->GetUserID());
	pCurTableFrame->SetPassword(dwPassword);

	//设置房间自动解散，默认一分钟 -- 这里是指不开始游戏 自动一分钟后解散
	pCurTableFrame->SetTableAutoDismiss();
	
	//获得空椅子
	WORD wChairID = pCurTableFrame->GetNullChairID(); 

	//椅子号有效
	if (INVALID_CHAIR != wChairID)
	{
		//用户坐下		TODO 后面让客户端主动发送
		if(pCurTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, 0, true)==false)
		{
			SendRequestFailure(pIServerUserItem, TEXT("用户坐下失败!"), REQUEST_FAILURE_NORMAL);
			return false;
		}		
	}
	else
	{
		SendRequestFailure(pIServerUserItem, TEXT("房间已满,无法加入房间!"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	return true;
}

//检查加入桌子门票
bool CAttemperEngineSink::CheckJoinTableTicket(tagTableRule *pCfg, IServerUserItem *pIServerUserItem)
{
	//内部调用, 不校验指针

	switch( pCfg->GameMode )
	{
	case TABLE_MODE_FK://房卡模式
		{
			//大厅的房卡场：AA支付，且用户房卡不足
			if( (1 == pCfg->cbPayType) &&
				(0 == pCfg->byClubCreate) &&
				(pIServerUserItem->GetUserRoomCard() < pCfg->GameCountType) )
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在进入AA支付的房卡房, 因房卡不足,无法加入"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			break;
		}
	case TABLE_MODE_JJC://竞技场模式
		{
			break;
		}
	case TABLE_MODE_GOLD://金币模式
		{
			//大厅的金币场
			if(  pIServerUserItem->GetUserGold() < pCfg->dwLevelGold)
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在进入金币房,因金币不足,无法加入"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			break;
		}
	case TABLE_MODE_FK_GOLD://房卡金币场
		{
			//大厅的房卡金币场  校验一:  AA支付，且用户房卡不足
			if( (1 == pCfg->cbPayType) && 
				(0 == pCfg->byClubCreate) &&
				(pIServerUserItem->GetUserRoomCard() < pCfg->GameCountType) )
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在进入的为AA支付的房卡金币房, 因房卡不足,无法加入"), REQUEST_FAILURE_NORMAL);
				return false;
			}

			//俱乐部的房卡金币场 或者 大厅的房卡金币场
			if( pIServerUserItem->GetUserGold()  < pCfg->dwLevelGold)
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在进入房卡金币房, 因金币不足,无法加入"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			break;
		}
	}

	return true;
}

//检查创建桌子门票
bool CAttemperEngineSink::CheckCreateTableTicket(tagTableRule *pCfg, IServerUserItem *pIServerUserItem)
{
	//内部调用, 不校验指针
	//仅 大厅 使用, 俱乐部不使用该函数

	switch( pCfg->GameMode )
	{
	case TABLE_MODE_FK://房卡模式
		{
			//校验用户房卡: 房主支付，且用户房卡不足
			if(0 == pCfg->cbPayType && 
				(pIServerUserItem->GetUserRoomCard() < ((pCfg->GameCountType) * (pCfg->PlayerCount)) ))	  
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在创建房主支付的房卡房, 因房卡不足,无法创建"), REQUEST_FAILURE_NORMAL);
				return false;
			}

			//校验用户房卡：AA支付，且用户房卡不足
			if( (1 == pCfg->cbPayType) && 
				(pIServerUserItem->GetUserRoomCard() < pCfg->GameCountType) )
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在创建AA支付的房卡房, 因房卡不足,无法创建"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			break;
		}
	case TABLE_MODE_JJC://竞技场模式
		{
			break;
		}
	case TABLE_MODE_GOLD://金币模式
		{
			break;
		}
	case TABLE_MODE_FK_GOLD://房卡金币场
		{
			//校验用户房卡: 房主支付，且用户房卡不足
			if(0 == pCfg->cbPayType && 
				(pIServerUserItem->GetUserRoomCard() < ((pCfg->GameCountType) * (pCfg->PlayerCount)) ))	  
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在创建房主支付的房卡金币房, 因房卡不足,无法创建"), REQUEST_FAILURE_NORMAL);
				return false;
			}

			//校验用户房卡：AA支付，且用户房卡不足
			if( (1 == pCfg->cbPayType) && 
				(pIServerUserItem->GetUserRoomCard() < pCfg->GameCountType) )
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在创建AA支付的房卡金币房, 因房卡不足,无法创建"), REQUEST_FAILURE_NORMAL);
				return false;
			}

			//大厅的房卡金币场
			if( pIServerUserItem->GetUserGold()  < pCfg->dwLevelGold)
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在创建房卡金币房, 因金币不足,无法创建"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			break;
		}
	}

	return true;

}

//创建桌子 -- 金币大厅
void CAttemperEngineSink::HallTableCreate(DWORD dwUserID, DWORD dwKindID, BYTE byGameType, DWORD dwTableID)
{
    STR_DBR_HALL_GOLD_TABLE_INFO Dbr;
	ZeroMemory(&Dbr, sizeof(Dbr));
	Dbr.dwUserID = dwUserID;
	Dbr.dwKindID = dwKindID;	
	Dbr.byGameType = byGameType;
	Dbr.dwTableID =dwTableID;
	Dbr.dwTableState = 0;
	Dbr.byMask =1;

	Dbr.dwGameID = (m_pGameServiceOption->dwServerID) & 0xFFFFFF00;

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR, DBR_HALL_GOLD_TABLE_INFO, 0, &Dbr, sizeof(Dbr));
}


#pragma endregion