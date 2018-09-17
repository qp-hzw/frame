#include "StdAfx.h"
#include <vector>
#include <string>
#include <algorithm> 
#include "ServiceUnits.h"
#include "DataBaseEngineSink.h"
#include "LogicTraceHelper.h"
#include "RankManager.h"


#include "..\..\依赖项\内核引擎\TraceService.h"
#include "..\..\依赖项\服务核心\WHService.h"
#include "..\..\依赖项\全局定义\Array.h"
#include "..\..\全局定义\Define.h"
#include "..\..\消息定义\STR_CMD_GameServer.h"

//构造函数
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//组件变量
	m_pIDataBaseEngineEvent=NULL;

	return;
}
//接口查询
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//创建对象
	if ((m_AccountsDBModule.GetInterface()==NULL)&&(m_AccountsDBModule.CreateInstance()==false))
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

	try
	{
		//设置连接
		m_PlatformDBModule->SetConnectionInfo(1);
		m_AccountsDBModule->SetConnectionInfo(2);
		m_TreasureDBModule->SetConnectionInfo(3);

		//发起连接
		m_AccountsDBModule->OpenConnection();
		m_AccountsDBAide.SetDataBase(m_AccountsDBModule.GetInterface());

		//发起连接
		m_TreasureDBModule->OpenConnection();
		m_TreasureDBAide.SetDataBase(m_TreasureDBModule.GetInterface());

		//发起连接
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		return false;
	}

	return true;
}

