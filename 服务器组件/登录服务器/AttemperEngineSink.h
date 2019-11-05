#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE
#include "Stdafx.h"
#include "DataBasePacket.h"


//绑定参数
struct tagBindParameter
{
	//网络参数
	DWORD							dwUserID;							//用户ID
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址
	DWORD							dwActiveTime;						//激活时间
};

//////////////////////////////////////////////////////////////////////////////////
//1、继承内核中的IAttemperEngineSink类
//2、处理具体收到的 【网络事件 + 时间事件 + 数据库事件】
//////////////////////////////////////////////////////////////////////////////////
class CAttemperEngineSink : public IAttemperEngineSink
{
	//变量定义
protected:
	tagBindParameter *				m_pBindParameter;					//辅助数组

#pragma region 构造析构
	//函数定义
public:
	//构造函数
	CAttemperEngineSink();
	//析构函数
	virtual ~CAttemperEngineSink(){}

	//管理接口
public:
	//启动服务
	virtual bool StartService(){return true;}
	//停止服务
	virtual bool ConcludeService(){return true;}
#pragma endregion

#pragma region 回调接口
	//异步接口
public:
	//启动事件
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx){ return true; }
	//停止事件
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx) { return true;}

	//事件接口
public:
	//控制事件
	virtual bool OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize) { return false;}

	//内核事件
public:
	//时间事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);
	//数据库事件
	virtual bool OnEventDataBaseResult(WORD wRequestID, DWORD dwScoketID, VOID * pData, WORD wDataSize);

	//socket::client （与协调服连接）
public:
	//连接事件
	virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//关闭事件
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//读取事件
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//socket::server （与client连接）
public:
	//应答事件
	virtual bool OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID);
	//关闭事件
	virtual bool OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID);
	//读取事件
	virtual bool OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);
#pragma endregion
};


extern CAttemperEngineSink* g_AttemperEngineSink;

//////////////////////////////////////////////////////////////////////////////////

#endif