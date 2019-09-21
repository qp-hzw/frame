#include "StdAfx.h"
#include "ServiceUnits.h"
#include "DataBaseEngineSink.h"
#include "LogicTraceHelper.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//配置变量
	m_pGameParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//组件变量
	m_pIDataBaseEngine=NULL;
	m_pIGameServiceManager=NULL;
	m_pIDataBaseEngineEvent=NULL;
	m_pIGameDataBaseEngineSink=NULL;
	m_pIDBCorrespondManager=NULL;

	return;
}

//析构函数
CDataBaseEngineSink::~CDataBaseEngineSink()
{
	//释放对象
	SafeRelease(m_pIGameDataBaseEngineSink);

	return;
}

//接口查询
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE(IGameDataBaseEngine,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//获取对象
VOID * CDataBaseEngineSink::GetDataBase(REFGUID Guid, DWORD dwQueryVer)
{
	//效验状态
	ASSERT(m_GameDBModule.GetInterface()!=NULL);
	if (m_GameDBModule.GetInterface()==NULL) return NULL;

	//查询对象
	IDataBase * pIDataBase=m_GameDBModule.GetInterface();
	VOID * pIQueryObject=pIDataBase->QueryInterface(Guid,dwQueryVer);

	return pIQueryObject;
}

//获取对象
VOID * CDataBaseEngineSink::GetDataBaseEngine(REFGUID Guid, DWORD dwQueryVer)
{
	//效验状态
	ASSERT(m_pIDataBaseEngine!=NULL);
	if (m_pIDataBaseEngine==NULL) return NULL;

	//查询对象
	VOID * pIQueryObject=m_pIDataBaseEngine->QueryInterface(Guid,dwQueryVer);

	return pIQueryObject;
}

//投递结果
bool CDataBaseEngineSink::PostGameDataBaseResult(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return true;
}

//启动事件
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//查询对象
	ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDataBaseEngine)!=NULL);
	m_pIDataBaseEngine=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDataBaseEngine);

	//创建对象
	if ((m_GameDBModule.GetInterface()==NULL)&&(m_GameDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//创建对象
	if ((m_TreasureDBModule.GetInterface()==NULL)&&(m_TreasureDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//创建对象
	if ((m_PlatformDBModule.GetInterface()==NULL)&&(m_PlatformDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//连接游戏
	try
	{
		//一个GameID对应一个服务器, 一个服务器只会开启一个进程, 所以这边只会有一个
		//设置连接
		m_PlatformDBModule->SetConnectionInfo(1, _TEST);
		//设置连接
		m_GameDBModule->SetConnectionInfo(3, _TEST);
		//设置连接
		m_TreasureDBModule->SetConnectionInfo(3, _TEST);
		

		//发起连接
		m_GameDBModule->OpenConnection();
		m_GameDBAide.SetDataBase(m_GameDBModule.GetInterface());

		//发起连接
		m_TreasureDBModule->OpenConnection();
		m_TreasureDBAide.SetDataBase(m_TreasureDBModule.GetInterface());

		//平台数据库
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());

		//数据钩子
		ASSERT(m_pIGameServiceManager!=NULL);
		m_pIGameDataBaseEngineSink=(IGameDataBaseEngineSink *)m_pIGameServiceManager->CreateGameDataBaseEngineSink(IID_IGameDataBaseEngineSink,VER_IGameDataBaseEngineSink);

		//配置对象
		if ((m_pIGameDataBaseEngineSink!=NULL)&&(m_pIGameDataBaseEngineSink->InitializeSink(QUERY_ME_INTERFACE(IUnknownEx))==false))
		{
			//错误断言
			ASSERT(FALSE);

			//输出信息
			CLog::Log(TEXT("游戏数据库扩展钩子引擎对象配置失败"),log_error);

			return false;
		}

		//清除锁定表yang
		OnClearDB();
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CLog::Log(pIException->GetExceptionDescribe(),log_error);

		return false;
	}

	return true;
}

//停止事件
bool CDataBaseEngineSink::OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx)
{
	//清除锁定表yang
	OnClearDB();
	//配置变量
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//组件变量
	m_pIGameServiceManager=NULL;
	m_pIDataBaseEngineEvent=NULL;

	//设置对象
	m_GameDBAide.SetDataBase(NULL);

	//释放对象
	SafeRelease(m_pIGameDataBaseEngineSink);

	//关闭连接
	if (m_GameDBModule.GetInterface()!=NULL)
	{
		m_GameDBModule->CloseConnection();
		m_GameDBModule.CloseInstance();
	}

	//关闭连接
	if (m_TreasureDBModule.GetInterface()!=NULL)
	{
		m_TreasureDBModule->CloseConnection();
		m_TreasureDBModule.CloseInstance();
	}

	//关闭连接
	if (m_PlatformDBModule.GetInterface()!=NULL)
	{
		m_PlatformDBModule->CloseConnection();
		m_PlatformDBModule.CloseInstance();
	}

	return true;
}

//时间事件
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//控制事件
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//请求事件
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	bool bSucceed = false;
	DWORD dwUserID = 0L;

	//请求处理
	switch (wRequestID)
	{
	case DBR_GP_GAME_USER_STATE: //socket连接关闭
		{
			bSucceed = On_DBR_GP_QUIT(dwContextID,pData,wDataSize,dwUserID);
		}
	case DBR_CG_LOGON_USERID:			//I D 登录
		{
			bSucceed = On_DBR_Logon_UserID(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_CG_USER_CREATE_GROUP_ROOM:	//创建牌友圈房间
		{
			bSucceed = On_DBR_User_CreateGroupRoom(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_CG_USER_JOIN_GROUP_ROOM:	//加入牌友圈房间
		{
			bSucceed = On_DBR_User_JoinGroupRoom(dwContextID,pData,wDataSize,dwUserID);
		}
		break;		
	case DBR_GR_WRITE_GAME_SCORE:		//游戏写分
		{
			bSucceed = OnRequestWriteGameScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LEAVE_GAME_SERVER:		//离开游戏服
		{
			bSucceed = OnRequestLeaveGameServer(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_GAME_SCORE_RECORD:		//游戏记录
		{
			bSucceed = OnRequestGameScoreRecord(dwContextID,pData,wDataSize,dwUserID);
		}
		break;

	case DBR_GR_LOAD_ANDROID_USER:		//加载机器
		{
			bSucceed = OnRequestLoadAndroidUser(dwContextID,pData,wDataSize,dwUserID);
			break;
		}
	case DBR_GR_LOAD_OFFLINE:			//加载断线重连
		{
			bSucceed = On_DBR_GR_LOAD_OFFLINE(dwContextID,pData,wDataSize,dwUserID);
			break;
		}
	case DBR_GR_MODIFY_USER_TREASURE:		//修改用户财富信息
		{
			bSucceed = On_DBR_ModifyUserTreasure(dwContextID,pData,wDataSize,dwUserID);
			break;
		}
	case DBR_GR_SAVE_RECORDINFO:			//保存小局录像信息
		{
			bSucceed = On_DBR_SaveGameRecord(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MANAGE_USER_RIGHT:		//用户权限
		{
			bSucceed = OnRequestManageUserRight(dwContextID,pData,wDataSize,dwUserID);
		}
		break;

	case DBR_GR_MATCH_FEE:				//比赛费用
		{
			bSucceed = OnRequestMatchFee(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_START:			//比赛开始
		{
			bSucceed = OnRequestMatchStart(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_OVER:				//比赛结束
		{
			bSucceed = OnRequestMatchOver(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_REWARD:			//比赛奖励
		{
			bSucceed = OnRequestMatchReward(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_QUIT:				//退出比赛
		{
			bSucceed = OnRequestMatchQuit(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_ROOMLEVELMODIFY:				//修改房间难度
		{
			bSucceed = OnRequestRoomLevelModify(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_ROOMCONTROLVALMODIFY:				//修改房间控制值
		{
			bSucceed = OnRequestRoomControlValModify(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_WRITE_CURRENT_STOCK:
		{
			bSucceed = OnWriteCurrentStock(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_CLUB_ROOM_INFO:	//club牌友圈房间信息
		{
			bSucceed = On_DBR_CLUB_ROOM_INFO(dwContextID,pData,wDataSize);
			break;
		}
	case DBR_CLUB_TABLE_INFO: //club牌友圈桌子信息
		{
			bSucceed = On_DBR_CLUB_TABLE_INFO(dwContextID,pData,wDataSize);
			break;
		}
	case DBR_CLUB_PLAYER_INFO:	//club牌友圈玩家信息
		{
			bSucceed = On_DBR_CLUB_PLAYER_INFO(dwContextID,pData,wDataSize);
			break;
		}
	case DBR_HALL_GOLD_TABLE_INFO: //金币大厅 桌子信息
		{
			bSucceed = On_DBR_HALL_GOLD_TABLE_INFO(dwContextID,pData,wDataSize);
			break;
		}
	case DBR_HALL_GOLD_PLAYER_INFO: //金币大厅 玩家信息
		{
			bSucceed = On_DBR_HALL_GOLD_PLAYER_INFO(dwContextID,pData,wDataSize);
			break;
		}
	case DBR_GR_ADD_TABLE_INFO:		//添加开房信息
		{
			bSucceed = OnRequestAddTableInfo(dwContextID,pData,wDataSize);
		}
		break;
	case DBR_GR_START_TABLE:	//开始桌子
		{
			bSucceed = OnRequestStartTable(dwContextID,pData,wDataSize);
		}
		break;
	case DBR_GR_END_TABLE:	//结束桌子
		{
			bSucceed = OnRequestEndTable(dwContextID,pData,wDataSize);
		}
		break;
	case DBR_GR_ADD_TABLEUSER://增加桌用户
		{
			bSucceed = OnRequestAddUserToTable(dwContextID,pData,wDataSize);
		}
		break;
	case DBR_GR_DELETE_TABLEUSER://删除桌用户
		{
			bSucceed = OnRequestDeleteUserToTable(dwContextID,pData,wDataSize);
		}
		break;
	case DBR_GR_QUERY_LOTTERY:
		{
			bSucceed = OnQueryLottery(dwContextID,pData,wDataSize);
		}
		break;
	case DBR_SC_TABLE_UPDATE_TASK_STATUS:		//更新游戏任务状态
		{
			bSucceed = On_Table_UpdateGameTaskStatus(dwContextID,pData,wDataSize);
		}
		break;
	case DBR_CG_CLUB_CREATE_TABLE://牌友圈创建桌子
		{
			bSucceed = On_DBR_CG_CLUB_CREATE_TABLE(dwContextID,pData,wDataSize);
			break;
		}
	case DBR_CG_USER_JOIN_TABLE_NO_PASS://加入桌子 不需要密码,快速加入
		{
			bSucceed = On_DBR_CG_USER_JOIN_TABLE_NO_PASS(dwContextID,pData,wDataSize);
			break;
		}
	case DBR_CG_JOIN_TABLE: //加入桌子
		{
			bSucceed = On_DBR_CG_JOIN_TABLE(dwContextID,pData,wDataSize);
			break;
		}
	case DBR_CG_USER_JOIN_TABLE_HALL_GOLD: //加入桌子 -- 金币大厅桌子
		{
			bSucceed = On_DBR_CG_USER_JOIN_TABLE_HALL_GOLD(dwContextID,pData,wDataSize);
			break;
		}
	}

	//协调通知
	if(m_pIDBCorrespondManager) m_pIDBCorrespondManager->OnPostRequestComplete(dwUserID, bSucceed);

	return bSucceed;
}

//ID 登录
bool CDataBaseEngineSink::On_DBR_Logon_UserID(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//数据校验
	if (wDataSize!=sizeof(STR_DBR_CG_LOGON_USERID)) return false;

	//执行查询
	STR_DBR_CG_LOGON_USERID *pLogonUserID = (STR_DBR_CG_LOGON_USERID *)pData;
	dwUserID = pLogonUserID->dwUserID;

	try
	{
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE *pClientAddr = (BYTE *)&pLogonUserID->dwClientAddr;
		if ( NULL != pClientAddr )
		{
			_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);
		}
		
		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pLogonUserID->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@strPassword"),pLogonUserID->szPassword);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pLogonUserID->szMachineID);
		m_TreasureDBAide.AddParameter(TEXT("@dwGameID"),m_pGameServiceOption->dwGameID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CG_Logon_UserID"));

		//执行查询
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CG_Logon_UserID"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		On_DBO_Logon_UserID(dwContextID, pLogonUserID->dLongitude, pLogonUserID->dLatitude, lResultCode, CW2CT(DBVarValue.bstrVal));

		return true;	
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CLog::Log(pIException->GetExceptionDescribe(),log_error);
		return false;
	}

	return true;
}
//用户Socket连接关闭
bool CDataBaseEngineSink::On_DBR_GP_QUIT(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//效验参数
		if (wDataSize!=sizeof(DBR_GP_UserQuitInfo)) return false;

		DBR_GP_UserQuitInfo* groupInfo = (DBR_GP_UserQuitInfo*)pData;
		//构造参数
		m_PlatformDBAide.ResetParameter();

		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),groupInfo->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@byOnlineMask"),groupInfo->byOnlineMask);

		//执行输出
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_CL_Quit"),false);	

		return true;
	}
	catch(IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(),log_error);
		return false;
	}

}


//ID登录返回
bool CDataBaseEngineSink::On_DBO_Logon_UserID(DWORD dwContextID, const double &dLongitude, const double &dLatitude, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	//发送数据
	STR_DBO_CG_LOGON_USERID LogonUserID;
	ZeroMemory(&LogonUserID, sizeof(STR_DBO_CG_LOGON_USERID));

	//赋值
	LogonUserID.lResultCode = dwErrorCode;
	lstrcpyn(LogonUserID.szDescribeString, pszErrorString, CountArray(LogonUserID.szDescribeString));

	//执行成功，获取用户信息
	if ( DB_SUCCESS == dwErrorCode )
	{	
		//属性资料
		LogonUserID.wFaceID = m_TreasureDBAide.GetValue_WORD(TEXT("FaceID"));
		LogonUserID.dwUserID=m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
		LogonUserID.dwGameID=m_TreasureDBAide.GetValue_DWORD(TEXT("GameID"));
		LogonUserID.dwGroupID=m_TreasureDBAide.GetValue_DWORD(TEXT("GroupID"));
		LogonUserID.dwCustomID=m_TreasureDBAide.GetValue_DWORD(TEXT("CustomID"));
		m_TreasureDBAide.GetValue_String(TEXT("NickName"),LogonUserID.szNickName,CountArray(LogonUserID.szNickName));
		m_TreasureDBAide.GetValue_String(TEXT("GroupName"),LogonUserID.szGroupName,CountArray(LogonUserID.szGroupName));
		m_TreasureDBAide.GetValue_String(TEXT("HeadUrl"),LogonUserID.szHeadUrl,CountArray(LogonUserID.szHeadUrl));

		//用户资料
		LogonUserID.cbGender=m_TreasureDBAide.GetValue_BYTE(TEXT("Gender"));
		LogonUserID.cbMemberOrder=m_TreasureDBAide.GetValue_BYTE(TEXT("MemberOrder"));
		LogonUserID.cbMasterOrder=m_TreasureDBAide.GetValue_BYTE(TEXT("MasterOrder"));
		m_TreasureDBAide.GetValue_String(TEXT("UnderWrite"),LogonUserID.szUnderWrite,CountArray(LogonUserID.szUnderWrite));

		//积分信息
		LogonUserID.lScore=m_TreasureDBAide.GetValue_LONGLONG(TEXT("Score"));
		LogonUserID.lGrade=m_TreasureDBAide.GetValue_LONGLONG(TEXT("Grade"));
		LogonUserID.lControlScore=m_TreasureDBAide.GetValue_LONGLONG(TEXT("ControlScore"));

		//房卡信息 add by guo
		LogonUserID.lOpenRoomCard = m_TreasureDBAide.GetValue_LONGLONG(TEXT("OpenRoomCard"));
		LogonUserID.lDiamond = m_TreasureDBAide.GetValue_LONGLONG(TEXT("Diamond"));
		LogonUserID.lGold = m_TreasureDBAide.GetValue_LONGLONG(TEXT("Gold"));

		//局数信息
		LogonUserID.dwWinCount=m_TreasureDBAide.GetValue_LONG(TEXT("WinCount"));
		LogonUserID.dwLostCount=m_TreasureDBAide.GetValue_LONG(TEXT("LostCount"));
		LogonUserID.dwDrawCount=m_TreasureDBAide.GetValue_LONG(TEXT("DrawCount"));
		LogonUserID.dwFleeCount=m_TreasureDBAide.GetValue_LONG(TEXT("FleeCount"));
		LogonUserID.dwUserMedal=m_TreasureDBAide.GetValue_LONG(TEXT("UserMedal"));
		LogonUserID.dwExperience=m_TreasureDBAide.GetValue_LONG(TEXT("Experience"));
		LogonUserID.lLoveLiness=m_TreasureDBAide.GetValue_LONG(TEXT("LoveLiness"));

		//附加信息
		LogonUserID.dwUserRight=m_TreasureDBAide.GetValue_DWORD(TEXT("UserRight"));
		LogonUserID.dwMasterRight=m_TreasureDBAide.GetValue_DWORD(TEXT("MasterRight"));
		LogonUserID.cbDeviceType = DEVICE_TYPE_PC;
		LogonUserID.wBehaviorFlags = 0;
		LogonUserID.wPageTableCount = 0;

		//位置信息
		LogonUserID.dLatitude = dLatitude;
		LogonUserID.dLongitude = dLongitude;

		//索引变量
		LogonUserID.dwInoutIndex = m_TreasureDBAide.GetValue_DWORD(TEXT("InoutIndex"));
	}

	//发送数据
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CG_LOGON_USERID, dwContextID, &LogonUserID, sizeof(STR_DBO_CG_LOGON_USERID));
	return true;
}

//创建牌友圈房间
bool CDataBaseEngineSink::On_DBR_User_CreateGroupRoom(DWORD dwContextID, VOID *pData, WORD wDataSize, DWORD &dwUserID)
{
	//数据
	STR_DBR_CG_USER_CREATE_GROUP_ROOM *pDBRCreateGoupRoom = (STR_DBR_CG_USER_CREATE_GROUP_ROOM *)pData;
	dwUserID = pDBRCreateGoupRoom->dwUserID;

	//效验参数
	ASSERT(wDataSize == sizeof(STR_DBR_CG_USER_CREATE_GROUP_ROOM));
	if (wDataSize != sizeof(STR_DBR_CG_USER_CREATE_GROUP_ROOM)) return false;

	try
	{
		tagTableRule *pCfg = (tagTableRule* )pDBRCreateGoupRoom->rule;
		if (NULL == pCfg)
			return false;
		
		//构造输入参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"), pDBRCreateGoupRoom->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@dwGroupID"), pDBRCreateGoupRoom->dwGroupID);
		m_TreasureDBAide.AddParameter(TEXT("@RoomPayCost"), pCfg->lSinglePayCost);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CG_User_CreateGroupRoom"));

		//执行查询
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CG_User_CreateGroupRoom"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

		On_DBO_User_CreateGroupRoom(dwContextID, pDBRCreateGoupRoom->rule, lResultCode, CW2CT(DBVarValue.bstrVal));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CLog::Log(pIException->GetExceptionDescribe(),log_error);
		return false;
	}

	return true;
}

//创建牌友圈房间返回
VOID CDataBaseEngineSink::On_DBO_User_CreateGroupRoom(DWORD dwContextID, BYTE RULE[LEN_PRIVATE_TABLE_RULE], DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	//构造数据
	STR_DBO_CG_USER_CREATE_GROUP_ROOM CreateGroupRoom;
	ZeroMemory(&CreateGroupRoom, sizeof(STR_DBO_CG_USER_CREATE_GROUP_ROOM));

	//描述消息
	CreateGroupRoom.dwResultCode = dwErrorCode;
	lstrcpyn(CreateGroupRoom.szDescribeString, pszErrorString, sizeof(STR_DBO_CG_USER_CREATE_GROUP_ROOM) );

	//执行成功
	if(dwErrorCode == DB_SUCCESS)
	{
		//属性资料
		CreateGroupRoom.dwUserID = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
		CreateGroupRoom.dwGroupID = m_TreasureDBAide.GetValue_DWORD(TEXT("GroupID"));

		//规则
		memcpy_s(CreateGroupRoom.rule, sizeof(CreateGroupRoom.rule), RULE, sizeof(CreateGroupRoom.rule));
	}

	//发送数据
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CG_USER_CREATE_GROUP_ROOM, dwContextID, &CreateGroupRoom, sizeof(STR_DBO_CG_USER_CREATE_GROUP_ROOM));
}

//加入牌友圈房间
bool CDataBaseEngineSink::On_DBR_User_JoinGroupRoom(DWORD dwContextID, VOID *pData, WORD wDataSize, DWORD &dwUserID)
{
	//数据
	STR_DBR_CG_USER_JOIN_GROUP_ROOM *pDBRJoinGoupRoom = (STR_DBR_CG_USER_JOIN_GROUP_ROOM *)pData;
	dwUserID = pDBRJoinGoupRoom->dwUserID;

	//效验参数
	ASSERT(wDataSize == sizeof(STR_DBR_CG_USER_JOIN_GROUP_ROOM));
	if (wDataSize != sizeof(STR_DBR_CG_USER_JOIN_GROUP_ROOM)) return false;

	try
	{		
		//构造输入参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"), pDBRJoinGoupRoom->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@dwGroupID"), pDBRJoinGoupRoom->dwGroupID);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CG_User_JoinGroupRoom"));

		//执行查询
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CG_User_JoinGroupRoom"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

		On_DBO_User_JoinGroupRoom(dwContextID, pDBRJoinGoupRoom->dwGroupID, lResultCode, CW2CT(DBVarValue.bstrVal));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CLog::Log(pIException->GetExceptionDescribe(),log_error);
		return false;
	}

	return true;
}

//加入牌友圈房间返回
VOID CDataBaseEngineSink::On_DBO_User_JoinGroupRoom(DWORD dwContextID,  DWORD dwGroupID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	//构造数据
	STR_DBO_CG_USER_JOIN_GROUP_ROOM JoinGroupRoom;
	ZeroMemory(&JoinGroupRoom, sizeof(STR_DBO_CG_USER_JOIN_GROUP_ROOM));

	//描述消息
	JoinGroupRoom.dwResultCode = dwErrorCode;
	lstrcpyn(JoinGroupRoom.szDescribeString, pszErrorString, sizeof(STR_DBO_CG_USER_JOIN_GROUP_ROOM) );

	//执行成功
	if(dwErrorCode == DB_SUCCESS)
	{
		JoinGroupRoom.dwGroupID = dwGroupID/*m_TreasureDBAide.GetValue_DWORD(TEXT("GroupID"))*/;
	}

	//发送数据
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CG_USER_JOIN_GROUP_ROOM, dwContextID, &JoinGroupRoom, sizeof(STR_DBO_CG_USER_JOIN_GROUP_ROOM));
}


//游戏写分
bool CDataBaseEngineSink::OnRequestWriteGameScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//请求处理
	DBR_GR_WriteGameScore * pWriteGameScore=(DBR_GR_WriteGameScore *)pData;
	dwUserID=pWriteGameScore->dwUserID;

	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_WriteGameScore));
		if (wDataSize!=sizeof(DBR_GR_WriteGameScore)) return false;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pWriteGameScore->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_GameDBAide.ResetParameter();

		//用户信息
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pWriteGameScore->dwUserID);

		//变更成绩
		m_GameDBAide.AddParameter(TEXT("@lVariationScore"),pWriteGameScore->VariationInfo.lScore);
		m_GameDBAide.AddParameter(TEXT("@lVariationGrade"),pWriteGameScore->VariationInfo.lGrade);
		m_GameDBAide.AddParameter(TEXT("@lVariationInsure"),pWriteGameScore->VariationInfo.lInsure);
		m_GameDBAide.AddParameter(TEXT("@lVariationRevenue"),pWriteGameScore->VariationInfo.lRevenue);
		m_GameDBAide.AddParameter(TEXT("@lVariationWinCount"),pWriteGameScore->VariationInfo.dwWinCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationLostCount"),pWriteGameScore->VariationInfo.dwLostCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationDrawCount"),pWriteGameScore->VariationInfo.dwDrawCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationFleeCount"),pWriteGameScore->VariationInfo.dwFleeCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationUserMedal"),pWriteGameScore->VariationInfo.dwUserMedal);
		m_GameDBAide.AddParameter(TEXT("@lVariationExperience"),pWriteGameScore->VariationInfo.dwExperience);
		m_GameDBAide.AddParameter(TEXT("@lVariationLoveLiness"),pWriteGameScore->VariationInfo.lLoveLiness);
		m_GameDBAide.AddParameter(TEXT("@dwVariationPlayTimeCount"),pWriteGameScore->VariationInfo.dwPlayTimeCount);

		//控制值
		m_GameDBAide.AddParameter(TEXT("@lControlScore"),pWriteGameScore->lControlScore);

		//抽水信息
		m_GameDBAide.AddParameter(TEXT("@lChoushui"),pWriteGameScore->VariationInfo.lChoushui);;
		m_GameDBAide.AddParameter(TEXT("@dwChoushuiType"),pWriteGameScore->VariationInfo.dwChoushuiType);;

		//属性信息
		//TODONOW 
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->dwGameID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->dwServerID);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出执行
		LogicTraceHelper(TEXT("GSP_GR_WriteGameScore"));

		//执行查询
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_WriteGameScore"),true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(),log_error);

		return false;
	}

	return true;
}

//修改用户财富
bool CDataBaseEngineSink::On_DBR_ModifyUserTreasure(DWORD dwContextID, void * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//数据大小校验
		if (sizeof(STR_DBR_GR_MODIFY_USER_TREASURE) != wDataSize)
			return false;

		//构造数据
		STR_DBR_GR_MODIFY_USER_TREASURE* pDBR = (STR_DBR_GR_MODIFY_USER_TREASURE*)pData;

		//输入参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@UserID"),pDBR->dwUserId);
		m_TreasureDBAide.AddParameter(TEXT("@dwKind"),pDBR->dwKind);
		m_TreasureDBAide.AddParameter(TEXT("@dwTableID"),pDBR->dwTableID);
		m_TreasureDBAide.AddParameter(TEXT("@byTableMode"),pDBR->byTableMode);
		m_TreasureDBAide.AddParameter(TEXT("@byRound"),pDBR->byRound);
		m_TreasureDBAide.AddParameter(TEXT("@lUserTreasure"),pDBR->lUserTreasure);

		TCHAR szString[512]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),
			TEXT("Kind:%ld, TableID:%ld, TableMode:%d, Round:%d, UserID:%ld, Treasure:%ld"),
			pDBR->dwKind,
			pDBR->dwTableID,
			pDBR->byTableMode,
			pDBR->byRound,
			pDBR->dwUserId,
			pDBR->lUserTreasure
			);

		//提示消息
		CLog::Log(szString,log_debug);

		BYTE byWinOrLose = 0; //0平;  1赢;  2负

		if( pDBR->byRound != 0xFF) //小局
		{
			if(pDBR->lUserTreasure > 0)
			{
				byWinOrLose = 1;
			}
			else if(pDBR->lUserTreasure < 0)
			{
				byWinOrLose = 2;
			}
		}
		else //大局
		{
			byWinOrLose = pDBR->byWin;
		}
		m_TreasureDBAide.AddParameter(TEXT("@byWinOrLose"),byWinOrLose);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_MODIFY_USER_TREASURE_TABLE"));

		//执行查询
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_MODIFY_USER_TREASURE_TABLE"), true);

		//结果处理
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

		//执行成功
		if(DB_SUCCESS == lResultCode)
		{
			//构造数据
			STR_DBO_GR_MODIFY_USER_TREASURE ModifyTreasure;
			ZeroMemory(&ModifyTreasure, sizeof(STR_DBO_GR_MODIFY_USER_TREASURE));

			//描述消息
			ModifyTreasure.dwResultCode = lResultCode;
			lstrcpyn(ModifyTreasure.szDescribeString, CW2CT(DBVarValue.bstrVal), CountArray(ModifyTreasure.szDescribeString) );

			ModifyTreasure.dwUserID = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
			//用户游戏币
			ModifyTreasure.lUserDiamond = m_TreasureDBAide.GetValue_LONGLONG(TEXT("UserDiamond"));
			//用户游戏币
			ModifyTreasure.lUserGold = m_TreasureDBAide.GetValue_LONGLONG(TEXT("UserGold"));
			//用户房卡
			ModifyTreasure.lOpenRoomCard = m_TreasureDBAide.GetValue_LONGLONG(TEXT("OpenRoomCard"));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_SG_MODIFY_USER_TREASURE, dwContextID, &ModifyTreasure, sizeof(STR_DBO_GR_MODIFY_USER_TREASURE));
		}
#pragma region log日志
		else
		{
			//错误信息写入log日志
			TCHAR szString[512]=TEXT("");
			_sntprintf_s(szString,CountArray(szString),
				TEXT("lResultCode = %ld, UserID = %ld, dwKind = %ld, dwTableID = %ld, byTableMode = %d, byRound = %d, lUserTreasure = %ld"),
				lResultCode,
				pDBR->dwUserId,
				pDBR->dwKind,
				pDBR->dwTableID,
				pDBR->byTableMode,
				pDBR->byRound,
				pDBR->lUserTreasure
				);

			//提示消息
			CLog::Log(szString,log_warn);
		}
#pragma endregion
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(),log_error);

		return false;
	}
}

//保存小局录像信息
bool CDataBaseEngineSink::On_DBR_SaveGameRecord(DWORD dwContextID, void * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		if (sizeof(DBR_GR_GameRecordInfo) != wDataSize)
			return false;

		//入参
		DBR_GR_GameRecordInfo *pDbReq = (DBR_GR_GameRecordInfo*)pData;

		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwTableID"),pDbReq->dwTableID);
		m_TreasureDBAide.AddParameter(TEXT("@curRound"),pDbReq->wCurrentCount);
		CString szData = toHexString(pDbReq->szData, LEN_MAX_RECORD_SIZE);
		m_TreasureDBAide.AddParameter(TEXT("@VideoData"),szData);
		m_TreasureDBAide.AddParameter(TEXT("@VideoSize"),LEN_MAX_RECORD_SIZE);
		
		m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_SaveGameRecord"), true);
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(),log_error);

		return false;
	}

	return true;
}

//离开房间
bool CDataBaseEngineSink::OnRequestLeaveGameServer(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//请求处理
	DBR_GR_LeaveGameServer * pLeaveGameServer=(DBR_GR_LeaveGameServer *)pData;
	dwUserID=pLeaveGameServer->dwUserID;

	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_LeaveGameServer));
		if (wDataSize!=sizeof(DBR_GR_LeaveGameServer)) return false;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLeaveGameServer->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_GameDBAide.ResetParameter();

		//用户信息
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pLeaveGameServer->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwOnLineTimeCount"),pLeaveGameServer->dwOnLineTimeCount);

		//系统信息
		m_GameDBAide.AddParameter(TEXT("@dwInoutIndex"),pLeaveGameServer->dwInoutIndex);
		m_GameDBAide.AddParameter(TEXT("@dwLeaveReason"),pLeaveGameServer->dwLeaveReason);

		//记录成绩
		m_GameDBAide.AddParameter(TEXT("@lRecordScore"),pLeaveGameServer->RecordInfo.lScore);
		m_GameDBAide.AddParameter(TEXT("@lRecordGrade"),pLeaveGameServer->RecordInfo.lGrade);
		m_GameDBAide.AddParameter(TEXT("@lRecordInsure"),pLeaveGameServer->RecordInfo.lInsure);
		m_GameDBAide.AddParameter(TEXT("@lRecordRevenue"),pLeaveGameServer->RecordInfo.lRevenue);
		m_GameDBAide.AddParameter(TEXT("@lRecordWinCount"),pLeaveGameServer->RecordInfo.dwWinCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordLostCount"),pLeaveGameServer->RecordInfo.dwLostCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordDrawCount"),pLeaveGameServer->RecordInfo.dwDrawCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordFleeCount"),pLeaveGameServer->RecordInfo.dwFleeCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordUserMedal"),pLeaveGameServer->RecordInfo.dwUserMedal);
		m_GameDBAide.AddParameter(TEXT("@lRecordExperience"),pLeaveGameServer->RecordInfo.dwExperience);
		m_GameDBAide.AddParameter(TEXT("@lRecordLoveLiness"),pLeaveGameServer->RecordInfo.lLoveLiness);
		m_GameDBAide.AddParameter(TEXT("@dwRecordPlayTimeCount"),pLeaveGameServer->RecordInfo.dwPlayTimeCount);

		//变更成绩
		m_GameDBAide.AddParameter(TEXT("@lVariationScore"),pLeaveGameServer->VariationInfo.lScore);
		m_GameDBAide.AddParameter(TEXT("@lVariationGrade"),pLeaveGameServer->VariationInfo.lGrade);
		m_GameDBAide.AddParameter(TEXT("@lVariationInsure"),pLeaveGameServer->VariationInfo.lInsure);
		m_GameDBAide.AddParameter(TEXT("@lVariationRevenue"),pLeaveGameServer->VariationInfo.lRevenue);
		m_GameDBAide.AddParameter(TEXT("@lVariationWinCount"),pLeaveGameServer->VariationInfo.dwWinCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationLostCount"),pLeaveGameServer->VariationInfo.dwLostCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationDrawCount"),pLeaveGameServer->VariationInfo.dwDrawCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationFleeCount"),pLeaveGameServer->VariationInfo.dwFleeCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationUserMedal"),pLeaveGameServer->VariationInfo.dwUserMedal);
		m_GameDBAide.AddParameter(TEXT("@lVariationExperience"),pLeaveGameServer->VariationInfo.dwExperience);
		m_GameDBAide.AddParameter(TEXT("@lVariationLoveLiness"),pLeaveGameServer->VariationInfo.lLoveLiness);
		m_GameDBAide.AddParameter(TEXT("@dwVariationPlayTimeCount"),pLeaveGameServer->VariationInfo.dwPlayTimeCount);

		//////控制信息(不需要写)
		//m_GameDBAide.AddParameter(TEXT("@lControlScore"),pLeaveGameServer->lControlScore);

		////抽水信息
		//m_GameDBAide.AddParameter(TEXT("@lChoushui"),pLeaveGameServer->VariationInfo.lChoushui);
		//m_GameDBAide.AddParameter(TEXT("@dwChoushuiType"),pLeaveGameServer->VariationInfo.dwChoushuiType);

		//其他参数
		//TODONOW 
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->dwGameID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->dwServerID);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pLeaveGameServer->szMachineID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_GR_LeaveGameServer"));

		//执行查询
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_LeaveGameServer"),true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(),log_error);

		return false;
	}

	return true;
}

