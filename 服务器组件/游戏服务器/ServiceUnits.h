#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

#include "Stdafx.h"
#include "AttemperEngineSink.h"
#include "DataBaseEngineSink.h"

//////////////////////////////////////////////////////////////////////////////////

//网络标示
#define NETWORK_CORRESPOND			1									//协调连接



//服务单元
class CServiceUnits : public CWnd
{
	//服务对象
public:
	ITimerEngine				    *m_TimerEngine;						//时间引擎
	IAttemperEngine					*m_AttemperEngine;					//调度引擎
	ITCPNetworkEngine				*m_TCPNetworkEngine;				//socket::server
	ITCPSocketEngine				*m_TCPSocketEngine;					//socket::client -> 目标服务器 协调服

	//回调对象
public:
	CAttemperEngineSink				m_AttemperEngineSink;				//调度钩子
	CDataBaseEngineSink				m_DataBaseEngineSink;	  		    //数据钩子

	//组件配置
protected:
	tagGameServiceAttrib			m_GameServiceAttrib;				//服务属性
	tagGameServiceOption			m_GameServiceOption;				//服务配置

	//subgame_dll_name
protected:
	std::string			            m_subgame_dll_name;				    //子游戏dll名字
	

	//函数定义
public:
	//构造函数
	CServiceUnits(std::string dll_name);
	//析构函数
	virtual ~CServiceUnits();


	//服务控制
public:
	//启动服务
	bool StartService();
	//停止服务
	bool ConcludeService();

public:
	//启动socket::server
	bool StartNetworkService();

	//辅助函数
protected:
	//配置组件
	int InitializeService();
	//启动内核
	int StartKernelService();

};

extern CServiceUnits               *g_pServiceUnits;                     
extern IAttemperEngine			   *g_AttemperEngine;					//调度引擎
extern ITCPNetworkEngine		   *g_TCPNetworkEngine;				    //socket::server
extern ITCPSocketEngine			   *g_TCPSocketEngine;					//socker::client -- 目标服务器为 协调服
extern ITimerEngine				   *g_TimerEngine;						//定时器


#endif