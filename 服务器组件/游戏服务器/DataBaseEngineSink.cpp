#include "StdAfx.h"
#include "GameCtrl.h"
#include "DataBaseEngineSink.h"
#include "AttemperEngineSink.h"
#include "RobotManager.h"
#include "MatchManager.h"

IDataBase		 *g_TreasureDB = NULL; 

//构造函数
CDataBaseEngineSink::CDataBaseEngineSink()
{
	return;
}

//析构函数
CDataBaseEngineSink::~CDataBaseEngineSink()
{
	return;
}

//启动事件
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	m_TreasureDB = static_cast<IDataBase*>(CWHModule::Database());
	
	if(m_TreasureDB == NULL) return false;

	//发起连接
	bool bRet2 = m_TreasureDB->Connect(3);

	if(!bRet2)
	{
		return false;
	}

	g_TreasureDB = m_TreasureDB;
	return true;
}

//停止事件
bool CDataBaseEngineSink::OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx)
{
	return true;
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
		break;
	case DBR_CG_LOGON_USERID:			//I D 登录
		{
			bSucceed = On_DBR_Logon_UserID(dwContextID,pData,wDataSize,dwUserID);
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
	case DBR_GR_MANAGE_USER_RIGHT:		//用户权限
		{
			bSucceed = OnRequestManageUserRight(dwContextID,pData,wDataSize,dwUserID);
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
	case DBR_SC_TABLE_UPDATE_TASK_STATUS:		//更新游戏任务状态
		{
			bSucceed = On_Table_UpdateGameTaskStatus(dwContextID,pData,wDataSize);
		}
		break;
	}

	return bSucceed;
}

//ID 登录
bool CDataBaseEngineSink::On_DBR_Logon_UserID(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//数据校验
	if (wDataSize!=sizeof(STR_DBR_CG_LOGON_USERID)) return false;

	//执行查询
	STR_DBR_CG_LOGON_USERID *pLogonUserID = (STR_DBR_CG_LOGON_USERID *)pData;

	//转化地址
	TCHAR szClientAddr[16]=TEXT("");
	BYTE *pClientAddr = (BYTE *)&pLogonUserID->dwClientAddr;
	if ( NULL != pClientAddr )
	{
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);
	}

	//构造参数
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pLogonUserID->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@strPassword"),pLogonUserID->szPassword);
	m_TreasureDB->AddParameter(TEXT("@strClientIP"),szClientAddr);
	m_TreasureDB->AddParameter(TEXT("@strMachineID"),pLogonUserID->szMachineID);

	//输出参数
	TCHAR szDescribeString[128]=TEXT("");
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString),adParamOutput);

	//执行查询
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_CG_Logon_UserID"),true);

	//结果处理
	CDBVarValue DBVarValue;
	m_TreasureDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	On_DBO_Logon_UserID(dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal));

	return true;
}
//用户Socket连接关闭
bool CDataBaseEngineSink::On_DBR_GP_QUIT(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	if (wDataSize!=sizeof(DBR_GP_UserQuitInfo)) return false;

	DBR_GP_UserQuitInfo* groupInfo = (DBR_GP_UserQuitInfo*)pData;
	//构造参数
	m_TreasureDB->ResetParameter();

	m_TreasureDB->AddParameter(TEXT("@dwUserID"),groupInfo->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@byOnlineMask"),groupInfo->byOnlineMask);

	//执行输出
	m_TreasureDB->ExecuteProcess(TEXT("GSP_CL_Quit"),false);	  

	return true;

}


//ID登录返回
bool CDataBaseEngineSink::On_DBO_Logon_UserID(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	//发送数据
	STR_DBO_CG_LOGON_USERID DBOLogonAccount;
	ZeroMemory(&DBOLogonAccount, sizeof(STR_DBO_CG_LOGON_USERID));

	//赋值
	DBOLogonAccount.lResultCode = dwErrorCode;
	lstrcpyn(DBOLogonAccount.szDescribeString, pszErrorString, CountArray(DBOLogonAccount.szDescribeString));

	//登录成功获取信息
	if(DB_SUCCESS == dwErrorCode)
	{
		//用户标志
		DBOLogonAccount.useInfo.dwUserID=m_TreasureDB->GetValue_DWORD(TEXT("UserID"));
		//用户昵称
		m_TreasureDB->GetValue_String(TEXT("NickName"),DBOLogonAccount.useInfo.szNickName,CountArray(DBOLogonAccount.useInfo.szNickName));
		//用户性别
		DBOLogonAccount.useInfo.cbGender=m_TreasureDB->GetValue_BYTE(TEXT("Gender"));
		//头像索引
		m_TreasureDB->GetValue_String(TEXT("HeadUrl"),DBOLogonAccount.useInfo.szHeadUrl,CountArray(DBOLogonAccount.useInfo.szHeadUrl));
		//个性签名
		m_TreasureDB->GetValue_String(TEXT("MySignature"),DBOLogonAccount.useInfo.szUnderWrite,CountArray(DBOLogonAccount.useInfo.szUnderWrite));

		//社团ID
		//DBOLogonAccount.useInfo.dwGroupID=m_TreasureDB->GetValue_BYTE(TEXT("GroupID"));
		//社团名字
		//m_TreasureDB->GetValue_String(TEXT("GroupName"),DBOLogonAccount.useInfo.szGroupName,CountArray(DBOLogonAccount.useInfo.szGroupName));

		//会员等级
		DBOLogonAccount.useInfo.cbMemberOrder=m_TreasureDB->GetValue_BYTE(TEXT("MemberOrder"));
		//人物等级
		DBOLogonAccount.useInfo.dwLevel=m_TreasureDB->GetValue_BYTE(TEXT("cbLevel"));
		//经验数值
		DBOLogonAccount.useInfo.dwExperience=m_TreasureDB->GetValue_DWORD(TEXT("Experience"));

		//用户房卡
		DBOLogonAccount.useInfo.lOpenRoomCard = m_TreasureDB->GetValue_LONGLONG(TEXT("UserRoomCard"));
		//钻石
		DBOLogonAccount.useInfo.lDiamond = m_TreasureDB->GetValue_LONGLONG(TEXT("UserDiamond"));
		//用户游戏币
		DBOLogonAccount.useInfo.lGold = m_TreasureDB->GetValue_LONGLONG(TEXT("UserGold"));
	}

	//发送数据
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_CG_LOGON_USERID, dwContextID, &DBOLogonAccount, sizeof(STR_DBO_CG_LOGON_USERID));
	return true;
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


	//构造输入参数
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"), pDBRJoinGoupRoom->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwGroupID"), pDBRJoinGoupRoom->dwGroupID);

	//输出参数
	TCHAR szDescribeString[128] = TEXT("");
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString),adParamOutput);

	//执行查询
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_CG_User_JoinGroupRoom"),true);

	//结果处理
	CDBVarValue DBVarValue;
	m_TreasureDB->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

	On_DBO_User_JoinGroupRoom(dwContextID, pDBRJoinGoupRoom->dwGroupID, lResultCode, CW2CT(DBVarValue.bstrVal));

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
		JoinGroupRoom.dwGroupID = dwGroupID/*m_TreasureDB->GetValue_DWORD(TEXT("GroupID"))*/;
	}

	//发送数据
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_CG_USER_JOIN_GROUP_ROOM, dwContextID, &JoinGroupRoom, sizeof(STR_DBO_CG_USER_JOIN_GROUP_ROOM));
}


