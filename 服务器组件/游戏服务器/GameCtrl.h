#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

#include <string>
#include "Stdafx.h"
#include "AttemperEngineSink.h"
#include "DataBaseEngineSink.h"

//////////////////////////////////////////////////////////////////////////////////

//网络标示
#define NETWORK_CORRESPOND			1									//协调连接


//服务单元
class CGameCtrl
{
	//服务对象
public:
	ITimerEngine				    *m_TimerEngine;						//时间引擎
	IAttemperEngine					*m_AttemperEngine;					//调度引擎
	ITCPNetworkEngine				*m_TCPNetworkEngine;				//socket::server
	ITCPSocketEngine				*m_TCPSocketEngine;					//socket::client -> 目标服务器 协调服
	//ITableFrameSink					*m_TableFrameSink;					//Sub桌子对象

	//回调对象
public:
	CAttemperEngineSink				m_AttemperEngineSink;				//调度钩子
	CDataBaseEngineSink				m_DataBaseEngineSink;	  		    //数据钩子

	//subgame_dll_name
protected:
	std::string			            m_subgame_dll_name;				    //子游戏dll名字
	
	DWORD                           m_ServerID;                         //serverid

	//函数定义
public:
	//构造函数
	CGameCtrl(std::string dll_name);
	//析构函数
	virtual ~CGameCtrl();

	//服务控制
public:
	//启动服务
	bool StartService();
	//停止服务
	bool ConcludeService();

	//启动socket::server
	bool StartNetworkService();

	//配置端口
	int SetNetworkPort(WORD Port);

	//获取ITableFrameSink
	ITableFrameSink* GetITableFrameSink();

	//获取ServerID
	DWORD GetServerID(){return m_ServerID; }
	void SetServerID(DWORD serverid){m_ServerID = serverid;}
	//获取KindID
	DWORD GetKindID(){return (m_ServerID & 0xFFFF0000) >> 16;}

	//辅助函数
protected:
	//配置组件
	int InitializeService();
	//启动内核
	int StartKernelService();

	/*********************** 消息发送 Socket::Server -> Socket::Client ***********************/
public:
	//发送数据
	virtual bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//发送数据
	virtual bool SendData(CPlayer * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//消息接口 2
public:
	//房间消息
	virtual bool SendRoomMessage(LPCTSTR lpszMessage, WORD wType);
	//游戏消息
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType);
	//房间消息
	virtual bool SendRoomMessage(CPlayer * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//游戏消息
	virtual bool SendGameMessage(CPlayer * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);

	//房间消息
	virtual bool SendRoomMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType, bool bAndroid);
	//发送到大厅和所有游戏 用前要慎重考虑( 将会发送给所有与登陆服相连的玩家. 还会发送给所有和游戏服相连的玩家)
	virtual bool SendMessageLobbyAndAllGame(LPCTSTR lpszMessage, WORD wType,WORD MsgID);


	/***********************           消息发送  -> DB                ***********************/
public:
	bool PostDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);


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


#endif