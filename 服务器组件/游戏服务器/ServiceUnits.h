#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

#include "Stdafx.h"
#include "AttemperEngineSink.h"
#include "DataBaseEngineSink.h"
#include "DBCorrespondManager.h"

//////////////////////////////////////////////////////////////////////////////////

//网络标示
#define NETWORK_CORRESPOND			1									//协调连接



//服务单元
class CServiceUnits : public CWnd
{
	//组件配置
protected:
	tagGameServiceAttrib			m_GameServiceAttrib;				//服务属性
	tagGameServiceOption			m_GameServiceOption;				//服务配置

	//组件变量
private:
	CWHDataQueue					m_DataQueue;						//数据队列
	CCriticalSection				m_CriticalSection;					//同步对象

	//服务组件
public:
	CAttemperEngineSink				m_AttemperEngineSink;				//调度钩子
	CDataBaseEngineSink				m_RecordDataBaseSink[4];			//数据钩子
	CDataBaseEngineSink				m_KernelDataBaseSink[4];			//数据钩子
	
	//数据组件
public:
	CDBCorrespondManager            m_DBCorrespondManager;				//协调管理
	CDataBaseEngineHelper			m_RecordDataBaseEngine;				//数据引擎
	CDataBaseEngineHelper			m_KernelDataBaseEngine;				//数据引擎

	//内核组件
public:
	CTimerEngineHelper				m_TimerEngine;						//时间引擎
	CAttemperEngineHelper			m_AttemperEngine;					//调度引擎
	CTCPNetworkEngineHelper			m_TCPNetworkEngine;					//socket::server
	CTCPSocketEngineHelper			m_TCPSocketEngine;					//socket::client -> 目标服务器 协调服
	CGameServiceManagerHelper		m_GameServiceManager;				//子游戏模块 加载

	//函数定义
public:
	//构造函数
	CServiceUnits();
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
	//创建模块
	int CreateServiceDLL();
	//配置组件
	int InitializeService();
	//启动内核
	int StartKernelService();

};

extern   CServiceUnits *			g_pServiceUnits;					//对象指针

#endif