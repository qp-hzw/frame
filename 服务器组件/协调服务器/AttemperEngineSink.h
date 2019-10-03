#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GlobalInfoManager.h"

#pragma region 辅助结构体
//服务类型
enum enServiceKind
{
	ServiceKind_None,				//无效服务
	ServiceKind_Game,				//游戏服
	ServiceKind_Logon,				//登录服
};

//绑定参数
struct tagBindParameter
{
	//网络数据
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址
	DWORD							dwActiveTime;						//激活时间

	//附加数据
	DWORD							dwServiceID;						//ServerID
	enServiceKind					ServiceKind;						//服务类型
};

//数组定义
typedef CWHArray<WORD> CWordArrayTemplate;
#pragma endregion


//调度钩子
class CAttemperEngineSink : public IAttemperEngineSink
{
	friend class CServiceUnits;

#pragma region 成员变量
	//状态变量
protected:
	WORD							m_wCollectItem;						//汇总连接
	CWordArrayTemplate				m_WaitCollectItemArray;				//汇总等待

	//变量定义
protected:
    tagBindParameter *				m_pBindParameter;					//辅助数组

	//组件变量
protected:
	CGlobalInfoManager				m_GlobalInfoManager;				//全局管理

	//组件接口
protected:
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络引擎
#pragma endregion

#pragma region 初始化函数
	//函数定义
public:
	//构造函数
	CAttemperEngineSink();
	//析构函数
	virtual ~CAttemperEngineSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);
#pragma endregion

#pragma region 事件接口
	//异步接口
public:
	//启动事件
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx);

	//事件接口
public:
	//控制事件
	virtual bool OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize){return false;}

	//socket::client
public:
	//连接事件
    virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode){return false;}
	//关闭事件
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason){return false;}
	//读取事件
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize){return true;}

	//socket::server
public:
	//应答事件
	virtual bool OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID);
	//关闭事件
	virtual bool OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID);
	//读取事件
	virtual bool OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//
public:
	//定时器事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam){return false;}
	//数据库事件
	virtual bool OnEventDataBaseResult(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize){return false;}
#pragma endregion

#pragma region Socket系统函数
	//网络Socket事件 读取事件 辅助函数
protected:
	//注册模块
	bool OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//列表命令
	bool OnTCPNetworkMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//中转服务
	bool OnTCPNetworkMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户汇总
	bool OnTCPNetworkMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//后台管理
	bool OnTCPNetworkMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
#pragma endregion

#pragma region 发送函数
	//辅助函数
protected:
	//发送列表
	bool SendServerListItem(DWORD dwSocketID);

	//数据发送
protected:
	//房间发送
	bool SendDataToGame(DWORD wServerID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//大厅发送 根据wSocketID来判断是群发 还是发送给特定的大厅
	bool SendDataToPlaza(WORD wSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//用户发送 TODONOW 接口待实现, 可以通过发送给所有的client 好 gameserver实现
	bool SendDataToUser(DWORD wServerID, DWORD dwUserID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//发送给游戏的所有房间
	bool SendDataToGameAllRoom(WORD wKindID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
#pragma endregion

};

//////////////////////////////////////////////////////////////////////////////////

#endif