//游戏记录
bool CDataBaseEngineSink::OnRequestGameScoreRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//变量定义
		DBR_GR_GameScoreRecord * pGameScoreRecord=(DBR_GR_GameScoreRecord *)pData;
		dwUserID=INVALID_DWORD;

		//效验参数
		ASSERT(wDataSize<=sizeof(DBR_GR_GameScoreRecord));
		ASSERT(wDataSize>=(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord)));
		ASSERT(wDataSize==(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord)+pGameScoreRecord->wRecordCount*sizeof(pGameScoreRecord->GameScoreRecord[0])));

		//效验参数
		if (wDataSize>sizeof(DBR_GR_GameScoreRecord)) return false;
		if (wDataSize<(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord))) return false;
		if (wDataSize!=(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord)+pGameScoreRecord->wRecordCount*sizeof(pGameScoreRecord->GameScoreRecord[0]))) return false;

		//房间信息
		m_GameDBAide.ResetParameter();
		//TODONOW 
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->dwGameID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->dwServerID);

		//桌子信息
		m_GameDBAide.AddParameter(TEXT("@wTableID"),pGameScoreRecord->wTableID);
		m_GameDBAide.AddParameter(TEXT("@wUserCount"),pGameScoreRecord->wUserCount);
		m_GameDBAide.AddParameter(TEXT("@wAndroidCount"),pGameScoreRecord->wAndroidCount);

		//税收损耗
		m_GameDBAide.AddParameter(TEXT("@lWasteCount"),pGameScoreRecord->lWasteCount);
		m_GameDBAide.AddParameter(TEXT("@lRevenueCount"),pGameScoreRecord->lRevenueCount);

		//统计信息
		m_GameDBAide.AddParameter(TEXT("@dwUserMemal"),pGameScoreRecord->dwUserMemal);
		m_GameDBAide.AddParameter(TEXT("@dwPlayTimeCount"),pGameScoreRecord->dwPlayTimeCount);

		//时间信息
		m_GameDBAide.AddParameter(TEXT("@SystemTimeStart"),pGameScoreRecord->SystemTimeStart);
		m_GameDBAide.AddParameter(TEXT("@SystemTimeConclude"),pGameScoreRecord->SystemTimeConclude);

		//添加库存相关yang
		m_GameDBAide.AddParameter(TEXT("@ChangeStockScore"),pGameScoreRecord->wChangeStockScore);

		//输出执行
		LogicTraceHelper(TEXT("GSP_GR_RecordDrawInfo"));

		//执行查询
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_RecordDrawInfo"),true);

		//写入记录
		if (lResultCode==DB_SUCCESS)
		{
			//获取标识
			DWORD dwDrawID=m_GameDBAide.GetValue_DWORD(TEXT("DrawID"));

			//输出执行
			LogicTraceHelper(TEXT("GSP_GR_RecordDrawScore"));

			//写入记录
			for (WORD i=0;i<pGameScoreRecord->wRecordCount;i++)
			{
				//重置参数
				m_GameDBAide.ResetParameter();

				//房间信息
				m_GameDBAide.AddParameter(TEXT("@dwDrawID"),dwDrawID);
				m_GameDBAide.AddParameter(TEXT("@dwUserID"),pGameScoreRecord->GameScoreRecord[i].dwUserID);
				m_GameDBAide.AddParameter(TEXT("@wChairID"),pGameScoreRecord->GameScoreRecord[i].wChairID);

				//成绩信息
				m_GameDBAide.AddParameter(TEXT("@lScore"),pGameScoreRecord->GameScoreRecord[i].lScore);
				m_GameDBAide.AddParameter(TEXT("@lGrade"),pGameScoreRecord->GameScoreRecord[i].lGrade);
				m_GameDBAide.AddParameter(TEXT("@lRevenue"),pGameScoreRecord->GameScoreRecord[i].lRevenue);
				m_GameDBAide.AddParameter(TEXT("@lControlScore"),pGameScoreRecord->GameScoreRecord[i].lControlScore);
				m_GameDBAide.AddParameter(TEXT("@lChoushui"),pGameScoreRecord->GameScoreRecord[i].lChoushui);
				m_GameDBAide.AddParameter(TEXT("@dwChoushuiType"),pGameScoreRecord->GameScoreRecord[i].dwChoushuiType);
				m_GameDBAide.AddParameter(TEXT("@dwUserMedal"),pGameScoreRecord->GameScoreRecord[i].dwUserMemal);
				m_GameDBAide.AddParameter(TEXT("@dwPlayTimeCount"),pGameScoreRecord->GameScoreRecord[i].dwPlayTimeCount);

				CLog::TraceStringEx(log_debug,TEXT("GSP_GR_RecordDrawInfo UserID: %ld, lScore:%I64d, lRevenue: %I64d, lChoushui: %I64d"), 
					pGameScoreRecord->GameScoreRecord[i].dwUserID,
					pGameScoreRecord->GameScoreRecord[i].lScore,
					pGameScoreRecord->GameScoreRecord[i].lRevenue,
					pGameScoreRecord->GameScoreRecord[i].lChoushui);
				//执行查询
				m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_RecordDrawScore"),true);
			}
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(),log_error);

		return false;
	}

	return true;
}