//游戏写分
bool CDataBaseEngineSink::OnRequestWriteGameScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//请求处理
	DBR_GR_WriteGameScore * pWriteGameScore=(DBR_GR_WriteGameScore *)pData;
	dwUserID=pWriteGameScore->dwUserID;

	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_WriteGameScore));
	if (wDataSize!=sizeof(DBR_GR_WriteGameScore)) return false;

	//转化地址
	TCHAR szClientAddr[16]=TEXT("");
	BYTE * pClientAddr=(BYTE *)&pWriteGameScore->dwClientAddr;
	_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

	//构造参数
	m_TreasureDB->ResetParameter();

	//用户信息
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pWriteGameScore->dwUserID);

	//变更成绩
	m_TreasureDB->AddParameter(TEXT("@lVariationInsure"),pWriteGameScore->VariationInfo.lInsure);
	m_TreasureDB->AddParameter(TEXT("@lVariationRevenue"),pWriteGameScore->VariationInfo.lRevenue);
	m_TreasureDB->AddParameter(TEXT("@lVariationWinCount"),pWriteGameScore->VariationInfo.dwWinCount);
	m_TreasureDB->AddParameter(TEXT("@lVariationLostCount"),pWriteGameScore->VariationInfo.dwLostCount);
	m_TreasureDB->AddParameter(TEXT("@lVariationDrawCount"),pWriteGameScore->VariationInfo.dwDrawCount);
	m_TreasureDB->AddParameter(TEXT("@lVariationFleeCount"),pWriteGameScore->VariationInfo.dwFleeCount);
	//m_TreasureDB->AddParameter(TEXT("@lVariationUserMedal"),pWriteGameScore->VariationInfo.dwUserMedal);
	m_TreasureDB->AddParameter(TEXT("@lVariationExperience"),pWriteGameScore->VariationInfo.dwExperience);
	//m_TreasureDB->AddParameter(TEXT("@lVariationLoveLiness"),pWriteGameScore->VariationInfo.lLoveLiness);
	m_TreasureDB->AddParameter(TEXT("@dwVariationPlayTimeCount"),pWriteGameScore->VariationInfo.dwPlayTimeCount);

	//控制值
	//m_TreasureDB->AddParameter(TEXT("@lControlScore"),pWriteGameScore->lControlScore);

	//抽水信息
	m_TreasureDB->AddParameter(TEXT("@lChoushui"),pWriteGameScore->VariationInfo.lChoushui);;
	m_TreasureDB->AddParameter(TEXT("@dwChoushuiType"),pWriteGameScore->VariationInfo.dwChoushuiType);;

	//属性信息
	//TODONOW 
	m_TreasureDB->AddParameter(TEXT("@wKindID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@wServerID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@strClientIP"),szClientAddr);

	//执行查询
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_WriteGameScore"),true);

	return true;
}


