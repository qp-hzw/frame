#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

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
class CGameCtrl
{
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
	CGameCtrl();
	//析构函数
	virtual ~CGameCtrl();

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


	/*********************** 消息发送 Socket::Server -> Socket::Client ***********************/
public:
	//发送函数
	bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID);
	//发送函数
	bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//批量发送
	bool SendDataBatch(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);


	/***********************           消息发送  -> DB                ***********************/
public:
	bool PostDataBaseRequest(WORD wRequestID, DWORD dwScoketID, VOID * pData, WORD wDataSize);


	/***********************            Timer                         ***********************/
public:
	//设置定时器
	bool SetTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter); 
	//删除定时器
	bool KillTimer(DWORD dwTimerID);
	//删除定时器
	bool KillAllTimer();
	//获取定时器剩余时间（毫秒）
	DWORD GetTimerLeftTickCount(DWORD dwTimerID);
};

extern CGameCtrl                   *g_GameCtrl;                     
extern ITCPNetworkEngine		   *g_TCPNetworkEngine;				    //socket::server
extern ITCPSocketEngine			   *g_TCPSocketEngine;					//socker::client -- 目标服务器为 协调服

 
//////////////////////////////////////////////////////////////////////////////////

#endif