//加载机器
bool CDataBaseEngineSink::OnRequestLoadAndroidUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//变量定义
		DBO_GR_GameAndroidInfo GameAndroidInfo;
		ZeroMemory(&GameAndroidInfo,sizeof(GameAndroidInfo));

		//用户帐户
		m_GameDBAide.ResetParameter();
		//TOODNOW WangChengQing sql中需要同步修改 wKindID 为GameID
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceAttrib->dwGameID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->dwServerID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_GR_LoadAndroidUser"));

		//执行查询
		GameAndroidInfo.lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_LoadAndroidUser"),true);


		//读取信息
		for (WORD i=0;i<CountArray(GameAndroidInfo.AndroidParameter);i++)
		{
			//结束判断
			if (m_GameDBModule->IsRecordsetEnd()==true) break;

			//溢出效验
			ASSERT(GameAndroidInfo.wAndroidCount<CountArray(GameAndroidInfo.AndroidParameter));
			if (GameAndroidInfo.wAndroidCount>=CountArray(GameAndroidInfo.AndroidParameter)) break;

			//读取数据
			GameAndroidInfo.wAndroidCount++;
			GameAndroidInfo.AndroidParameter[i].dwUserID=m_GameDBAide.GetValue_DWORD(TEXT("UserID"));
			GameAndroidInfo.AndroidParameter[i].dwServiceTime=m_GameDBAide.GetValue_DWORD(TEXT("ServiceTime"));
			GameAndroidInfo.AndroidParameter[i].dwMinPlayDraw=m_GameDBAide.GetValue_DWORD(TEXT("MinPlayDraw"));
			GameAndroidInfo.AndroidParameter[i].dwMaxPlayDraw=m_GameDBAide.GetValue_DWORD(TEXT("MaxPlayDraw"));
			GameAndroidInfo.AndroidParameter[i].dwMinReposeTime=m_GameDBAide.GetValue_DWORD(TEXT("MinReposeTime"));
			GameAndroidInfo.AndroidParameter[i].dwMaxReposeTime=m_GameDBAide.GetValue_DWORD(TEXT("MaxReposeTime"));
			GameAndroidInfo.AndroidParameter[i].dwServiceGender=m_GameDBAide.GetValue_DWORD(TEXT("ServiceGender"));
			GameAndroidInfo.AndroidParameter[i].lMinTakeScore=m_GameDBAide.GetValue_LONGLONG(TEXT("MinTakeScore"));
			GameAndroidInfo.AndroidParameter[i].lMaxTakeScore=m_GameDBAide.GetValue_LONGLONG(TEXT("MaxTakeScore"));

			//移动记录
			m_GameDBModule->MoveToNext();
		}

		//发送信息
		WORD wHeadSize=sizeof(GameAndroidInfo)-sizeof(GameAndroidInfo.AndroidParameter);
		WORD wDataSize=GameAndroidInfo.wAndroidCount*sizeof(GameAndroidInfo.AndroidParameter[0]);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_GAME_ANDROID_INFO,dwContextID,&GameAndroidInfo,wHeadSize+wDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(),log_error);

		//变量定义
		DBO_GR_GameAndroidInfo GameAndroidInfo;
		ZeroMemory(&GameAndroidInfo,sizeof(GameAndroidInfo));

		//构造变量
		GameAndroidInfo.wAndroidCount=0L;
		GameAndroidInfo.lResultCode=DB_ERROR;

		//发送信息
		WORD wHeadSize=sizeof(GameAndroidInfo)-sizeof(GameAndroidInfo.AndroidParameter);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_GAME_ANDROID_INFO,dwContextID,&GameAndroidInfo,wHeadSize);
	}

	return false;
}