//游戏记录
bool CDataBaseEngineSink::OnRequestGameScoreRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
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
	m_TreasureDB->ResetParameter();
	//TODONOW 
	m_TreasureDB->AddParameter(TEXT("@wKindID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@wServerID"),g_GameCtrl->GetServerID());

	//桌子信息
	m_TreasureDB->AddParameter(TEXT("@wTableID"),pGameScoreRecord->wTableID);
	m_TreasureDB->AddParameter(TEXT("@wUserCount"),pGameScoreRecord->wUserCount);
	m_TreasureDB->AddParameter(TEXT("@wAndroidCount"),pGameScoreRecord->wAndroidCount);

	//税收损耗
	m_TreasureDB->AddParameter(TEXT("@lWasteCount"),pGameScoreRecord->lWasteCount);
	m_TreasureDB->AddParameter(TEXT("@lRevenueCount"),pGameScoreRecord->lRevenueCount);

	//统计信息
	m_TreasureDB->AddParameter(TEXT("@dwUserMemal"),pGameScoreRecord->dwUserMemal);
	m_TreasureDB->AddParameter(TEXT("@dwPlayTimeCount"),pGameScoreRecord->dwPlayTimeCount);

	//时间信息
	m_TreasureDB->AddParameter(TEXT("@SystemTimeStart"),pGameScoreRecord->SystemTimeStart);
	m_TreasureDB->AddParameter(TEXT("@SystemTimeConclude"),pGameScoreRecord->SystemTimeConclude);

	//添加库存相关yang
	m_TreasureDB->AddParameter(TEXT("@ChangeStockScore"),pGameScoreRecord->wChangeStockScore);

	//执行查询
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_RecordDrawInfo"),true);

	//写入记录
	if (lResultCode==DB_SUCCESS)
	{
		//获取标识
		DWORD dwDrawID=m_TreasureDB->GetValue_DWORD(TEXT("DrawID"));

		//写入记录
		for (WORD i=0;i<pGameScoreRecord->wRecordCount;i++)
		{
			//重置参数
			m_TreasureDB->ResetParameter();

			//房间信息
			m_TreasureDB->AddParameter(TEXT("@dwDrawID"),dwDrawID);
			m_TreasureDB->AddParameter(TEXT("@dwUserID"),pGameScoreRecord->GameScoreRecord[i].dwUserID);
			m_TreasureDB->AddParameter(TEXT("@wChairID"),pGameScoreRecord->GameScoreRecord[i].wChairID);

			//成绩信息
			//m_TreasureDB->AddParameter(TEXT("@lScore"),pGameScoreRecord->GameScoreRecord[i].lScore);
			//m_TreasureDB->AddParameter(TEXT("@lGrade"),pGameScoreRecord->GameScoreRecord[i].lGrade);
			m_TreasureDB->AddParameter(TEXT("@lRevenue"),pGameScoreRecord->GameScoreRecord[i].lRevenue);
			//m_TreasureDB->AddParameter(TEXT("@lControlScore"),pGameScoreRecord->GameScoreRecord[i].lControlScore);
			m_TreasureDB->AddParameter(TEXT("@lChoushui"),pGameScoreRecord->GameScoreRecord[i].lChoushui);
			m_TreasureDB->AddParameter(TEXT("@dwChoushuiType"),pGameScoreRecord->GameScoreRecord[i].dwChoushuiType);
			//m_TreasureDB->AddParameter(TEXT("@dwUserMedal"),pGameScoreRecord->GameScoreRecord[i].dwUserMemal);
			m_TreasureDB->AddParameter(TEXT("@dwPlayTimeCount"),pGameScoreRecord->GameScoreRecord[i].dwPlayTimeCount);

			//执行查询
			m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_RecordDrawScore"),true);
		}
	}

	return true;
}