//停止事件
bool CDataBaseEngineSink::OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx)
{
	//设置对象
	m_AccountsDBAide.SetDataBase(NULL);
	m_TreasureDBAide.SetDataBase(NULL);
	m_PlatformDBAide.SetDataBase(NULL);

	//关闭连接
	if (m_AccountsDBModule.GetInterface()!=NULL)
	{
		m_AccountsDBModule->CloseConnection();
		m_AccountsDBModule.CloseInstance();
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

	//组件变量
	m_pIDataBaseEngineEvent=NULL;

	return true;
}

//数据库请求事件
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBR_GP_LOGON_USER_STATE: //用户Sockt连接关闭
		{
			return On_DBR_GP_QUIT(dwContextID,pData,wDataSize);
		}
	case DBR_CL_LOGON_ACCOUNTS:			//帐号登录
		{
			return On_DBR_Logon_Accounts(dwContextID,pData,wDataSize);
		}
	case DBR_CL_LOGON_REGISTER:			//注册帐号
		{
			return On_DBR_Logon_Register(dwContextID,pData,wDataSize);
		}
	case DBR_CL_LOGON_PLATFORM:			//平台登录
		{
			return On_DBR_Logon_Platform(dwContextID,pData,wDataSize);
		}
	case DBR_CL_USER_QUERY_SCORE_INFO:		//查询金币钻石信息
		{
			return On_DBR_QueryScoreInfo(dwContextID,pData,wDataSize);
		}
#pragma region 启动命令
	case DBR_GP_LOAD_GAME_LIST:				//加载列表
		{
			return OnRequestLoadGameList(dwContextID,pData,wDataSize);
		}
	case DBR_GP_ONLINE_COUNT_INFO:			//在线统计 -- 发给数据库
		{
			return OnRequestOnLineCountInfo(dwContextID,pData,wDataSize);
		}
	case DBR_UPDATA_MARQUEE:				//加载跑马灯消息
		{
			return On_DBR_UPDATA_MARQUEE(dwContextID,pData,wDataSize);
		}
#pragma endregion 
	case DBR_CL_SERVICE_USER_FEEDBACK:			//玩家反馈
		{
			return On_DBR_Service_UserFeedback(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_REFRESH_USER_INFO:		//刷新用户信息
		{
			return On_DBR_Service_RefreshUserInfo(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_QUERY_ROOM_LIST:		//查询开房信息列表
		{
			return On_DBR_Service_QueryRoomList(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_GET_RICH_LIST:			//获取富豪榜
		{
			return On_DBR_Service_GetRichList(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_GET_USER_RECORD_LIST:	//获取用户戏录像列表
		{
			return On_DBR_Service_GetUserRecordList(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_GET_SPECIFIC_RECORD:	//获取指定ID录像数据（小局）
		{
			return On_DBR_Service_GetSpecifiRecord(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_ONLINE_REWARD:			//获取在线奖励
		{
			return On_DBR_Service_OnlineReward(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_GET_TASK_LIST:			//获取任务列表
		{
			return On_DBR_Service_GetTaskList(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_GET_TASK_REWARD:		//领取任务奖励
		{
			return On_DBR_Service_GetTaskReward(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_REQUEST_LOTTERY:		//抽奖请求
		{
			return On_DBR_Service_RequestLottery(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_PURE_STANDING_LIST:		//pure大厅排行榜
		{
			return On_DBR_CL_SERVICE_PURE_STANDING_LIST(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_PURE_RECORD_LIST:		//pure大局战绩
		{
			return On_DBR_CL_SERVICE_PURE_RECORD_LIST(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_PURE_XJ_RECORD_LIST:	//pure小局战绩
		{
			return On_DBR_CL_SERVICE_PURE_XJ_RECORD_LIST(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_XJ_RECORD_PLAYBACK:		//小局录像回放
		{
			return On_DBR_CL_Service_XJRecordPlayback(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_CUSTOMER_MESSEGE:		//客服消息
		{
			return On_DBR_CL_SERVICE_CUSTOMER_MESSEGE(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_GOLD_INFO:				//请求金币大厅信息
		{
			return On_DBR_CL_SERVICE_GOLD_INFO(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_MODIFY_PERSONAL_INFO:	//修改个人资料
		{
			return On_DBR_Service_ModifyPersonalInfo(dwContextID,pData,wDataSize);
		}
	case DBR_CL_USER_RECHARGE_INFO:				//充值信息
		{
			return On_DBR_Other_RechargeInfo(dwContextID,pData,wDataSize);
		}
	case DBR_CL_USER_EXCHANGE_INFO:				//兑换信息
		{
			return On_DBR_Other_ExchangeInfo(dwContextID,pData,wDataSize);
		}
#pragma region MDM_GIFT 礼物道具
	case DBR_CL_GIFT_GIVE_PROPS:			//赠送道具
		{
			return On_DBR_CL_GIFT_GIVE_PROPS(dwContextID,pData,wDataSize);
		}
#pragma endregion
	case DBR_GP_UPDATE_RANK_VALUE:			//更新排行榜用户数据
		{
			return OnDBUpdateRankUserItem(dwContextID,pData,wDataSize);
		}
	case DBR_GP_READ_RANK_LIST:				//获取排行榜列表
		{
			return OnDBReadRankList(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_GET_RANK_REWARD:	//领取排行榜奖励
		{
			return OnReceiveRankReward(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_USER_INSURE:			//修改用户财富信息
		{
			return OnModifyUserInsure(dwContextID,pData,wDataSize);
		}
#pragma region MDM_CLUB 牌友圈(版本2)
	case DBR_CL_CLUB_ALL_CLUB_INFO_LIST:  //查询牌友圈列表
		{
			return On_DBR_CL_CLUB_ALL_CLUB_INFO_LIST(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_ROOM_LIST: //查询指定牌友圈房间列表
		{
			return On_DBR_CL_CLUB_ROOM_LIST(dwContextID,pData,wDataSize); 
		}
	case DBR_CL_CLUB_RANDOM_CLUB_LIST:  //查询未满员, 随机牌友圈(最大9个)
		{
			return On_DBR_CL_CLUB_RANDOM_CLUB_LIST(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_JOIN_CLUB: //申请加入牌友圈
		{
			return On_DBR_CL_CLUB_JOIN_CLUB(dwContextID,pData,wDataSize);	 
		}
	case DBR_CL_CLUB_DISS_CLUB: //解散牌友圈
		{
			return On_DBR_CL_CLUB_DISS_CLUB(dwContextID,pData,wDataSize);	
		}
	case DBR_CL_CLUB_ROOM_SETTING: //房间设置
		{
			return On_DBR_CL_CLUB_ROOM_SETTING(dwContextID,pData,wDataSize);	
		}
	case DBR_CL_CLUB_ROOM_QUERY_SETTING: //请求房间设置
		{
			return On_DBR_CL_CLUB_ROOM_QUERY_SETTING(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_ROOM_DISSOLVE: //解散房间请求
		{
			return On_DBR_CL_CLUB_ROOM_DISSOLVE(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_TABLE_DISSOLVE: //解散桌子请求
		{
			return On_DBR_CL_CLUB_TABLE_DISSOLVE(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_ROOM_USER_LEAVE://玩家离开俱乐部房间
		{
			return On_DBR_CL_CLUB_ROOM_USER_LEAVE(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_NOTICE: //牌友圈公告
		{
			return On_DBR_CL_CLUB_NOTICE(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_MESSAGE: //牌友圈简介
		{
			return On_DBR_CL_CLUB_MESSAGE(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_CONTRIBUTE_FK://贡献房卡
		{
			return On_DBR_CL_CLUB_CONTRIBUTE_FK(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_AUTO_AGREE://牌友圈设置
		{
			return On_DBR_CL_CLUB_AUTO_AGREE(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_INVITE: //邀请他人加入牌友圈
		{
			return On_DBR_CL_CLUB_INVITE(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_INVITE_RESULT: //被邀人 回复
		{
			return On_DBR_CL_CLUB_INVITE_RESULT(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_INQUERY_LIST://被邀请人查看自己的邀请列表
		{
			return On_DBR_CL_CLUB_INQUERY_LIST(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_APPLICANT_LIST://申请人列表
		{
			return On_DBR_CL_CLUB_APPLICANT_LIST(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_APPOINTMENT://职务任免
		{
			return On_DBR_CL_CLUB_APPOINTMENT(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_CHAT://牌友圈聊天
		{
			return On_DBR_CL_CLUB_CHAT(dwContextID,pData,wDataSize);
		}
	case DBR_CL_WORD_CHAT: //世界聊天
		{
			return On_DBR_CL_WORD_CHAT(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SYSTEM_CHAT: //系统聊天
		{
			return On_DBR_CL_SYSTEM_CHAT(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SECRET_CHAT: //私密聊天
		{
			return On_DBR_CL_SECRET_CHAT(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_STICKY_POST:	//牌友圈置顶
		{
			return On_DBR_CL_STICKY_POST(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_RECORD_LIST: //工会战绩统计
		{
			return On_DBR_CL_CLUB_RECORD_LIST(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_CREATE_CLUB:  //创建牌友圈
		{
			return On_DBR_CL_CLUB_CREATE_CLUB(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_JOIN_ROOM:  //申请加入房间
		{
			return On_DBR_CL_CLUB_JOIN_ROOM(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_APPLICANT_RESULT:  //群主|管理对申请消息的答复(同意|拒绝)
		{
			return On_DBR_CL_CLUB_APPLICANT_RESULT(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_MEMBER_MANAGER: //请求成员数据
		{
			return On_DBR_CL_CLUB_MEMBER_MANAGER(dwContextID,pData,wDataSize); 
		}
	case DBR_CL_CLUB_QUIT:  //踢出退出请求
		{
			return On_DBR_CL_CLUB_QUIT(dwContextID,pData,wDataSize);
		}
#pragma endregion
#pragma region MDM_SHOP 商城道具
	case DBR_CL_SHOP_QUERY:  //查询商城
		{
			return On_DBR_CL_SHOP_QUERY(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SHOP_MONEY: //金钱购买道具
		{
			//return On_DBR_CL_SHOP_MONEY(dwContextID,pData,wDataSize); 
		}
	case DBR_CL_SHOP_DIAMOND:  //钻石购买道具
		{
			return On_DBR_CL_SHOP_DIAMOND(dwContextID,pData,wDataSize);
		}
	case DBR_CL_BAG_QUERY: //物品背包查询
		{
			return On_DBR_CL_BAG_QUERY(dwContextID,pData,wDataSize);	 
		}
#pragma endregion
	}

	return false;
}

/* **************************【MAIN:3    MDM_SERVICE    服务】*************************************/
//玩家反馈
bool CDataBaseEngineSink::On_DBR_Service_UserFeedback( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//效验参数
	ASSERT(wDataSize==sizeof(STR_DBR_CL_SERVICE_FEEDBACK));
	if (wDataSize!=sizeof(STR_DBR_CL_SERVICE_FEEDBACK)) return false;

	//请求处理
	STR_DBR_CL_SERVICE_FEEDBACK * pUserSuggestion = (STR_DBR_CL_SERVICE_FEEDBACK *)pData;

	try
	{
		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@iUserID"),pUserSuggestion->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@chFB_Title"),pUserSuggestion->szFB_Title);
		m_AccountsDBAide.AddParameter(TEXT("@chFB_Content"),pUserSuggestion->szFB_Content);
		m_AccountsDBAide.AddParameter(TEXT("@chContact"),pUserSuggestion->szContact);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_Service_Feedback"));	

		//结果处理
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_Service_Feedback"),true);		//在QPAccountsDB数据库的AccountsInfo表中插入用户反馈

		//返回结果处理
		On_DBO_Service_UserFeedback(dwContextID, lResultCode, TEXT("玩家反馈处理完成!"));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		On_DBO_Service_UserFeedback(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"));

		return false;
	}

	return true;
}

//玩家反馈返回
bool CDataBaseEngineSink::On_DBO_Service_UserFeedback(DWORD dwContextID, DWORD dwResultCode, LPCTSTR pszErrorString)
{
	//变量定义
	STR_DBO_CL_SERVICE_FEEDBACK OperateResult;
	ZeroMemory(&OperateResult,sizeof(OperateResult));

	//构造数据
	OperateResult.lResultCode = dwResultCode;
	lstrcpyn(OperateResult.szDescribeString,pszErrorString,CountArray(OperateResult.szDescribeString));

	//发送结果
	WORD wDataSize=CountStringBuffer(OperateResult.szDescribeString);
	WORD wHeadSize=sizeof(OperateResult)-sizeof(OperateResult.szDescribeString);
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_USER_FEEDBACK,dwContextID,&OperateResult,wHeadSize+wDataSize);

	return true;
}

//刷新用户信息
bool CDataBaseEngineSink::On_DBR_Service_RefreshUserInfo( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//效验参数
	ASSERT(wDataSize == sizeof(STR_DBR_CL_SERCIVR_REFRESH_INFO));
	if (wDataSize != sizeof(STR_DBR_CL_SERCIVR_REFRESH_INFO)) return false;

	//请求处理
	STR_DBR_CL_SERCIVR_REFRESH_INFO * pUserRequest=(STR_DBR_CL_SERCIVR_REFRESH_INFO *)pData;

	try
	{
		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pUserRequest->dwUserID);

		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_Service_RefreshUserInfo"));		

		//结果处理
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_Service_RefreshUserInfo"),true);

		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);


		//结果处理
		On_DBO_Service_RefreshUserInfo(dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal));
	}
	catch(IDataBaseException * pIException)		//异常通用处理
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		////异常结果处理
		//DBR_CommandSource wRequestID ;
		//ZeroMemory(&wRequestID,sizeof(wRequestID));
		//wRequestID.MainCommand = MDM_SERVICE;
		//wRequestID.SubCommand  = SUB_CL_SERVICE_REFRESH_USER_INFO;

		//On_DBO_CommonOperateResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),wRequestID);

		return false;
	}
	return true;
}

//刷新用户信息返回
bool CDataBaseEngineSink::On_DBO_Service_RefreshUserInfo(DWORD dwContextID, DWORD dwResultCode, LPCTSTR pszErrorString)
{
	//构造数据
	STR_DBO_CL_SERCIVR_REFRESH_INFO UserInfo;
	ZeroMemory( &UserInfo, sizeof(UserInfo));
	UserInfo.dwResultCode = dwResultCode;
	lstrcpyn(UserInfo.szDescribeString, pszErrorString, CountArray(UserInfo.szDescribeString));

	if (dwResultCode == DB_SUCCESS)		//成功
	{
		/* *****************************    用户信息     ****************************/
		//用户标志
		UserInfo.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
		//用户昵称
		m_AccountsDBAide.GetValue_String(TEXT("NickName"),UserInfo.szNickName,CountArray(UserInfo.szNickName));
		//用户性别
		UserInfo.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
		//头像索引
		UserInfo.wFaceID=m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
		//个性签名
		m_AccountsDBAide.GetValue_String(TEXT("MySignature"),UserInfo.szMySignature,CountArray(UserInfo.szMySignature));

		//用户元宝
		UserInfo.dwUserDiamond=m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserDiamond"));
		//用户奖牌
		UserInfo.dwUserMedal=m_AccountsDBAide.GetValue_DWORD(TEXT("UserMedal"));
		//经验数值
		UserInfo.dwExperience=m_AccountsDBAide.GetValue_DWORD(TEXT("Experience"));
		//用户魅力
		UserInfo.dwLoveLiness=m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));

		//管理员等级
		UserInfo.cbMasterOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MasterOrder"));
		//会员等级
		UserInfo.cbMemberOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MemberOrder"));
		//会员到期时
		m_AccountsDBAide.GetValue_SystemTime(TEXT("MemberOverDate"),UserInfo.MemberOverDate);

		//真实姓名
		m_AccountsDBAide.GetValue_String(TEXT("IdentityName"),UserInfo.szIdentityName,CountArray(UserInfo.szIdentityName));
		//身份证号
		m_AccountsDBAide.GetValue_String(TEXT("IdentityNum"),UserInfo.szIdentityNum,CountArray(UserInfo.szIdentityNum));
		//手机号码
		m_AccountsDBAide.GetValue_String(TEXT("MobilePhone"),UserInfo.szMobilePhone,CountArray(UserInfo.szMobilePhone));
		/* *****************************    账号信息     ****************************/
		//最后登录地址
		m_AccountsDBAide.GetValue_String(TEXT("LastLogonIP"),UserInfo.szLasLogonIp,CountArray(UserInfo.szLasLogonIp));
		//最后上线时间 
		m_AccountsDBAide.GetValue_SystemTime(TEXT("LastLogonDate"),UserInfo.LasLogonDate);
		/* *****************************    附加数据     ****************************/
		//社团标志 -- 牌友圈
		UserInfo.dwGroupID=m_AccountsDBAide.GetValue_DWORD(TEXT("GroupID"));
		//社团名字 -- 牌友圈
		m_AccountsDBAide.GetValue_String(TEXT("GroupName"),UserInfo.szGroupName,CountArray(UserInfo.szGroupName));

		//用户积分
		UserInfo.lUserScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserScore"));
		//用户游戏币
		UserInfo.lUserGold = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserGold"));
		//用户房卡
		UserInfo.lUserRoomCard = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserRoomCard"));

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_REFRESH_USER_INFO, dwContextID, &UserInfo, sizeof(UserInfo));
	}
	else								//失败处理
	{
		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_REFRESH_USER_INFO, dwContextID, &UserInfo, sizeof(UserInfo));
	}

	return true;
}

//查询开房信息列表
bool CDataBaseEngineSink::On_DBR_Service_QueryRoomList(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		STR_DBR_CL_SERCIVR_QUERY_ROOMLIST* pGetTableInfo = (STR_DBR_CL_SERCIVR_QUERY_ROOMLIST* )pData;
		if(pData == NULL || wDataSize != sizeof(STR_DBR_CL_SERCIVR_QUERY_ROOMLIST))
			return false;

		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("dwOwnerID"),pGetTableInfo->dwUserID);


		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_Service_GetRoomList"));		

		//执行查询
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_Service_GetRoomList"),true);	//查询QPTreasure数据库的TableInfo表

		//结果处理
		On_DBO_Service_QueryRoomList(dwContextID, lResultCode);
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//TODO 错误的处理

		return false;
	}

	return true;

}

//查询开房信息列表返回
bool CDataBaseEngineSink::On_DBO_Service_QueryRoomList(DWORD dwContextID, DWORD dwResultCode)
{
	//结果处理
	if (dwResultCode == DB_SUCCESS)		//成功
	{
		for(int i=0;i < m_TreasureDBModule->GetRecordCount() && i < 10;i++,m_TreasureDBModule->MoveToNext())
		{
			//变量定义
			STR_DBO_CL_SERCIVR_QUERY_ROOMLIST TableInfo;
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

			//用户ID
			TableInfo.dwUserID[0] = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID1"));
			TableInfo.dwUserID[1] = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID2"));
			TableInfo.dwUserID[2] = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID3"));
			TableInfo.dwUserID[3] = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID4"));
			TableInfo.dwUserID[4] = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID5"));

			//用户昵称
			m_TreasureDBAide.GetValue_String(TEXT("NickName1"),TableInfo.szNickName[0],CountArray(TableInfo.szNickName[0]));
			m_TreasureDBAide.GetValue_String(TEXT("NickName2"),TableInfo.szNickName[1],CountArray(TableInfo.szNickName[1]));
			m_TreasureDBAide.GetValue_String(TEXT("NickName3"),TableInfo.szNickName[2],CountArray(TableInfo.szNickName[2]));
			m_TreasureDBAide.GetValue_String(TEXT("NickName4"),TableInfo.szNickName[3],CountArray(TableInfo.szNickName[3]));
			m_TreasureDBAide.GetValue_String(TEXT("NickName5"),TableInfo.szNickName[4],CountArray(TableInfo.szNickName[4]));

			//用户积分
			TableInfo.dwScore[0] = m_TreasureDBAide.GetValue_DWORD(TEXT("Score1"));
			TableInfo.dwScore[1] = m_TreasureDBAide.GetValue_DWORD(TEXT("Score2"));
			TableInfo.dwScore[2] = m_TreasureDBAide.GetValue_DWORD(TEXT("Score3"));
			TableInfo.dwScore[3] = m_TreasureDBAide.GetValue_DWORD(TEXT("Score4"));
			TableInfo.dwScore[4] = m_TreasureDBAide.GetValue_DWORD(TEXT("Score5"));

			m_TreasureDBAide.GetValue_String(TEXT("CreateTime"),TableInfo.szTime,CountArray(TableInfo.szTime));	

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_QUERY_ROOM_LIST, dwContextID, &TableInfo, sizeof(STR_DBO_CL_SERCIVR_QUERY_ROOMLIST));
		}
	}
	else			//TODO 失败是否需要加入通用处理
	{

	}

	return true;
}

//获取富豪榜
bool CDataBaseEngineSink::On_DBR_Service_GetRichList(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		STR_DBR_CL_SERCIVR_GET_RICHLIST* pRichList = (STR_DBR_CL_SERCIVR_GET_RICHLIST* )pData;
		if(pData == NULL || wDataSize != sizeof(STR_DBR_CL_SERCIVR_GET_RICHLIST))
			return false;

		m_AccountsDBAide.ResetParameter();

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_Service_GetRichList")); 
		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_Service_GetRichList"), true);		//查询数据库[QPAccountsDB]中的RegalRank表

		//结果处理
		On_DBO_Service_GetRichList(dwContextID, lResultCode);

	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;

}

//获取富豪榜返回
bool CDataBaseEngineSink::On_DBO_Service_GetRichList(DWORD dwContextID, DWORD dwResultCode)
{
	if(dwResultCode == DB_SUCCESS)
	{
		STR_DBO_CL_SERCIVR_GET_RICHLIST result;
		ZeroMemory(&result,sizeof(STR_DBO_CL_SERCIVR_GET_RICHLIST));

		for(int i=0;i < m_AccountsDBModule->GetRecordCount() && i < 15;i++)
		{
			result.byCount ++;
			m_AccountsDBAide.GetValue_String(TEXT("Name"),result.RegalInfoList[i].szName,CountArray(result.RegalInfoList[i].szName));
			result.RegalInfoList[i].dwMoney = m_AccountsDBAide.GetValue_DWORD(TEXT("Money"));
			m_AccountsDBAide.GetValue_String(TEXT("Wechat"),result.RegalInfoList[i].szWechatAccount,CountArray(result.RegalInfoList[i].szWechatAccount));
			result.RegalInfoList[i].dwRegistID = m_AccountsDBAide.GetValue_DWORD(TEXT("RegistID"));	
			m_AccountsDBAide.GetValue_String(TEXT("QQ"),result.RegalInfoList[i].szQQ,CountArray(result.RegalInfoList[i].szQQ));
			m_AccountsDBAide.GetValue_String(TEXT("MobilePhone"),result.RegalInfoList[i].szMobilePhone,CountArray(result.RegalInfoList[i].szMobilePhone));

			m_AccountsDBModule->MoveToNext();
		}

		if(result.byCount > 0)
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_GET_RICH_LIST,dwContextID,&result,sizeof(STR_DBO_CL_SERCIVR_GET_RICHLIST));
	}

	return true;
}

//获取录像列表
bool CDataBaseEngineSink::On_DBR_Service_GetUserRecordList(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_DBR_CL_SERCIVR_GET_RECORDLIST) < wDataSize)
		{
			return false;
		}

		STR_DBR_CL_SERCIVR_GET_RECORDLIST* pDbReq = (STR_DBR_CL_SERCIVR_GET_RECORDLIST*)pData;

		m_TreasureDBAide.ResetParameter();

		m_TreasureDBAide.AddParameter(TEXT("@UserID"),pDbReq->dwUserID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_Service_GetRecordList"));

		//执行查询
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_Service_GetRecordList"),true);		//查询数据库[QPTreasureDB]中的GameRecord表

		//结果处理
		On_DBO_Service_GetUserRecordList(dwContextID,lResultCode,NULL);
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//获取录像列表返回
bool CDataBaseEngineSink::On_DBO_Service_GetUserRecordList(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	if (dwErrorCode == DB_SUCCESS)
	{
		//获得所有桌子ID
		vector <DWORD> dwTableID;
		for (int i = 0; i < m_TreasureDBModule->GetRecordCount(); i++)
		{
			//删除重复的桌子
			DWORD dwTmpTableID = m_TreasureDBAide.GetValue_DWORD(TEXT("TableID"));
			std::vector <DWORD>::iterator result = find(dwTableID.begin(), dwTableID.end(), dwTmpTableID);
			if ( result == dwTableID.end() )
			{
				dwTableID.push_back(dwTmpTableID);
			}
			m_TreasureDBModule->MoveToNext();
		}

		//移到开头
		m_TreasureDBModule->MoveToFirst();
		int nRecordListNum = m_TreasureDBModule->GetRecordCount();

		//大局得分
		LONGLONG lTotalscore[10][5];
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				lTotalscore[i][j] = 0;
			}			
		}

		//DBO数据
		STR_DBO_CL_SERCIVR_GET_RECORDLIST *RecordList = new STR_DBO_CL_SERCIVR_GET_RECORDLIST[nRecordListNum];
		//TableID赋值
		for (DWORD i = 0; i < dwTableID.size(); i++)
		{
			RecordList[i].dwTableID = dwTableID[i];
		}

		for(int i=0;i < nRecordListNum;i++)
		{
			m_TreasureDBAide.ResetParameter();

			//获得桌子ID
			DWORD dwTempTableID = m_TreasureDBAide.GetValue_DWORD(TEXT("TableID"));

			//获得该桌子的数据
			for (DWORD j = 0; j < dwTableID.size(); j++)
			{
				if ( dwTempTableID == dwTableID[j] )
				{
					RecordList[i].dwUserID[0] = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID0"));
					RecordList[i].dwUserID[1] = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID1"));
					RecordList[i].dwUserID[2] = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID2"));
					RecordList[i].dwUserID[3] = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID3"));
					RecordList[i].dwUserID[4] = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID4"));
					m_TreasureDBAide.GetValue_String(TEXT("Nick0"),RecordList[i].szUserName[0],LEN_NICKNAME);
					m_TreasureDBAide.GetValue_String(TEXT("Nick1"),RecordList[i].szUserName[1],LEN_NICKNAME);
					m_TreasureDBAide.GetValue_String(TEXT("Nick2"),RecordList[i].szUserName[2],LEN_NICKNAME);
					m_TreasureDBAide.GetValue_String(TEXT("Nick3"),RecordList[i].szUserName[3],LEN_NICKNAME);
					m_TreasureDBAide.GetValue_String(TEXT("Nick4"),RecordList[i].szUserName[4],LEN_NICKNAME);
					RecordList[i].byGameMode = m_TreasureDBAide.GetValue_BYTE(TEXT("GameMode"));
					RecordList[i].byZhuangType = m_TreasureDBAide.GetValue_BYTE(TEXT("ZhuangType"));
					RecordList[i].wCurrentJuShu = m_TreasureDBAide.GetValue_WORD(TEXT("CurrentJuShu"));
					RecordList[i].wTotalJuShu = m_TreasureDBAide.GetValue_WORD(TEXT("TotalJuShu"));
					m_TreasureDBAide.GetValue_String(TEXT("GameTime"),RecordList[i].szGameTime,30);
					m_TreasureDBAide.GetValue_String(TEXT("Score"),RecordList[i].szScore,50);

					//获取总分数
					CString strScore = RecordList[i].szScore;
					int nPos = 0;
					int score_index = 0;
					CString strTemp = strScore.Tokenize(CString(TEXT(',')), nPos);
					while (strTemp.Trim() != _T("") && score_index < 4)
					{
						lTotalscore[j][score_index++] += StrToInt(strTemp.GetBuffer());
						strTemp = strScore.Tokenize(CString(TEXT(',')), nPos);
					}
				}
			}

			m_TreasureDBModule->MoveToNext();
		}

		//发送大局录像数据
		for (DWORD i = 0; i < dwTableID.size(); i++)
		{
			//增加大局分数
			_sntprintf_s(RecordList[i].szTotalScore, CountArray(RecordList[i].szTotalScore), TEXT("%lld,%lld,%lld,%lld,%lld"),
				lTotalscore[i][0], lTotalscore[i][1], lTotalscore[i][2], lTotalscore[i][3], lTotalscore[i][4]);

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_GET_USER_RECORD_LIST, dwContextID, &RecordList[i], 
				sizeof(STR_DBO_CL_SERCIVR_GET_RECORDLIST));
		}

		delete RecordList;
	}
	else if(dwErrorCode == DB_NEEDMB)
	{
	}

	return true;
}

//获取指定ID录像
bool CDataBaseEngineSink::On_DBR_Service_GetSpecifiRecord(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_DBR_CL_SERCIVR_GET_SPECIFIC_RECORD) != wDataSize)
		{
			return false;
		}

		STR_DBR_CL_SERCIVR_GET_SPECIFIC_RECORD* pDbReq = (STR_DBR_CL_SERCIVR_GET_SPECIFIC_RECORD*)pData;

		m_TreasureDBAide.ResetParameter();

		m_TreasureDBAide.AddParameter(TEXT("@dwTableID"),pDbReq->dwTableID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);
		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_Service_GetSpecificRecord"));

		//执行查询
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_Service_GetSpecificRecord"),true);		//查询数据库[QPTreasureDB]中的GameRecord表

		//结果处理
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		On_DBO_Service_GetSpecificRecord(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal));
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;

}

//获取指定ID录像返回
bool CDataBaseEngineSink::On_DBO_Service_GetSpecificRecord(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	if (dwErrorCode==DB_SUCCESS)
	{
		for(int i=0; i < m_TreasureDBModule->GetRecordCount(); i++)
		{
			//变量定义
			STR_DBO_CL_SERCIVR_GET_SPECIFIC_RECORD RecordInfo;
			ZeroMemory(&RecordInfo,sizeof(RecordInfo));

			//获得数据
			RecordInfo.wCurrentJuShu = m_TreasureDBAide.GetValue_WORD(TEXT("CurrentJuShu"));
			m_TreasureDBAide.GetValue_String(TEXT("Nick0"), RecordInfo.szNickName[0], LEN_NICKNAME);
			m_TreasureDBAide.GetValue_String(TEXT("Nick1"), RecordInfo.szNickName[1], LEN_NICKNAME);
			m_TreasureDBAide.GetValue_String(TEXT("Nick2"), RecordInfo.szNickName[2], LEN_NICKNAME);
			m_TreasureDBAide.GetValue_String(TEXT("Nick3"), RecordInfo.szNickName[3], LEN_NICKNAME);
			m_TreasureDBAide.GetValue_String(TEXT("Nick4"), RecordInfo.szNickName[4], LEN_NICKNAME);
			m_TreasureDBAide.GetValue_String(TEXT("Score"), RecordInfo.szScore, 50);

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_GET_SPECIFIC_RECORD, dwContextID, &RecordInfo, sizeof(STR_DBO_CL_SERCIVR_GET_SPECIFIC_RECORD));

			m_TreasureDBModule->MoveToNext();
		}
	}

	return true;
}

//获取在线奖励
bool CDataBaseEngineSink::On_DBR_Service_OnlineReward(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_DBR_CL_SERCIVR_ONLINE_REWARD) < wDataSize)
		{
			return false;
		}

		STR_DBR_CL_SERCIVR_ONLINE_REWARD* pDbReq = (STR_DBR_CL_SERCIVR_ONLINE_REWARD*)pData;

		//登录奖励200金币

		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@UserID"), pDbReq->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@lGold"), 200);
		m_TreasureDBAide.AddParameter(TEXT("@lOpenRoomCard"), 0);
		m_TreasureDBAide.AddParameter(TEXT("@lDiamond"), 0);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_Service_ModifyUserMoney")); 
		//执行查询
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_Service_ModifyUserMoney"), true);	//修改[QPTreasureDB]数据库中的GameScoreInfo表

		//结果处理
		On_DBO_Service_OnlineReward(dwContextID, lResultCode, TEXT("领取登录奖励失败！"));

	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;

}

//获取在线奖励返回
bool CDataBaseEngineSink::On_DBO_Service_OnlineReward(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	if(dwErrorCode == DB_SUCCESS)
	{
		STR_DBO_CL_SERCIVR_ONLINE_REWARD result;
		result.byType = 1;
		result.dwCount = m_TreasureDBAide.GetValue_DWORD(TEXT("Gold"));
		lstrcpyn(result.szDescribe,TEXT("成功领取登录奖励！"),CountArray(result.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_ONLINE_REWARD, dwContextID, &result, sizeof(STR_DBO_CL_SERCIVR_ONLINE_REWARD));

		//财富变更处理
		STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO UserScoreInfo;
		UserScoreInfo.dwUserID = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
		UserScoreInfo.lGold = m_TreasureDBAide.GetValue_LONGLONG(TEXT("Gold"));
		UserScoreInfo.lOpenRoomCard = m_TreasureDBAide.GetValue_LONGLONG(TEXT("OpenRoomCard"));
		UserScoreInfo.lDiamond = m_TreasureDBAide.GetValue_LONGLONG(TEXT("Diamond"));

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_USER_QUERY_SCORE_INFO,dwContextID,&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));

	}
	else
	{
		STR_DBO_CL_SERCIVR_ONLINE_REWARD result;
		result.byType = 0;
		result.dwCount = 0;
		lstrcpyn(result.szDescribe, pszErrorString, CountArray(result.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_ONLINE_REWARD,dwContextID,&result,sizeof(STR_DBO_CL_SERCIVR_ONLINE_REWARD));
	}

	return true;
}

//获取任务列表
bool CDataBaseEngineSink::On_DBR_Service_GetTaskList(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_DBR_CL_SERCIVR_GET_TASKLIST) != wDataSize)
		{
			return false;
		}

		STR_DBR_CL_SERCIVR_GET_TASKLIST* pDbReq = (STR_DBR_CL_SERCIVR_GET_TASKLIST*)pData;

		//获取任务列表
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_Service_GetTaskList"));

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_Service_GetTaskList"), true);		//查询[QPAccountsDB]数据库的TaskList表

		//结果处理
		On_DBO_Service_GetTaskList(pDbReq->dwUserID, dwContextID, lResultCode, TEXT("任务处理失败，请稍后重试!"));

	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;

}

//获取任务列表返回
bool CDataBaseEngineSink::On_DBO_Service_GetTaskList(DWORD dwUserID, DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	//构造数据
	STR_DBO_CL_SERCIVR_GET_TASKLIST taskItem;
	ZeroMemory(&taskItem,sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST));

	//临时数据
	int nTaskNum = m_AccountsDBModule->GetRecordCount();
	//TODO 任务个数的控制后面让web控制，这里暂时写死
	if ( MAX_SEND_CLIENT_TASK_NUM < nTaskNum )
	{
		nTaskNum = MAX_SEND_CLIENT_TASK_NUM;
	}

	//构造数据
	STR_DBO_CL_SERCIVR_GET_TASKLIST *arTaskList = new STR_DBO_CL_SERCIVR_GET_TASKLIST [nTaskNum]; 

	//查询成功，返回数据	
	if(dwErrorCode == DB_SUCCESS)
	{
		//网络数据
		WORD wSendSize=0;

		//循环获得每个任务数据
		for(int i=0; i<nTaskNum; i++)
		{
			//获得任务列表
			taskItem.wTaskID = m_AccountsDBAide.GetValue_WORD(TEXT("TaskID"));
			taskItem.cbTaskType = m_AccountsDBAide.GetValue_BYTE(TEXT("TaskType"));
			taskItem.cbActionType = m_AccountsDBAide.GetValue_BYTE(TEXT("ActionType"));
			taskItem.dwNeedCount = m_AccountsDBAide.GetValue_DWORD(TEXT("NeedCount"));
			m_AccountsDBAide.GetValue_String(TEXT("Describe"), taskItem.szDescribe, CountArray(taskItem.szDescribe));		
			taskItem.byRewardType = m_AccountsDBAide.GetValue_BYTE(TEXT("RewardType"));
			taskItem.dwRewardCount = m_AccountsDBAide.GetValue_DWORD(TEXT("RewardCount"));

			//获得任务状态
			taskItem.cbTaskStatus = m_AccountsDBAide.GetValue_BYTE(TEXT("TaskStatus"));
			taskItem.dwFinishedCount = m_AccountsDBAide.GetValue_DWORD(TEXT("LeftCount"));

			//赋值
			memcpy_s(&arTaskList[i], sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST), &taskItem, sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST));

			m_AccountsDBModule->MoveToNext();
		}		
	}

	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_GET_TASK_LIST, dwContextID, arTaskList, nTaskNum*sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST));

	//TODO 在后面使用的时候清空
	/*delete [] arTaskList;*/

	return true;
}

//领取任务奖励
bool CDataBaseEngineSink::On_DBR_Service_GetTaskReward(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_DBR_CL_SERCIVR_GET_TASK_REWARD) < wDataSize)
		{
			return false;
		}

		STR_DBR_CL_SERCIVR_GET_TASK_REWARD* pDbReq = (STR_DBR_CL_SERCIVR_GET_TASK_REWARD*)pData;

		m_AccountsDBAide.ResetParameter();

		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@wTaskID"), pDbReq->wTaskID);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_Service_GetTaskReward"));

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_Service_GetTaskReward"), true);		//查询数据库[QPAccountsDB]中的AccountsTaskStatus表，更新数据库中的GameScoreInfo表
		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//领取任务奖励返回
		On_DBO_Service_GetTaskReward(pDbReq->wTaskID, dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal));

	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;

}

//领取任务奖励返回
bool CDataBaseEngineSink::On_DBO_Service_GetTaskReward(DWORD dwUserID, DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	if(dwErrorCode == DB_SUCCESS)
	{
		//获取更新用户金币房卡钻石
		STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO UserScoreInfo;
		ZeroMemory(&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));

		UserScoreInfo.dwUserID = dwUserID;
		UserScoreInfo.lGold = m_AccountsDBAide.GetValue_LONG(TEXT("Gold"));
		UserScoreInfo.lOpenRoomCard = m_AccountsDBAide.GetValue_LONG(TEXT("OpenRoomCard"));
		UserScoreInfo.lDiamond = m_AccountsDBAide.GetValue_LONG(TEXT("Diamond"));

		//发送资产更新消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_USER_QUERY_SCORE_INFO, dwContextID, &UserScoreInfo, sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));
	}

	//收到任务奖励
	STR_DBO_CL_SERCIVR_GET_TASK_REWARD result;
	ZeroMemory(&result,sizeof(result));
	result.lResultCode = dwErrorCode;
	lstrcpyn( result.szDescribe, pszErrorString, CountArray( result.szDescribe));

	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_GET_TASK_REWARD,dwContextID,&result,sizeof(result));

	return true;
}

//请求抽奖
bool CDataBaseEngineSink::On_DBR_Service_RequestLottery(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_DBR_CL_SERCIVR_REQUEST_LOTTERY) != wDataSize)
		{
			return false;
		}

		STR_DBR_CL_SERCIVR_REQUEST_LOTTERY* pDbReq = (STR_DBR_CL_SERCIVR_REQUEST_LOTTERY*)pData;

		//获取一个1-100的随机数
		srand((unsigned int)time(NULL)) ;
		int index = rand() % 100 + 1;

		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@UserID"), pDbReq->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@Rand"), index);
		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_Service_RequestLottery")); 
		//执行查询,执行抽奖
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_Service_RequestLottery"), true);		//查询数据库[QPAccountsDB]的LotteryList表，执行抽奖操作；游戏服也有个请求抽奖；
		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		On_DBO_Service_RequestLottery(pDbReq->dwUserID, dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal));
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//请求抽奖返回
bool CDataBaseEngineSink::On_DBO_Service_RequestLottery(DWORD dwUserID, DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	//构造数据
	STR_DBO_CL_SERCIVR_REQUEST_LOTTERY LotteryResult;
	ZeroMemory(&LotteryResult, sizeof(STR_DBO_CL_SERCIVR_REQUEST_LOTTERY));

	//消息描述
	LotteryResult.lResultCode = dwErrorCode;
	lstrcpyn(LotteryResult.szDescribeString, pszErrorString, CountArray(LotteryResult.szDescribeString));

	//执行成功
	if(dwErrorCode == DB_SUCCESS)
	{
		//数据库1-6对应客户端0-5，数据库7-11对应客户端7-11，客户端6代表未中奖，数据库未存储
		LotteryResult.byIndex = m_AccountsDBAide.GetValue_INT(TEXT("RandIndex"));
		if ( LotteryResult.byIndex >=1 && LotteryResult.byIndex <= 11 )
		{
			if ( LotteryResult.byIndex <= 6 )
			{
				LotteryResult.byIndex = LotteryResult.byIndex - 1;
			}

			//发送抽奖结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_REQUEST_LOTTERY, dwContextID, &LotteryResult, sizeof(STR_DBO_CL_SERCIVR_REQUEST_LOTTERY));

		}
		else		//未中奖返回6
		{
			//赋值
			LotteryResult.byIndex = 6;

			//发送抽奖结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_REQUEST_LOTTERY, dwContextID, &LotteryResult, sizeof(STR_DBO_CL_SERCIVR_REQUEST_LOTTERY));
		}	
	}
	else	//失败，直接发送抽奖结果
	{
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_REQUEST_LOTTERY, dwContextID, &LotteryResult, sizeof(STR_DBO_CL_SERCIVR_REQUEST_LOTTERY));
	}

	//构造财富变更数据
	STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO UserScoreInfo;
	ZeroMemory(&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));

	UserScoreInfo.dwUserID = dwUserID;
	UserScoreInfo.lGold = m_AccountsDBAide.GetValue_LONG(TEXT("Gold"));
	UserScoreInfo.lOpenRoomCard = m_AccountsDBAide.GetValue_LONG(TEXT("OpenRoomCard"));
	UserScoreInfo.lDiamond = m_AccountsDBAide.GetValue_LONG(TEXT("Diamond"));
	UserScoreInfo.lRewardCard = m_AccountsDBAide.GetValue_LONG(TEXT("RewardCard"));
	UserScoreInfo.lScore = m_AccountsDBAide.GetValue_LONG(TEXT("Score"));

	//发送财富变更消息
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_USER_QUERY_SCORE_INFO, dwContextID, &UserScoreInfo, sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));

	return true;
}

//pure大厅 排行榜 
bool CDataBaseEngineSink::On_DBR_CL_SERVICE_PURE_STANDING_LIST(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_SUB_CL_SERVICE_PURE_STANDING_LIST) != wDataSize)
		{
			return false;
		}

		STR_SUB_CL_SERVICE_PURE_STANDING_LIST* pDbReq = (STR_SUB_CL_SERVICE_PURE_STANDING_LIST*)pData;

		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("mystery"), _MYSTERY);
		m_AccountsDBAide.AddParameter(TEXT("dwUserID"), pDbReq->dwUserID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_SERVICE_PURE_STANDING_LIST")); 
		//执行查询,执行抽奖
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_SERVICE_PURE_STANDING_LIST"), true);		//查询数据库[QPAccountsDB]的LotteryList表，执行抽奖操作；游戏服也有个请求抽奖；

		//列表发送
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_SERVICE_PURE_STANDING_LIST * pDBO=NULL;
		STR_CMD_LC_SERVICE_PURE_STANDING_FINISH DBOFinish;
		ZeroMemory(&DBOFinish, sizeof(DBOFinish));

		while (m_AccountsDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(STR_CMD_LC_SERVICE_PURE_STANDING_LIST))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_STANDING_LIST,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}
			//读取信息
			pDBO=(STR_CMD_LC_SERVICE_PURE_STANDING_LIST *)(cbBuffer+wPacketSize);
			pDBO->byRanking = m_AccountsDBAide.GetValue_BYTE(TEXT("Ranking"));
			pDBO->dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			pDBO->dwLoveLiness=m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));
			m_AccountsDBAide.GetValue_String(TEXT("NickName"),pDBO->szNickName,CountArray(pDBO->szNickName));
			m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),pDBO->szHeadUrl,CountArray(pDBO->szHeadUrl));

			if(pDBO->dwUserID == pDbReq->dwUserID)
			{
				DBOFinish.dwRanking = pDBO->byRanking;
				DBOFinish.dwLoveLiness = pDBO->dwLoveLiness;
			}
			//设置位移
			wPacketSize+=sizeof(STR_CMD_LC_SERVICE_PURE_STANDING_LIST);

			//移动记录
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_STANDING_LIST,dwContextID,cbBuffer,wPacketSize);

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_STANDING_FINISH,dwContextID,&DBOFinish,sizeof(DBOFinish));
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//pure大厅 大局战绩 
bool CDataBaseEngineSink::On_DBR_CL_SERVICE_PURE_RECORD_LIST(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_SUB_CL_SERVICE_PURE_RECORD_LIST) != wDataSize)
		{
			return false;
		}

		STR_SUB_CL_SERVICE_PURE_RECORD_LIST* pDbReq = (STR_SUB_CL_SERVICE_PURE_RECORD_LIST*)pData;

#pragma region 大局信息
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("dwUserID"), pDbReq->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("dwClubID"), pDbReq->dwClubID);
		m_TreasureDBAide.AddParameter(TEXT("tmQueryStartData"), pDbReq->tmQueryStartData);
		m_TreasureDBAide.AddParameter(TEXT("byMask"), pDbReq->byMask);
		//m_TreasureDBAide.AddParameter(TEXT("tmQueryEndData"), pDbReq->tmQueryEndData);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_SERVICE_PURE_RECORD_LIST")); 
		//执行查询,执行抽奖
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_SERVICE_PURE_RECORD_LIST"), true);

		//列表发送
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_SERVICE_PURE_RECORD_LIST * pDBO=NULL;

		while ((lResultCode == DB_SUCCESS) && (m_TreasureDBModule->IsRecordsetEnd()==false))
		{
			//发送信息
			if ((wPacketSize+sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_RECORD_LIST,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}
			//读取信息
			pDBO=(STR_CMD_LC_SERVICE_PURE_RECORD_LIST *)(cbBuffer+wPacketSize);
			BYTE gameMode = m_TreasureDBAide.GetValue_BYTE(TEXT("ModeID"));
			//0房卡 1竞技  2金币  3房卡金币
			if(0 == gameMode)
			{
				pDBO->byMask = 1;
			}
			else if((3 == gameMode) || (2 ==gameMode) )
			{
				pDBO->byMask = 2;
			}
			
			pDBO->dwDrawID = m_TreasureDBAide.GetValue_DWORD(TEXT("DrawID"));
			m_TreasureDBAide.GetValue_String(TEXT("KindName"),pDBO->szKindName,CountArray(pDBO->szKindName));
			pDBO->dwTableID=m_TreasureDBAide.GetValue_DWORD(TEXT("TableID"));
			SYSTEMTIME timeStartTime,tiemEndTime;
			m_TreasureDBAide.GetValue_SystemTime(TEXT("StartTime"), timeStartTime);
			m_TreasureDBAide.GetValue_SystemTime(TEXT("EndTime"), tiemEndTime);

			CString temp;
			temp.Format(TEXT("%d/%d/%d %d:%d:%d - %d:%d:%d"),
				timeStartTime.wYear, timeStartTime.wMonth, timeStartTime.wDay,
				timeStartTime.wHour,timeStartTime.wMinute,timeStartTime.wSecond,
				tiemEndTime.wHour,tiemEndTime.wMinute,tiemEndTime.wSecond
				);

			memcpy(pDBO->szTime, temp, sizeof(TCHAR)*128);

			//设置位移
			wPacketSize+=sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST);

			//移动记录
			m_TreasureDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_RECORD_LIST,dwContextID,cbBuffer,wPacketSize);

#pragma endregion

#pragma region 大局上的玩家信息
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("dwUserID"), pDbReq->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("dwClubID"), pDbReq->dwClubID);
		m_TreasureDBAide.AddParameter(TEXT("tmQueryStartData"), pDbReq->tmQueryStartData);
		m_TreasureDBAide.AddParameter(TEXT("byMask"), pDbReq->byMask);
		//m_TreasureDBAide.AddParameter(TEXT("tmQueryEndData"), pDbReq->tmQueryEndData);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_SERVICE_PURE_RECORD_LIST_PLAYINFO")); 
		//执行查询,执行抽奖
		LONG lResultCode2 = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_SERVICE_PURE_RECORD_LIST_PLAYINFO"), true);

		//列表发送
		WORD wPacketSize2=0;
		BYTE cbBuffer2[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize2=0;
		STR_CMD_LC_SERVICE_PURE_RECORD_LIST_PLAYERINFO * pDBO2=NULL;
	
		while ((lResultCode2 == DB_SUCCESS ) && (m_TreasureDBModule->IsRecordsetEnd()==false))
		{
			//发送信息
			if ((wPacketSize2+sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST_PLAYERINFO))>sizeof(cbBuffer2))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_RECORD_LIST_PINFO,dwContextID,cbBuffer2,wPacketSize2);
				wPacketSize2=0;
			}
			//读取信息
			pDBO2=(STR_CMD_LC_SERVICE_PURE_RECORD_LIST_PLAYERINFO*)(cbBuffer2+wPacketSize2);
			pDBO2->dwDrawID = m_TreasureDBAide.GetValue_DWORD(TEXT("DrawID"));
			pDBO2->dwUserID = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
			m_TreasureDBAide.GetValue_String(TEXT("NickName"),pDBO2->szNickName,CountArray(pDBO2->szNickName));
			m_TreasureDBAide.GetValue_String(TEXT("HeadUrl"),pDBO2->szHeadUrl,CountArray(pDBO2->szHeadUrl));
			pDBO2->lScore=m_TreasureDBAide.GetValue_LONGLONG(TEXT("Score"));

			//设置位移
			wPacketSize2+=sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST_PLAYERINFO);

			//移动记录
			m_TreasureDBModule->MoveToNext();
		}
		if (wPacketSize2>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_RECORD_LIST_PINFO,dwContextID,cbBuffer2,wPacketSize2);

#pragma endregion

#pragma region 大局信息结束
		STR_CMD_LC_SERVICE_PURE_RECORD_LIST_FINIST DBOFinish;
		DBOFinish.byMask = 1;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_RECORD_FINISH,dwContextID,&DBOFinish,sizeof(DBOFinish));

		return true;
#pragma endregion
		}
		
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//pure大厅 小局战绩 
bool CDataBaseEngineSink::On_DBR_CL_SERVICE_PURE_XJ_RECORD_LIST(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST) != wDataSize)
		{
			return false;
		}

		STR_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST* pDbReq = (STR_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST*)pData;

#pragma region 小局信息
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("dwDrawID"), pDbReq->dwDrawID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_SERVICE_PURE_XJ_RECORD_LIST")); 
		//执行查询
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_SERVICE_PURE_XJ_RECORD_LIST"), true);

		//列表发送
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST * pDBO=NULL;

		if(lResultCode == DB_SUCCESS)
		{
			while (m_TreasureDBModule->IsRecordsetEnd()==false)
			{
				//发送信息
				if ((wPacketSize+sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST))>sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_XJ_RECORD_LIST,dwContextID,cbBuffer,wPacketSize);
					wPacketSize=0;
				}
				//读取信息
				pDBO=(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST *)(cbBuffer+wPacketSize);
				pDBO->bRoundCount = m_TreasureDBAide.GetValue_BYTE(TEXT("RoundCount"));
				pDBO->dwRecordID = m_TreasureDBAide.GetValue_DWORD(TEXT("RoundID"));

				//设置位移
				wPacketSize+=sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST);

				//移动记录
				m_TreasureDBModule->MoveToNext();
			}
			if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_XJ_RECORD_LIST,dwContextID,cbBuffer,wPacketSize);
		}
		else
		{
			TCHAR szString[512]=TEXT("");
			_sntprintf_s(szString,CountArray(szString),TEXT("lResultCode = %ld"), lResultCode);

			//提示消息
			CTraceService::TraceString(szString,TraceLevel_Warning);
		}
#pragma endregion

#pragma region 小局上的玩家信息
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("dwDrawID"), pDbReq->dwDrawID);
	
		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_SERVICE_PURE_XJ_RECORD_LIST_PLAYINFO")); 
		//执行查询,执行抽奖
		LONG lResultCode2 = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_SERVICE_PURE_XJ_RECORD_LIST_PLAYINFO"), true);

		//列表发送
		WORD wPacketSize2=0;
		BYTE cbBuffer2[MAX_ASYNCHRONISM_DATA/10];
		STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PLAYERINFO * pDBO2=NULL;
	
		if(lResultCode2 == DB_SUCCESS)
		{
			while (m_TreasureDBModule->IsRecordsetEnd()==false)
			{
				//发送信息
				if ((wPacketSize2+sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PLAYERINFO))>sizeof(cbBuffer2))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO,dwContextID,cbBuffer2,static_cast<WORD>(wPacketSize2));
					wPacketSize2=0;
				}
				//读取信息
				pDBO2=(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PLAYERINFO*)(cbBuffer2+wPacketSize2);
				pDBO2->bRoundCount = m_TreasureDBAide.GetValue_BYTE(TEXT("RoundCount"));
				pDBO2->dwUserID = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
				m_TreasureDBAide.GetValue_String(TEXT("NickName"),pDBO2->szNickName,CountArray(pDBO2->szNickName));
				pDBO2->lScore=m_TreasureDBAide.GetValue_LONGLONG(TEXT("Score"));

				//设置位移
				wPacketSize2+=sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PLAYERINFO);

				//移动记录
				m_TreasureDBModule->MoveToNext();
			}
			if (wPacketSize2>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO,dwContextID,cbBuffer2,wPacketSize2);
		}
		else
		{
			TCHAR szString[512]=TEXT("");
			_sntprintf_s(szString,CountArray(szString),TEXT("lResultCode = %ld"), lResultCode2);

			//提示消息
			CTraceService::TraceString(szString,TraceLevel_Warning);
		}
#pragma endregion

#pragma region 小局信息结束
		STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_FINISH DBOFinish;
		DBOFinish.byMask = 1;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_XJ_RECORD_FINISH,dwContextID,&DBOFinish,sizeof(DBOFinish));
#pragma endregion
		}
		
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//小局录像回放
bool CDataBaseEngineSink::On_DBR_CL_Service_XJRecordPlayback(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_DBR_CL_SERVICE_XJ_RECORD_PLAYBACK) != wDataSize)
		{
			return false;
		}

		STR_DBR_CL_SERVICE_XJ_RECORD_PLAYBACK* pDbReq = (STR_DBR_CL_SERVICE_XJ_RECORD_PLAYBACK*)pData;

		//输入参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwRoundID"), pDbReq->dwRecordID);

		//输出变量
		WCHAR szDescribe[128] = L"";
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_GETVIDEORECORD"));
		//执行查询
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_GETVIDEORECORD"), true);
		if (lResultCode == DB_SUCCESS)
		{
			//结果处理
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造DBO数据
			STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK DBO;
			ZeroMemory(&DBO, sizeof(STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK));

			//获取录像数据
			TCHAR szData[2*LEN_MAX_RECORD_SIZE];
			m_TreasureDBAide.GetValue_String(TEXT("VideoData"), szData, CountArray(szData));
			StrToBin(szData, DBO.cbRecordData, 0, LEN_MAX_RECORD_SIZE*2-1);

			DBO.dwDataSize = m_TreasureDBAide.GetValue_DWORD(TEXT("VideoSize"));
			DBO.wCurrentCount =  m_TreasureDBAide.GetValue_WORD(TEXT("CurrentRound"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_XJ_RECORD_PLAYBACK, dwContextID, &DBO, sizeof(STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK));
	
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//客服消息
bool CDataBaseEngineSink::On_DBR_CL_SERVICE_CUSTOMER_MESSEGE(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_DBR_CL_SERVICE_CUSTOMER_MESSEGE) != wDataSize)
		{
			return false;
		}

		STR_DBR_CL_SERVICE_CUSTOMER_MESSEGE* pDbReq = (STR_DBR_CL_SERVICE_CUSTOMER_MESSEGE*)pData;

		//输入参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@FirmID"), _MYSTERY);
		m_PlatformDBAide.AddParameter(TEXT("@MessageType"), pDbReq->cbMessegeFlag);
		
		//输出执行
		LogicTraceHelper(TEXT("GSP_GP_GET_SERVICE_MESSAGE"));
		//执行查询
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_GET_SERVICE_MESSAGE"), true);
		if (lResultCode == DB_SUCCESS)
		{
			//构造DBO数据
			STR_DBO_LC_SERVICE_CUSTOMER_MESSEGE DBO;
			ZeroMemory(&DBO, sizeof(STR_DBO_LC_SERVICE_CUSTOMER_MESSEGE));

			//获取录像数据
			m_PlatformDBAide.GetValue_String(TEXT("MessageText"), DBO.szMessege, CountArray(DBO.szMessege));
			DBO.cbMessegeFlag = m_PlatformDBAide.GetValue_BYTE(TEXT("MessageType"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_CUSTOMER_MESSEGE, dwContextID, &DBO, sizeof(STR_DBO_LC_SERVICE_CUSTOMER_MESSEGE));
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//请求金币大厅信息 
bool CDataBaseEngineSink::On_DBR_CL_SERVICE_GOLD_INFO(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_SUB_CL_SERVICE_GOLD_INFO) != wDataSize)
		{
			return false;
		}

		STR_SUB_CL_SERVICE_GOLD_INFO* pDbReq = (STR_SUB_CL_SERVICE_GOLD_INFO*)pData;

#pragma region 请求金币大厅信息
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("dwUserID"), pDbReq->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("dwModID"), pDbReq->dwModID);
		m_TreasureDBAide.AddParameter(TEXT("dwKindID"), pDbReq->dwKindID);
		m_TreasureDBAide.AddParameter(TEXT("mystery"), _MYSTERY);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_SERVICE_GOLD_INFO")); 
		//执行查询
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_SERVICE_GOLD_INFO"), true);

		//列表发送
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_SERVICE_GOLD_INFO * pDBO=NULL;

		while ((lResultCode == DB_SUCCESS) && m_TreasureDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(STR_CMD_LC_SERVICE_GOLD_INFO))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_GOLD_INFO,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}
			//读取信息
			pDBO=(STR_CMD_LC_SERVICE_GOLD_INFO *)(cbBuffer+wPacketSize);
			pDBO->byGoldMod = m_TreasureDBAide.GetValue_BYTE(TEXT("GoldMod"));
			pDBO->dwScore = m_TreasureDBAide.GetValue_DWORD(TEXT("GoldScore"));
			pDBO->dwGold = m_TreasureDBAide.GetValue_DWORD(TEXT("Gold"));
			pDBO->dwServiceGold = m_TreasureDBAide.GetValue_DWORD(TEXT("ServiceGold"));

			//设置位移
			wPacketSize+=sizeof(STR_CMD_LC_SERVICE_GOLD_INFO);

			//移动记录
			m_TreasureDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_GOLD_INFO,dwContextID,cbBuffer,wPacketSize);

#pragma endregion

#pragma region 请求金币大厅信息 结束
		STR_CMD_LC_SERVICE_GOLD_INFO_FINISH DBOFinish;
		DBOFinish.byMask = 1;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_GOLD_INFO_FINISH,dwContextID,&DBOFinish,sizeof(DBOFinish));
#pragma endregion
		}
		
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}


//修改个人资料
bool CDataBaseEngineSink::On_DBR_Service_ModifyPersonalInfo(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_SUB_CL_SERVICE_MODIFY_PERSONAL_INFO) != wDataSize)
		{
			return false;
		}

		STR_SUB_CL_SERVICE_MODIFY_PERSONAL_INFO* pDbReq = (STR_SUB_CL_SERVICE_MODIFY_PERSONAL_INFO*)pData;

		//输入参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@UserID"), pDbReq->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@OldLogonPassword"), pDbReq->szOldLogonPassword);
		m_AccountsDBAide.AddParameter(TEXT("@NewLogonPassword"), pDbReq->szNewLogonPassword);		
		m_AccountsDBAide.AddParameter(TEXT("@NickName"), pDbReq->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@Gender"), pDbReq->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@HeadImageUrl"), pDbReq->szHeadImageUrl);
		m_AccountsDBAide.AddParameter(TEXT("@Signature"), pDbReq->szSignature);
		m_AccountsDBAide.AddParameter(TEXT("@RealName"), pDbReq->szRealName);
		m_AccountsDBAide.AddParameter(TEXT("@IDCardNum"), pDbReq->szIDCardNum);
		m_AccountsDBAide.AddParameter(TEXT("@PhoneNum"), pDbReq->szPhoneNum);
		m_AccountsDBAide.AddParameter(TEXT("@ProxyUserID"), pDbReq->dwProxyUserID);		//TODO 代理ID在数据库中暂时未增加，后面增加

		//输出变量
		WCHAR szDescribe[128] = L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_Service_ModifyPersonalInfo"));
		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_Service_ModifyPersonalInfo"), true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//处理
		On_DBO_Service_ModifyPersonalInfo(dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal));
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//修改个人资料返回
bool CDataBaseEngineSink::On_DBO_Service_ModifyPersonalInfo(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	//个人信息
	STR_DBO_CL_MODIFY_PERSONL_INFO PersonalInfo;
	ZeroMemory(&PersonalInfo, sizeof(PersonalInfo));

	//数据赋值
	PersonalInfo.lResultCode = dwErrorCode;
	lstrcpyn(PersonalInfo.szDescribeString, pszErrorString, CountArray(PersonalInfo.szDescribeString));

	//发送资产更新消息
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_MODIFY_PERSONAL_INFO, dwContextID, &PersonalInfo, sizeof(STR_DBO_CL_MODIFY_PERSONL_INFO));

	return true;
}