//加载断线重连
bool CDataBaseEngineSink::On_DBR_GR_LOAD_OFFLINE(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//参数校验
		if(wDataSize != sizeof(STR_DBR_GR_LOAD_OFFLINE)) return false;
		STR_DBR_GR_LOAD_OFFLINE *pDbr = (STR_DBR_GR_LOAD_OFFLINE*) pData;

		//用户帐户
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@mystery"),pDbr->byMystery);
		m_PlatformDBAide.AddParameter(TEXT("@dwGameID"),pDbr->dwGameID);

		//执行查询
		LogicTraceHelper(TEXT("GSP_GS_GAMEPROGRESS_INFO"));
		long lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_GAMEPROGRESS_INFO"),true);

		if(DB_SUCCESS == lResultCode)
		{
			BYTE GameProgress = m_PlatformDBAide.GetValue_BYTE(TEXT("GameProgress"));
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOAD_OFFLINE,dwContextID,&GameProgress,sizeof(GameProgress));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(),log_error);
	}

	return false;
}

//比赛报名
bool CDataBaseEngineSink::OnRequestMatchFee(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_MatchFee));
	if (wDataSize!=sizeof(DBR_GR_MatchFee)) return false;

	//变量定义
	DBR_GR_MatchFee * pMatchFee=(DBR_GR_MatchFee *)pData;
	dwUserID=INVALID_DWORD;

	//请求处理
	try
	{
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pMatchFee->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pMatchFee->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchFee"),pMatchFee->dwMatchFee);
		//TODONOW 
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->dwGameID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->dwServerID);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pMatchFee->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchNO"),pMatchFee->dwMatchNO);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pMatchFee->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_GR_UserMatchFee"));

		//结果处理
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_UserMatchFee"),true);

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_FEE_RESULT,dwContextID,&lReturnValue,sizeof(lReturnValue));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CLog::Log(pIException->GetExceptionDescribe(),log_error);

		return false;
	}

	return true;
}