//加载机器
bool CDataBaseEngineSink::OnRequestLoadAndroidUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//变量定义
	DBO_GR_GameAndroidInfo GameAndroidInfo;
	ZeroMemory(&GameAndroidInfo,sizeof(GameAndroidInfo));

	//用户帐户
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@wKindID"),g_GameCtrl->GetKindID());
	m_TreasureDB->AddParameter(TEXT("@wServerID"),g_GameCtrl->GetServerID());

	//执行查询
	GameAndroidInfo.lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_LoadAndroidUser"),true);


	//读取信息
	for (WORD i=0;i<CountArray(GameAndroidInfo.AndroidParameter);i++)
	{
		//结束判断
		if (m_TreasureDB->IsRecordsetEnd()==true) break;

		//溢出效验
		ASSERT(GameAndroidInfo.wAndroidCount<CountArray(GameAndroidInfo.AndroidParameter));
		if (GameAndroidInfo.wAndroidCount>=CountArray(GameAndroidInfo.AndroidParameter)) break;

		//读取数据
		GameAndroidInfo.wAndroidCount++;
		GameAndroidInfo.AndroidParameter[i].dwUserID=m_TreasureDB->GetValue_DWORD(TEXT("UserID"));
		m_TreasureDB->GetValue_String(TEXT("NickName"), GameAndroidInfo.AndroidParameter[i].szNickName, CountArray(GameAndroidInfo.AndroidParameter[i].szNickName));
		m_TreasureDB->GetValue_String(TEXT("HeadUrl"), GameAndroidInfo.AndroidParameter[i].szHeadUrl, CountArray(GameAndroidInfo.AndroidParameter[i].szHeadUrl));

		GameAndroidInfo.AndroidParameter[i].cbGender=m_TreasureDB->GetValue_BYTE(TEXT("Gender"));
		GameAndroidInfo.AndroidParameter[i].lDiamond=m_TreasureDB->GetValue_LONGLONG(TEXT("UserDiamond"));
		GameAndroidInfo.AndroidParameter[i].lGold=m_TreasureDB->GetValue_LONGLONG(TEXT("UserGold"));
		GameAndroidInfo.AndroidParameter[i].lOpenRoomCard=m_TreasureDB->GetValue_LONGLONG(TEXT("UserRoomCard"));

		//移动记录
		m_TreasureDB->MoveToNext();
	}
	CLog::Log(log_debug, "wAndroidCount: %d", GameAndroidInfo.wAndroidCount);


	//增加机器玩家
	for (int i = 0; i < GameAndroidInfo.wAndroidCount; i++)
	{
		//构造数据
		tagUserInfo Info;
		ZeroMemory(&Info, sizeof(tagUserInfo));

		//数据赋值
		Info.cbGender = GameAndroidInfo.AndroidParameter[i].cbGender;
		Info.dwUserID = GameAndroidInfo.AndroidParameter[i].dwUserID;
		Info.lDiamond = GameAndroidInfo.AndroidParameter[i].lDiamond;
		Info.lGold = GameAndroidInfo.AndroidParameter[i].lGold;
		Info.lOpenRoomCard = GameAndroidInfo.AndroidParameter[i].lOpenRoomCard;

		lstrcpyn(Info.szNickName, GameAndroidInfo.AndroidParameter[i].szNickName, CountArray(Info.szNickName));
		lstrcpyn(Info.szHeadUrl, GameAndroidInfo.AndroidParameter[i].szHeadUrl, CountArray(Info.szHeadUrl));

		//状态设置
		Info.cbUserStatus=US_FREE;
		Info.wTableID=INVALID_TABLE;
		Info.wChairID=INVALID_CHAIR;

		//插入数据
		CRobotManager::InsertRobot(Info);
	}

	//设置机器人自动加入房间定时器
	CRobotManager::SetRobotTimer();

	return true;
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
	//转化地址
	TCHAR szClientAddr[16]=TEXT("");
	BYTE * pClientAddr=(BYTE *)&pMatchFee->dwClientAddr;
	_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

	//构造参数
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pMatchFee->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwMatchFee"),pMatchFee->dwMatchFee);
	//TODONOW 
	m_TreasureDB->AddParameter(TEXT("@wKindID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@wServerID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@strClientIP"),szClientAddr);
	m_TreasureDB->AddParameter(TEXT("@dwMatchID"),pMatchFee->dwMatchID);
	m_TreasureDB->AddParameter(TEXT("@dwMatchNO"),pMatchFee->dwMatchNO);
	m_TreasureDB->AddParameter(TEXT("@strMachineID"),pMatchFee->szMachineID);

	//输出参数
	TCHAR szDescribeString[128]=TEXT("");
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//结果处理
	LONG lReturnValue=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_UserMatchFee"),true);

	//发送结果
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_GR_MATCH_FEE_RESULT,dwContextID,&lReturnValue,sizeof(lReturnValue));

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
	//构造参数
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@wMatchID"),pMatchStart->dwMatchID);
	m_TreasureDB->AddParameter(TEXT("@wMatchNo"),pMatchStart->dwMatchNo);
	m_TreasureDB->AddParameter(TEXT("@wMatchCount"),pMatchStart->wMatchCount);	
	m_TreasureDB->AddParameter(TEXT("@lInitScore"),pMatchStart->lInitScore);	

	//结果处理
	LONG lReturnValue=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_MatchStart"),true);

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

	//构造参数
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@wMatchID"),pMatchOver->dwMatchID);
	m_TreasureDB->AddParameter(TEXT("@wMatchNo"),pMatchOver->dwMatchNo);
	m_TreasureDB->AddParameter(TEXT("@wMatchCount"),pMatchOver->wMatchCount);		

	//结果处理
	LONG lReturnValue=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_MatchOver"),true);

	if(lReturnValue==DB_SUCCESS)
	{
		LONG lCount=0;
		if(m_TreasureDB !=NULL)
			lCount=m_TreasureDB->GetRecordCount();
		DBO_GR_MatchRank *pMatchRank=new DBO_GR_MatchRank[lCount];
		for(LONG i=0; i<lCount; i++)
		{
			pMatchRank[i].cbRank=(BYTE)i;
			m_TreasureDB->GetValue_String(TEXT("NickName"),pMatchRank[i].szNickName,CountArray(pMatchRank[i].szNickName));
			pMatchRank[i].lMatchScore=m_TreasureDB->GetValue_LONG(TEXT("HScore"));
			m_TreasureDB->MoveToNext();
			if(m_TreasureDB->IsRecordsetEnd()) break;
		}
		g_AttemperEngineSink->OnEventDataBaseResult(DBO_GR_MATCH_RANK,dwContextID,pMatchRank,(WORD)(sizeof(DBO_GR_MatchRank)*lCount));
		if(pMatchRank!=NULL)
			SafeDeleteArray(pMatchRank);
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
	//转化地址
	TCHAR szClientAddr[16]=TEXT("");
	BYTE * pClientAddr=(BYTE *)&pMatchReward->dwClientAddr;
	_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

	//构造参数
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pMatchReward->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwMatchID"),pMatchReward->dwMatchID);
	m_TreasureDB->AddParameter(TEXT("@dwMatchNO"),pMatchReward->dwMatchNO);
	m_TreasureDB->AddParameter(TEXT("@wRank"),pMatchReward->wRank);
	m_TreasureDB->AddParameter(TEXT("@lMatchScore"),pMatchReward->lMatchScore);
	m_TreasureDB->AddParameter(TEXT("@dwExperience"),pMatchReward->dwExperience);
	m_TreasureDB->AddParameter(TEXT("@dwGold"),pMatchReward->dwGold);
	m_TreasureDB->AddParameter(TEXT("@dwMedal"),pMatchReward->dwMedal);
	//TODONWO 
	m_TreasureDB->AddParameter(TEXT("@wKindID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@wServerID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@strClientIP"),szClientAddr);

	//结果处理
	LONG lReturnValue=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_MatchReward"),true);

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

	//转化地址
	TCHAR szClientAddr[16]=TEXT("");
	BYTE * pClientAddr=(BYTE *)&pMatchFee->dwClientAddr;
	_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

	//构造参数
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pMatchFee->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwMatchFee"),pMatchFee->dwMatchFee);
	//TOODNOW 
	m_TreasureDB->AddParameter(TEXT("@wKindID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@wServerID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@strClientIP"),szClientAddr);
	m_TreasureDB->AddParameter(TEXT("@dwMatchID"),pMatchFee->dwMatchID);
	m_TreasureDB->AddParameter(TEXT("@dwMatchNO"),pMatchFee->dwMatchNO);
	m_TreasureDB->AddParameter(TEXT("@strMachineID"),pMatchFee->szMachineID);

	//输出参数
	TCHAR szDescribeString[128]=TEXT("");
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//结果处理
	LONG lReturnValue=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_UserMatchQuit"),true);

	//发送结果
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_GR_MATCH_QUIT_RESULT,dwContextID,&lReturnValue,sizeof(lReturnValue));

	return true;

}

