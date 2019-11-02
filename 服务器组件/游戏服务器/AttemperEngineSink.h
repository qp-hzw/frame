#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#include "Stdafx.h"
#include "TableFrame.h"
#include "DataBasePacket.h"
#include "Player.h"
#include "PlayerManager.h"

//绑定参数
struct tagBindParameter
{
	//连接属性
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址
	DWORD							dwActiveTime;						//激活时间
	//用户属性
	CPlayer *				        pCPlayer;					        //用户接口
};

//系统消息
struct tagSystemMessage
{
	DWORD							dwLastTime;						   //发送时间
	DBR_GR_SystemMessage            SystemMessage;                     //系统消息
};


//////////////////////////////////////////////////////////////////////////////////

//调度钩子
class CAttemperEngineSink : public IAttemperEngineSink
{
#pragma region 成员变量
	//绑定信息
protected:
	tagBindParameter *				m_pNormalParameter;					//绑定信息
	tagBindParameter *				m_pAndroidParameter;				//绑定信息

	//配置信息
protected:
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption *			m_pGameServiceOption;				//服务配置

#pragma endregion

#pragma region 初始化函数
	//函数定义
public:
	//构造函数
	CAttemperEngineSink();
	//析构函数
	virtual ~CAttemperEngineSink();

	//管理接口
public:
	//启动服务
	virtual bool StartService(){return true;}
	//停止服务
	virtual bool ConcludeService(){return true;}
#pragma endregion

#pragma region 启动停止事件(内核)
	//异步接口
public:
	//启动事件
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx);

#pragma endregion

#pragma region 事件总控制器
	//事件接口
public:
	//控制事件
	virtual bool OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize);

	//内核事件
public:
	//定时器事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);
	//数据库事件
	virtual bool OnEventDataBaseResult(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
#pragma endregion

#pragma region Socket系统函数
	//socket::client
public:
	//连接事件
	virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//关闭事件
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//读取事件
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//socket::server
public:
	//应答事件
	virtual bool OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID);
	//关闭事件
	virtual bool OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID);
	//读取事件 -- return false时候将会关闭socket连接
	virtual bool OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//网络Socket事件 读取事件 辅助函数
protected:
    //登录处理 MDM_LOGON
	bool OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户处理 MDM_USER
	bool OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//框架处理 MDM_FRAME
	bool OnTCPNetworkMainFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
    //游戏处理 MDM_GAME
	bool OnTCPNetworkMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//比赛命令 MDM_MATCH
	bool OnTCPNetworkMainMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//本地Socket事件 读取事件 辅助函数
protected:
	//注册事件
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//中转服务
	bool OnTCPSocketMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//汇总事件
	bool OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize);

#pragma endregion

#pragma region 用户接口

	//用户接口
public:
	//用户状态
	virtual bool OnEventUserItemStatus(CPlayer * pCPlayer, WORD wOldTableID=INVALID_TABLE, WORD wOldChairID=INVALID_CHAIR);
	//用户权限
	virtual bool OnEventUserItemRight(CPlayer *pCPlayer, DWORD dwAddRight, DWORD dwRemoveRight,bool bGameRight=true);
	//更新用户财富信息
	virtual bool OnEventModifyUserTreasure(CPlayer *pCPlayer, DWORD dwTableID, BYTE byTableMode, BYTE byRound, SCORE lUserTreasuse, BYTE byWin);
#pragma endregion

#pragma region 登录模块 MDM_LOGON
	//登录模块 MDM_LOGON