//比赛开始
bool CDataBaseEngineSink::OnRequestMatchStart(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_MatchStart));
	if (wDataSize!=sizeof(DBR_GR_MatchStart)) return false;

	//变量定义
	DBR_GR_MatchStart * pMatchStart=(DBR_GR_MatchStart *)pData;
	dwUserID=INVALID_DWORD;

	//请求处理
	try
	{
		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wMatchID"),pMatchStart->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@wMatchNo"),pMatchStart->dwMatchNo);
		m_GameDBAide.AddParameter(TEXT("@wMatchCount"),pMatchStart->wMatchCount);	
		m_GameDBAide.AddParameter(TEXT("@lInitScore"),pMatchStart->lInitScore);	

		// 		//输出参数
		// 		TCHAR szDescribeString[128]=TEXT("");
		// 		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);
		//输出执行
		LogicTraceHelper(TEXT("GSP_GR_MatchStart"));

		//结果处理
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_MatchStart"),true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CLog::Log(pIException->GetExceptionDescribe(),log_error);

		return false;
	}

	return true;
}

//比赛结束
bool CDataBaseEngineSink::OnRequestMatchOver(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_MatchOver));
	if (wDataSize!=sizeof(DBR_GR_MatchOver)) return false;

	//变量定义
	DBR_GR_MatchOver * pMatchOver=(DBR_GR_MatchOver *)pData;
	dwUserID=INVALID_DWORD;

	//请求处理
	try
	{
		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wMatchID"),pMatchOver->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@wMatchNo"),pMatchOver->dwMatchNo);
		m_GameDBAide.AddParameter(TEXT("@wMatchCount"),pMatchOver->wMatchCount);		

		// 		//输出参数
		// 		TCHAR szDescribeString[128]=TEXT("");
		// 		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_GR_MatchOver"));

		//结果处理
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_MatchOver"),true);

		if(lReturnValue==DB_SUCCESS)
		{
			LONG lCount=0;
			if(m_GameDBModule.GetInterface()!=NULL)
				lCount=m_GameDBModule->GetRecordCount();
			DBO_GR_MatchRank *pMatchRank=new DBO_GR_MatchRank[lCount];
			for(LONG i=0; i<lCount; i++)
			{
				pMatchRank[i].cbRank=(BYTE)i;
				m_GameDBAide.GetValue_String(TEXT("NickName"),pMatchRank[i].szNickName,CountArray(pMatchRank[i].szNickName));
				pMatchRank[i].lMatchScore=m_GameDBAide.GetValue_LONG(TEXT("HScore"));
				m_GameDBModule->MoveToNext();
				if(m_GameDBModule->IsRecordsetEnd()) break;
			}
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_RANK,dwContextID,pMatchRank,(WORD)(sizeof(DBO_GR_MatchRank)*lCount));
			if(pMatchRank!=NULL)
				SafeDeleteArray(pMatchRank);
		}


		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CLog::Log(pIException->GetExceptionDescribe(),log_error);

		return false;
	}

	return true;
}

//比赛奖励
bool CDataBaseEngineSink::OnRequestMatchReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_MatchReward));
	if (wDataSize!=sizeof(DBR_GR_MatchReward)) return false;

	//变量定义
	DBR_GR_MatchReward * pMatchReward=(DBR_GR_MatchReward *)pData;
	dwUserID=INVALID_DWORD;

	//请求处理
	try
	{
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pMatchReward->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pMatchReward->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pMatchReward->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchNO"),pMatchReward->dwMatchNO);
		m_GameDBAide.AddParameter(TEXT("@wRank"),pMatchReward->wRank);
		m_GameDBAide.AddParameter(TEXT("@lMatchScore"),pMatchReward->lMatchScore);
		m_GameDBAide.AddParameter(TEXT("@dwExperience"),pMatchReward->dwExperience);
		m_GameDBAide.AddParameter(TEXT("@dwGold"),pMatchReward->dwGold);
		m_GameDBAide.AddParameter(TEXT("@dwMedal"),pMatchReward->dwMedal);
		//TODONWO 
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->dwGameID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->dwServerID);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出执行
		LogicTraceHelper(TEXT("GSP_GR_MatchReward"));

		//结果处理
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_MatchReward"),true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CLog::Log(pIException->GetExceptionDescribe(),log_error);

		return false;
	}

	return true;
}

//退出比赛
bool CDataBaseEngineSink::OnRequestMatchQuit(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_MatchFee));
	if (wDataSize!=sizeof(DBR_GR_MatchFee)) return false;

	//变量定义
	DBR_GR_MatchFee * pMatchFee=(DBR_GR_MatchFee *)pData;
	dwUserID=INVALID_DWORD;

	//请求处理
	try
	{
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pMatchFee->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pMatchFee->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchFee"),pMatchFee->dwMatchFee);
		//TOODNOW 
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->dwGameID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->dwServerID);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pMatchFee->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchNO"),pMatchFee->dwMatchNO);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pMatchFee->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_GR_UserMatchQuit"));

		//结果处理
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_UserMatchQuit"),true);

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_QUIT_RESULT,dwContextID,&lReturnValue,sizeof(lReturnValue));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CLog::Log(pIException->GetExceptionDescribe(),log_error);

		return false;
	}
}

bool CDataBaseEngineSink::OnRequestRoomLevelModify(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_RoomLevelModify));
	if (wDataSize!=sizeof(DBR_GR_RoomLevelModify)) return false;

	//变量定义
	DBR_GR_RoomLevelModify * pRoomLevelModify=(DBR_GR_RoomLevelModify *)pData;


	//请求处理
	try
	{
		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wServerID"),pRoomLevelModify->wSeverID);
		m_PlatformDBAide.AddParameter(TEXT("@wGameLevel"),pRoomLevelModify->wGameLevel);

		//输出执行
		LogicTraceHelper(TEXT("GSP_GR_RoomLevelModify"));

		//结果处理
		LONG lReturnValue=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_RoomLevelModify"),false);

		//发送结果
		if (lReturnValue == DB_SUCCESS)
		{
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_ROOMLEVELMODIFY,dwContextID,pRoomLevelModify,wDataSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CLog::Log(pIException->GetExceptionDescribe(),log_error);

		return false;
	}
}

bool CDataBaseEngineSink::OnRequestRoomControlValModify(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_RoomControlValModify));
	if (wDataSize!=sizeof(DBR_GR_RoomControlValModify)) return false;

	//变量定义
	DBR_GR_RoomControlValModify * pRoomControlVal=(DBR_GR_RoomControlValModify *)pData;


	//请求处理
	try
	{
		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wServerID"),pRoomControlVal->wSeverID);
		m_PlatformDBAide.AddParameter(TEXT("@wControlVal"),pRoomControlVal->wControlVal);

		//输出执行
		LogicTraceHelper(TEXT("GSP_GR_RoomControlVal"));

		//结果处理
		LONG lReturnValue=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_RoomControlVal"),false);

		if ( lReturnValue == DB_SUCCESS)
		{
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_ROOMCONTROLVALMODIFY,dwContextID,pRoomControlVal,wDataSize);
		}


		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CLog::Log(pIException->GetExceptionDescribe(),log_error);

		return false;
	}

	return true;
}

//写当前库存
bool CDataBaseEngineSink::OnWriteCurrentStock( DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID )
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_WriteCurrentStock));
		if (wDataSize!=sizeof(DBR_GR_WriteCurrentStock)) return false;

		DBR_GR_WriteCurrentStock * pWriteStock = (DBR_GR_WriteCurrentStock *)pData;
		//房间信息
		m_PlatformDBAide.ResetParameter();

		m_PlatformDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->dwServerID);
		m_PlatformDBAide.AddParameter(TEXT("@ConfigName"),1);

		TCHAR  extend[50] = TEXT("");
		wsprintf( extend,TEXT("%d"), pWriteStock->wCurrentStock);
		m_PlatformDBAide.AddParameter(TEXT("@CurrentStockScore"),extend);


		//输出执行
		LogicTraceHelper(TEXT("GSP_GR_WriteStockScore"));

		//执行查询
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_WriteStockScore"),true);

		return true;
	}
	catch(IDataBaseException * pIException)
	{
		//错误信息
		CLog::Log(pIException->GetExceptionDescribe(),log_error);
		return false;
	}
	return true;
}

//用户权限
bool CDataBaseEngineSink::OnRequestManageUserRight(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_ManageUserRight));
		if (wDataSize!=sizeof(DBR_GR_ManageUserRight)) return false;

		//请求处理
		DBR_GR_ManageUserRight * pManageUserRight=(DBR_GR_ManageUserRight *)pData;
		dwUserID=pManageUserRight->dwUserID;

		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pManageUserRight->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwAddRight"),pManageUserRight->dwAddRight);
		m_GameDBAide.AddParameter(TEXT("@dwRemoveRight"),pManageUserRight->dwRemoveRight);

		//输出执行
		LogicTraceHelper(TEXT("GSP_GR_ManageUserRight"));

		//执行查询
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_ManageUserRight"),false);


		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CLog::Log(pIException->GetExceptionDescribe(),log_error);

		return false;
	}

	return true;
}

//清除数据表数据yang
VOID CDataBaseEngineSink::OnClearDB()
{
	//构造参数
	m_TreasureDBAide.ResetParameter();
	m_TreasureDBAide.AddParameter(TEXT("ServerID"),m_pGameServiceOption->dwServerID);

	//输出执行
	LogicTraceHelper(TEXT("GSP_GR_ClearRoomLocker"));

	//执行查询
	LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_ClearRoomLocker"),true);
}

#pragma region Club牌友圈

// byte数组转为 string  TODONOW 暂时放在这里处理
const CString toHexString(const byte * input, const int datasize)
{
    CString output;
    char ch[3];

	for(int i = 0; i < datasize; ++i)
	{
		if( (input+i) != NULL)
		{
			sprintf_s(ch, 3, "%02x", input[i]);
			output += ch;
		}
	}
	return output;
} 