//写当前库存
bool CDataBaseEngineSink::OnWriteCurrentStock( DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID )
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_WriteCurrentStock));
	if (wDataSize!=sizeof(DBR_GR_WriteCurrentStock)) return false;

	DBR_GR_WriteCurrentStock * pWriteStock = (DBR_GR_WriteCurrentStock *)pData;
	//房间信息
	m_TreasureDB->ResetParameter();

	m_TreasureDB->AddParameter(TEXT("@wServerID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@ConfigName"),1);

	TCHAR  extend[50] = TEXT("");
	wsprintf( extend,TEXT("%d"), pWriteStock->wCurrentStock);
	m_TreasureDB->AddParameter(TEXT("@CurrentStockScore"),extend);

	//执行查询
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_WriteStockScore"),true);

	return true;
}

//用户权限
bool CDataBaseEngineSink::OnRequestManageUserRight(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{

	//效验参数
	if (wDataSize!=sizeof(DBR_GR_ManageUserRight)) return false;

	//请求处理
	DBR_GR_ManageUserRight * pManageUserRight=(DBR_GR_ManageUserRight *)pData;
	dwUserID=pManageUserRight->dwUserID;

	//构造参数
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pManageUserRight->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwAddRight"),pManageUserRight->dwAddRight);
	m_TreasureDB->AddParameter(TEXT("@dwRemoveRight"),pManageUserRight->dwRemoveRight);

	//执行查询
	LONG lReturnValue=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_ManageUserRight"),false);

	return true;
}

//清除数据表数据yang
VOID CDataBaseEngineSink::OnClearDB()
{
	//构造参数
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("ServerID"),g_GameCtrl->GetServerID());

	//执行查询
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_ClearRoomLocker"),true);
}

#pragma region Club牌友圈