protected:
	//I D 登录
	bool On_SUB_CG_Logon_UserID(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ID登录返回
	bool On_CMD_GC_Logon_UserID(DWORD dwContextID, VOID * pData, WORD wDataSize);

    //登录模块 辅助函数
protected:
	//ID登录成功，激活用户（为了给pCPlayer赋值，传进去双指针，pCPlayer传进去的就是NULL）
	void ActiveUserItem(CPlayer **pCPlayer, DWORD dwContextID, 
		tagBindParameter *pBindParameter, STR_DBO_CG_LOGON_USERID *pDBOLogon, WORD wBindIndex);

	//用户登录
	VOID OnEventUserLogon(CPlayer * pCPlayer, bool bAlreadyOnLine);
	//用户登出
	VOID OnEventUserLogout(CPlayer * pCPlayer, DWORD dwLeaveReason);

	//切换连接
	bool SwitchUserItemConnect(CPlayer *pCPlayer, TCHAR szMachineID[LEN_MACHINE_ID], WORD wTargetIndex, 
								const double &dLongitude, const double &dLatitude, 
								BYTE cbDeviceType=DEVICE_TYPE_PC, WORD wBehaviorFlags=0, WORD wPageTableCount=0);
#pragma endregion

#pragma region  用户命令 MDM_USER
	// 用户命令 MDM_USER
protected:
	//创建房间
	bool On_SUB_CG_USER_CREATE_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//创建桌子(仅限 牌友圈)
	bool On_SUB_CG_USER_CREATE_TABLE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//加入桌子 --需要密码
	//需要数据库校验 1. 工会的房卡数 是否足够
	//				 2. 非工会成员不能进入工会桌子
	bool On_SUB_User_JoinFkRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//加入桌子 -- 不需要密码
	//需要数据库判断并返回可加入的桌子号
	bool On_SUB_CG_USER_JOIN_TABLE_NO_PASS(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	
	//加入桌子 -- 加入大厅金币场桌子
	//需要数据库判断并返回可加入的桌子号, 如果没有则会创建一个新的桌子
	bool On_SUB_CG_USER_JOIN_GOLD_HALL_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//加入房间 -- 加入大厅金币场桌子 返回
	bool On_CMD_GC_USER_JOIN_TABLE_HALL_GOLD( DWORD dwContextID, VOID * pData, WORD wDataSize );
	

	//断线重连
	bool On_SUB_User_ReconnectRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//加入金币房
	bool On_SUB_User_JoinGoldRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//解散房间
	bool On_SUB_User_DismissRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//创建牌友圈房间
	bool On_Sub_CG_User_CreateGroupRoom(VOID *pData, WORD wDataSize, DWORD dwSocketID);
	//创建牌友圈房间返回
	bool On_CMD_GC_User_CreateGroupRoom(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//加入牌友圈房间
	bool On_Sub_CG_User_JoinGroupRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//加入牌友圈房间返回
	bool On_CMD_GC_User_JoinGroupRoom(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//修改用户财富信息
	bool On_CMD_GC_User_ModifyUserTreasure(DWORD dwContextID, VOID * pData, WORD wDataSize);


	//用户坐下
	bool On_SUB_CG_User_SitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户起立
	bool On_SUB_CG_User_StandUp(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//邀请用户
	bool On_SUB_CG_User_InviteUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//拒绝玩家坐下
	bool On_SUB_CG_User_RefuseSit(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//踢出命用户
	bool On_SUB_CG_User_KickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//请求更换位置
	bool OnTCPNetworkSubUserChairReq(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//请求房间列表
	bool OnTCPNetworkSubGetTableList(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//请求抽奖
	bool OnTcpNetworkQueryLottery(VOID * pData, WORD wDataSize, DWORD dwSocketID);

    //抽奖结果
	bool OnDBLotteryResult( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//Club牌友圈创建桌子
	bool On_CMD_CG_CLUB_CREATE_TABLE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//加入房间  不需要密码
	bool On_CMD_GC_USER_JOIN_TABLE_NO_PASS( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//加入房间  需要密码 -- 数据库校验
	bool On_CMD_GC_JOIN_TABLE( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
#pragma endregion

#pragma region 框架命令 MDM_FRAME
	//框架命令 MDM_FRAME
protected:
	//申请解散房间
	bool On_SUB_Common_ApplyDismissRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//表决是否解散房间
	bool On_SUB_Common_VoteDismissRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
#pragma endregion

#pragma region 待处理
    //控制消息
public:
	//库存计算是否控制
	virtual bool IsControl( SCORE expectStock );
	//修改当前库存值
	virtual bool SetStockScore(SCORE currentStock);
	//设置库存值
	virtual SCORE GetChangeStockScore();

	//数据事件
protected:
	//机器信息
	bool OnDBGameAndroidInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//加载断线重连
	bool On_DBO_GR_LOAD_OFFLINE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//发送函数
protected:
	//发送用户信息
	bool SendUserInfoPacket(CPlayer * pCPlayer, DWORD dwSocketID);

	//辅助函数
protected:
	//请求失败
	bool SendRequestFailure(CPlayer * pCPlayer, LPCTSTR pszDescribe, LONG lErrorCode);

	//辅助函数
public:
	//绑定用户
	CPlayer * GetBindUserItem(WORD wBindIndex);
	//绑定参数
	tagBindParameter * GetBindParameter(WORD wBindIndex);
	//道具类型
	WORD GetPropertyType(WORD wPropertyIndex);
	//获取空闲虚拟用户 
	CPlayer * GetVirtualUserItem();

	//辅助函数
protected:
	//设置参数
	void SetMobileUserParameter(CPlayer * pCPlayer,BYTE cbDeviceType,WORD wBehaviorFlags,WORD wPageTableCount);
	//群发数据
	virtual bool SendDataBatchToMobileUser(WORD wCmdTable, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//停止服务
	void StopService();
	//通知服务停止
	void SendServiceStopped();
#pragma endregion

#pragma region  创建房间 && 创建桌子 && 加入桌子
    //创建房间 && 创建桌子
protected: 
	//检查创建桌子的门票
	bool CheckCreateTableTicket(tagTableRule * pCfg, CPlayer *pCPlayer);

    //创建普通桌子(1. 为自己创建桌子  2.为他人创建桌子)
	bool CreateTableNormal(tagTableRule * pCfg, CPlayer *pCPlayer, STR_SUB_CG_USER_CREATE_ROOM* pCreateRoom);
	//创建牌友圈房间
	bool CreateRoomClub(tagTableRule * pCfg, CPlayer *pCPlayer, STR_SUB_CG_USER_CREATE_ROOM* pCreateRoom);
	//创建牌友圈桌子(先读取数据库中的房间规则)
	bool CreateTableClub(STR_DBO_GC_CLUB_CREATE_TABLE* , CPlayer *pCPlayer);

	//创建桌子 金币大厅桌子  -- 玩家进入金币大厅时候, 如果没有找到可用的桌子,就会创建桌子
	bool CreateTableHallGold(STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD* , CPlayer *pCPlayer);

	//创建桌子 俱乐部桌子  -- 玩家进入俱乐部桌子时候, 如果没有找到可用的桌子,就会创建桌子
	bool CreateTableAutoClub(STR_DBO_CG_USER_JOIN_TABLE_NO_PASS* , CPlayer *pCPlayer);



    //创建房间 && 创建桌子 辅助函数
protected: 
    //为自己创建桌子
	bool HandleCreateTable(CTableFrame *pCurTableFrame, CPlayer *pCPlayer, tagTableRule *pCfg);
	//为他人创建桌子
	void HandleCreateTableForOthers(CTableFrame *pCurTableFrame, CPlayer *pCPlayer, tagTableRule *pCfg);

	//牌友圈房间信息需要写入数据库
	bool WriteClubRoomToDB(STR_DBR_CLUB_ROOM_INFO* pTableInfo);

    //寻找下一个空桌子
	CTableFrame* GetNextEmptyTable();
    //生成桌子ID，即加入房间密码
	DWORD GenerateTablePassword();

    //根据密码寻找指定桌子
	CTableFrame* GetDesignatedTable(const DWORD &dwPassword);

    //查找金币房空椅子
	CTableFrame* GetGlodRoomEmptyChair(WORD &wChairID);

	//加入桌子
protected:
	 //检查加入桌子的门票
	bool CheckJoinTableTicket(tagTableRule * pCfg, CPlayer *pCPlayer);

#pragma endregion 

#pragma region DB事件通知
	//替他人开房
public:
	//添加替他人开房
	bool AddOtherRoomInfo(DBR_GR_UpdateTableInfo* pTableInfo);
	//替他人开房结束  && club 游戏开始
	virtual void ConcludeTable(DWORD dwTableID);
	//替他人开房开始 && club小局结束
	virtual void StartTable(DWORD dwTableID);
	//用户加入替他人开房 && club桌子解散
	virtual void JoinTable(DWORD dwTableID, DWORD dwUserID);
	//用户离开替他人开房
	virtual void LeaveTable(DWORD dwTableID, DWORD dwUserID);


	//Club牌友圈2  &&  金币大厅（1.用户信息  2.桌子信息）
public:
	//用户坐下
	virtual void ClubPlayerSitDown(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold);
	//用户起立
	virtual void ClubPlayerSitUp(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold);
	//最后一个用户起立
	virtual void ClubLastPlayerSitUp(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold);
	//用户掉线
	virtual void ClubPlayerOffline(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold){}

	//创建桌子 -- 俱乐部
	virtual void ClubTableCreate(DWORD dwClubRoomID, DWORD dwUserID, DWORD dwTableID, DWORD dwLockState);
	//创建桌子 -- 金币大厅
	void HallTableCreate(DWORD dwUserID, DWORD dwKindID,BYTE byGameType, DWORD dwTableID);
	//房间开始游戏
	virtual void ClubTableStart(DWORD dwTableID, BYTE byClubOrHallGold);
	//桌子人数已满
	virtual void ClubTableMax(DWORD dwTableID, BYTE byClubOrHallGold);
	//桌子人数 不满
	virtual void ClubTableNotMax(DWORD dwTableID, BYTE byClubOrHallGold);
	//小局结束游戏 -- 俱乐部
	virtual void ClubTableXJ(DWORD dwTableID);
	//大局结束游戏 -- 俱乐部
	virtual void ClubTableDJ(DWORD dwTableID);

#pragma endregion

};

extern CAttemperEngineSink * g_AttemperEngineSink;

//////////////////////////////////////////////////////////////////////////////////





#endif