//club牌友圈房间信息
bool CDataBaseEngineSink::On_DBR_CLUB_ROOM_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize!=sizeof(STR_DBR_CLUB_ROOM_INFO)) return false;
		STR_DBR_CLUB_ROOM_INFO* pTableInfo = (STR_DBR_CLUB_ROOM_INFO*)pData;

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pTableInfo->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@dwClubID"),pTableInfo->dwClubID);
		m_TreasureDBAide.AddParameter(TEXT("@dwGameID"),pTableInfo->dwGameID);
		m_TreasureDBAide.AddParameter(TEXT("@byModeID"),pTableInfo->byModeID);
		//m_TreasureDBAide.AddParameter(TEXT("@szKindName"),pTableInfo->dwLockState);
		m_TreasureDBAide.AddParameter(TEXT("@dwServiceGold"),pTableInfo->dwServiceGold);
		m_TreasureDBAide.AddParameter(TEXT("@dwRevenue"),pTableInfo->dwRevenue);
		m_TreasureDBAide.AddParameter(TEXT("@byPlayerNum"),0); //创建房间时候默认为0
		m_TreasureDBAide.AddParameter(TEXT("@DissolveMinute"),pTableInfo->DissolveMinute);
		m_TreasureDBAide.AddParameter(TEXT("@byChairNum"),pTableInfo->byChairNum);
		m_TreasureDBAide.AddParameter(TEXT("@byAllRound"),pTableInfo->byAllRound);

		//房间规则说明 40个TCHAR字符
		TCHAR szRoomRuleNote[40] = TEXT("");
		memcpy(szRoomRuleNote, (pTableInfo->szRealRoomRule) + 128, sizeof(TCHAR)*40);

		//房间名字 16个TCHAR字符
		TCHAR szRoomName[16] = TEXT("");
		memcpy(szRoomName, (pTableInfo->szRealRoomRule) + 128 + sizeof(TCHAR)*40, sizeof(TCHAR)*16);

		//16进制的房间规则 256个字节
		CString strRealRoomRule = toHexString(pTableInfo->szRealRoomRule, 255); //舍弃最后一个字符

		m_TreasureDBAide.AddParameter(TEXT("@szRoomRule"),szRoomRuleNote);
		m_TreasureDBAide.AddParameter(TEXT("@szRealRoomRule"), strRealRoomRule);

		m_TreasureDBAide.AddParameter(TEXT("@dwDizhu"),pTableInfo->dwDizhu);
		m_TreasureDBAide.AddParameter(TEXT("@dwGold"), pTableInfo->dwGold);
		m_TreasureDBAide.AddParameter(TEXT("@byMask"),pTableInfo->byMask);
		m_TreasureDBAide.AddParameter(TEXT("@RoomName"),szRoomName);
		
		//日志
		LogicTraceHelper(TEXT("GSP_WRITE_CLUB_ROOM_INFO"));

		//执行过程
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_WRITE_CLUB_ROOM_INFO"),false);

		if (lResultCode != DB_SUCCESS)
		{
		}

		return true;
	}
	catch(IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(),log_error);
		return false;
	}

	return true;
}

//club牌友圈桌子信息
bool CDataBaseEngineSink::On_DBR_CLUB_TABLE_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize!=sizeof(STR_DBR_CLUB_TABLE_INFO)) return false;
		STR_DBR_CLUB_TABLE_INFO* pTableInfo = (STR_DBR_CLUB_TABLE_INFO*)pData;

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pTableInfo->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@dwClubRoomID"),pTableInfo->dwClubRoomID);
		m_TreasureDBAide.AddParameter(TEXT("@dwTableID"),pTableInfo->dwTableID);
		m_TreasureDBAide.AddParameter(TEXT("@dwTableState"),pTableInfo->dwTableState);
		m_TreasureDBAide.AddParameter(TEXT("@dwLockState"),pTableInfo->dwLockState);
		m_TreasureDBAide.AddParameter(TEXT("@byCurrentRound"),pTableInfo->byCurrentRound);
		m_TreasureDBAide.AddParameter(TEXT("@byMask"),pTableInfo->byMask);
		m_TreasureDBAide.AddParameter(TEXT("@byCompanyID"),pTableInfo->byCompanyID);
		m_TreasureDBAide.AddParameter(TEXT("@dwGameID"),pTableInfo->dwGameID);

		//日志
		LogicTraceHelper(TEXT("GSP_WRITE_CLUB_TABLE_INFO"));
		//执行过程
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_WRITE_CLUB_TABLE_INFO"),true);

		if (lResultCode != DB_SUCCESS)
		{
			return true;
		}

		STR_CMD_LC_CLUB_TABLE_LIST CMD;
		CMD.dwClubRoomID  = m_TreasureDBAide.GetValue_DWORD(TEXT("RoomID"));
		CMD.dwTableID = m_TreasureDBAide.GetValue_DWORD(TEXT("TableID"));
		CMD.dwClubID = m_TreasureDBAide.GetValue_DWORD(TEXT("ClubID"));
		CMD.byMask = m_TreasureDBAide.GetValue_BYTE(TEXT("IsOwner"));
		CMD.TableState = m_TreasureDBAide.GetValue_DWORD(TEXT("TableState"));

		DWORD dwResuleLockState =  m_TreasureDBAide.GetValue_DWORD(TEXT("LockState"));
		CMD.CurrentRound =  m_TreasureDBAide.GetValue_DWORD(TEXT("CurrentRound"));
		CMD.dwOwnerID =  m_TreasureDBAide.GetValue_DWORD(TEXT("OwnerID"));
		CMD.byDel = (pTableInfo->byMask == 3) ? 1 : 0;

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GC_CLUB_UPDATE_TABLE_INFO,dwContextID,&CMD,sizeof(CMD));

		return true;
	}
	catch(IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(),log_error);
		return false;
	}

	return true;
}

//club牌友圈玩家信息
bool CDataBaseEngineSink::On_DBR_CLUB_PLAYER_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize!=sizeof(STR_DBR_CLUB_PLAYER_INFO)) return false;
		STR_DBR_CLUB_PLAYER_INFO* pTableInfo = (STR_DBR_CLUB_PLAYER_INFO*)pData;

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pTableInfo->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@dwTableID"),pTableInfo->dwTableID);
		m_TreasureDBAide.AddParameter(TEXT("@byChairID"),pTableInfo->byChairID);
		m_TreasureDBAide.AddParameter(TEXT("@byMask"),pTableInfo->byMask);

		//日志
		LogicTraceHelper(TEXT("GSP_WRITE_CLUB_PLAYER_INFO"));
		//执行过程
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_WRITE_CLUB_PLAYER_INFO"),true);

		if (lResultCode != DB_SUCCESS)
		{
			return true;
		}

		STR_CMD_LC_CLUB_TABLE_USER_LIST CMD;
		CMD.dwClubRoomID=m_TreasureDBAide.GetValue_DWORD(TEXT("ClubRoomID"));
		CMD.dwUserID=m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
		m_TreasureDBAide.GetValue_String(TEXT("UserName"),CMD.szUserName,CountArray(CMD.szUserName));
		CMD.bySex=m_TreasureDBAide.GetValue_BYTE(TEXT("Sex"));
		CMD.wLevel=m_TreasureDBAide.GetValue_WORD(TEXT("UserLevel"));
		m_TreasureDBAide.GetValue_String(TEXT("HeadUrl"),CMD.szHeadUrl,CountArray(CMD.szHeadUrl));
		CMD.byClubRole=m_TreasureDBAide.GetValue_BYTE(TEXT("ClubRole"));
		CMD.byClubReputation=m_TreasureDBAide.GetValue_BYTE(TEXT("ClubReputation"));
		CMD.dwTableID=m_TreasureDBAide.GetValue_DWORD(TEXT("TableID"));
		CMD.byChairID=m_TreasureDBAide.GetValue_BYTE(TEXT("ChairID"));
		CMD.byDel = (pTableInfo->byMask == 1) ? 0 : 1;

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GC_CLUB_UPDATE_PLAYER_INFO,dwContextID,&CMD,sizeof(CMD));

	}
	catch(IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(),log_error);
		return false;
	}

	return true;
}

//金币大厅 桌子信息
bool CDataBaseEngineSink::On_DBR_HALL_GOLD_TABLE_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize!=sizeof(STR_DBR_HALL_GOLD_TABLE_INFO)) return false;
		STR_DBR_HALL_GOLD_TABLE_INFO* pTableInfo = (STR_DBR_HALL_GOLD_TABLE_INFO*)pData;

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@mystery"),_MYSTERY);
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pTableInfo->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@dwKindID"),pTableInfo->dwKindID);
		m_TreasureDBAide.AddParameter(TEXT("@byGameType"),pTableInfo->byGameType);
		m_TreasureDBAide.AddParameter(TEXT("@dwTableID"),pTableInfo->dwTableID);
		m_TreasureDBAide.AddParameter(TEXT("@dwTableState"),pTableInfo->dwTableState);
		m_TreasureDBAide.AddParameter(TEXT("@byMask"),pTableInfo->byMask);
		m_TreasureDBAide.AddParameter(TEXT("@dwGameID"),pTableInfo->dwGameID);

		//日志
		LogicTraceHelper(TEXT("GSP_WRITE_HALL_GOLD_TABLE_INFO"));
		//执行过程
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_WRITE_HALL_GOLD_TABLE_INFO"),true);

		if (lResultCode != DB_SUCCESS)
		{
			return true;
		}

		return true;
	}
	catch(IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(),log_error);
		return false;
	}

	return true;
}

//金币大厅 玩家信息
bool CDataBaseEngineSink::On_DBR_HALL_GOLD_PLAYER_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize!=sizeof(STR_DBR_HALL_GOLD_PLAYER_INFO)) return false;
		STR_DBR_HALL_GOLD_PLAYER_INFO* pTableInfo = (STR_DBR_HALL_GOLD_PLAYER_INFO*)pData;

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pTableInfo->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@dwTableID"),pTableInfo->dwTableID);
		m_TreasureDBAide.AddParameter(TEXT("@byChairID"),pTableInfo->byChairID);
		m_TreasureDBAide.AddParameter(TEXT("@byMask"),pTableInfo->byMask);

		//日志
		LogicTraceHelper(TEXT("GSP_WRITE_HALL_GOLD_PLAYER_INFO"));
		//执行过程
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_WRITE_HALL_GOLD_PLAYER_INFO"),true);

		if (lResultCode != DB_SUCCESS)
		{
			return true;
		}
	}
	catch(IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(),log_error);
		return false;
	}

	return true;
}

#pragma endregion
////////////////////////////////////////////////////////////////////////////////// 
//添加开房信息
bool CDataBaseEngineSink::OnRequestAddTableInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_UpdateTableInfo));
		if (wDataSize!=sizeof(DBR_GR_UpdateTableInfo)) return false;

		DBR_GR_UpdateTableInfo* pTableInfo = (DBR_GR_UpdateTableInfo*)pData;
		//构造参数
		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wTableID"),pTableInfo->dwTableID);
		m_GameDBAide.AddParameter(TEXT("@dwPassword"),pTableInfo->dwPassword);
		m_GameDBAide.AddParameter(TEXT("@dwCreaterID"),pTableInfo->dwCreaterID);
		m_GameDBAide.AddParameter(TEXT("@wJuShu"),pTableInfo->wJuShu);
		m_GameDBAide.AddParameter(TEXT("@byMode"),pTableInfo->byMode);
		m_GameDBAide.AddParameter(TEXT("@byZhuangType"),pTableInfo->byZhuangType);
		m_GameDBAide.AddParameter(TEXT("@byPlayerCount"),pTableInfo->byPlayerCount);
		m_GameDBAide.AddParameter(TEXT("@byMaxPlayerCount"),pTableInfo->byMaxPlayerCount);
		m_GameDBAide.AddParameter(TEXT("@byIsStart"),pTableInfo->byIsStart);
		m_GameDBAide.AddParameter(TEXT("@byCost"),pTableInfo->byCost);
		m_GameDBAide.AddParameter(TEXT("@szCreateTime"),pTableInfo->szTime);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//日志
		LogicTraceHelper(TEXT("GSP_GR_UpdateTableInfo"));
		//执行过程
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_UpdateTableInfo"),true);
		//结果处理
		CDBVarValue DBVarValue;
		m_GameDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		if (lResultCode==DB_SUCCESS)
		{
		}
		else
		{
		}
	}
	catch(IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(),log_error);
		return false;
	}

	return true;
}

