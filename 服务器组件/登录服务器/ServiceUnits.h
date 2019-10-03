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
	//友元定义
	friend class CAttemperEngineSink;
	friend class CDataBaseEngineSink;

	//组件变量
private:
	CWHDataQueue					m_DataQueue;						//数据队列
	CCriticalSection				m_CriticalSection;					//同步对象

	//服务组件
protected:
	CAttemperEngineSink				m_AttemperEngineSink;				//调度钩子
	CDataBaseEngineSink				m_DataBaseEngineSink[4];			//数据钩子

	//内核组件
protected:
	CTimerEngineHelper				m_TimerEngine;						//时间引擎
	CAttemperEngineHelper			m_AttemperEngine;					//调度引擎
	CDataBaseEngineHelper			m_DataBaseEngine;					//数据引擎
	CTCPNetworkEngineHelper			m_TCPNetworkEngine;					//网络引擎
	CTCPSocketEngineHelper			m_TCPSocketEngine;					//协调服务 -- 目标服务器为 协调服

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
	//投递请求
	bool PostControlRequest(WORD wIdentifier, VOID * pData, WORD wDataSize);

	//辅助函数
protected:
	//配置组件
	int InitializeService();
	//启动内核
	int StartKernelService();
	//启动网络 -- 协调服 返回成功才会启动
	int StartNetworkService();

	//消息映射
protected:
	//控制消息
	LRESULT OnUIControlRequest(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

extern CServiceUnits *			g_pServiceUnits;					//对象指针

//////////////////////////////////////////////////////////////////////////////////

#endif