//充值信息
bool CDataBaseEngineSink::On_DBR_Other_RechargeInfo( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//效验参数
	ASSERT(wDataSize==sizeof(STR_DBR_CL_OTHER_RECHARGE_INFO));
	if (wDataSize!=sizeof(STR_DBR_CL_OTHER_RECHARGE_INFO)) return false;

	//请求处理
	STR_DBR_CL_OTHER_RECHARGE_INFO * pUserRequest=(STR_DBR_CL_OTHER_RECHARGE_INFO *)pData;

	try
	{
		//构造参数
		m_AccountsDBAide.ResetParameter();

		//输出执行
		LogicTraceHelper(TEXT("GSP_GP_UserRechangeInfo"));

		//结果处理
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserRechangeInfo"),true);

		On_DBO_Other_RechargeInfo(dwContextID, lResultCode, TEXT("数据库处理出错!"));
	}
	catch(IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		DBR_CommandSource wRequestID ;
		ZeroMemory(&wRequestID,sizeof(wRequestID));
		wRequestID.MainCommand = MDM_GIFT;
		wRequestID.SubCommand  = SUB_CL_OTHERS_RECHARGE_INFO;

		//结果处理
		On_DBO_CommonOperateResult(dwContextID, DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"), wRequestID);

		return false;
	}

	return true;
}

//充值信息返回
bool CDataBaseEngineSink::On_DBO_Other_RechargeInfo(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	//结果处理
	if (dwErrorCode == DB_SUCCESS)
	{
		//构造数据
		STR_DBO_CL_OTHER_RECHARGE_INFO RechangeInfo;
		ZeroMemory( &RechangeInfo, sizeof(RechangeInfo));

		RechangeInfo.dwMinMoney = m_AccountsDBAide.GetValue_LONGLONG(TEXT("MinMoney"));
		RechangeInfo.dwChangeScale = m_AccountsDBAide.GetValue_UINT(TEXT("ChangeScale"));

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_USER_RECHARGE_INFO, dwContextID, &RechangeInfo, sizeof(RechangeInfo));
	}
	else
	{
		DBR_CommandSource wRequestID ;
		ZeroMemory(&wRequestID,sizeof(wRequestID));
		wRequestID.MainCommand = MDM_GIFT;
		wRequestID.SubCommand  = SUB_CL_OTHERS_RECHARGE_INFO;

		//错误信息
		On_DBO_CommonOperateResult(dwContextID, dwErrorCode, pszErrorString,wRequestID);
	}

	return true;
}