//开始桌子
bool CDataBaseEngineSink::OnRequestStartTable(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DWORD));
		if (wDataSize!=sizeof(DWORD)) return false;

		DWORD* pTableID = (DWORD*)pData;
		//构造参数
		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwTableID"),*pTableID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//日志
		LogicTraceHelper(TEXT("GSP_GR_StartTable"));
		//执行过程
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_StartTable"),true);
		//结果处理
		CDBVarValue DBVarValue;
		m_GameDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		if (lResultCode==DB_SUCCESS)
		{

		}
		else
		{
		}
	}
	catch(IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(),log_error);
		return false;
	}
	return true;
}

//结束桌子
bool CDataBaseEngineSink::OnRequestEndTable(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_EndTable));
		if (wDataSize!=sizeof(DBR_GP_EndTable)) return false;

		DBR_GP_EndTable* pEndTable = (DBR_GP_EndTable*)pData;
		//构造参数
		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwTableID"),pEndTable->dwTableID);
		m_GameDBAide.AddParameter(TEXT("@dwUserID1"),pEndTable->dwUserID[0]);
		m_GameDBAide.AddParameter(TEXT("@dwUserID2"),pEndTable->dwUserID[1]);
		m_GameDBAide.AddParameter(TEXT("@dwUserID3"),pEndTable->dwUserID[2]);
		m_GameDBAide.AddParameter(TEXT("@dwUserID4"),pEndTable->dwUserID[3]);
		m_GameDBAide.AddParameter(TEXT("@dwUserID5"),pEndTable->dwUserID[4]);
		m_GameDBAide.AddParameter(TEXT("@dwScore1"),pEndTable->dwScore[0]);
		m_GameDBAide.AddParameter(TEXT("@dwScore2"),pEndTable->dwScore[1]);
		m_GameDBAide.AddParameter(TEXT("@dwScore3"),pEndTable->dwScore[2]);
		m_GameDBAide.AddParameter(TEXT("@dwScore4"),pEndTable->dwScore[3]);
		m_GameDBAide.AddParameter(TEXT("@dwScore5"),pEndTable->dwScore[4]);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//日志
		LogicTraceHelper(TEXT("GSP_GR_EndTable"));
		//执行过程
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_EndTable"),true);
		//结果处理
		CDBVarValue DBVarValue;
		m_GameDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		if (lResultCode==DB_SUCCESS)
		{

		}
		else
		{
		}
	}	
	catch(IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(),log_error);
		return false;
	}
	return true;
}




//添加用户到房间
bool CDataBaseEngineSink::OnRequestAddUserToTable(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_AddTableUser));
		if (wDataSize!=sizeof(DBR_GP_AddTableUser)) return false;

		DBR_GP_AddTableUser* pEndTable = (DBR_GP_AddTableUser*)pData;
		//构造参数
		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwTableID"),pEndTable->dwTableID);
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pEndTable->dwUserID);
		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//日志
		LogicTraceHelper(TEXT("GSP_GR_AddTableUser"));	//添加用户到TableInfo表
		//执行过程
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_AddTableUser"),true);
		//结果处理
		CDBVarValue DBVarValue;
		m_GameDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		if (lResultCode==DB_SUCCESS)
		{

		}
		else
		{
		}
	}
	catch(IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(),log_error);
		return false;
	}
	return true;
}

//删除房间用户
bool CDataBaseEngineSink::OnRequestDeleteUserToTable(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_DeleteTableUser));
		if (wDataSize!=sizeof(DBR_GP_DeleteTableUser)) return false;

		DBR_GP_DeleteTableUser* pEndTable = (DBR_GP_DeleteTableUser*)pData;
		//构造参数
		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwTableID"),pEndTable->dwTableID);
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pEndTable->dwUserID);
		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//日志
		LogicTraceHelper(TEXT("GSP_GR_DeleteTableUser"));
		//执行过程
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_DeleteTableUser"),true);
		//结果处理
		CDBVarValue DBVarValue;
		m_GameDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		if (lResultCode==DB_SUCCESS)
		{
			//
		}
		else
		{
		}
	}
	catch(IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(),log_error);
		return false;
	}

	return true;
}


//更新开房信息
bool CDataBaseEngineSink::OnUpdateTableInfo(DWORD dwTableID)
{
	m_TreasureDBAide.ResetParameter();
	m_TreasureDBAide.AddParameter(TEXT("dwTableID"),dwTableID);


	//输出执行
	LogicTraceHelper(TEXT("GSP_GR_GetTableInfo"));

	//执行查询
	LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_GetTableInfo"),true);

	//结果处理
	if (lResultCode==DB_SUCCESS)
	{

		//变量定义
		DBO_GP_TableInfo TableInfo;
		ZeroMemory(&TableInfo,sizeof(TableInfo));

		TableInfo.dwTableID = m_TreasureDBAide.GetValue_DWORD(TEXT("TableID"));
		TableInfo.dwCreaterID = m_TreasureDBAide.GetValue_DWORD(TEXT("CreaterID"));
		TableInfo.dwPassword = m_TreasureDBAide.GetValue_DWORD(TEXT("Password"));
		TableInfo.wJuShu = m_TreasureDBAide.GetValue_WORD(TEXT("JuShu"));
		TableInfo.byMode = m_TreasureDBAide.GetValue_BYTE(TEXT("GameMode"));
		TableInfo.byZhuangType = m_TreasureDBAide.GetValue_BYTE(TEXT("ZhuangType"));
		TableInfo.byPlayerCount = m_TreasureDBAide.GetValue_BYTE(TEXT("PlayerCount"));
		TableInfo.byMaxPlayerCount = m_TreasureDBAide.GetValue_BYTE(TEXT("MaxPlayerCount"));
		TableInfo.byCost = m_TreasureDBAide.GetValue_BYTE(TEXT("PayCost"));
		TableInfo.byIsStart = m_TreasureDBAide.GetValue_BYTE(TEXT("IsStart"));

		TableInfo.dwUserID[0] =  m_TreasureDBAide.GetValue_DWORD(TEXT("UserID1"));
		TableInfo.dwUserID[1] =  m_TreasureDBAide.GetValue_DWORD(TEXT("UserID2"));
		TableInfo.dwUserID[2] =  m_TreasureDBAide.GetValue_DWORD(TEXT("UserID3"));
		TableInfo.dwUserID[3] =  m_TreasureDBAide.GetValue_DWORD(TEXT("UserID4"));
		TableInfo.dwUserID[4] =  m_TreasureDBAide.GetValue_DWORD(TEXT("UserID5"));

		lstrcpyn( TableInfo.szNickName[0],TEXT("NickName1"),CountArray(TableInfo.szNickName[0]));
		lstrcpyn( TableInfo.szNickName[1],TEXT("100012"),CountArray(TableInfo.szNickName[1]));
		lstrcpyn( TableInfo.szNickName[2],TEXT(""),CountArray(TableInfo.szNickName[1]));
		lstrcpyn( TableInfo.szNickName[3],TEXT(""),CountArray(TableInfo.szNickName[2]));
		lstrcpyn( TableInfo.szNickName[4],TEXT(""),CountArray(TableInfo.szNickName[3]));

		m_TreasureDBAide.GetValue_String(TEXT("CreateTime"),TableInfo.szTime,CountArray(TableInfo.szTime));	

		//m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TABLE_INFO_RESULT,dwContextID,&TableInfo,sizeof(DBO_GP_TableInfo));
	}





	return true;
}

//更新已开房信息
bool CDataBaseEngineSink::OnUpdateEndTableInfo(DWORD dwTableID)
{
	return true;
}



//请求抽奖和抢红包
bool CDataBaseEngineSink::OnQueryLottery(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(DBR_GR_QueryLottery) < wDataSize)
		{
			return false;
		}

		DBR_GR_QueryLottery* pDbReq = (DBR_GR_QueryLottery*)pData;
		//BYTE type = pDbReq->byType;

		//获取一个1-100的随机数
		srand((unsigned int)time(NULL)) ;
		int index = rand() % 100 + 1;


		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@UserID"), pDbReq->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@Type"), pDbReq->byType);
		m_GameDBAide.AddParameter(TEXT("@Index"), index);
		//输出变量
		WCHAR szDescribe[128] = L"";
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_GP_ImplementLottery")); 
		//执行查询,执行抽奖
		LONG lResultCode = m_GameDBAide.ExecuteProcess(TEXT("GSP_GP_ImplementLottery"), true);
		//结果处理
		CDBVarValue DBVarValue;
		m_GameDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		if(lResultCode == DB_SUCCESS)
		{
			DBO_GR_LotteryResult LotteryResult;
			ZeroMemory(&LotteryResult,sizeof(DBO_GR_LotteryResult));

			LotteryResult.byIndex = -1;
			LotteryResult.byType = m_GameDBAide.GetValue_INT(TEXT("RewardType"));
			LotteryResult.dwRewardCount = m_GameDBAide.GetValue_DWORD(TEXT("RewardCount"));
			m_GameDBAide.GetValue_String(TEXT("PacketID"),LotteryResult.szPacketID,CountArray(LotteryResult.szPacketID));
			lstrcpyn(LotteryResult.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(LotteryResult.szDescribe));

			//通知抽奖结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOTTERY_RESULT,dwContextID,&LotteryResult,sizeof(DBO_GR_LotteryResult));

			//if(LotteryResult.byType != 0)
			//{
			//	//获取更新用户金币房卡钻石
			//	DBO_GR_ScoreInfo UserScoreInfo;
			//	ZeroMemory(&UserScoreInfo,sizeof(DBO_GP_ScoreInfo));

			//	UserScoreInfo.dwUserID = pDbReq->dwUserID;
			//	UserScoreInfo.lGold = m_GameDBAide.GetValue_LONG(TEXT("Gold"));
			//	UserScoreInfo.lOpenRoomCard = m_GameDBAide.GetValue_LONG(TEXT("OpenRoomCard"));
			//	UserScoreInfo.lDiamond = m_GameDBAide.GetValue_LONG(TEXT("Diamond"));
			//	UserScoreInfo.lRewardCard = m_GameDBAide.GetValue_LONG(TEXT("RewardCard"));
			//	UserScoreInfo.lScore = m_GameDBAide.GetValue_LONG(TEXT("Score"));

			//	//发送财富变更消息
			//	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_QUERY_SCOREINFO,dwContextID,&UserScoreInfo,sizeof(DBO_GP_ScoreInfo));
			//}
		}else
		{
			//没有奖品，抽到空
			DBO_GR_LotteryResult LotteryResult;
			ZeroMemory(&LotteryResult,sizeof(DBO_GR_LotteryResult));

			LotteryResult.byIndex = -1;
			LotteryResult.byType = 0;
			LotteryResult.dwRewardCount = 0;

			lstrcpyn(LotteryResult.szPacketID,TEXT(""),CountArray(LotteryResult.szPacketID));
			lstrcpyn(LotteryResult.szDescribe,TEXT("人品就差一点点，请再接再厉吧！"),CountArray(LotteryResult.szDescribe));

			//通知抽奖结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOTTERY_RESULT,dwContextID,&LotteryResult,sizeof(DBO_GR_LotteryResult));
		}

	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(), log_error);

		return false;
	}

	return true;

}