//club牌友圈房间信息
bool CDataBaseEngineSink::On_DBR_CLUB_ROOM_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	if (wDataSize!=sizeof(STR_DBR_CLUB_ROOM_INFO)) return false;
	STR_DBR_CLUB_ROOM_INFO* pTableInfo = (STR_DBR_CLUB_ROOM_INFO*)pData;

	//构造参数
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pTableInfo->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwClubID"),pTableInfo->dwClubID);
	m_TreasureDB->AddParameter(TEXT("@dwGameID"),pTableInfo->dwGameID);
	m_TreasureDB->AddParameter(TEXT("@byModeID"),pTableInfo->byModeID);
	//m_TreasureDB->AddParameter(TEXT("@szKindName"),pTableInfo->dwLockState);
	m_TreasureDB->AddParameter(TEXT("@dwServiceGold"),pTableInfo->dwServiceGold);
	m_TreasureDB->AddParameter(TEXT("@dwRevenue"),pTableInfo->dwRevenue);
	m_TreasureDB->AddParameter(TEXT("@byPlayerNum"),0); //创建房间时候默认为0
	m_TreasureDB->AddParameter(TEXT("@DissolveMinute"),pTableInfo->DissolveMinute);
	m_TreasureDB->AddParameter(TEXT("@byChairNum"),pTableInfo->byChairNum);
	m_TreasureDB->AddParameter(TEXT("@byAllRound"),pTableInfo->byAllRound);

	//房间规则说明 40个TCHAR字符
	TCHAR szRoomRuleNote[40] = TEXT("");
	memcpy(szRoomRuleNote, (pTableInfo->szRealRoomRule) + 128, sizeof(TCHAR)*40);

	//房间名字 16个TCHAR字符
	TCHAR szRoomName[16] = TEXT("");
	memcpy(szRoomName, (pTableInfo->szRealRoomRule) + 128 + sizeof(TCHAR)*40, sizeof(TCHAR)*16);

	//16进制的房间规则 256个字节
	CString strRealRoomRule = toHexString(pTableInfo->szRealRoomRule, 255); //舍弃最后一个字符

	m_TreasureDB->AddParameter(TEXT("@szRoomRule"),szRoomRuleNote);
	m_TreasureDB->AddParameter(TEXT("@szRealRoomRule"), strRealRoomRule);

	m_TreasureDB->AddParameter(TEXT("@dwDizhu"),pTableInfo->dwDizhu);
	m_TreasureDB->AddParameter(TEXT("@dwGold"), pTableInfo->dwGold);
	m_TreasureDB->AddParameter(TEXT("@byMask"),pTableInfo->byMask);
	m_TreasureDB->AddParameter(TEXT("@RoomName"),szRoomName);

	//执行过程
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_WRITE_CLUB_ROOM_INFO"),false);

	if (lResultCode != DB_SUCCESS)
	{
	}


	return true;
}

//金币大厅 桌子信息
bool CDataBaseEngineSink::On_DBR_HALL_GOLD_TABLE_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	if (wDataSize!=sizeof(STR_DBR_HALL_GOLD_TABLE_INFO)) return false;
	STR_DBR_HALL_GOLD_TABLE_INFO* pTableInfo = (STR_DBR_HALL_GOLD_TABLE_INFO*)pData;

	//构造参数
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@FirmID"),_MYSTERY);
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pTableInfo->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwKindID"),pTableInfo->dwKindID);
	m_TreasureDB->AddParameter(TEXT("@byGameType"),pTableInfo->byGameType);
	m_TreasureDB->AddParameter(TEXT("@dwTableID"),pTableInfo->dwTableID);
	m_TreasureDB->AddParameter(TEXT("@dwTableState"),pTableInfo->dwTableState);
	m_TreasureDB->AddParameter(TEXT("@byMask"),pTableInfo->byMask);
	m_TreasureDB->AddParameter(TEXT("@dwGameID"),pTableInfo->dwGameID);

	//执行过程
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_WRITE_HALL_GOLD_TABLE_INFO"),true);

	if (lResultCode != DB_SUCCESS)
	{
		return true;
	}


	return true;
}

//金币大厅 玩家信息
bool CDataBaseEngineSink::On_DBR_HALL_GOLD_PLAYER_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	if (wDataSize!=sizeof(STR_DBR_HALL_GOLD_PLAYER_INFO)) return false;
	STR_DBR_HALL_GOLD_PLAYER_INFO* pTableInfo = (STR_DBR_HALL_GOLD_PLAYER_INFO*)pData;

	//构造参数
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pTableInfo->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwTableID"),pTableInfo->dwTableID);
	m_TreasureDB->AddParameter(TEXT("@byChairID"),pTableInfo->byChairID);
	m_TreasureDB->AddParameter(TEXT("@byMask"),pTableInfo->byMask);

	//执行过程
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_WRITE_HALL_GOLD_PLAYER_INFO"),true);

	if (lResultCode != DB_SUCCESS)
	{
		return true;
	}

	return true;
}

