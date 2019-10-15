#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AttemperEngineSink.h"
#include "DataBaseEngineSink.h"
//////////////////////////////////////////////////////////////////////////////////

//网络标示
#define NETWORK_CORRESPOND			1									//登录连接

//消息定义
#define WM_UICONTROL_REQUEST		(WM_USER+100)						//控制请求


//////////////////////////////////////////////////////////////////////////////////

//服务单元
class CServiceUnits : public CWnd
{
	//组件变量
private:
	CWHDataQueue					m_DataQueue;						//数据队列
	CCriticalSection				m_CriticalSection;					//同步对象

	//服务组件
protected:
	CAttemperEngineSink				m_AttemperEngineSink;				//Attemper钩子
	CDataBaseEngineSink				m_DataBaseEngineSink;			    //DB钩子

	//内核组件
public:
	IAttemperEngine			       *m_AttemperEngine;					//调度引擎
	ITCPNetworkEngine			   *m_TCPNetworkEngine;				    //socket::server
	ITCPSocketEngine			   *m_TCPSocketEngine;					//socker::client -- 目标服务器为 协调服
	ITimerEngine				   *m_TimerEngine;						//定时器

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
	int StartNetworkService();

	//辅助函数
protected:
	//配置组件
	int InitializeService();
	//启动内核
	int StartKernelService();
};

extern CServiceUnits *			g_pServiceUnits;					//对象指针

//////////////////////////////////////////////////////////////////////////////////

#endif