//更新游戏任务状态
bool CDataBaseEngineSink::On_Table_UpdateGameTaskStatus(DWORD dwContextID, void *pData, WORD wDataSize)
{
	try
	{
		//数据包校验
		if (sizeof(STR_DBR_CG_TABLE_UPDATE_TASK_STATUS) != wDataSize)
			return false;

		STR_DBR_CG_TABLE_UPDATE_TASK_STATUS* pDbReq = (STR_DBR_CG_TABLE_UPDATE_TASK_STATUS*)pData;

		//数据库传入参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@cbCurGameCount"), pDbReq->cbCurGameCount);
		m_GameDBAide.AddParameter(TEXT("@cbTableMode"), pDbReq->cbTableMode);

		//输出变量
		WCHAR szDescribe[128] = L"";
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_SG_Table_UpdateGameTaskStatus")); 
		//执行查询,执行抽奖
		LONG lResultCode = m_GameDBAide.ExecuteProcess(TEXT("GSP_SG_Table_UpdateGameTaskStatus"), true);

		//只需要更新，不需要返回，获得任务列表时会重新获得任务状态
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(), log_error);

		return false;
	}

	return true;
}


// string 转为byte数组  TODONOW 暂时放在这里处理
int StrToBin(TCHAR* inWord, BYTE* OutBin, int source_len_begin, int source_len_end)
{
	TCHAR t2;
	int count = 0;

	for(int t = source_len_begin ;t < source_len_end; t ++)
	{   
		t2 = inWord[t];

		BYTE intTemp = 0;
		if(t2 == '0')
		{
			intTemp=0;
		}
		else if(t2 == '1')
		{
			intTemp=1;
		}
		else if(t2 == '2')
		{
			intTemp=2;
		}
		else if(t2 == '3')
		{
			intTemp=3;
		}
		else if(t2 == '4')
		{
			intTemp=4;
		}
		else if(t2 == '5')
		{
			intTemp=5;
		}
		else if(t2 == '6')
		{
			intTemp=6;
		}
		else if(t2 == '7')
		{
			intTemp=7;
		}
		else if(t2 == '8')
		{
			intTemp=8;
		}
		else if(t2 == '9')
		{
			intTemp=9;
		}
		else if(t2 == 'a' || t2 == 'A')
		{
			intTemp=10;
		}
		else if(t2 == 'b' || t2 == 'B')
		{
			intTemp=11;
		}
		else if(t2 == 'c' || t2 == 'C')
		{
			intTemp=12;
		}
		else if(t2 == 'd' || t2 == 'D')
		{
			intTemp=13;
		}
		else if(t2 == 'e' || t2 == 'E')
		{
			intTemp=14;
		}
		else if(t2 == 'f' || t2 =='F')
		{
			intTemp=15;
		}


		count = (t-source_len_begin)/2;

		if((t % 2) == 0) //高位
		{
			OutBin[count] = intTemp<<4;
		}
		else if((t % 2) == 1) //低位
		{
			OutBin[count] += intTemp;
		}
	}
	return count;
}

//牌友圈创建桌子信息
bool CDataBaseEngineSink::On_DBR_CG_CLUB_CREATE_TABLE(DWORD dwContextID, void *pData, WORD wDataSize)
{
	try
	{
		//数据包校验
		if (sizeof(STR_SUB_CG_USER_CREATE_TABLE) != wDataSize)
			return false;
		
		STR_SUB_CG_USER_CREATE_TABLE* pDbReq = (STR_SUB_CG_USER_CREATE_TABLE*)pData;

		//数据库传入参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@dwClubRoomID"), pDbReq->dwClubRoomID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_ROOM_INFO")); 
		//执行查询
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_INFO"), true);

		if(lResultCode != DB_SUCCESS)
		{
			return false ;
		}

		//获取到数据
		STR_DBO_GC_CLUB_CREATE_TABLE Dbo;
		ZeroMemory(&Dbo,sizeof(STR_DBO_GC_CLUB_CREATE_TABLE));
		Dbo.byTableMode = pDbReq->byTableMode;
		Dbo.dwClubRoomID = pDbReq->dwClubRoomID;

		TCHAR szRealRoomRule[2048];
		m_TreasureDBAide.GetValue_String(TEXT("RealRoomRule"), szRealRoomRule, sizeof(szRealRoomRule));

		StrToBin(szRealRoomRule, Dbo.strCreateRoom.CommonRule, 0, 255);
		StrToBin(szRealRoomRule, Dbo.strCreateRoom.SubGameRule, 256, 512);

		STR_SUB_CG_USER_CREATE_ROOM CreateRoom = Dbo.strCreateRoom;
		tagTableRule *pCfg = (tagTableRule*)CreateRoom.CommonRule;

		BYTE byPlayNum = pCfg->PlayerCount;
		BYTE byGameCountType = pCfg->GameCountType;
		SCORE cost = byPlayNum * byGameCountType;

		//数据库传入参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@cost"), cost);
		m_TreasureDBAide.AddParameter(TEXT("@dwClubRoomID"), pDbReq->dwClubRoomID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_ROOM_INFO_COST")); 
		//执行查询
		lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_INFO_COST"), false);
		Dbo.lResultCode = lResultCode;
		return m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GC_CLUB_CREATE_TABLE,dwContextID,&Dbo,sizeof(Dbo));
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(), log_error);

		return false;
	}
}

//加入桌子 不需要密码
bool CDataBaseEngineSink::On_DBR_CG_USER_JOIN_TABLE_NO_PASS(DWORD dwContextID, void *pData, WORD wDataSize)
{
	try
	{
		//数据包校验
		if (sizeof(STR_SUB_CG_USER_JOIN_TABLE_NO_PASS) != wDataSize)
			return false;

		STR_SUB_CG_USER_JOIN_TABLE_NO_PASS* pDbReq = (STR_SUB_CG_USER_JOIN_TABLE_NO_PASS*)pData;

		//数据库传入参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@dwClubRoomID"), pDbReq->dwClubRoomID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CG_USER_JOIN_TABLE_NO_PASS")); 
		//执行查询
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CG_USER_JOIN_TABLE_NO_PASS"), true);

		STR_DBO_CG_USER_JOIN_TABLE_NO_PASS Dbo;
		ZeroMemory(&Dbo,sizeof(STR_DBO_CG_USER_JOIN_TABLE_NO_PASS));
		Dbo.lResultCode = lResultCode;
		if(lResultCode == DB_SUCCESS)
		{
			TCHAR szRealRoomRule[2048];
			m_TreasureDBAide.GetValue_String(TEXT("RealRoomRule"), szRealRoomRule, sizeof(szRealRoomRule));

			StrToBin(szRealRoomRule, Dbo.strCreateRoom.CommonRule, 0, 255);
			StrToBin(szRealRoomRule, Dbo.strCreateRoom.SubGameRule, 256, 512);

			Dbo.dLongitude = pDbReq->dLongitude;
			Dbo.dLatitude = pDbReq->dLatitude;
			Dbo.dwPassword = m_TreasureDBAide.GetValue_DWORD(TEXT("TableID"));
			Dbo.dwClubRoomID = pDbReq->dwClubRoomID;

			
			//这个存储过程是判断 房卡是否足够
			STR_SUB_CG_USER_CREATE_ROOM CreateRoom = Dbo.strCreateRoom;
			tagTableRule *pCfg = (tagTableRule*)CreateRoom.CommonRule;

			BYTE byPlayNum = pCfg->PlayerCount;
			BYTE byGameCountType = pCfg->GameCountType;
			SCORE cost = byPlayNum * byGameCountType;

			//数据库传入参数
			m_TreasureDBAide.ResetParameter();
			m_TreasureDBAide.AddParameter(TEXT("@cost"), cost);
			m_TreasureDBAide.AddParameter(TEXT("@dwClubRoomID"), pDbReq->dwClubRoomID);

			//输出执行
			LogicTraceHelper(TEXT("GSP_CL_CLUB_ROOM_INFO_COST")); 
			//执行查询
			Dbo.lResultCode2 = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_INFO_COST"), false);
		}

		return m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GC_USER_JOIN_TABLE_NO_PASS,dwContextID,&Dbo,sizeof(Dbo));
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(), log_error);

		return false;
	}
}

//加入桌子
bool CDataBaseEngineSink::On_DBR_CG_JOIN_TABLE(DWORD dwContextID, void *pData, WORD wDataSize)
{
	try
	{
		//数据包校验
		if (sizeof(STR_DBR_JOIN_ROOM) != wDataSize)
			return false;

		STR_DBR_JOIN_ROOM* pDbReq = (STR_DBR_JOIN_ROOM*)pData;

		//数据库传入参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@dwTableID"), pDbReq->dwTableID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CG_USER_JOIN_TABLE")); 
		//执行查询
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CG_USER_JOIN_TABLE"), false);
		pDbReq->lResultCode = lResultCode;

		return m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GC_JOIN_TABLE,dwContextID, pDbReq, wDataSize);
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(), log_error);

		return false;
	}
}

//加入桌子 金币大厅桌子
bool CDataBaseEngineSink::On_DBR_CG_USER_JOIN_TABLE_HALL_GOLD(DWORD dwContextID, void *pData, WORD wDataSize)
{
	try
	{
		//数据包校验
		if (sizeof(STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM) != wDataSize)
			return false;

		STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM* pDbReq = (STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM*)pData;

		//数据库传入参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@byGameMod"), pDbReq->byGameMod);
		m_TreasureDBAide.AddParameter(TEXT("@byType"), pDbReq->byType);
		m_TreasureDBAide.AddParameter(TEXT("@dwKindID"), pDbReq->dwKindID);
		m_TreasureDBAide.AddParameter(TEXT("@mystery"), _MYSTERY);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CG_USER_JOIN_GOLD_HALL_ROOM")); 
		//执行查询
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CG_USER_JOIN_GOLD_HALL_ROOM"), true);

		STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD Dbo;
		ZeroMemory(&Dbo,sizeof(STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD));
		Dbo.lResultCode = lResultCode;
		Dbo.dwKindID =  pDbReq->dwKindID;
		Dbo.byGameType =  pDbReq->byType;

		if(lResultCode == DB_SUCCESS)
		{
			TCHAR szRealRoomRule[2048];
			m_TreasureDBAide.GetValue_String(TEXT("RealRoomRule"), szRealRoomRule, sizeof(szRealRoomRule));

			StrToBin(szRealRoomRule, Dbo.strCreateRoom.CommonRule, 0, 255);
			StrToBin(szRealRoomRule, Dbo.strCreateRoom.SubGameRule, 256, 512);

			Dbo.dwPassword = m_TreasureDBAide.GetValue_DWORD(TEXT("TableID"));
		}

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GC_USER_JOIN_TABLE_HALL_GOLD,dwContextID,&Dbo,sizeof(Dbo));
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CLog::Log(pIException->GetExceptionDescribe(), log_error);

		return false;
	}

	return true;
}