//兑换道具信息
bool CDataBaseEngineSink::On_DBR_Other_ExchangeInfo( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//效验参数
	ASSERT(wDataSize==sizeof(STR_DBR_CL_OTHER_EXCHANGE_INFO));
	if (wDataSize!=sizeof(STR_DBR_CL_OTHER_EXCHANGE_INFO)) return false;

	//请求处理
	STR_DBR_CL_OTHER_EXCHANGE_INFO * pUserRequest=(STR_DBR_CL_OTHER_EXCHANGE_INFO *)pData;

	try
	{
		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@UserID"),pUserRequest->dwUserID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_GP_UserExchangeInfo"));

		//结果处理
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserExchangeInfo"),true);

		On_DBO_Other_ExchangeInfo(dwContextID, lResultCode, TEXT("数据库错误!"));
	}
	catch(IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		DBR_CommandSource wRequestID ;
		ZeroMemory(&wRequestID,sizeof(wRequestID));
		wRequestID.MainCommand = MDM_GIFT;
		wRequestID.SubCommand  = SUB_CL_OTHERS_EXCHANGE_INFO;

		On_DBO_CommonOperateResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),wRequestID);

		return false;
	}
	return true;
}

//兑换道具信息返回
bool CDataBaseEngineSink::On_DBO_Other_ExchangeInfo(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	//结果处理
	if (dwErrorCode == DB_SUCCESS)
	{
		STR_DBO_CL_OTHER_EXCHANGE_INFO ExchangeInfo;
		ZeroMemory( &ExchangeInfo, sizeof(ExchangeInfo));

		ExchangeInfo.dwMinMoney = m_AccountsDBAide.GetValue_LONGLONG(TEXT("MinMoney"));
		ExchangeInfo.dwChangeScale = m_AccountsDBAide.GetValue_UINT(TEXT("ChangeScale"));
		ExchangeInfo.dwBankMoney = m_AccountsDBAide.GetValue_LONGLONG(TEXT("BankMoney"));
		ExchangeInfo.dwWithdrawals = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Withdrawals"));

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_USER_EXCHANGE_INFO,dwContextID,&ExchangeInfo,sizeof(ExchangeInfo));
	}
	else
	{
		//错误处理
		DBR_CommandSource wRequestID ;
		ZeroMemory(&wRequestID,sizeof(wRequestID));
		wRequestID.MainCommand = MDM_GIFT;
		wRequestID.SubCommand  = SUB_CL_OTHERS_EXCHANGE_INFO;

		On_DBO_CommonOperateResult(dwContextID, dwErrorCode, TEXT("数据库出错!"), wRequestID);
	}

	return true;
}

#pragma region 启动命令
//加载列表
bool CDataBaseEngineSink::OnRequestLoadGameList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//变量定义
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];

		//加载类型
		m_PlatformDBAide.ResetParameter();

		//输出执行
		LogicTraceHelper(TEXT("GSP_GP_LoadGameTypeItem"));

		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGameTypeItem"),true);

		//发送种类
		wPacketSize=0;
		tagGameType * pGameType=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(tagGameType))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_TYPE_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pGameType=(tagGameType *)(cbBuffer+wPacketSize);
			pGameType->wTypeID=m_PlatformDBAide.GetValue_WORD(TEXT("TypeID"));
			m_PlatformDBAide.GetValue_String(TEXT("TypeName"),pGameType->szTypeName,CountArray(pGameType->szTypeName));

			//设置位移
			wPacketSize+=sizeof(tagGameType);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_TYPE_ITEM,dwContextID,cbBuffer,wPacketSize);

		//读取类型
		m_PlatformDBAide.ResetParameter();

		//输出执行
		LogicTraceHelper(TEXT("GSP_GP_LoadGameKindItem"));

		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGameKindItem"),true);

		//发送类型
		wPacketSize=0;
		tagGameKind * pGameKind=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(tagGameKind))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_KIND_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pGameKind=(tagGameKind *)(cbBuffer+wPacketSize);
			pGameKind->wKindID=m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGameKind->wTypeID=m_PlatformDBAide.GetValue_WORD(TEXT("TypeID"));
			m_PlatformDBAide.GetValue_String(TEXT("KindName"),pGameKind->szKindName,CountArray(pGameKind->szKindName));

			//设置位移
			wPacketSize+=sizeof(tagGameKind);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_KIND_ITEM,dwContextID,cbBuffer,wPacketSize);

		//读取节点
		m_PlatformDBAide.ResetParameter();

		//输出执行
		LogicTraceHelper(TEXT("GSP_GP_LoadGameNodeItem"));

		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGameNodeItem"),true);

		//发送节点
		wPacketSize=0;
		tagGameNode * pGameNode=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(tagGameNode))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_NODE_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pGameNode=(tagGameNode *)(cbBuffer+wPacketSize);
			pGameNode->wKindID=m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGameNode->wNodeID=m_PlatformDBAide.GetValue_WORD(TEXT("NodeID"));
			m_PlatformDBAide.GetValue_String(TEXT("NodeName"),pGameNode->szNodeName,CountArray(pGameNode->szNodeName));

			//设置位移
			wPacketSize+=sizeof(tagGameNode);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_NODE_ITEM,dwContextID,cbBuffer,wPacketSize);

		//变量定义
		DBO_GP_GameListResult GameListResult;
		ZeroMemory(&GameListResult,sizeof(GameListResult));

		//设置变量
		GameListResult.cbSuccess=TRUE;

		//发送消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_LIST_RESULT,dwContextID,&GameListResult,sizeof(GameListResult));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//变量定义
		DBO_GP_GameListResult GameListResult;
		ZeroMemory(&GameListResult,sizeof(GameListResult));

		//设置变量
		GameListResult.cbSuccess=FALSE;

		//发送消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_LIST_RESULT,dwContextID,&GameListResult,sizeof(GameListResult));

		return false;
	}

	return true;
}

//在线统计 -- 发给数据库
bool CDataBaseEngineSink::OnRequestOnLineCountInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		if (wDataSize != sizeof(DBR_GP_OnLineCountInfo)) return false;

		//变量定义
		DBR_GP_OnLineCountInfo * pOnLineCountInfo=(DBR_GP_OnLineCountInfo *)pData;

		//构造信息
		TCHAR szOnLineCountGame[2048]=TEXT("");
		for (WORD i=0; i<pOnLineCountInfo->dwGameCount; i++)
		{
			INT nLength=lstrlen(szOnLineCountGame);
			_sntprintf(&szOnLineCountGame[nLength],CountArray(szOnLineCountGame)-nLength,
				TEXT("%ld:%ld;"),
				pOnLineCountInfo->OnLineCountGame[i].dwGameID,
				pOnLineCountInfo->OnLineCountGame[i].dwOnLineCount);
		}


		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@byCompanyID"),_MYSTERY);
		m_PlatformDBAide.AddParameter(TEXT("@dwGameCount"),pOnLineCountInfo->dwGameCount);
		m_PlatformDBAide.AddParameter(TEXT("@dwOnLineCountSum"),pOnLineCountInfo->dwOnLineCountSum);
		m_PlatformDBAide.AddParameter(TEXT("@strOnLineCountGame"),szOnLineCountGame);

		//输出执行
		LogicTraceHelper(TEXT("GSP_GP_OnLineCountInfo"));

		//执行命令
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_OnLineCountInfo"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//加载跑马灯消息
bool CDataBaseEngineSink::On_DBR_UPDATA_MARQUEE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@byCompanyID"),_MYSTERY);

		//输出执行
		LogicTraceHelper(TEXT("GSP_GP_UPDATA_MARQUEE"));
		//执行命令
		long lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_UPDATA_MARQUEE"),true);

		//变量定义
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		STR_DBO_UPDATA_MARQUEE * pDbo=NULL;
		while ((lResultCode == DB_SUCCESS) && (m_PlatformDBModule->IsRecordsetEnd()==false))
		{
			//发送信息
			if ((wPacketSize+sizeof(STR_DBO_UPDATA_MARQUEE))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_UPDATA_MARQUEE,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pDbo=(STR_DBO_UPDATA_MARQUEE *)(cbBuffer+wPacketSize);
			pDbo->byMask  = 1; //新增数据
			pDbo->dwMarqueeID  =  m_PlatformDBAide.GetValue_DWORD(TEXT("MarqueeID"));
			pDbo->dwMaruqeeTime = m_PlatformDBAide.GetValue_DWORD(TEXT("MaruqeeTime"));
			m_PlatformDBAide.GetValue_SystemTime(TEXT("StartTime"), pDbo->timeStartTime);
			m_PlatformDBAide.GetValue_SystemTime(TEXT("EndTime"), pDbo->timeEndTime);
			m_PlatformDBAide.GetValue_String(TEXT("MarqueeMsg"),pDbo->szMarqueeMsg,CountArray(pDbo->szMarqueeMsg));

			//设置位移
			wPacketSize+=sizeof(STR_DBO_UPDATA_MARQUEE);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_UPDATA_MARQUEE,dwContextID,cbBuffer,wPacketSize);

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_UPDATA_MARQUEE_FINISH,dwContextID,NULL,0);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

#pragma endregion

//查询用户金币房卡
bool CDataBaseEngineSink::On_DBR_QueryScoreInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(STR_DBR_CL_SERCIVR_QUERY_SCORE_INFO));
		if (wDataSize!=sizeof(STR_DBR_CL_SERCIVR_QUERY_SCORE_INFO)) return false;

		//请求处理
		STR_DBR_CL_SERCIVR_QUERY_SCORE_INFO * pQueryScoreInfo =(STR_DBR_CL_SERCIVR_QUERY_SCORE_INFO *)pData;

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pQueryScoreInfo->dwUserID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_Service_GetUserScoreInfo"));

		//执行查询
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_Service_GetUserScoreInfo"),true);

		//结果处理
		On_DBO_QueryScoreInfo(dwContextID,lResultCode,NULL,false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		//OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//查询金币房卡返回
VOID CDataBaseEngineSink::On_DBO_QueryScoreInfo(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient)
{
	//构造数据
	STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO UserScoreInfo;
	ZeroMemory(&UserScoreInfo,sizeof(UserScoreInfo));

	//描述信息
	UserScoreInfo.lResultCode = dwErrorCode;
	lstrcpyn(UserScoreInfo.szDescribeString, pszErrorString, CountArray(UserScoreInfo.szDescribeString));

	if (dwErrorCode==DB_SUCCESS)
	{
		//构造变量
		UserScoreInfo.dwUserID = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
		UserScoreInfo.lGold = m_TreasureDBAide.GetValue_LONGLONG(TEXT("Gold"));
		UserScoreInfo.lOpenRoomCard = m_TreasureDBAide.GetValue_LONGLONG(TEXT("OpenRoomCard"));
		UserScoreInfo.lDiamond = m_TreasureDBAide.GetValue_LONGLONG(TEXT("Diamond"));

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_USER_QUERY_SCORE_INFO,dwContextID,&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));
	}
	else
	{
		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_USER_QUERY_SCORE_INFO,dwContextID,&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));
	}

	return;
}

//用户Socket连接关闭
bool CDataBaseEngineSink::On_DBR_GP_QUIT(DWORD dwContextID, VOID * pData, WORD wDataSize)
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
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_CL_Quit"),true);	

		return true;
	}
	catch(IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		return false;
	}

}

//更新排行榜用户信息
bool CDataBaseEngineSink::OnDBUpdateRankUserItem(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(DBR_GP_UserRankItem) < wDataSize)
		{
			return false;
		}

		DBR_GP_UserRankItem* pDbReq = (DBR_GP_UserRankItem*)pData;

		m_AccountsDBAide.ResetParameter();

		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@wRankNum"), pDbReq->byRankNum);
		m_AccountsDBAide.AddParameter(TEXT("@wReceived"), pDbReq->byReceived);
		m_AccountsDBAide.AddParameter(TEXT("@dwCount"), pDbReq->dwCount);
		m_AccountsDBAide.AddParameter(TEXT("@wIndex"), pDbReq->byIndex);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_GP_UpdateRankUserItem"));

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UpdateRankUserItem"), true);
		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		if(lResultCode == DB_SUCCESS)
		{

		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}


//获取排行榜信息
bool CDataBaseEngineSink::OnDBReadRankList(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(RankManager) != wDataSize && NULL != pData)
		{
			return false;
		}

		RankManager* pRankManager = (RankManager*)pData;
		m_AccountsDBAide.ResetParameter();
		//输出执行
		//LogicTraceHelper(TEXT("GSP_GP_GetRankList"));

		/* 删除 创梦版 大厅的 排行榜功能 added by WangChengQing 2018/6/4
		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_GetRankList"), true);
		//结果处理
		if(lResultCode == DB_SUCCESS)
		{
			pRankManager->InitRankList(m_AccountsDBAide,m_AccountsDBModule);
		}
		*/
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;

}

