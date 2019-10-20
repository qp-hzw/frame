#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE


#include "Stdafx.h"
#include "AttemperEngineSink.h"


//服务单元
class CServiceUnits
{
	//服务组件
protected:
	CAttemperEngineSink				m_AttemperEngineSink;				//调度钩子

	//内核组件
protected:
	IAttemperEngine			       *m_AttemperEngine;					//调度引擎
    ITCPNetworkEngine			   *m_TCPNetworkEngine;					//网络引擎


	//函数定义
public:
	//构造函数
	CServiceUnits() {} 
	//析构函数
	virtual ~CServiceUnits() {}

	//服务控制
public:
	//启动服务
	bool StartService();
	//停止服务
	bool ConcludeService();

	//辅助函数
protected:
	//配置组件
	int InitializeService();
	//启动内核
	int StartKernelService();
};
           
extern IAttemperEngine			   *g_AttemperEngine;					//调度引擎
extern ITCPNetworkEngine		   *g_TCPNetworkEngine;				    //socket::server

#endif