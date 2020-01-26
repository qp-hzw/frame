#ifndef HANDLE_FROM_CLIENT_HEAD_FILE_L
#define HANDLE_FROM_CLIENT_HEAD_FILE_L

#include "Stdafx.h"
#include "DataBasePacket.h"
#include "TableFrame.h"

class CHandleFromGate
{
	//总分发函数
public:
	static bool HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	static bool HandlePacketDB(WORD wRequestID, DWORD dwScoketID, VOID * pData, WORD wDataSize);

	//一级分发函数
private:
    //登录处理 MDM_LOGON
	static bool OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户处理 MDM_USER
	static bool OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//框架处理 MDM_FRAME
	static bool OnTCPNetworkMainFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
    //游戏处理 MDM_GAME
	static bool OnTCPNetworkMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//比赛命令 MDM_MATCH
	static bool OnTCPNetworkMainMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//具体处理
private:
	#pragma region 登录模块 MDM_LOGON
	//I D 登录
	static bool On_SUB_CG_Logon_UserID(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ID登录返回
	static bool On_CMD_GC_Logon_UserID(DWORD dwContextID, VOID * pData, WORD wDataSize);

    //登录模块 辅助函数
private:
	//ID登录成功，激活用户（为了给pCPlayer赋值，传进去双指针，pCPlayer传进去的就是NULL）
	static void ActiveUserItem(DWORD dwContextID, STR_DBO_CG_LOGON_USERID *pDBOLogon);

	//用户登出
	static VOID OnEventUserLogout(CPlayer * pCPlayer, DWORD dwLeaveReason);

#pragma endregion

	#pragma region  用户命令 MDM_USER
	// 用户命令 MDM_USER
protected:
	//解散房间
	static bool On_SUB_User_DismissRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//加入牌友圈房间返回
	static bool On_CMD_GC_User_JoinGroupRoom(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//修改用户财富信息
	static bool On_CMD_GC_User_ModifyUserTreasure(DWORD dwContextID, VOID * pData, WORD wDataSize);


	//用户坐下
	static bool On_SUB_CG_User_SitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户起立
	static bool On_SUB_CG_User_StandUp(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户准备
	static bool On_SUB_CG_User_Ready(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//邀请用户
	static bool On_SUB_CG_User_InviteUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//踢出命用户
	static bool On_SUB_CG_User_KickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//请求更换位置
	static bool OnTCPNetworkSubUserChairReq(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//发起申请解散房间
	static bool On_SUB_RG_USER_ASK_DISMISS(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//表决解散房间
	static bool On_SUB_RG_USER_VOTE_DISMISS(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//Club牌友圈创建桌子
	static bool On_CMD_CG_CLUB_CREATE_TABLE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//加入房间  不需要密码
	static bool On_CMD_GC_USER_JOIN_TABLE_NO_PASS( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
#pragma endregion

	#pragma region 框架命令 MDM_FRAME
	//框架命令 MDM_FRAME
protected:

#pragma endregion

	#pragma region 待处理
	//数据事件
protected:
	//加载断线重连
	static bool On_DBO_GR_LOAD_OFFLINE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//辅助函数
protected:
	//请求失败
	static bool SendRequestFailure(CPlayer * pCPlayer, LPCTSTR pszDescribe, LONG lErrorCode);

	//辅助函数
public:
	//绑定用户
	static CPlayer * GetBindUserItem(WORD wBindIndex);

	//辅助函数
protected:
	//群发数据
	static bool SendDataBatchToMobileUser(WORD wCmdTable, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
#pragma endregion

	#pragma region  创建房间 && 创建桌子 && 加入桌子
    //流程函数
public: 
	//加入桌子 --需要密码
	//需要数据库校验 1. 工会的房卡数 是否足够
	//				 2. 非工会成员不能进入工会桌子
	static bool On_SUB_User_JoinFkRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//加入桌子 -- 不需要密码
	//需要数据库判断并返回可加入的桌子号
	static bool On_SUB_CG_USER_JOIN_TABLE_NO_PASS(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	
	//加入桌子 -- 加入大厅金币场桌子
	//需要数据库判断并返回可加入的桌子号, 如果没有则会创建一个新的桌子
	static bool On_SUB_CG_USER_JOIN_GOLD_HALL_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//申请创建房间
	static bool On_SUB_CG_USER_CREATE_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//设置房间规则
	static bool On_SUB_CG_USER_SET_ROOM_RULE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	
	//创建牌友圈房间
	static bool CreateRoomClub(tagTableRule * pCfg, CPlayer *pCPlayer, STR_SUB_CG_USER_CREATE_ROOM* pCreateRoom);
	//创建牌友圈桌子(先读取数据库中的房间规则)
	static bool CreateTableClub(STR_DBO_GC_CLUB_CREATE_TABLE* , CPlayer *pCPlayer);

	//创建桌子 金币大厅桌子  -- 玩家进入金币大厅时候, 如果没有找到可用的桌子,就会创建桌子
	static bool CreateTableHallGold(STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD* , CPlayer *pCPlayer);

	//创建桌子 俱乐部桌子  -- 玩家进入俱乐部桌子时候, 如果没有找到可用的桌子,就会创建桌子
	static bool CreateTableAutoClub(STR_DBO_CG_USER_JOIN_TABLE_NO_PASS* , CPlayer *pCPlayer);

    //辅助函数
protected: 
	//校验 申请创建桌子
	static bool CheckCreateRoom(CPlayer * player, BYTE gameMode);
	//检查 创建桌子的门票
	static bool CheckCreateTableTicket(tagTableRule * pCfg, CPlayer *pCPlayer);

	//检查加入桌子的门票
	static bool CheckJoinTableTicket(tagTableRule * pCfg, CPlayer *pCPlayer);

	//牌友圈房间信息需要写入数据库
	static bool WriteClubRoomToDB(STR_DBR_CLUB_ROOM_INFO* pTableInfo);

    //寻找下一个空桌子
	static CTableFrame* GetNextEmptyTable();
    //生成桌子ID，即加入房间密码
	static DWORD GenerateTablePassword();

    //根据密码寻找指定桌子
	static CTableFrame* GetDesignatedTable(const DWORD &dwPassword);
#pragma endregion 

	#pragma region DB事件通知
	//替他人开房
public:
	//添加替他人开房
	static bool AddOtherRoomInfo(DBR_GR_UpdateTableInfo* pTableInfo);
	//替他人开房结束  && club 游戏开始
	static  void ConcludeTable(DWORD dwTableID);
	//替他人开房开始 && club小局结束
	static  void StartTable(DWORD dwTableID);
	//用户加入替他人开房 && club桌子解散
	static  void JoinTable(DWORD dwTableID, DWORD dwUserID);
	//用户离开替他人开房
	static  void LeaveTable(DWORD dwTableID, DWORD dwUserID);


	//Club牌友圈2  &&  金币大厅（1.用户信息  2.桌子信息）
public:
	//用户坐下
	static void ClubPlayerSitDown(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold);
	//用户起立
	static void ClubPlayerSitUp(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold);
	//最后一个用户起立
	static void ClubLastPlayerSitUp(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold);
	
	//创建桌子 -- 俱乐部
	static void ClubTableCreate(DWORD dwClubRoomID, DWORD dwUserID, DWORD dwTableID, DWORD dwLockState);
	//创建桌子 -- 金币大厅
	static void HallTableCreate(DWORD dwUserID, DWORD dwKindID,BYTE byGameType, DWORD dwTableID);
	//房间开始游戏
	static void ClubTableStart(DWORD dwTableID, BYTE byClubOrHallGold);
	//桌子人数已满
	static void ClubTableMax(DWORD dwTableID, BYTE byClubOrHallGold);
	//桌子人数 不满
	static void ClubTableNotMax(DWORD dwTableID, BYTE byClubOrHallGold);
	//小局结束游戏 -- 俱乐部
	static void ClubTableXJ(DWORD dwTableID);
	//大局结束游戏 -- 俱乐部
	static void ClubTableDJ(DWORD dwTableID);

#pragma endregion

		//用户接口
public:
	//更新用户财富信息
	static bool OnEventModifyUserTreasure(CPlayer *pCPlayer, DWORD dwTableID, BYTE byTableMode, BYTE byRound, SCORE lUserTreasuse, BYTE byWin);
};

#endif