//领取任务奖励
bool CDataBaseEngineSink::OnReceiveRankReward(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_DBR_CL_SERCIVR_GET_RANK_REWARD) < wDataSize)
		{
			return false;
		}

		STR_DBR_CL_SERCIVR_GET_RANK_REWARD* pDbReq = (STR_DBR_CL_SERCIVR_GET_RANK_REWARD*)pData;

		m_AccountsDBAide.ResetParameter();

		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@byIndex"), pDbReq->byIndex);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_GP_GetRankReward"));

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_GetRankReward"), true);
		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		if(lResultCode == DB_SUCCESS)
		{
			//获取更新用户金币房卡钻石
			STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO UserScoreInfo;
			ZeroMemory(&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));

			UserScoreInfo.dwUserID = pDbReq->dwUserID;
			UserScoreInfo.lGold = m_AccountsDBAide.GetValue_LONG(TEXT("@Gold"));
			UserScoreInfo.lOpenRoomCard = m_AccountsDBAide.GetValue_LONG(TEXT("@OpenRoomCard"));
			UserScoreInfo.lDiamond = m_AccountsDBAide.GetValue_LONG(TEXT("@Diamond"));

			//发送资产更新消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_USER_QUERY_SCORE_INFO,dwContextID,&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));
		}

		//收到任务奖励
		STR_DBO_CL_SERCIVR_GET_TASK_REWARD result;
		ZeroMemory(&result,sizeof(result));
		result.lResultCode = lResultCode;
		lstrcpyn( result.szDescribe, CW2CT(DBVarValue.bstrVal), CountArray( result.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_GET_RANK_REWARD,dwContextID,&result,sizeof(result));

	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;

}

//修改用户财富信息
bool CDataBaseEngineSink::OnModifyUserInsure(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(DBR_GP_ModifyUserInsure) < wDataSize)
		{
			return false;
		}

		DBR_GP_ModifyUserInsure* pDbReq = (DBR_GP_ModifyUserInsure*)pData;

		m_TreasureDBAide.ResetParameter();

		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@lGold"), pDbReq->lGold);
		m_TreasureDBAide.AddParameter(TEXT("@lOpenRoomCard"), pDbReq->lOpenRoomCard);
		m_TreasureDBAide.AddParameter(TEXT("@lDiamond"), pDbReq->lDiamond);
		m_TreasureDBAide.AddParameter(TEXT("@lRewardCard"), pDbReq->lRewardCard);
		m_TreasureDBAide.AddParameter(TEXT("@lScore"), pDbReq->lScore);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_GP_GetRankReward"));

		//执行查询
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GP_GetRankReward"), true);
		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		if(lResultCode == DB_SUCCESS)
		{
			//获取更新用户金币房卡钻石
			STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO UserScoreInfo;
			ZeroMemory(&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));

			UserScoreInfo.dwUserID = pDbReq->dwUserID;
			UserScoreInfo.lGold = m_AccountsDBAide.GetValue_LONG(TEXT("@Gold"));
			UserScoreInfo.lOpenRoomCard = m_AccountsDBAide.GetValue_LONG(TEXT("@OpenRoomCard"));
			UserScoreInfo.lDiamond = m_AccountsDBAide.GetValue_LONG(TEXT("@Diamond"));
			UserScoreInfo.lRewardCard = m_AccountsDBAide.GetValue_LONG(TEXT("@RewardCard"));
			UserScoreInfo.lScore = m_AccountsDBAide.GetValue_LONG(TEXT("@Score"));

			//发送财富变更消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_USER_QUERY_SCORE_INFO,dwContextID,&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;

}

//向其他用户赠送道具
bool CDataBaseEngineSink::On_DBR_CL_GIFT_GIVE_PROPS(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_SUB_CL_GIFT_GIVE_PROPS) != wDataSize)
		{
			return false;
		}

		STR_SUB_CL_GIFT_GIVE_PROPS* pDbReq = (STR_SUB_CL_GIFT_GIVE_PROPS*)pData;

		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwSrcUserID"), pDbReq->dwSrcUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDstUserID"), pDbReq->dwDstUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwPropsID"), pDbReq->dwPropsID);
		m_AccountsDBAide.AddParameter(TEXT("@dwCount"), pDbReq->dwCount);

		//输出变量
		TCHAR szDescribe[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_GIFT_GIVE_PROPS")); 
		//执行查询
		long lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_GIFT_GIVE_PROPS"), true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		STR_CMD_LC_GIFT_GIVE_PROPS CMD;
		CMD.dwResultCode = lResultCode;
		lstrcpyn( CMD.szDescribeString, CW2CT(DBVarValue.bstrVal), CountArray(CMD.szDescribeString));

		if(lResultCode == DB_SUCCESS)
		{
			CMD.dwPropsID = pDbReq->dwPropsID;
			CMD.dwCount = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsCount"));
			CMD.dwLoveness = m_AccountsDBAide.GetValue_DWORD(TEXT("Loveness"));

			STR_CMD_LC_GIFT_GIVE_PROPS_SHOW DBO;
			DBO.dwTargetID = pDbReq->dwDstUserID;
			lstrcpyn( DBO.szDescribeString, CW2CT(DBVarValue.bstrVal), CountArray(DBO.szDescribeString));

			//发送DBO
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_GIFT_GIVE_PROPS_SHOW,dwContextID,&DBO,sizeof(DBO));
		}

		//发送DBO
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_GIFT_GIVE_PROPS,dwContextID,&CMD,sizeof(CMD));

	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

#pragma region MDM_LOGON 登录模块
/***************************** 登录处理*******************************************/
//帐号登录
bool CDataBaseEngineSink::On_DBR_Logon_Accounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize!=sizeof(STR_DBR_CL_LOGON_ACCOUNTS)) return false;

		//请求处理
		STR_DBR_CL_LOGON_ACCOUNTS * pDBRLogonAccounts=(STR_DBR_CL_LOGON_ACCOUNTS *)pData;

		//Socket判断
		tagBindParameter * pBindParameter=(tagBindParameter *)pDBRLogonAccounts->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pBindParameter->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);
		
		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@mystery"),_MYSTERY);
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pDBRLogonAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pDBRLogonAccounts->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pDBRLogonAccounts->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@ProxyID"),pDBRLogonAccounts->dwProxyID);

		//构造提示
		TCHAR szString[512]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),
			TEXT("账号登录 代理 = %ld"),
			pDBRLogonAccounts->dwProxyID);
		CTraceService::TraceString(szString,TraceLevel_Normal);

		//输出参数ssss
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_Logon_Accounts"));
		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_Logon_Accounts"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		On_DBO_Logon_Accounts(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		On_DBO_Logon_Accounts(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"));

		return false;
	}

	return true;
}

//账号登录返回
bool CDataBaseEngineSink::On_DBO_Logon_Accounts(DWORD dwContextID, DWORD dwResultCode, LPCTSTR pszErrorString)
{
	//变量定义
	STR_CMD_LC_LOGON_PLATFORM DBOLogonAccount;
	ZeroMemory(&DBOLogonAccount,sizeof(DBOLogonAccount));

	//登录成功获取信息
	if(DB_SUCCESS == dwResultCode)
	{
		/* *****************************    用户信息     ****************************/
		//用户标志
		DBOLogonAccount.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
		//用户昵称
		m_AccountsDBAide.GetValue_String(TEXT("NickName"),DBOLogonAccount.szNickName,CountArray(DBOLogonAccount.szNickName));
		//用户性别
		DBOLogonAccount.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
		//头像索引
		m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),DBOLogonAccount.szHeadUrl,CountArray(DBOLogonAccount.szHeadUrl));
		//个性签名
		m_AccountsDBAide.GetValue_String(TEXT("MySignature"),DBOLogonAccount.szMySignature,CountArray(DBOLogonAccount.szMySignature));

		//用户元宝
		DBOLogonAccount.dwUserDiamond=m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserDiamond"));
		//用户奖牌
		DBOLogonAccount.dwUserMedal=m_AccountsDBAide.GetValue_DWORD(TEXT("UserMedal"));
		//经验数值
		DBOLogonAccount.byLevel=m_AccountsDBAide.GetValue_BYTE(TEXT("UserLevel"));
		//用户魅力
		DBOLogonAccount.dwLoveLiness=m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));

		//管理员等级
		DBOLogonAccount.cbMasterOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MasterOrder"));
		//会员等级
		DBOLogonAccount.cbMemberOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MemberOrder"));
		//会员到期时间
		m_AccountsDBAide.GetValue_SystemTime(TEXT("MemberOverDate"),DBOLogonAccount.MemberOverDate);

		//真实姓名
		m_AccountsDBAide.GetValue_String(TEXT("IdentityName"),DBOLogonAccount.szIdentityName,CountArray(DBOLogonAccount.szIdentityName));
		//身份证号
		m_AccountsDBAide.GetValue_String(TEXT("IdentityNum"),DBOLogonAccount.szIdentityNum,CountArray(DBOLogonAccount.szIdentityNum));
		//手机号码
		m_AccountsDBAide.GetValue_String(TEXT("MobilePhone"),DBOLogonAccount.szMobilePhone,CountArray(DBOLogonAccount.szMobilePhone));
		/* *****************************    账号信息     ****************************/
		//最后登录地址
		m_AccountsDBAide.GetValue_String(TEXT("LastLogonIP"),DBOLogonAccount.szLasLogonIp,CountArray(DBOLogonAccount.szLasLogonIp));
		//最后上线时间 
		m_AccountsDBAide.GetValue_SystemTime(TEXT("LastLogonDate"),DBOLogonAccount.LasLogonDate);

		/* *****************************    附加数据     ****************************/
		//用户积分
		DBOLogonAccount.lUserScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserScore"));
		//用户游戏币
		DBOLogonAccount.lUserGold = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserGold"));
		//用户房卡
		DBOLogonAccount.lOpenRoomCard = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserRoomCard"));
	}

	//构造数据
	DBOLogonAccount.dwResultCode=dwResultCode;
	lstrcpyn(DBOLogonAccount.szDescribeString,pszErrorString,CountArray(DBOLogonAccount.szDescribeString));

	//发送结果
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_LOGON_ACCOUNTS,dwContextID,&DBOLogonAccount, sizeof(DBOLogonAccount));

	return true;
}

//注册处理
bool CDataBaseEngineSink::On_DBR_Logon_Register(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize!=sizeof(STR_DBR_CL_LOGON_REGISTER)) return false;

		//请求处理
		STR_DBR_CL_LOGON_REGISTER * pDBRLogonRegister=(STR_DBR_CL_LOGON_REGISTER *)pData;

		//Socket判断
		tagBindParameter * pBindParameter=(tagBindParameter *)pDBRLogonRegister->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pBindParameter->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@byMYSTERY"), _MYSTERY);
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pDBRLogonRegister->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pDBRLogonRegister->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pDBRLogonRegister->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pDBRLogonRegister->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pDBRLogonRegister->strMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pDBRLogonRegister->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@dwProxyID"),pDBRLogonRegister->dwProxyID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_Logon_Register"));

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_Logon_Register"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		On_DBO_Logon_Accounts(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		On_DBO_Logon_Accounts(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"));

		return false;
	}

	return true;
}

//平台登录
bool CDataBaseEngineSink::On_DBR_Logon_Platform(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(STR_DBR_CL_LOGON_PLATFORM));
		if (wDataSize!=sizeof(STR_DBR_CL_LOGON_PLATFORM)) return false;

		//请求处理
		STR_DBR_CL_LOGON_PLATFORM * pDBRLogonPlatform=(STR_DBR_CL_LOGON_PLATFORM *)pData;

		//Socket判断
		tagBindParameter * pBindParameter=(tagBindParameter *)pDBRLogonPlatform->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pBindParameter->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@byMystery"), _MYSTERY);
		m_AccountsDBAide.AddParameter(TEXT("@strOpenID"), pDBRLogonPlatform->szOpenID);
		m_AccountsDBAide.AddParameter(TEXT("@szUnionID"), pDBRLogonPlatform->szUnionID);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"), pDBRLogonPlatform->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"), pDBRLogonPlatform->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strHeadUrl"), pDBRLogonPlatform->strHeadUrl);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pDBRLogonPlatform->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@dwProxyID"), pDBRLogonPlatform->dwProxyID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_Logon_Platform"));
		//执行查询			
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_Logon_Platform"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		On_DBO_Logon_Platform(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		On_DBO_Logon_Platform(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"));

		return false;
	}

	return true;
}

//平台登录返回
bool CDataBaseEngineSink::On_DBO_Logon_Platform(DWORD dwContextID, DWORD dwResultCode, LPCTSTR pszErrorString)
{
	//变量定义
	STR_CMD_LC_LOGON_PLATFORM DBOLogonPlatform;
	ZeroMemory(&DBOLogonPlatform,sizeof(DBOLogonPlatform));

	//登录成功获取信息
	if(DB_SUCCESS == dwResultCode)
	{
		/* *****************************    用户信息     ****************************/
		//用户标志
		DBOLogonPlatform.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
		//用户昵称
		m_AccountsDBAide.GetValue_String(TEXT("NickName"),DBOLogonPlatform.szNickName,CountArray(DBOLogonPlatform.szNickName));
		//用户性别
		DBOLogonPlatform.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
		//头像索引
		m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),DBOLogonPlatform.szHeadUrl,CountArray(DBOLogonPlatform.szHeadUrl));
		//个性签名
		m_AccountsDBAide.GetValue_String(TEXT("MySignature"),DBOLogonPlatform.szMySignature,CountArray(DBOLogonPlatform.szMySignature));

		//用户元宝
		DBOLogonPlatform.dwUserDiamond=m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserDiamond"));
		//用户奖牌
		DBOLogonPlatform.dwUserMedal=m_AccountsDBAide.GetValue_DWORD(TEXT("UserMedal"));
		//经验数值
		DBOLogonPlatform.byLevel=m_AccountsDBAide.GetValue_BYTE(TEXT("UserLevel"));
		//用户魅力
		DBOLogonPlatform.dwLoveLiness=m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));

		//管理员等级
		DBOLogonPlatform.cbMasterOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MasterOrder"));
		//会员等级
		DBOLogonPlatform.cbMemberOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MemberOrder"));
		//会员到期时间
		m_AccountsDBAide.GetValue_SystemTime(TEXT("MemberOverDate"),DBOLogonPlatform.MemberOverDate);

		//真实姓名
		m_AccountsDBAide.GetValue_String(TEXT("IdentityName"),DBOLogonPlatform.szIdentityName,CountArray(DBOLogonPlatform.szIdentityName));
		//身份证号
		m_AccountsDBAide.GetValue_String(TEXT("IdentityNum"),DBOLogonPlatform.szIdentityNum,CountArray(DBOLogonPlatform.szIdentityNum));
		//手机号码
		m_AccountsDBAide.GetValue_String(TEXT("MobilePhone"),DBOLogonPlatform.szMobilePhone,CountArray(DBOLogonPlatform.szMobilePhone));
		/* *****************************    账号信息     ****************************/
		//最后登录地址
		m_AccountsDBAide.GetValue_String(TEXT("LastLogonIP"),DBOLogonPlatform.szLasLogonIp,CountArray(DBOLogonPlatform.szLasLogonIp));
		//最后上线时间
		m_AccountsDBAide.GetValue_SystemTime(TEXT("LastLogonDate"),DBOLogonPlatform.LasLogonDate);

		/* *****************************    附加数据     ****************************/
		//用户积分 TODONOW 数据库缺少该字段
		//DBOLogonPlatform.lUserScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserScore"));
		//用户游戏币
		DBOLogonPlatform.lUserGold = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserGold"));
		//用户房卡
		DBOLogonPlatform.lOpenRoomCard = m_AccountsDBAide.GetValue_LONGLONG(TEXT("OpenRoomCard"));
	}

	//构造数据
	DBOLogonPlatform.dwResultCode=dwResultCode;
	lstrcpyn(DBOLogonPlatform.szDescribeString,pszErrorString,CountArray(DBOLogonPlatform.szDescribeString));

	//发送结果
	WORD wDataSize=CountStringBuffer(DBOLogonPlatform.szDescribeString);
	WORD wHeadSize=sizeof(DBOLogonPlatform)-sizeof(DBOLogonPlatform.szDescribeString);

	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_LOGON_PLATFORM,dwContextID,&DBOLogonPlatform,wHeadSize+wDataSize);
	return true;
}

#pragma endregion
//主消息号6-其他消息：公用操作结果
VOID CDataBaseEngineSink::On_DBO_CommonOperateResult( DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, DBR_CommandSource wRequestID )
{
	//变量定义
	DBO_GP_OperateCommand OperateResult;
	ZeroMemory(&OperateResult,sizeof(OperateResult));

	//构造数据
	OperateResult.lResultCode=dwErrorCode;
	lstrcpyn(OperateResult.szDescribeString,pszErrorString,CountArray(OperateResult.szDescribeString));
	OperateResult.mCommand.MainCommand = wRequestID.MainCommand;
	OperateResult.mCommand.SubCommand = wRequestID.SubCommand;

	//发送结果
	WORD wDataSize=CountStringBuffer(OperateResult.szDescribeString);
	WORD wHeadSize=sizeof(OperateResult)-sizeof(OperateResult.szDescribeString);
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_USER_COMMAND_RESULT,dwContextID,&OperateResult,wHeadSize+wDataSize);

	return;
}
#pragma region MDM_CLUB 牌友圈(版本2)
//查询牌友圈列表
bool CDataBaseEngineSink::On_DBR_CL_CLUB_ALL_CLUB_INFO_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize!=sizeof(STR_SUB_CL_CLUB_ALL_CLUB_INFO_LIST)) return false;
		STR_SUB_CL_CLUB_ALL_CLUB_INFO_LIST *pDbr = (STR_SUB_CL_CLUB_ALL_CLUB_INFO_LIST *)pData;

		//加载类型
		m_AccountsDBAide.ResetParameter();

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_ALL_CLUB_INFO_LIST"));

		m_AccountsDBAide.AddParameter(TEXT("@UserID"),pDbr->dwUserID);
		m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_ALL_CLUB_INFO_LIST"),true);

		//列表发送
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST * pCMD=NULL;
		while (m_AccountsDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_ALL_CLUB_INFO_LIST,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pCMD=(STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST *)(cbBuffer+wPacketSize);
			pCMD->dwClubID=m_AccountsDBAide.GetValue_DWORD(TEXT("ClubID"));	
			m_AccountsDBAide.GetValue_String(TEXT("ClubName"),pCMD->szClubName,CountArray(pCMD->szClubName));
			pCMD->byClubRole=m_AccountsDBAide.GetValue_BYTE(TEXT("ClubRole"));

			//设置位移
			wPacketSize+=sizeof(STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST);

			//移动记录
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_ALL_CLUB_INFO_LIST,dwContextID,cbBuffer,wPacketSize);

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_ALL_INFO_FINISH,dwContextID,NULL,0);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}
}

