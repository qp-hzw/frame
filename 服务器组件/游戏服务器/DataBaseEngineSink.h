#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#include "Stdafx.h"
#include "DataBasePacket.h"

//数据库类
class CDataBaseEngineSink : public IDataBaseEngineSink
{
	//游戏数据库
protected:
	IDataBase					   *m_TreasureDB;					    //财富数据库
	IDataBase					   *m_PlatformDB;					    //平台数据库

	//函数定义
public:
	//构造函数
	CDataBaseEngineSink();
	//析构函数
	virtual ~CDataBaseEngineSink();

	//管理接口
public:
	//启动服务
	virtual bool StartService(){return true;}
	//停止服务
	virtual bool ConcludeService(){return true;}

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



	//主消息号1
protected:
	
	//Sockt连接关闭
	bool On_DBR_GP_QUIT(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	
	//ID 登录
	bool On_DBR_Logon_UserID(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//ID登录返回
	bool On_DBO_Logon_UserID(DWORD dwContextID, const double &dLongitude, const double &dLatitude, DWORD dwErrorCode, LPCTSTR pszErrorString);

	//主消息号3
protected:
	//加入牌友圈房间
	bool On_DBR_User_JoinGroupRoom(DWORD dwContextID, VOID *pData, WORD wDataSize, DWORD &dwUserID);
	//加入牌友圈房间返回
	VOID On_DBO_User_JoinGroupRoom(DWORD dwContextID, DWORD dwGroupID, DWORD dwErrorCode, LPCTSTR pszErrorString);

	//系统处理
protected:
	//游戏写分
	bool OnRequestWriteGameScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//离开房间
	bool OnRequestLeaveGameServer(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//游戏记录
	bool OnRequestGameScoreRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//加载机器
	bool OnRequestLoadAndroidUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//机器人自动加入房间
	bool OnRequestAndroidJoinGame(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//加载断线重连
	bool On_DBR_GR_LOAD_OFFLINE(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	
	//用户权限
	bool OnRequestManageUserRight(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//比赛报名
	bool OnRequestMatchFee(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//比赛开始
	bool OnRequestMatchStart(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//比赛结束
	bool OnRequestMatchOver(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//比赛奖励
	bool OnRequestMatchReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//退出比赛
	bool OnRequestMatchQuit(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//写当前库存
	bool OnWriteCurrentStock(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

protected:
	//更新用户财富信息
	bool On_DBR_ModifyUserTreasure(DWORD dwContextID, void * pData, WORD wDataSize, DWORD &dwUserID);

protected:
	//录像保存
	bool On_DBR_SaveGameRecord(DWORD dwContextID, void * pData, WORD wDataSize, DWORD &dwUserID);

	//辅助函数
private:

	//清除数据表数据
	VOID OnClearDB();

protected: //Club牌友圈
	//club牌友圈房间信息
	bool On_DBR_CLUB_ROOM_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//club牌友圈桌子信息
	bool On_DBR_CLUB_TABLE_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//club牌友圈玩家信息
	bool On_DBR_CLUB_PLAYER_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
protected: //金币大厅 桌子
	//金币大厅 桌子信息
	bool On_DBR_HALL_GOLD_TABLE_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//金币大厅 玩家信息
	bool On_DBR_HALL_GOLD_PLAYER_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize);

protected:
	//添加开房信息
	bool OnRequestAddTableInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//开始桌子
	bool OnRequestStartTable(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//结束桌子
	bool OnRequestEndTable(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//添加用户到房间
	bool OnRequestAddUserToTable(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//删除房间用户
	bool OnRequestDeleteUserToTable(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//更新开房信息
	bool OnUpdateTableInfo(DWORD dwTableID);
	//更新已开房信息
	bool OnUpdateEndTableInfo(DWORD dwTableID);

	//*********桌子信息记录函数**********
	//更新游戏任务状态
	bool On_Table_UpdateGameTaskStatus(DWORD dwContextID, void *pData, WORD wDataSize);


	//club牌友圈创建桌子
	bool On_DBR_CG_CLUB_CREATE_TABLE(DWORD dwContextID, void *pData, WORD wDataSize);

	//加入桌子 不需要密码
	bool On_DBR_CG_USER_JOIN_TABLE_NO_PASS(DWORD dwContextID, void *pData, WORD wDataSize);

	//加入桌子 金币大厅桌子
	bool On_DBR_CG_USER_JOIN_TABLE_HALL_GOLD(DWORD dwContextID, void *pData, WORD wDataSize);
};

// byte数组转为 string  TODONOW 暂时放在这里处理
const CString toHexString(const byte * input, const int datasize);

// string 转为byte数组  TODONOW 暂时放在这里处理
int StrToBin(TCHAR* inWord, BYTE* OutBin, int source_len_begin, int source_len_end);

//////////////////////////////////////////////////////////////////////////////////

#endif