#pragma endregion
////////////////////////////////////////////////////////////////////////////////// 
//添加开房信息
bool CDataBaseEngineSink::OnRequestAddTableInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_UpdateTableInfo));
	if (wDataSize!=sizeof(DBR_GR_UpdateTableInfo)) return false;

	DBR_GR_UpdateTableInfo* pTableInfo = (DBR_GR_UpdateTableInfo*)pData;
	//构造参数
	//构造参数
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@wTableID"),pTableInfo->dwTableID);
	m_TreasureDB->AddParameter(TEXT("@dwPassword"),pTableInfo->dwPassword);
	m_TreasureDB->AddParameter(TEXT("@dwCreaterID"),pTableInfo->dwCreaterID);
	m_TreasureDB->AddParameter(TEXT("@wJuShu"),pTableInfo->wJuShu);
	m_TreasureDB->AddParameter(TEXT("@byMode"),pTableInfo->byMode);
	m_TreasureDB->AddParameter(TEXT("@byZhuangType"),pTableInfo->byZhuangType);
	m_TreasureDB->AddParameter(TEXT("@byPlayerCount"),pTableInfo->byPlayerCount);
	m_TreasureDB->AddParameter(TEXT("@byMaxPlayerCount"),pTableInfo->byMaxPlayerCount);
	m_TreasureDB->AddParameter(TEXT("@byIsStart"),pTableInfo->byIsStart);
	m_TreasureDB->AddParameter(TEXT("@byCost"),pTableInfo->byCost);
	m_TreasureDB->AddParameter(TEXT("@szCreateTime"),pTableInfo->szTime);

	//输出参数
	TCHAR szDescribeString[128]=TEXT("");
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//执行过程
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_UpdateTableInfo"),true);
	//结果处理
	CDBVarValue DBVarValue;
	m_TreasureDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	if (lResultCode==DB_SUCCESS)
	{
	}
	else
	{
	}

	return true;
}

//开始桌子
bool CDataBaseEngineSink::OnRequestStartTable(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DWORD));
	if (wDataSize!=sizeof(DWORD)) return false;

	DWORD* pTableID = (DWORD*)pData;
	//构造参数
	//构造参数
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwTableID"),*pTableID);

	//输出参数
	TCHAR szDescribeString[128]=TEXT("");
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//执行过程
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_StartTable"),true);
	//结果处理
	CDBVarValue DBVarValue;
	m_TreasureDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	if (lResultCode==DB_SUCCESS)
	{

	}
	else
	{
	}

	return true;
}

//结束桌子
bool CDataBaseEngineSink::OnRequestEndTable(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_EndTable));
	if (wDataSize!=sizeof(DBR_GP_EndTable)) return false;

	DBR_GP_EndTable* pEndTable = (DBR_GP_EndTable*)pData;
	//构造参数
	//构造参数
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwTableID"),pEndTable->dwTableID);
	m_TreasureDB->AddParameter(TEXT("@dwUserID1"),pEndTable->dwUserID[0]);
	m_TreasureDB->AddParameter(TEXT("@dwUserID2"),pEndTable->dwUserID[1]);
	m_TreasureDB->AddParameter(TEXT("@dwUserID3"),pEndTable->dwUserID[2]);
	m_TreasureDB->AddParameter(TEXT("@dwUserID4"),pEndTable->dwUserID[3]);
	m_TreasureDB->AddParameter(TEXT("@dwUserID5"),pEndTable->dwUserID[4]);
	m_TreasureDB->AddParameter(TEXT("@dwScore1"),pEndTable->dwScore[0]);
	m_TreasureDB->AddParameter(TEXT("@dwScore2"),pEndTable->dwScore[1]);
	m_TreasureDB->AddParameter(TEXT("@dwScore3"),pEndTable->dwScore[2]);
	m_TreasureDB->AddParameter(TEXT("@dwScore4"),pEndTable->dwScore[3]);
	m_TreasureDB->AddParameter(TEXT("@dwScore5"),pEndTable->dwScore[4]);

	//输出参数
	TCHAR szDescribeString[128]=TEXT("");
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//执行过程
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_EndTable"),true);
	//结果处理
	CDBVarValue DBVarValue;
	m_TreasureDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	if (lResultCode==DB_SUCCESS)
	{

	}
	else
	{
	}

	return true;
}



//添加用户到房间
bool CDataBaseEngineSink::OnRequestAddUserToTable(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_AddTableUser));
	if (wDataSize!=sizeof(DBR_GP_AddTableUser)) return false;

	DBR_GP_AddTableUser* pEndTable = (DBR_GP_AddTableUser*)pData;
	//构造参数
	//构造参数
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwTableID"),pEndTable->dwTableID);
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pEndTable->dwUserID);
	//输出参数
	TCHAR szDescribeString[128]=TEXT("");
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//执行过程
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_AddTableUser"),true);
	//结果处理
	CDBVarValue DBVarValue;
	m_TreasureDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	if (lResultCode==DB_SUCCESS)
	{

	}
	else
	{
	}

	return true;
}