//查询指定牌友圈房间列表
bool CDataBaseEngineSink::On_DBR_CL_CLUB_ROOM_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize!=sizeof(STR_DBR_CL_CLUB_ROOM_LIST)) return false;

		STR_DBR_CL_CLUB_ROOM_LIST *pDBR = (STR_DBR_CL_CLUB_ROOM_LIST*) pData;

		STR_DBR_CL_CLUB_ROOM_LIST DBR;
		DBR.dwClubID = pDBR->dwClubID;

		//加载类型
		m_AccountsDBAide.ResetParameter();

		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("@ClubID"),DBR.dwClubID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_ROOM_LIST"));

		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_LIST"),true);

		//列表发送
		//变量定义
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_CLUB_ROOM_LIST * pDBO=NULL;
		while ((lResultCode == DB_SUCCESS) && (m_AccountsDBModule->IsRecordsetEnd()==false))
		{
			//发送信息
			if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_ROOM_LIST))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_ROOM_LIST,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pDBO=(STR_CMD_LC_CLUB_ROOM_LIST *)(cbBuffer+wPacketSize);
			pDBO->dwRoomID=m_AccountsDBAide.GetValue_DWORD(TEXT("RoomID"));
			DWORD dwGameID =m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
			pDBO->dwKindID = (dwGameID >> 16);

			BYTE byMode = m_AccountsDBAide.GetValue_BYTE(TEXT("ModeID"));
			pDBO->byGoldOrFK = 1;
			if(0 == byMode) //房卡场  是俱乐部中的房卡成
			{
				pDBO->byGoldOrFK = 1;
			}
			else if(3 == byMode) //房卡金币场  是俱乐部中金币场
			{
				pDBO->byGoldOrFK = 2;
			}

			m_AccountsDBAide.GetValue_String(TEXT("KindName"),pDBO->szKindName,CountArray(pDBO->szKindName));
			m_AccountsDBAide.GetValue_String(TEXT("RoomName"),pDBO->szRoomName,CountArray(pDBO->szRoomName));

			pDBO->wPlayerNum=m_AccountsDBAide.GetValue_WORD(TEXT("PlayerNum"));
			m_AccountsDBAide.GetValue_String(TEXT("RoomRule"),pDBO->szRoomRule,CountArray(pDBO->szRoomRule));
			pDBO->byAllRound=m_AccountsDBAide.GetValue_BYTE(TEXT("AllRound"));
			pDBO->byChairNum=m_AccountsDBAide.GetValue_BYTE(TEXT("ChairNum"));

			pDBO->bDissolve=m_AccountsDBAide.GetValue_BYTE(TEXT("DissolveMinute"));
			pDBO->dwDissolveTime=m_AccountsDBAide.GetValue_DWORD(TEXT("DissolveMinute"));

			pDBO->dwAmount=m_AccountsDBAide.GetValue_DWORD(TEXT("ServiceGold"));
			pDBO->dwOwnerPercentage=m_AccountsDBAide.GetValue_DWORD(TEXT("Revenue"));

			pDBO->byMask=m_AccountsDBAide.GetValue_BYTE(TEXT("Mask"));
			pDBO->dwDizhu=m_AccountsDBAide.GetValue_DWORD(TEXT("Dizhu"));
			pDBO->dwGold=m_AccountsDBAide.GetValue_DWORD(TEXT("Gold"));

			//设置位移
			wPacketSize+=sizeof(STR_CMD_LC_CLUB_ROOM_LIST);

			//移动记录
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_ROOM_LIST,dwContextID,cbBuffer,wPacketSize);

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_ROOM_LIST_FINISH,dwContextID,NULL,0);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}
}

//查询未满员, 随机牌友圈(最大9个)
bool CDataBaseEngineSink::On_DBR_CL_CLUB_RANDOM_CLUB_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize!=sizeof(STR_SUB_CL_CLUB_RANDOM_CLUB_LIST)) return false;
		STR_SUB_CL_CLUB_RANDOM_CLUB_LIST *pDBR = (STR_SUB_CL_CLUB_RANDOM_CLUB_LIST*) pData;

		//加载类型
		m_AccountsDBAide.ResetParameter();
		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("@Mystery"),_MYSTERY);
		m_AccountsDBAide.AddParameter(TEXT("@UserID"),pDBR->dwUserID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_RANDOM_CLUB_LIST"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_RANDOM_CLUB_LIST"),true);

		//列表发送
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_CLUB_RANDOM_CLUB_LIST * pDBO=NULL;
		while (m_AccountsDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_RANDOM_CLUB_LIST))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_RANDOM_CLUB_LIST,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pDBO=(STR_CMD_LC_CLUB_RANDOM_CLUB_LIST *)(cbBuffer+wPacketSize);
			pDBO->dwClubID = m_AccountsDBAide.GetValue_DWORD(TEXT("ClubID"));
			m_AccountsDBAide.GetValue_String(TEXT("ClubName"),pDBO->szClubName,CountArray(pDBO->szClubName));
			pDBO->dwMajorKindID=m_AccountsDBAide.GetValue_DWORD(TEXT("MajorKindID"));
			//TODONOW ManiorKindID 需要转换
			//KINDID
			pDBO->byClubLevel=m_AccountsDBAide.GetValue_BYTE(TEXT("ClubLevel"));
			pDBO->wMemberNum=m_AccountsDBAide.GetValue_WORD(TEXT("MemberNum"));
			m_AccountsDBAide.GetValue_String(TEXT("Notice"),pDBO->szNotice,CountArray(pDBO->szNotice));
			m_AccountsDBAide.GetValue_String(TEXT("Message"),pDBO->szMessage,CountArray(pDBO->szMessage));
			pDBO->dwClubOwner=m_AccountsDBAide.GetValue_DWORD(TEXT("ClubOwner"));
			m_AccountsDBAide.GetValue_String(TEXT("ClubOwnerName"),pDBO->szClubOwnerName,CountArray(pDBO->szClubOwnerName));

			//设置位移
			wPacketSize+=sizeof(STR_CMD_LC_CLUB_RANDOM_CLUB_LIST);

			//移动记录
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_RANDOM_CLUB_LIST,dwContextID,cbBuffer,wPacketSize);

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_RANDOM_CLUB_LIST_FINISH,dwContextID,NULL,0);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}
}

//申请加入牌友圈
bool CDataBaseEngineSink::On_DBR_CL_CLUB_JOIN_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_JOIN_CLUB)) return false;
		STR_SUB_CL_CLUB_JOIN_CLUB *pDbr = (STR_SUB_CL_CLUB_JOIN_CLUB*)pData;

		//加载类型
		m_AccountsDBAide.ResetParameter();
		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("@Mystery"),_MYSTERY);
		m_AccountsDBAide.AddParameter(TEXT("@UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@ClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("@Note"),pDbr->szNote);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_JOIN_CLUB"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_JOIN_CLUB"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//结构体构造
		STR_CMD_LC_CLUB_JOIN_CLUB CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_JOIN_CLUB,dwContextID,&CMD,sizeof(CMD));


		/* 申请加入牌友圈 广播 */
		if(lResultCode == DB_SUCCESS)
		{
			STR_SUB_CL_CLUB_JOIN_CLUB_BDCAST CMD2;
			CMD2.dwClubID = pDbr->dwClubID;
			CMD2.dwApplicantNum = m_AccountsDBAide.GetValue_DWORD(TEXT("ApplicantNum"));

			//如果是自动加入俱乐部, 则通知客户端刷新
			BYTE byMask = m_AccountsDBAide.GetValue_BYTE(TEXT("mask"));
			if(1 == byMask )
			{
				STR_CMD_LC_CLUB_LIST_RE CMD3;
				CMD3.byMask = 1;
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_JOIN_CLUB_RE,dwContextID,&CMD3,sizeof(CMD3));
			}

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_JOIN_CLUB_BDCAST,dwContextID,&CMD2,sizeof(CMD2));
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//牌友圈公告
bool CDataBaseEngineSink::On_DBR_CL_CLUB_NOTICE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_NOTICE)) return false;
		STR_SUB_CL_CLUB_NOTICE* pDbr = (STR_SUB_CL_CLUB_NOTICE*) pData;

		//加载类型
		m_AccountsDBAide.ResetParameter();

		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("Notice"),pDbr->szNotice);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_NOTICE"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_NOTICE"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//结构体构造
		STR_CMD_LC_CLUB_NOTICE CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_NOTICE,dwContextID,&CMD,sizeof(CMD));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//牌友圈简介
bool CDataBaseEngineSink::On_DBR_CL_CLUB_MESSAGE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_MESSAGE)) return false;
		STR_SUB_CL_CLUB_MESSAGE* pDbr = (STR_SUB_CL_CLUB_MESSAGE*) pData;

		//加载类型
		m_AccountsDBAide.ResetParameter();

		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("Message"),pDbr->szMessage);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_MESSAGE"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_MESSAGE"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//结构体构造
		STR_CMD_LC_CLUB_MESSAGE CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_MESSAGE,dwContextID,&CMD,sizeof(CMD));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//贡献房卡
bool CDataBaseEngineSink::On_DBR_CL_CLUB_CONTRIBUTE_FK(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_CONTRIBUTE_FK)) return false;
		STR_SUB_CL_CLUB_CONTRIBUTE_FK* pDbr = (STR_SUB_CL_CLUB_CONTRIBUTE_FK*) pData;

		//加载类型
		m_AccountsDBAide.ResetParameter();

		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("FK"),pDbr->dwFK);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_CONTRIBUTE_FK"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_CONTRIBUTE_FK"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//结构体构造
		STR_CMD_LC_CLUB_CONTRIBUTE_FK CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));
		CMD.dwClubFK=m_AccountsDBAide.GetValue_DWORD(TEXT("ClubFK"));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_CONTRIBUTE_FK,dwContextID,&CMD,sizeof(CMD));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//牌友圈设置
bool CDataBaseEngineSink::On_DBR_CL_CLUB_AUTO_AGREE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_AUTO_AGREE)) return false;
		STR_SUB_CL_CLUB_AUTO_AGREE* pDbr = (STR_SUB_CL_CLUB_AUTO_AGREE*) pData;

		//加载类型
		m_AccountsDBAide.ResetParameter();

		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("AutoAgree"),pDbr->byAutoAgree);
		m_AccountsDBAide.AddParameter(TEXT("Sex"),pDbr->bySex);
		m_AccountsDBAide.AddParameter(TEXT("Level"),pDbr->wLevel);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_AUTO_AGREE"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_AUTO_AGREE"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//结构体构造
		STR_CMD_LC_CLUB_AUTO_AGREE CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_AUTO_AGREE,dwContextID,&CMD,sizeof(CMD));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//邀请他人加入牌友圈
bool CDataBaseEngineSink::On_DBR_CL_CLUB_INVITE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_INVITE)) return false;
		STR_SUB_CL_CLUB_INVITE* pDbr = (STR_SUB_CL_CLUB_INVITE*) pData;

		//加载类型
		m_AccountsDBAide.ResetParameter();

		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("Mystery"),_MYSTERY);
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("TagID"),pDbr->dwTagID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_INVITE"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_INVITE"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//结构体构造
		STR_CMD_LC_CLUB_INVITE CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_INVITE,dwContextID,&CMD,sizeof(CMD));


		/* 被邀请人的提醒 */
		STR_CMD_LC_CLUB_INVITE_REMIND CMD2;
		CMD2.dwTagID = pDbr->dwTagID;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_INVITE_REMIND,dwContextID,&CMD2,sizeof(CMD2));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//被邀请人回复
bool CDataBaseEngineSink::On_DBR_CL_CLUB_INVITE_RESULT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_INVITE_RESULT)) return false;
		STR_SUB_CL_CLUB_INVITE_RESULT* pDbr = (STR_SUB_CL_CLUB_INVITE_RESULT*) pData;

		//加载类型
		m_AccountsDBAide.ResetParameter();

		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("Agree"),pDbr->byAgree);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_INVITE_RESULT"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_INVITE_RESULT"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//结构体构造
		STR_CMD_LC_CLUB_INVITE_RESULT CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_INVITE_RESULT,dwContextID,&CMD,sizeof(CMD));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//被邀请人查看自己的邀请列表
bool CDataBaseEngineSink::On_DBR_CL_CLUB_INQUERY_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_INQUERY_LIST)) return false;
		STR_SUB_CL_CLUB_INQUERY_LIST* pDbr = (STR_SUB_CL_CLUB_INQUERY_LIST*) pData;

		//加载类型
		m_AccountsDBAide.ResetParameter();

		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_INQUERY_LIST"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_INQUERY_LIST"),true);

		//发送列表
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_CLUB_INQUERY_LIST * pDBO=NULL;
		while (m_AccountsDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_INQUERY_LIST))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_INQUERY_LIST,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pDBO=(STR_CMD_LC_CLUB_INQUERY_LIST *)(cbBuffer+wPacketSize);
			pDBO->dwClubID = m_AccountsDBAide.GetValue_DWORD(TEXT("ClubID"));
			m_AccountsDBAide.GetValue_String(TEXT("ClubName"),pDBO->szClubName,CountArray(pDBO->szClubName));
			pDBO->dwMajorKindID=m_AccountsDBAide.GetValue_DWORD(TEXT("MajorKindID"));
			//TODONOW ManiorKindID 需要转换
			//KINDID
			pDBO->byClubLevel=m_AccountsDBAide.GetValue_BYTE(TEXT("ClubLevel"));
			pDBO->wMemberNum=m_AccountsDBAide.GetValue_WORD(TEXT("MemberNum"));
			m_AccountsDBAide.GetValue_String(TEXT("Notice"),pDBO->szNotice,CountArray(pDBO->szNotice));
			m_AccountsDBAide.GetValue_String(TEXT("Message"),pDBO->szMessage,CountArray(pDBO->szMessage));
			pDBO->dwClubOwner=m_AccountsDBAide.GetValue_DWORD(TEXT("ClubOwner"));
			m_AccountsDBAide.GetValue_String(TEXT("ClubOwnerName"),pDBO->szClubOwnerName,CountArray(pDBO->szClubOwnerName));

			//设置位移
			wPacketSize+=sizeof(STR_CMD_LC_CLUB_INQUERY_LIST);

			//移动记录
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_INQUERY_LIST,dwContextID,cbBuffer,wPacketSize);

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_INQUERY_LIST_FINISH,dwContextID,NULL,0);
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//申请人列表
bool CDataBaseEngineSink::On_DBR_CL_CLUB_APPLICANT_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_APPLICANT_LIST)) return false;
		STR_SUB_CL_CLUB_APPLICANT_LIST* pDbr = (STR_SUB_CL_CLUB_APPLICANT_LIST*) pData;

		//加载类型
		m_AccountsDBAide.ResetParameter();

		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_APPLICANT_LIST"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_APPLICANT_LIST"),true);

		//发送列表
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_CLUB_APPLICANT_LIST * pDBO=NULL;
		while (m_AccountsDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_APPLICANT_LIST))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_APPLICANT_LIST,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pDBO=(STR_CMD_LC_CLUB_APPLICANT_LIST *)(cbBuffer+wPacketSize);
			pDBO->dwUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			m_AccountsDBAide.GetValue_String(TEXT("UserName"),pDBO->szUserName,CountArray(pDBO->szUserName));
			pDBO->byUserSex=m_AccountsDBAide.GetValue_BYTE(TEXT("UserSex"));
			pDBO->byUserLevel=m_AccountsDBAide.GetValue_BYTE(TEXT("UserLevel"));
			m_AccountsDBAide.GetValue_String(TEXT("Note"),pDBO->szNote,CountArray(pDBO->szNote));
			m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),pDBO->szHeadUrl,CountArray(pDBO->szHeadUrl));

			//设置位移
			wPacketSize+=sizeof(STR_CMD_LC_CLUB_APPLICANT_LIST);

			//移动记录
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_APPLICANT_LIST,dwContextID,cbBuffer,wPacketSize);

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_APPLICANT_LIST_FINISH,dwContextID,NULL,0);
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//职务任免
bool CDataBaseEngineSink::On_DBR_CL_CLUB_APPOINTMENT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_APPOINTMENT)) return false;
		STR_SUB_CL_CLUB_APPOINTMENT* pDbr = (STR_SUB_CL_CLUB_APPOINTMENT*) pData;

		//加载类型
		m_AccountsDBAide.ResetParameter();

		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("TagID"),pDbr->dwTagID);
		m_AccountsDBAide.AddParameter(TEXT("Action"),pDbr->byAction);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_APPOINTMENT"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_APPOINTMENT"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//发送返回
		STR_CMD_LC_CLUB_APPOINTMENT CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_APPOINTMENT,dwContextID,&CMD,sizeof(CMD));

		//发送提醒
		STR_CMD_LC_CLUB_APPOINTMENT_NOTE CMD2;
		CMD2.byAction = pDbr->byAction;
		CMD2.dwUserID = pDbr->dwTagID;
		lstrcpyn(CMD2.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD2.szDescribe));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_APPOINTMENT_NOTE,dwContextID,&CMD2,sizeof(CMD2));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//工会战绩统计
bool CDataBaseEngineSink::On_DBR_CL_CLUB_RECORD_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_RECORD_LIST)) return false;
		STR_SUB_CL_CLUB_RECORD_LIST *pDbr = (STR_SUB_CL_CLUB_RECORD_LIST * )pData;

		//加载类型
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("dwClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("tmQueryStartData"),pDbr->tmQueryStartData);
		m_AccountsDBAide.AddParameter(TEXT("tmQueryEndData"),pDbr->tmQueryEndData);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_RECORD_LIST"));
		LONG lResultCode2 = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_RECORD_LIST"),true);

		//发送列表
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_CLUB_RECORD_LIST * pDBO=NULL;
		while ( (lResultCode2 == DB_SUCCESS) && (m_AccountsDBModule->IsRecordsetEnd()==false))
		{
			//发送信息
			if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_RECORD_LIST))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_RECORD_LIST,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pDBO=(STR_CMD_LC_CLUB_RECORD_LIST *)(cbBuffer+wPacketSize);
			BYTE gameMode  = m_AccountsDBAide.GetValue_BYTE(TEXT("ModeID"));
			//0房卡  1竞技  2金币 3房卡金币
			if(0 == gameMode)
			{
				pDBO->byMask = 1;
			}
			else if( (2 == gameMode) || (3 == gameMode ))
			{
				pDBO->byMask = 2;
			}

			pDBO->dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));

			m_AccountsDBAide.GetValue_String(TEXT("NickName"),pDBO->szNickName,CountArray(pDBO->szNickName));
			m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),pDBO->szHeadUrl,CountArray(pDBO->szHeadUrl));
			
			pDBO->dwAllCount = m_AccountsDBAide.GetValue_DWORD(TEXT("AllCount"));
			pDBO->lAllScore=m_AccountsDBAide.GetValue_LONGLONG(TEXT("AllScore"));
			pDBO->dwWinCount = m_AccountsDBAide.GetValue_DWORD(TEXT("WinCount"));
			pDBO->lWinScore=m_AccountsDBAide.GetValue_LONGLONG(TEXT("WinScore"));
			
			//设置位移
			wPacketSize+=sizeof(STR_CMD_LC_CLUB_RECORD_LIST);

			//移动记录
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_RECORD_LIST,dwContextID,cbBuffer,wPacketSize);

		//发送结束
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_RECORD_FINISH,dwContextID,NULL,0);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//牌友圈聊天
bool CDataBaseEngineSink::On_DBR_CL_CLUB_CHAT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_CHAT)) return false;
		STR_SUB_CL_CLUB_CHAT *pDbr = (STR_SUB_CL_CLUB_CHAT * )pData;

		/* 1. 获取发言者的信息 */
		TCHAR szTempUserNickName[32] = TEXT("");
		TCHAR szTempHeadUrl[256] = TEXT("");

		//加载类型
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_CHAT_GETINFO"));
		LONG lResultCode2 = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_CHAT_GETINFO"),true);

		if(lResultCode2 != DB_SUCCESS)
		{
			return true;
		}
		m_AccountsDBAide.GetValue_String(TEXT("UserNickName"), szTempUserNickName,CountArray(szTempUserNickName));
		m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),szTempHeadUrl,CountArray(szTempHeadUrl));

		/* 2.发送给俱乐部所有在线的人 */
		//加载类型
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_CHAT"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_CHAT"),true);

		//发送列表
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_CLUB_CHAT_BDCAST * pDBO=NULL;

		while (m_AccountsDBModule->IsRecordsetEnd()==false && lResultCode == DB_SUCCESS)
		{
			//发送信息
			if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_CHAT_BDCAST))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CHAT_ALL_BDCAST,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pDBO=(STR_CMD_LC_CLUB_CHAT_BDCAST *)(cbBuffer+wPacketSize);
			pDBO->dwTagID = m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));

			//聊天发起人的信息
			pDBO->byChatMode = pDbr->byChatMode;
			pDBO->dwClubID = pDbr->dwClubID;
			memcpy(pDBO->szChat, pDbr->szChat, sizeof(pDBO->szChat));
			pDBO->dwUserID = pDbr->dwUserID;
			memcpy(pDBO->szUserNickName, szTempUserNickName, sizeof(pDBO->szUserNickName));
			memcpy(pDBO->szHeadUrl, szTempHeadUrl, sizeof(pDBO->szHeadUrl));

			//设置位移
			wPacketSize+=sizeof(STR_CMD_LC_CLUB_CHAT_BDCAST);

			//移动记录
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CHAT_ALL_BDCAST,dwContextID,cbBuffer,wPacketSize);

		STR_CMD_LC_CLUB_CHAT CMD2;
		CMD2.byChatMode = pDbr->byChatMode;
		CMD2.lResultCode = lResultCode;
		memcpy(CMD2.szDescribe, TEXT("发送失败, 稍后重试"), sizeof(CMD2));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CHAT_ALL,dwContextID,&CMD2, sizeof(CMD2));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//世界聊天
