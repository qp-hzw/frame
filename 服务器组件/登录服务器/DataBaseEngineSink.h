#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE
#include "Stdafx.h"
#include "DataBasePacket.h"


//////////////////////////////////////////////////////////////////////////////////
//1、继承内核的数据库类
//2、处理游戏服和登陆服发来的数据库消息
//////////////////////////////////////////////////////////////////////////////////

//数据库类
class CDataBaseEngineSink : public IDataBaseEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//用户数据库
protected:
	CDataBaseAide					m_AccountsDBAide;					//用户数据库
	CDataBaseHelper					m_AccountsDBModule;					//用户数据库

	//游戏币数据库
protected:
	CDataBaseAide					m_TreasureDBAide;					//游戏币数据库
	CDataBaseHelper					m_TreasureDBModule;					//游戏币数据库

	//平台数据库
protected:
	CDataBaseAide					m_PlatformDBAide;					//平台数据库
	CDataBaseHelper					m_PlatformDBModule;					//平台数据库

	//函数定义
public:
	//构造函数
	CDataBaseEngineSink();
	//析构函数
	virtual ~CDataBaseEngineSink(){};

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//系统事件
public:
	//启动事件
	virtual bool OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx);

	//内核事件
public:
	//请求事件
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

#pragma region  MDM_LOGON 登陆模块
	/* **************************【MAIN:1    MDM_LOGON    登陆】*************************************/
