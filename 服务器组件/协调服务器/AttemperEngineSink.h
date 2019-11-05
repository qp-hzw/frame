#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#include "Stdafx.h"

//调度钩子
class CAttemperEngineSink : public IAttemperEngineSink
{
#pragma region 初始化函数
	//函数定义
public:
	//构造函数
	CAttemperEngineSink(){}
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
public:
	//启动事件
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx){return true;}
	//停止事件
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx){return true;}

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
	virtual bool OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID) {return true;}
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

#pragma region 辅助函数
public:
	//服务注册
	virtual bool DoServerRegister(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);
#pragma endregion
};

//////////////////////////////////////////////////////////////////////////////////

#endif