bool CDataBaseEngineSink::On_DBR_CL_WORD_CHAT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_CHAT)) return false;
		STR_SUB_CL_CLUB_CHAT *pDbr = (STR_SUB_CL_CLUB_CHAT * )pData;

		/* 1. 获取发言者的信息 */
		TCHAR szTempUserNickName[32] = TEXT("");
		TCHAR szTempHeadUrl[256] = TEXT("");

		//加载类型
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_CHAT_GETINFO"));
		LONG lResultCode2 = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_CHAT_GETINFO"),true);

		if(lResultCode2 != DB_SUCCESS)
		{
			return true;
		}
		m_AccountsDBAide.GetValue_String(TEXT("UserNickName"), szTempUserNickName,CountArray(szTempUserNickName));
		m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),szTempHeadUrl,CountArray(szTempHeadUrl));

		/* 2. 聊天广播 发送给该登录服所有在线的人 */
		STR_CMD_LC_CLUB_CHAT_BDCAST CMD3;
		CMD3.byChatMode = pDbr->byChatMode;
		CMD3.dwClubID = 0;
		CMD3.dwUserID = pDbr->dwUserID;
		memcpy(CMD3.szUserNickName, szTempUserNickName, sizeof(CMD3.szUserNickName));
		memcpy(CMD3.szHeadUrl, szTempHeadUrl, sizeof(CMD3.szHeadUrl));
		memcpy(CMD3.szChat, pDbr->szChat, sizeof(CMD3.szChat));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CHAT_ALL_BDCAST,dwContextID,&CMD3, sizeof(CMD3));

		/* 3. 聊天返回 */
		STR_CMD_LC_CLUB_CHAT CMD2;
		CMD2.byChatMode = pDbr->byChatMode;
		CMD2.lResultCode = lResultCode2;
		memcpy(CMD2.szDescribe, TEXT("发送失败, 稍后重试"), sizeof(CMD2));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CHAT_ALL,dwContextID,&CMD2, sizeof(CMD2));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//系统聊天 -- TODONOW 暂时不处理该逻辑
bool CDataBaseEngineSink::On_DBR_CL_SYSTEM_CHAT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_CHAT)) return false;
		STR_SUB_CL_CLUB_CHAT *pDbr = (STR_SUB_CL_CLUB_CHAT * )pData;

		/* 1. 获取发言者的信息 */
		TCHAR szTempUserNickName[32] = TEXT("");
		TCHAR szTempHeadUrl[256] = TEXT("");

		//加载类型
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_CHAT_GETINFO"));
		LONG lResultCode2 = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_CHAT_GETINFO"),true);

		if(lResultCode2 != DB_SUCCESS)
		{
			return true;
		}
		m_AccountsDBAide.GetValue_String(TEXT("UserNickName"), szTempUserNickName,CountArray(szTempUserNickName));
		m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),szTempHeadUrl,CountArray(szTempHeadUrl));

		
		/* 2. 聊天广播 发送给该登录服所有在线的人 */
		/*
		STR_CMD_LC_CLUB_CHAT_BDCAST CMD3;
		CMD3.byChatMode = pDbr->byChatMode;
		CMD3.dwClubID = 0;
		CMD3.dwUserID = pDbr->dwUserID;
		memcpy(CMD3.szUserNickName, szTempUserNickName, sizeof(CMD3.szUserNickName));
		memcpy(CMD3.szHeadUrl, szTempHeadUrl, sizeof(CMD3.szHeadUrl));
		memcpy(CMD3.szChat, pDbr->szChat, sizeof(CMD3.szChat));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_WORD_CHAT_BDCAST,dwContextID,&CMD3, sizeof(CMD3));
		*/

		/* 3. 聊天返回 */
		STR_CMD_LC_CLUB_CHAT CMD2;
		CMD2.byChatMode = pDbr->byChatMode;
		CMD2.lResultCode = lResultCode2;
		memcpy(CMD2.szDescribe, TEXT("发送失败, 稍后重试"), sizeof(CMD2));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CHAT_ALL,dwContextID,&CMD2, sizeof(CMD2));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//私密聊天
bool CDataBaseEngineSink::On_DBR_CL_SECRET_CHAT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_CHAT)) return false;
		STR_SUB_CL_CLUB_CHAT *pDbr = (STR_SUB_CL_CLUB_CHAT * )pData;

		/* 1. 获取发言者的信息 */
		TCHAR szTempUserNickName[32] = TEXT("");
		TCHAR szTempHeadUrl[256] = TEXT("");

		//加载类型
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_CHAT_GETINFO"));
		LONG lResultCode2 = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_CHAT_GETINFO"),true);

		if(lResultCode2 != DB_SUCCESS)
		{
			return true;
		}
		m_AccountsDBAide.GetValue_String(TEXT("UserNickName"), szTempUserNickName,CountArray(szTempUserNickName));
		m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),szTempHeadUrl,CountArray(szTempHeadUrl));

		/* 2. 聊天广播 发送给该登录服所有在线的人 */
		STR_CMD_LC_CLUB_CHAT_BDCAST CMD3;
		CMD3.byChatMode = pDbr->byChatMode;
		CMD3.dwClubID = 0;
		CMD3.dwUserID = pDbr->dwUserID;
		CMD3.dwTagID = pDbr->dwTagUserID;
		memcpy(CMD3.szUserNickName, szTempUserNickName, sizeof(CMD3.szUserNickName));
		memcpy(CMD3.szHeadUrl, szTempHeadUrl, sizeof(CMD3.szHeadUrl));
		memcpy(CMD3.szChat, pDbr->szChat, sizeof(CMD3.szChat));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CHAT_ALL_BDCAST,dwContextID,&CMD3, sizeof(CMD3));


		/* 3. 聊天返回 */
		STR_CMD_LC_CLUB_CHAT CMD2;
		CMD2.byChatMode = pDbr->byChatMode;
		CMD2.lResultCode = lResultCode2;
		memcpy(CMD2.szDescribe, TEXT("发送失败, 稍后重试"), sizeof(CMD2));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CHAT_ALL,dwContextID,&CMD2, sizeof(CMD2));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//牌友圈置顶
bool CDataBaseEngineSink::On_DBR_CL_STICKY_POST(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_DBR_CL_CLUB_STICKY_POST)) return false;
		STR_DBR_CL_CLUB_STICKY_POST *pDbr = (STR_DBR_CL_CLUB_STICKY_POST * )pData;

		//加载类型
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("dwUserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("dwClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("dwStickyMark"),pDbr->cbTopFlag);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_STICK"));
		LONG lResultCode2 = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_STICK"),true);

		//构造DBO数据
		STR_DBO_LC_CLUB_STICKY_POST DBO;
		ZeroMemory(&DBO, sizeof(STR_DBO_LC_CLUB_STICKY_POST));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_STICKY_POST, dwContextID, &DBO, sizeof(DBO));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//玩家离开俱乐部房间
bool CDataBaseEngineSink::On_DBR_CL_CLUB_ROOM_USER_LEAVE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_USER_LEAVE)) return false;
		STR_SUB_CL_CLUB_ROOM_USER_LEAVE* pDbr = (STR_SUB_CL_CLUB_ROOM_USER_LEAVE*) pData;

		//加载类型
		m_AccountsDBAide.ResetParameter();

		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("RoomID"),pDbr->dwClubRoomID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_ROOM_USER_LEAVE"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_USER_LEAVE"),true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//房间设置
bool CDataBaseEngineSink::On_DBR_CL_CLUB_ROOM_SETTING(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_SETTING)) return false;
		STR_SUB_CL_CLUB_ROOM_SETTING* pDbr = (STR_SUB_CL_CLUB_ROOM_SETTING*) pData;

#pragma region 1. 获取之前的房间规则, 并且构造新的房间规则
		//数据库传入参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwClubRoomID"), pDbr->dwRoomID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_ROOM_SETTING_QUERY_INFO")); 
		//执行查询
		LONG lResultCode1 = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_SETTING_QUERY_INFO"), true);

		if(lResultCode1 != DB_SUCCESS)
		{
			CTraceService::TraceString(TEXT("GSP_CL_CLUB_ROOM_SETTING_QUERY_INFO Return 非0"), TraceLevel_Warning);
			return false ;
		}

		//获取到数据
		TCHAR szRealRoomRule[2048];
		m_AccountsDBAide.GetValue_String(TEXT("RealRoomRule"), szRealRoomRule, sizeof(szRealRoomRule));

		//构造新的房间规则字符串
		STR_SUB_CG_USER_CREATE_ROOM strCreateRoom;
		StrToBin(szRealRoomRule, strCreateRoom.CommonRule, 0, 255);
		StrToBin(szRealRoomRule, strCreateRoom.SubGameRule, 256, 512);

		tagTableRule *pCfg = (tagTableRule*)strCreateRoom.CommonRule;
		pCfg->bDissolve = pDbr->bDissolve;
		pCfg->dwDissolveTime = pDbr->dwDissolveTime;
		pCfg->byMask = pDbr->byMask;
		pCfg->dwAmount = pDbr->dwAmount;
		pCfg->dwOwnerPercentage = 	pDbr->dwOwnerPercentage;
		pCfg->dwDizhu = pDbr->dwDizhu;
		pCfg->CellScore = pDbr->dwDizhu;
		pCfg->dwLevelGold = pDbr->dwGold;

		//修改房间名字 -- 房间名字在SubGameRule中
		struct tagSubRule
		{
			TCHAR	szRoomNote[40]; //房间规则说明
			TCHAR	szRoomName[16];	//房间名字
		};
		tagSubRule *pSub = (tagSubRule*)strCreateRoom.SubGameRule;
		memcpy(pSub->szRoomName, pDbr->szRoomName, sizeof(pSub->szRoomName));

		//16进制的房间规则 256个字节
		CString strCommonRoomRule = toHexString(strCreateRoom.CommonRule, 128);
		CString strSubRoomRule = toHexString(strCreateRoom.SubGameRule, 127); //舍弃最后一个字符
		CString strRealRoomRule = strCommonRoomRule + strSubRoomRule;


#pragma endregion

#pragma region 2. 新的数据 传递给数据库
		//加载类型
		m_AccountsDBAide.ResetParameter();

		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("RoomID"),pDbr->dwRoomID);	
		
		//数值转换
		BYTE byModeID = 0;
		if(1 == pDbr->byGoldOrFK)//俱乐部的房卡场 就是普通房卡场
		{
			byModeID  = 0;
		}
		else if( 2 == pDbr->byGoldOrFK)//俱乐部的金币场 其实是 房卡金币场
		{
			byModeID  = 3;
		}

		m_AccountsDBAide.AddParameter(TEXT("GoldOrFK"),byModeID);
		m_AccountsDBAide.AddParameter(TEXT("Dissolve"),pDbr->bDissolve);
		m_AccountsDBAide.AddParameter(TEXT("DissolveTime"),pDbr->dwDissolveTime);
		m_AccountsDBAide.AddParameter(TEXT("Amount"),pDbr->dwAmount);
		m_AccountsDBAide.AddParameter(TEXT("OwnerPercentage"),pDbr->dwOwnerPercentage);
		m_AccountsDBAide.AddParameter(TEXT("dwDizhu"),pDbr->dwDizhu);
		m_AccountsDBAide.AddParameter(TEXT("dwGold"),pDbr->dwGold);
		m_AccountsDBAide.AddParameter(TEXT("byMask"),pDbr->byMask);
		m_AccountsDBAide.AddParameter(TEXT("RoomName"),pDbr->szRoomName);
		m_AccountsDBAide.AddParameter(TEXT("RealRoomRule"),strRealRoomRule);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_ROOM_SETTING"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_SETTING"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//结构体构造
		STR_CMD_LC_CLUB_ROOM_SETTING CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		CMD.dwRoomID=m_AccountsDBAide.GetValue_DWORD(TEXT("RoomID"));

		//数值转换
		BYTE byResultModeID = m_AccountsDBAide.GetValue_BYTE(TEXT("ModeID"));;
		if(0 == byResultModeID)//俱乐部的房卡场 就是普通房卡场
		{
			CMD.byGoldOrFK  = 1;
		}
		else if( 3 == byResultModeID)//俱乐部的金币场 其实是 房卡金币场
		{
			CMD.byGoldOrFK  = 2;
		}

		CMD.dwDissolveTime=m_AccountsDBAide.GetValue_DWORD(TEXT("DissolveMinute"));
		CMD.byDissolve= m_AccountsDBAide.GetValue_BYTE(TEXT("DissolveMinute"));
		CMD.dwAmount=m_AccountsDBAide.GetValue_DWORD(TEXT("ServiceGold"));
		CMD.dwOwnerPercentage=m_AccountsDBAide.GetValue_DWORD(TEXT("Revenue"));
		CMD.byMask = pDbr->byMask;
		CMD.dwDizhu = pDbr->dwDizhu;
		CMD.dwGold = pDbr->dwGold;
		m_AccountsDBAide.GetValue_String(TEXT("RoomName"), CMD.szRoomName,CountArray(CMD.szRoomName));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_ROOM_SETTING,dwContextID,&CMD,sizeof(CMD));
#pragma endregion
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//请求房间设置
bool CDataBaseEngineSink::On_DBR_CL_CLUB_ROOM_QUERY_SETTING(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_QUERY_SETTING)) return false;
		STR_SUB_CL_CLUB_ROOM_QUERY_SETTING* pDbr = (STR_SUB_CL_CLUB_ROOM_QUERY_SETTING*) pData;

		//加载类型
		m_AccountsDBAide.ResetParameter();

		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("RoomID"),pDbr->dwRoomID);	

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_ROOM_QUERY_SETTING"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_QUERY_SETTING"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//结构体构造
		STR_CMD_LC_CLUB_ROOM_QUERY_SETTING CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		CMD.dwRoomID=m_AccountsDBAide.GetValue_DWORD(TEXT("RoomID"));

		//数值转换
		BYTE byResultModeID = m_AccountsDBAide.GetValue_BYTE(TEXT("ModeID"));;
		if(0 == byResultModeID)//俱乐部的房卡场 就是普通房卡场
		{
			CMD.byGoldOrFK  = 1;
		}
		else if( 3 == byResultModeID)//俱乐部的金币场 其实是 房卡金币场
		{
			CMD.byGoldOrFK  = 2;
		}

		CMD.dwDissolveTime=m_AccountsDBAide.GetValue_DWORD(TEXT("DissolveMinute"));
		CMD.byDissolve= m_AccountsDBAide.GetValue_BYTE(TEXT("DissolveMinute"));
		CMD.dwAmount=m_AccountsDBAide.GetValue_DWORD(TEXT("ServiceGold"));
		CMD.dwOwnerPercentage=m_AccountsDBAide.GetValue_DWORD(TEXT("Revenue"));
		CMD.byMask = m_AccountsDBAide.GetValue_BYTE(TEXT("Mask"));
		CMD.dwDizhu = m_AccountsDBAide.GetValue_DWORD(TEXT("Dizhu"));
		CMD.dwGold = m_AccountsDBAide.GetValue_DWORD(TEXT("Gold"));
		m_AccountsDBAide.GetValue_String(TEXT("RoomName"), CMD.szRoomName,CountArray(CMD.szRoomName));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_ROOM_QUERY_SETTING,dwContextID,&CMD,sizeof(CMD));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}


//解散房间请求
bool CDataBaseEngineSink::On_DBR_CL_CLUB_ROOM_DISSOLVE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_DISSOLVE)) return false;
		STR_SUB_CL_CLUB_ROOM_DISSOLVE* pDbr = (STR_SUB_CL_CLUB_ROOM_DISSOLVE*) pData;

		//加载类型
		m_AccountsDBAide.ResetParameter();

		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("RoomID"),pDbr->dwClubRoomID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_ROOM_DISSOLVE"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_DISSOLVE"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//结构体构造
		STR_CMD_LC_CLUB_ROOM_DISSOLVE CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_ROOM_DISSOLVE,dwContextID,&CMD,sizeof(CMD));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//解散桌子请求
bool CDataBaseEngineSink::On_DBR_CL_CLUB_TABLE_DISSOLVE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_TABLE_DISSOLVE)) return false;
		STR_SUB_CL_CLUB_TABLE_DISSOLVE* pDbr = (STR_SUB_CL_CLUB_TABLE_DISSOLVE*) pData;

		//加载类型
		m_AccountsDBAide.ResetParameter();

		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("RoomID"),pDbr->dwClubRoomID);
		m_AccountsDBAide.AddParameter(TEXT("TableID"),pDbr->dwTableID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_TABLE_DISSOLVE"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_TABLE_DISSOLVE"),true);

		if(lResultCode == DB_SUCCESS)
		{
			//返回结构体初始化
			STR_CMD_LC_CLUB_TABLE_DISSOLVE CMD;
			ZeroMemory(&CMD, sizeof(CMD));

			//结果处理
			CDBVarValue DBVarValue;
			m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
			lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

			CMD.byMask = m_AccountsDBAide.GetValue_BYTE(TEXT("Mask"));
			CMD.dwGameID = m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
			CMD.dwTableID = pDbr->dwTableID;

			CMD.lResultCode = lResultCode;
			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_TABLE_DISSOLVE,dwContextID,&CMD,sizeof(CMD));

			return true;
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//解散牌友圈
bool CDataBaseEngineSink::On_DBR_CL_CLUB_DISS_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_DISS_CLUB)) return false;
		STR_SUB_CL_CLUB_DISS_CLUB* pDbr = (STR_SUB_CL_CLUB_DISS_CLUB*) pData;

		//加载类型
		m_AccountsDBAide.ResetParameter();

		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_DISS_CLUB"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_DISS_CLUB"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//结构体构造
		STR_CMD_LC_CLUB_DISS_CLUB CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_DISS_CLUB,dwContextID,&CMD,sizeof(CMD));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//创建牌友圈
bool CDataBaseEngineSink::On_DBR_CL_CLUB_CREATE_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(STR_DBR_CL_CLUB_CREATE_CLUB));
		if (wDataSize!=sizeof(STR_DBR_CL_CLUB_CREATE_CLUB)) return false;

		STR_DBR_CL_CLUB_CREATE_CLUB *pDBR = (STR_DBR_CL_CLUB_CREATE_CLUB*) pData;

		//加载类型
		m_AccountsDBAide.ResetParameter();

		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("@Mystery"),_MYSTERY);
		m_AccountsDBAide.AddParameter(TEXT("@UserID"),pDBR->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@ClubName"),pDBR->szClubName);
		m_AccountsDBAide.AddParameter(TEXT("@MajorKindID"),pDBR->dwMajorKindID);
		m_AccountsDBAide.AddParameter(TEXT("@MinorKindID"), pDBR->szMinorKindID);
		m_AccountsDBAide.AddParameter(TEXT("@szNotice"), pDBR->szNotice);
		m_AccountsDBAide.AddParameter(TEXT("@szMessag"), pDBR->szMessag);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_CREATE_CLUB"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_CREATE_CLUB"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//结构体构造
		STR_CMD_LC_CLUB_CREATE_CLUB CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		if(lResultCode == DB_SUCCESS)
		{
			CMD.dwClubID =m_AccountsDBAide.GetValue_DWORD(TEXT("ClubID"));
			CMD.byClubLevel=m_AccountsDBAide.GetValue_BYTE(TEXT("ClubLevel"));
			CMD.dwFK=m_AccountsDBAide.GetValue_BYTE(TEXT("FK"));
		}

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_CREATE_CLUB,dwContextID,&CMD,sizeof(CMD));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//申请加入房间
bool CDataBaseEngineSink::On_DBR_CL_CLUB_JOIN_ROOM(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize!=sizeof(STR_SUB_CL_CLUB_JOIN_ROOM)) return false;

		STR_SUB_CL_CLUB_JOIN_ROOM * pDBbr = (STR_SUB_CL_CLUB_JOIN_ROOM*)pData;

		STR_CMD_LC_CLUB_JOIN_ROOM CMD1;