protected:
	//帐号登录
	bool On_DBR_Logon_Accounts(DWORD dwContextID, VOID * pData, WORD wDataSize);
	bool On_DBO_Logon_Accounts(DWORD dwContextID, DWORD dwResuleCode, LPCTSTR pszErrorString);
	//帐号注册
	bool On_DBR_Logon_Register(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//平台登录
	bool On_DBR_Logon_Platform(DWORD dwContextID, VOID * pData, WORD wDataSize);
	bool On_DBO_Logon_Platform(DWORD dwContextID, DWORD dwResuleCode, LPCTSTR pszErrorString);

	//变更用户财富	TODO 登陆奖励 + 老玩家奖励
	bool OnModifyUserInsure(DWORD dwContextID, void * pData, WORD wDataSize);

#pragma endregion

#pragma region MDM_SERVICE 用户服务
	/* **************************【MAIN:3    MDM_SERVICE    服务】*************************************/
protected:
	//玩家反馈
	bool On_DBR_Service_UserFeedback(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//玩家反馈返回
	bool On_DBO_Service_UserFeedback(DWORD dwContextID, DWORD dwResultCode, LPCTSTR pszErrorString);

	//刷新用户信息
	bool On_DBR_Service_RefreshUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//刷新用户信息返回
	bool On_DBO_Service_RefreshUserInfo(DWORD dwContextID, DWORD dwResultCode, LPCTSTR pszErrorString);

	//查询开房信息列表
	bool On_DBR_Service_QueryRoomList(DWORD dwContextID, void * pData, WORD wDataSize);
	//查询开房信息列表返回
	bool On_DBO_Service_QueryRoomList(DWORD dwContextID, DWORD dwResultCode);

	//获取富豪榜
	bool On_DBR_Service_GetRichList(DWORD dwContextID, void * pData, WORD wDataSize);
	//获取富豪榜返回
	bool On_DBO_Service_GetRichList(DWORD dwContextID, DWORD dwResultCode);

	//获取用户录像列表（大局）
	bool On_DBR_Service_GetUserRecordList(DWORD dwContextID, void * pData, WORD wDataSize);
	//获取用户录像列表返回
	bool On_DBO_Service_GetUserRecordList(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString);

	//获取指定ID录像（小局）
	bool On_DBR_Service_GetSpecifiRecord(DWORD dwContextID, void * pData, WORD wDataSize);
	//获取指定ID录像返回
	bool On_DBO_Service_GetSpecificRecord(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString);

	//获取在线奖励
	bool On_DBR_Service_OnlineReward(DWORD dwContextID, void * pData, WORD wDataSize);
	//获取在线奖励返回
	bool On_DBO_Service_OnlineReward(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString);

	//获取任务列表
	bool On_DBR_Service_GetTaskList(DWORD dwContextID, void * pData, WORD wDataSize);
	//获取任务列表返回
	bool On_DBO_Service_GetTaskList(DWORD dwUserID, DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString);

	//领取任务奖励
	bool On_DBR_Service_GetTaskReward(DWORD dwContextID, void * pData, WORD wDataSize);
	//领取任务奖励返回
	bool On_DBO_Service_GetTaskReward(DWORD dwUserID, DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString);

	//请求抽奖
	bool On_DBR_Service_RequestLottery(DWORD dwContextID, void * pData, WORD wDataSize);
	//请求抽奖返回
	bool On_DBO_Service_RequestLottery(DWORD dwUserID, DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString);
	
	//pure大厅排行榜 查询
	bool On_DBR_CL_SERVICE_PURE_STANDING_LIST(DWORD dwContextID, void * pData, WORD wDataSize);

	//pure大局战绩 查询
	bool On_DBR_CL_SERVICE_PURE_RECORD_LIST(DWORD dwContextID, void * pData, WORD wDataSize);

	//pure小局战绩 查询
	bool On_DBR_CL_SERVICE_PURE_XJ_RECORD_LIST(DWORD dwContextID, void * pData, WORD wDataSize);

	//小局录像回放
	bool On_DBR_CL_Service_XJRecordPlayback(DWORD dwContextID, void * pData, WORD wDataSize);

	//客服消息
	bool On_DBR_CL_SERVICE_CUSTOMER_MESSEGE(DWORD dwContextID, void * pData, WORD wDataSize);
	
	//请求金币大厅信息
	bool On_DBR_CL_SERVICE_GOLD_INFO(DWORD dwContextID, void * pData, WORD wDataSize);
	
	//修改个人资料
	bool On_DBR_Service_ModifyPersonalInfo(DWORD dwContextID, void * pData, WORD wDataSize);
	//修改个人资料返回
	bool On_DBO_Service_ModifyPersonalInfo(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString);

	//查询并更新用户金币房卡（只要涉及金币房卡等改变的消息都要用到这个函数，暂时放在这里面）
	bool On_DBR_QueryScoreInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询金币房卡返回（牌友圈 + 赠送道具时调用）
	VOID On_DBO_QueryScoreInfo(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient);

	//用户Socket连接关闭
	bool On_DBR_GP_QUIT(DWORD dwContextID, VOID * pData, WORD wDataSize); 
	
#pragma endregion

#pragma region MDM_GIFT 礼物道具
	/* **************************【MAIN:5    MDM_GIFT    礼物道具】*************************************/
protected:
	//向其他用户赠送道具
	bool On_DBR_CL_GIFT_GIVE_PROPS(DWORD dwContextID, void * pData, WORD wDataSize);

#pragma endregion

	//充值信息
	bool On_DBR_Other_RechargeInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//充值信息返回
	bool On_DBO_Other_RechargeInfo(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString);

	//兑换道具信息
	bool On_DBR_Other_ExchangeInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//兑换道具信息返回
	bool On_DBO_Other_ExchangeInfo(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString);

	//消息号6：公用操作失败结果
	VOID On_DBO_CommonOperateResult( DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, DBR_CommandSource wRequestID );

	// **************************服务器自身的排行榜类的响应函数 **************************
protected:
	//更新排行榜用户信息
	bool OnDBUpdateRankUserItem(DWORD dwContextID, void * pData, WORD wDataSize);
	//获取排行榜信息
	bool OnDBReadRankList(DWORD dwContextID, void * pData, WORD wDataSize);
	//领取排行榜奖励
	bool OnReceiveRankReward(DWORD dwContextID, void * pData, WORD wDataSize);

#pragma region 启动命令
	//启动命令
protected:
	//加载列表
	bool OnRequestLoadGameList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//在线信息
	bool OnRequestOnLineCountInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//加载跑马灯消息
	bool On_DBR_UPDATA_MARQUEE(DWORD dwContextID, VOID * pData, WORD wDataSize);
#pragma endregion

#pragma region MDM_CLUB 牌友圈(版本2)
protected:
	//查询牌友圈列表
	bool On_DBR_CL_CLUB_ALL_CLUB_INFO_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//查询指定牌友圈房间列表
	bool On_DBR_CL_CLUB_ROOM_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//查询未满员, 随机牌友圈(最大9个)
	bool On_DBR_CL_CLUB_RANDOM_CLUB_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//申请加入牌友圈
	bool On_DBR_CL_CLUB_JOIN_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize);
		
	//解散牌友圈
	bool On_DBR_CL_CLUB_DISS_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//房间设置
	bool On_DBR_CL_CLUB_ROOM_SETTING(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//请求房间设置
	bool On_DBR_CL_CLUB_ROOM_QUERY_SETTING(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//解散房间请求
	bool On_DBR_CL_CLUB_ROOM_DISSOLVE(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//解散桌子请求
	bool On_DBR_CL_CLUB_TABLE_DISSOLVE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//玩家离开俱乐部房间
	bool On_DBR_CL_CLUB_ROOM_USER_LEAVE(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//牌友圈公告
	bool On_DBR_CL_CLUB_NOTICE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//牌友圈简介
	bool On_DBR_CL_CLUB_MESSAGE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//贡献房卡
	bool On_DBR_CL_CLUB_CONTRIBUTE_FK(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//牌友圈设置
	bool On_DBR_CL_CLUB_AUTO_AGREE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//邀请他人加入牌友圈
	bool On_DBR_CL_CLUB_INVITE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//邀请他人加入牌友圈
	bool On_DBR_CL_CLUB_INVITE_RESULT(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//邀请他人加入牌友圈
	bool On_DBR_CL_CLUB_INQUERY_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//申请人列表
	bool On_DBR_CL_CLUB_APPLICANT_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//职务任免
	bool On_DBR_CL_CLUB_APPOINTMENT(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//牌友圈聊天
	bool On_DBR_CL_CLUB_CHAT(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//世界聊天
	bool On_DBR_CL_WORD_CHAT(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//系统聊天
	bool On_DBR_CL_SYSTEM_CHAT(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//私密聊天
	bool On_DBR_CL_SECRET_CHAT(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//牌友圈置顶
	bool On_DBR_CL_STICKY_POST(DWORD dwContextID, VOID * pData, WORD wDataSize);


	//工会战绩统计
	bool On_DBR_CL_CLUB_RECORD_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//创建牌友圈
	bool On_DBR_CL_CLUB_CREATE_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//申请加入房间
	bool On_DBR_CL_CLUB_JOIN_ROOM(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//群主|管理对申请消息的答复(同意|拒绝)
	bool On_DBR_CL_CLUB_APPLICANT_RESULT(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//请求成员数据
	bool On_DBR_CL_CLUB_MEMBER_MANAGER(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//踢出退出请求
	bool On_DBR_CL_CLUB_QUIT(DWORD dwContextID, VOID * pData, WORD wDataSize);

#pragma endregion

#pragma region MDM_SHOP 商城道具
protected:
	//查询商城
	bool On_DBR_CL_SHOP_QUERY(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//金钱购买道具
	bool On_DBR_CL_SHOP_MONEY(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//钻石购买道具
	bool On_DBR_CL_SHOP_DIAMOND(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//背包物品查询
	bool On_DBR_CL_BAG_QUERY(DWORD dwContextID, VOID * pData, WORD wDataSize);

#pragma endregion
};

// byte数组转为 string  TODONOW 暂时放在这里处理
const CString toHexString(const byte * input, const int datasize);

// string 转为byte数组  TODONOW 暂时放在这里处理
int StrToBin(TCHAR* inWord, BYTE* OutBin, int source_len_begin, int source_len_end);

//////////////////////////////////////////////////////////////////////////////////

#endif