//删除房间用户
bool CDataBaseEngineSink::OnRequestDeleteUserToTable(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_DeleteTableUser));
	if (wDataSize!=sizeof(DBR_GP_DeleteTableUser)) return false;

	DBR_GP_DeleteTableUser* pEndTable = (DBR_GP_DeleteTableUser*)pData;
	//构造参数
	//构造参数
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwTableID"),pEndTable->dwTableID);
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pEndTable->dwUserID);
	//输出参数
	TCHAR szDescribeString[128]=TEXT("");
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//执行过程
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_DeleteTableUser"),true);
	//结果处理
	CDBVarValue DBVarValue;
	m_TreasureDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	if (lResultCode==DB_SUCCESS)
	{
		//
	}
	else
	{
	}


	return true;
}


//更新开房信息
bool CDataBaseEngineSink::OnUpdateTableInfo(DWORD dwTableID)
{
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("dwTableID"),dwTableID);

	//执行查询
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_GetTableInfo"),true);

	//结果处理
	if (lResultCode==DB_SUCCESS)
	{

		//变量定义
		DBO_GP_TableInfo TableInfo;
		ZeroMemory(&TableInfo,sizeof(TableInfo));

		TableInfo.dwTableID = m_TreasureDB->GetValue_DWORD(TEXT("TableID"));
		TableInfo.dwCreaterID = m_TreasureDB->GetValue_DWORD(TEXT("CreaterID"));
		TableInfo.dwPassword = m_TreasureDB->GetValue_DWORD(TEXT("Password"));
		TableInfo.wJuShu = m_TreasureDB->GetValue_WORD(TEXT("JuShu"));
		TableInfo.byMode = m_TreasureDB->GetValue_BYTE(TEXT("GameMode"));
		TableInfo.byZhuangType = m_TreasureDB->GetValue_BYTE(TEXT("ZhuangType"));
		TableInfo.byPlayerCount = m_TreasureDB->GetValue_BYTE(TEXT("PlayerCount"));
		TableInfo.byMaxPlayerCount = m_TreasureDB->GetValue_BYTE(TEXT("MaxPlayerCount"));
		TableInfo.byCost = m_TreasureDB->GetValue_BYTE(TEXT("PayCost"));
		TableInfo.byIsStart = m_TreasureDB->GetValue_BYTE(TEXT("IsStart"));

		TableInfo.dwUserID[0] =  m_TreasureDB->GetValue_DWORD(TEXT("UserID1"));
		TableInfo.dwUserID[1] =  m_TreasureDB->GetValue_DWORD(TEXT("UserID2"));
		TableInfo.dwUserID[2] =  m_TreasureDB->GetValue_DWORD(TEXT("UserID3"));
		TableInfo.dwUserID[3] =  m_TreasureDB->GetValue_DWORD(TEXT("UserID4"));
		TableInfo.dwUserID[4] =  m_TreasureDB->GetValue_DWORD(TEXT("UserID5"));

		lstrcpyn( TableInfo.szNickName[0],TEXT("NickName1"),CountArray(TableInfo.szNickName[0]));
		lstrcpyn( TableInfo.szNickName[1],TEXT("100012"),CountArray(TableInfo.szNickName[1]));
		lstrcpyn( TableInfo.szNickName[2],TEXT(""),CountArray(TableInfo.szNickName[1]));
		lstrcpyn( TableInfo.szNickName[3],TEXT(""),CountArray(TableInfo.szNickName[2]));
		lstrcpyn( TableInfo.szNickName[4],TEXT(""),CountArray(TableInfo.szNickName[3]));

		m_TreasureDB->GetValue_String(TEXT("CreateTime"),TableInfo.szTime,CountArray(TableInfo.szTime));	

		//g_AttemperEngineSink->OnEventDataBaseResult(DBO_GP_TABLE_INFO_RESULT,dwContextID,&TableInfo,sizeof(DBO_GP_TableInfo));
	}

	return true;
}

//更新已开房信息
bool CDataBaseEngineSink::OnUpdateEndTableInfo(DWORD dwTableID)
{
	return true;
}

//更新游戏任务状态
bool CDataBaseEngineSink::On_Table_UpdateGameTaskStatus(DWORD dwContextID, void *pData, WORD wDataSize)
{
	//数据包校验
	if (sizeof(STR_DBR_CG_TABLE_UPDATE_TASK_STATUS) != wDataSize)
		return false;

	STR_DBR_CG_TABLE_UPDATE_TASK_STATUS* pDbReq = (STR_DBR_CG_TABLE_UPDATE_TASK_STATUS*)pData;

	//数据库传入参数
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@cbCurGameCount"), pDbReq->cbCurGameCount);
	m_TreasureDB->AddParameter(TEXT("@cbTableMode"), pDbReq->cbTableMode);

	//输出变量
	WCHAR szDescribe[128] = L"";
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

	//执行查询,执行抽奖
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_SG_Table_UpdateGameTaskStatus"), true);

	//只需要更新，不需要返回，获得任务列表时会重新获得任务状态

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
