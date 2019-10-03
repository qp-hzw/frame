#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"

//////////////////////////////////////////////////////////////////////////////////

//静态变量
CServiceUnits *			CServiceUnits::g_pServiceUnits=NULL;			//对象指针

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CServiceUnits, CWnd)
	ON_MESSAGE(WM_UICONTROL_REQUEST,OnUIControlRequest)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CServiceUnits::CServiceUnits()
{
	//设置对象
	if (g_pServiceUnits==NULL) g_pServiceUnits=this;

	return;
}

//析构函数
CServiceUnits::~CServiceUnits()
{
	ConcludeService();
}

//配置组件
int CServiceUnits::InitializeService()
{
	//设置服务器日志输出等级
	bool bRet = CLog::Init("logon.log");

	/***************************************************  各服务关联配置 *************************************************/
	//创建组件
	if ((m_TimerEngine.GetInterface()==NULL)&&(m_TimerEngine.CreateInstance()==false)) return 1;
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false)) return 2;
	if ((m_DataBaseEngine.GetInterface()==NULL)&&(m_DataBaseEngine.CreateInstance()==false)) return 3;
	if ((m_TCPNetworkEngine.GetInterface()==NULL)&&(m_TCPNetworkEngine.CreateInstance()==false)) return 4;
	if ((m_TCPSocketEngine.GetInterface()==NULL)&&(m_TCPSocketEngine.CreateInstance()==false)) return 5;

	//组件接口
	IUnknownEx * pIAttemperEngine=m_AttemperEngine.GetInterface();
	IUnknownEx * pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	IUnknownEx * pIAttemperEngineSink=QUERY_OBJECT_INTERFACE(m_AttemperEngineSink,IUnknownEx);

	//设置AttemperEngine回调
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return 6;

	//各服务设置回调为AttemperEngine
	if (m_TimerEngine->SetTimerEngineSink(pIAttemperEngine)==false) return 7;
	if (m_TCPNetworkEngine->SetTCPNetworkEngineSink(pIAttemperEngine)==false) return 8;
	if (m_TCPSocketEngine->SetTCPSocketEngineSink(pIAttemperEngine)==false) return 9;


	//数据库回调
	IUnknownEx * pIDataBaseEngineSink[CountArray(m_DataBaseEngineSink)];
	for (WORD i=0;i<CountArray(pIDataBaseEngineSink);i++) pIDataBaseEngineSink[i]=QUERY_OBJECT_INTERFACE(m_DataBaseEngineSink[i],IUnknownEx);
	if (m_DataBaseEngine->SetDataBaseEngineSink(pIDataBaseEngineSink,CountArray(pIDataBaseEngineSink))==false) return 10;

	//AttemperEngine
	m_AttemperEngineSink.m_pITimerEngine=m_TimerEngine.GetInterface();
	m_AttemperEngineSink.m_pIDataBaseEngine=m_DataBaseEngine.GetInterface();
	m_AttemperEngineSink.m_pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	m_AttemperEngineSink.m_pITCPSocketEngine=m_TCPSocketEngine.GetInterface();

	/***************************************************  服务配置信息 *************************************************/
	//协调服务（协调服务器）
	if (m_TCPSocketEngine->SetServiceID(NETWORK_CORRESPOND)==false) return 11;

	//配置网络
	WORD wMaxConnect=MAX_CONTENT;
	WORD wServicePort=PORT_LOGON;
	if (m_TCPNetworkEngine->SetServiceParameter(wServicePort,wMaxConnect)==false) return 12;

	return 0;
}

//启动服务
bool CServiceUnits::StartService()
{
	//配置服务
	int iRet = InitializeService();

	if (iRet != 0)
	{
		CLog::Log(log_error, "CServiceUnits::InitializeService %d", iRet);
		ConcludeService();
		return false;
	}
	
	//启动内核
	iRet = StartKernelService();
	if (iRet != 0)
	{
		CLog::Log(log_error, "CServiceUnits::StartKernelService  %d", iRet);
		ConcludeService();
		return false;
	}

	//连接协调
	m_AttemperEngine->OnEventControl(CT_CONNECT_CORRESPOND,NULL,0);

	//获取列表
	//m_AttemperEngine->OnEventControl(CT_LOAD_DB_GAME_LIST,NULL,0);

	return true;
}

//停止服务
bool CServiceUnits::ConcludeService()
{
	//停止服务
	if (m_TimerEngine.GetInterface()!=NULL) m_TimerEngine->ConcludeService();
	if (m_AttemperEngine.GetInterface()!=NULL) m_AttemperEngine->ConcludeService();
	if (m_DataBaseEngine.GetInterface()!=NULL) m_DataBaseEngine->ConcludeService();
	if (m_TCPNetworkEngine.GetInterface()!=NULL) m_TCPNetworkEngine->ConcludeService();
	if (m_TCPSocketEngine.GetInterface()!=NULL) m_TCPSocketEngine->ConcludeService();
	
	return true;
}

//启动内核
int CServiceUnits::StartKernelService()
{
	//时间引擎
	if (m_TimerEngine->StartService()==false)
	{
		return 1;
	}
	
	//调度引擎
	if (m_AttemperEngine->StartService()==false)
	{
		return 2;
	}

	//协调引擎
	if (m_TCPSocketEngine->StartService()==false)
	{
		return 3;
	}
	
	//数据引擎
	if (m_DataBaseEngine->StartService()==false)
	{
		return 4;
	}

	return 0;
}

//启动网络
int CServiceUnits::StartNetworkService()
{
	//网络引擎
	if (m_TCPNetworkEngine->StartService()==false)
	{
		return 1;
	}

	return 0;
}

//投递请求
bool CServiceUnits::PostControlRequest(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	//状态判断
	if (IsWindow(m_hWnd)==FALSE) return false;

	//插入队列
	CWHDataLocker DataLocker(m_CriticalSection);
	if (m_DataQueue.InsertData(wIdentifier,pData,wDataSize)==false) return false;

	//发送消息
	PostMessage(WM_UICONTROL_REQUEST,wIdentifier,wDataSize);

	return true;
}


//控制消息
LRESULT CServiceUnits::OnUIControlRequest(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	tagDataHead DataHead;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];

	//提取数据
	CWHDataLocker DataLocker(m_CriticalSection);
	if (m_DataQueue.DistillData(DataHead,cbBuffer,sizeof(cbBuffer))==false)
	{
		return NULL;
	}

	//数据处理
	switch (DataHead.wIdentifier)
	{
	case UI_LOAD_DB_LIST_RESULT:	//列表结果
		{
			//效验消息
			if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

			//变量定义
			CP_ControlResult * pControlResult=(CP_ControlResult *)cbBuffer;

			//失败处理
			if (pControlResult->cbSuccess==ER_FAILURE)
			{
				ConcludeService();
				return 0;
			}

			return 0;
		}
	case UI_CORRESPOND_RESULT:		//协调结果
		{
			//效验消息
			if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

			//变量定义
			CP_ControlResult * pControlResult=(CP_ControlResult *)cbBuffer;
			//失败处理
			if (pControlResult->cbSuccess==ER_FAILURE)
			{
				ConcludeService();
				return 0;
			}

			//成功处理
			if (pControlResult->cbSuccess==ER_SUCCESS)
			{
				//启动网络
				if (StartNetworkService()==false)
				{
					ConcludeService();
					return 0;
				}
			}

			return 0;
		}
	}

	return 0;
}