#pragma region 桌子列表
		/* 第一步 桌子列表 */
		//加载类型
		m_AccountsDBAide.ResetParameter();

		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("@UserID"),pDBbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@RoomID"),pDBbr->dwRoomID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_JOIN_ROOM_TABLE"));
		long ResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_JOIN_ROOM_TABLE"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		CMD1.lResultCode = ResultCode;
		lstrcpyn(CMD1.szDescribe, CW2CT(DBVarValue.bstrVal), CountArray(CMD1.szDescribe));

		//列表发送
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_CLUB_TABLE_LIST * pDBO=NULL;
		while ((ResultCode == DB_SUCCESS) && (m_AccountsDBModule->IsRecordsetEnd()==false ))
		{
			//发送信息
			if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_TABLE_LIST))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_TABLE_LIST_TABLE,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pDBO=(STR_CMD_LC_CLUB_TABLE_LIST *)(cbBuffer+wPacketSize);
			
			pDBO->dwClubRoomID  = m_AccountsDBAide.GetValue_DWORD(TEXT("RoomID"));
			pDBO->dwTableID=m_AccountsDBAide.GetValue_DWORD(TEXT("TableID"));
			pDBO->dwClubID = m_AccountsDBAide.GetValue_DWORD(TEXT("ClubID"));
			pDBO->byMask = m_AccountsDBAide.GetValue_BYTE(TEXT("IsOwner"));

			pDBO->TableState=m_AccountsDBAide.GetValue_DWORD(TEXT("TableState"));
			pDBO->LockState=m_AccountsDBAide.GetValue_DWORD(TEXT("LockState"));
			pDBO->CurrentRound=m_AccountsDBAide.GetValue_DWORD(TEXT("CurrentRound"));
			pDBO->dwOwnerID=m_AccountsDBAide.GetValue_DWORD(TEXT("OwnerID"));
			pDBO->byDel = 0;

			//设置位移
			wPacketSize+=sizeof(STR_CMD_LC_CLUB_TABLE_LIST);

			//移动记录
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_TABLE_LIST_TABLE,dwContextID,cbBuffer,wPacketSize);

#pragma endregion


#pragma region 玩家列表
		/* 第二步 玩家列表 */
		//加载类型
		m_AccountsDBAide.ResetParameter();

		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("@UserID"),pDBbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@RoomID"),pDBbr->dwRoomID);

		//输出参数
		TCHAR szDescribeString2[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString2),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_JOIN_ROOM_USER"));
		long ResultCode2 = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_JOIN_ROOM_USER"),true);

		//列表发送
		WORD wPacketSize2=0;
		BYTE cbBuffer2[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize2=0;
		STR_CMD_LC_CLUB_TABLE_USER_LIST * pDBO2=NULL;
		while ((m_AccountsDBModule->IsRecordsetEnd()==false) && (ResultCode2 == DB_SUCCESS))
		{
			//发送信息
			if ((wPacketSize2+sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST))>sizeof(cbBuffer2))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_TABLE_LIST_USER,dwContextID,cbBuffer2,wPacketSize2);
				wPacketSize2=0;
			}

			//读取信息
			pDBO2=(STR_CMD_LC_CLUB_TABLE_USER_LIST *)(cbBuffer2+wPacketSize2);
			pDBO2->dwClubRoomID=m_AccountsDBAide.GetValue_DWORD(TEXT("ClubRoomID"));
			pDBO2->dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			m_AccountsDBAide.GetValue_String(TEXT("UserName"),pDBO2->szUserName,CountArray(pDBO2->szUserName));
			pDBO2->bySex=m_AccountsDBAide.GetValue_BYTE(TEXT("Sex"));
			pDBO2->wLevel=m_AccountsDBAide.GetValue_WORD(TEXT("UserLevel"));
			m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),pDBO2->szHeadUrl,CountArray(pDBO2->szHeadUrl));
			pDBO2->byClubRole=m_AccountsDBAide.GetValue_BYTE(TEXT("ClubRole"));
			pDBO2->byClubReputation=m_AccountsDBAide.GetValue_BYTE(TEXT("ClubReputation"));
			pDBO2->dwTableID=m_AccountsDBAide.GetValue_DWORD(TEXT("TableID"));
			pDBO2->byChairID=m_AccountsDBAide.GetValue_BYTE(TEXT("ChairID"));
			pDBO2->byDel = 0;

			//设置位移
			wPacketSize2+=sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST);

			//移动记录
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize2>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_TABLE_LIST_USER,dwContextID,cbBuffer2,wPacketSize2);

#pragma endregion

#pragma region 房间人数
		//加载类型
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@RoomID"),pDBbr->dwRoomID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_JOIN_ROOM_USER_NUM"));
		ResultCode2 = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_JOIN_ROOM_USER_NUM"),true);

		if(ResultCode2 == DB_SUCCESS)
		{
			CMD1.dwPlayerNum = m_AccountsDBAide.GetValue_DWORD(TEXT("PlayerNum"));
		}

#pragma endregion

		/* 第三步 加入房间返回*/
		//client 不依赖次消息号刷新界面
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_JOIN_ROOM, dwContextID, &CMD1, sizeof(CMD1));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}


//群主|管理对申请消息的答复(同意|拒绝)
bool CDataBaseEngineSink::On_DBR_CL_CLUB_APPLICANT_RESULT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_APPLICANT_RESULT)) return false;

	STR_SUB_CL_CLUB_APPLICANT_RESULT *pDBR = (STR_SUB_CL_CLUB_APPLICANT_RESULT*) pData;

	//加载类型
	m_AccountsDBAide.ResetParameter();
	//构造参数
	m_AccountsDBAide.AddParameter(TEXT("@UserID"),pDBR->dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@CludID"),pDBR->dwCludID);
	m_AccountsDBAide.AddParameter(TEXT("@Result"),pDBR->dwResult);
	m_AccountsDBAide.AddParameter(TEXT("@TagID"),pDBR->dwTagID);

	//输出参数
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//输出执行
	LogicTraceHelper(TEXT("GSP_CL_CLUB_APPLICANT_RESULT"));
	LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_APPLICANT_RESULT"),true);

	//结果处理
	CDBVarValue DBVarValue;
	m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//结构体构造
	STR_CMD_LC_CLUB_APPLICANT_RESULT CMD;
	ZeroMemory(&CMD, sizeof(CMD));
	CMD.lResultCode = lResultCode;
	lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_APPLICANT_RESULT,dwContextID,&CMD,sizeof(CMD));

	//TODONOWW 管理员同意之后 需要通知客户端指定玩家实时刷新界面
	return true;
}

//用户退出请求
bool CDataBaseEngineSink::On_DBR_CL_CLUB_QUIT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	if (wDataSize!=sizeof(STR_SUB_CL_CLUB_QUIT)) return false;

	STR_SUB_CL_CLUB_QUIT *pDBR = (STR_SUB_CL_CLUB_QUIT*) pData;

	//加载类型
	m_AccountsDBAide.ResetParameter();

	//构造参数
	m_AccountsDBAide.AddParameter(TEXT("@UserID"),pDBR->dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@ClubID"),pDBR->dwClubID);

	//输出参数
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//输出执行
	LogicTraceHelper(TEXT("GSP_CL_CLUB_QUIT"));
	LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_QUIT"),true);

	//结果处理
	CDBVarValue DBVarValue;
	m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//结构体构造
	STR_CMD_LC_CLUB_QUIT DBO;
	ZeroMemory(&DBO, sizeof(DBO));
	DBO.lResultCode = lResultCode;
	lstrcpyn(DBO.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(DBO.szDescribe));

	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_QUIT,dwContextID,&DBO,sizeof(DBO));
	return true;
}

//请求成员数据
bool CDataBaseEngineSink::On_DBR_CL_CLUB_MEMBER_MANAGER(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//校验参数
		if (wDataSize!=sizeof(STR_SUB_CL_CLUB_MEMBER_MANAGER)) return false;
		STR_SUB_CL_CLUB_MEMBER_MANAGER *pDBR = (STR_SUB_CL_CLUB_MEMBER_MANAGER*) pData;

		/* 读取工会数据 */
		m_AccountsDBAide.ResetParameter();

		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("@UserID"),pDBR->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@ClubID"),pDBR->dwClubID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_DATA"));
		LONG lResultCode2 =  m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_DATA"),true);

		if(DB_SUCCESS == lResultCode2)
		{
			STR_CMD_LC_CLUB_DATA CMD1;
			m_AccountsDBAide.GetValue_String(TEXT("ClubName"),CMD1.szClubName,CountArray(CMD1.szClubName));
			m_AccountsDBAide.GetValue_String(TEXT("ClubOwnerName"),CMD1.szClubOwnerName,CountArray(CMD1.szClubOwnerName));
			CMD1.dwClubID =m_AccountsDBAide.GetValue_DWORD(TEXT("ClubID"));
			CMD1.dwClubOwner =m_AccountsDBAide.GetValue_DWORD(TEXT("ClubOwner"));
			CMD1.dwMajorKindID =m_AccountsDBAide.GetValue_DWORD(TEXT("MajorKindID"));
			//TODONOW dwMinorKindID
			CMD1.byClubLevel =m_AccountsDBAide.GetValue_BYTE(TEXT("ClubLevel"));
			CMD1.wMemberNum =m_AccountsDBAide.GetValue_WORD(TEXT("MemberNum"));
			m_AccountsDBAide.GetValue_String(TEXT("Notice"),CMD1.szNotice,CountArray(CMD1.szNotice));
			m_AccountsDBAide.GetValue_String(TEXT("Message"),CMD1.szMessage,CountArray(CMD1.szMessage));
			CMD1.dwFK =m_AccountsDBAide.GetValue_DWORD(TEXT("FK"));
			CMD1.byApplyNum =m_AccountsDBAide.GetValue_BYTE(TEXT("ApplyNum"));
			CMD1.byAutoAgree =m_AccountsDBAide.GetValue_BYTE(TEXT("AutoAgree"));
			CMD1.bySex =m_AccountsDBAide.GetValue_BYTE(TEXT("Sex"));
			CMD1.wLevel =m_AccountsDBAide.GetValue_WORD(TEXT("Level"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_DATA,dwContextID,&CMD1,sizeof(CMD1));
		}
		else //如果工会基本信息读取失败, 后面的成员也不需要读取了
		{
			return true;
		}

		/* 成员数据返回 */
		//加载类型
		m_AccountsDBAide.ResetParameter();

		//构造参数
		m_AccountsDBAide.AddParameter(TEXT("@UserID"),pDBR->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@ClubID"),pDBR->dwClubID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_CLUB_MEMBER_MANAGER"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_MEMBER_MANAGER"),true);

		//列表发送
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_CLUB_MEMBER_MANAGER * pCMD=NULL;
		while ((lResultCode == DB_SUCCESS) && (m_AccountsDBModule->IsRecordsetEnd()==false))
		{
			//发送信息
			if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_MEMBER_MANAGER))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_MEMBER_MANAGER,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pCMD=(STR_CMD_LC_CLUB_MEMBER_MANAGER *)(cbBuffer+wPacketSize);

			pCMD->dwUserID =m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			m_AccountsDBAide.GetValue_String(TEXT("NickName"),pCMD->szUserName,CountArray(pCMD->szUserName));
			pCMD->byClubRole =m_AccountsDBAide.GetValue_BYTE(TEXT("RoleID"));
			pCMD->byUserLevel =m_AccountsDBAide.GetValue_BYTE(TEXT("UserLevel"));
			pCMD->byClubReputation =m_AccountsDBAide.GetValue_BYTE(TEXT("ClubReputation"));
			pCMD->byUserState =m_AccountsDBAide.GetValue_BYTE(TEXT("IsOnline"));
			m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),pCMD->szHeadUrl,CountArray(pCMD->szHeadUrl));

			//设置位移
			wPacketSize+=sizeof(STR_CMD_LC_CLUB_MEMBER_MANAGER);

			//移动记录
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_MEMBER_MANAGER,dwContextID,cbBuffer,wPacketSize);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_MEMBER_MANAGER_FINISH,dwContextID,NULL,0);
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}

}

#pragma endregion

#pragma region MDM_SHOP 商城道具
//DBR && DBO查询商城
bool CDataBaseEngineSink::On_DBR_CL_SHOP_QUERY(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize!=sizeof(STR_DBR_CL_SHOP_QUERY)) return false;
		STR_DBR_CL_SHOP_QUERY *pDBR = (STR_DBR_CL_SHOP_QUERY*) pData;

		//加载类型
		m_PlatformDBAide.ResetParameter();

		m_PlatformDBAide.AddParameter(TEXT("@Mystery"),_MYSTERY);
		m_PlatformDBAide.AddParameter(TEXT("@GoodsType"),pDBR->byGoodsType);
		m_PlatformDBAide.AddParameter(TEXT("@UserID"),pDBR->dwUserID);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_SHOP_QUERY"));

		LONG lResultCode  = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_CL_SHOP_QUERY"),true);

		//列表发送
		//变量定义
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_DBO_CL_SHOP_QUERY * pDBO=NULL;
		while ((lResultCode == DB_SUCCESS) && (m_PlatformDBModule->IsRecordsetEnd()==false))
		{
			//发送信息
			if ((wPacketSize+sizeof(STR_DBO_CL_SHOP_QUERY))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SHOP_QUERY,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pDBO=(STR_DBO_CL_SHOP_QUERY *)(cbBuffer+wPacketSize);
			pDBO->dwGoodsID=m_PlatformDBAide.GetValue_DWORD(TEXT("GoodsID"));
			pDBO->byDiscount=m_PlatformDBAide.GetValue_BYTE(TEXT("Discount"));
			pDBO->dbSpreadScore=m_PlatformDBAide.GetValue_DWORD(TEXT("SpreadScore"));
			pDBO->dwPrice=m_PlatformDBAide.GetValue_DWORD(TEXT("Price"));

			//设置位移
			wPacketSize+=sizeof(STR_DBO_CL_SHOP_QUERY);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SHOP_QUERY,dwContextID,cbBuffer,wPacketSize);

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SHOP_QUERY_FINISH,dwContextID,NULL,0);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//DBR && DBO钻石购买道具
bool CDataBaseEngineSink::On_DBR_CL_SHOP_DIAMOND(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		if (wDataSize!=sizeof(STR_SUB_CL_SHOP_DIAMOND)) return false;

		STR_SUB_CL_SHOP_DIAMOND *pDBR = (STR_SUB_CL_SHOP_DIAMOND*) pData;

		STR_SUB_CL_SHOP_DIAMOND DBR;
		DBR.byGoodsType = pDBR->byGoodsType;
		DBR.dwShopper = pDBR->dwShopper;
		DBR.dwGoodsID = pDBR->dwGoodsID;
		DBR.byMask = pDBR->byMask;
		DBR.dwTargetID = pDBR->dwTargetID;
		DBR.dwGoodsNum = pDBR->dwGoodsNum;

		//加载类型
		m_PlatformDBAide.ResetParameter();

		//构造参数
		m_PlatformDBAide.AddParameter(TEXT("@Mystery"),_MYSTERY);
		m_PlatformDBAide.AddParameter(TEXT("@Shopper"),DBR.dwShopper);
		m_PlatformDBAide.AddParameter(TEXT("@GoodsType"),DBR.byGoodsType);
		m_PlatformDBAide.AddParameter(TEXT("@GoodsID"),DBR.dwGoodsID);
		m_PlatformDBAide.AddParameter(TEXT("@Mask"),DBR.byMask);
		m_PlatformDBAide.AddParameter(TEXT("@TargetID"), DBR.dwTargetID);
		m_PlatformDBAide.AddParameter(TEXT("@GoodsNum"), DBR.dwGoodsNum);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//输出执行
		LogicTraceHelper(TEXT("GSP_CL_SHOP_BUY"));

		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_CL_SHOP_BUY"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//结构体构造
		STR_DBO_CL_SHOP_DIAMOND DBO;
		ZeroMemory(&DBO, sizeof(DBO));
		DBO.lResultCode = lResultCode;
		lstrcpyn(DBO.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(DBO.szDescribeString));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SHOP_DIAMOND,dwContextID,&DBO,sizeof(DBO));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//DBR && DBO背包物品查询
bool CDataBaseEngineSink::On_DBR_CL_BAG_QUERY(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	if (wDataSize!=sizeof(STR_DBR_CL_BAG_QUERY)) return false;

	STR_DBR_CL_BAG_QUERY *pDBR = (STR_DBR_CL_BAG_QUERY*) pData;

	STR_DBR_CL_BAG_QUERY DBR;
	DBR.dwUserID = pDBR->dwUserID;

	//加载类型
	m_PlatformDBAide.ResetParameter();
	//构造参数
	m_PlatformDBAide.AddParameter(TEXT("@UserID"),DBR.dwUserID);

	//输出执行
	LogicTraceHelper(TEXT("GSP_CL_BAG_QUERY"));
	LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_CL_BAG_QUERY"),true);

	//列表发送
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize=0;
	STR_DBO_CL_BAG_QUERY * pDBO=NULL;
	while ((DB_SUCCESS == lResultCode) && (m_PlatformDBModule->IsRecordsetEnd()==false))
	{
		//发送信息
		if ((wPacketSize+sizeof(STR_DBO_CL_BAG_QUERY))>sizeof(cbBuffer))
		{
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_BAG_QUERY,dwContextID,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//读取信息
		pDBO=(STR_DBO_CL_BAG_QUERY *)(cbBuffer+wPacketSize);
		pDBO->dwGoodsID = m_PlatformDBAide.GetValue_DWORD(TEXT("GoodsID"));
		pDBO->dwGoodsNum = m_PlatformDBAide.GetValue_DWORD(TEXT("GoodsNum"));
		pDBO->byGoodsType = m_PlatformDBAide.GetValue_BYTE(TEXT("GoodsType"));

		//设置位移
		wPacketSize+=sizeof(STR_DBO_CL_BAG_QUERY);

		//移动记录
		m_PlatformDBModule->MoveToNext();
	}
	if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_BAG_QUERY,dwContextID,cbBuffer,wPacketSize);


	//加载类型
	m_PlatformDBAide.ResetParameter();
	//构造参数
	m_PlatformDBAide.AddParameter(TEXT("@UserID"),DBR.dwUserID);

	//输出执行
	LogicTraceHelper(TEXT("GSP_CL_BAG_QUERY_LOVENESS"));
	lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_CL_BAG_QUERY_LOVENESS"),true);

	STR_CMD_LC_BAG_FINISH CMD2;
	ZeroMemory(&CMD2, sizeof(CMD2));

	if(DB_SUCCESS == lResultCode)
	{
		CMD2.dwLoveness = m_PlatformDBAide.GetValue_DWORD(TEXT("Loveness"));
	}

	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_BAG_FINISH, dwContextID, &CMD2, sizeof(CMD2));

	return true;
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

#pragma endregion