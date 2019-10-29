#include <string>
#include <iostream>
#include "StdAfx.h"
#include "ServiceUnits.h"


//ȫ�ֱ���
CServiceUnits              *g_pServiceUnits = NULL; 
IAttemperEngine			   *g_AttemperEngine = NULL;					//��������
ITCPNetworkEngine		   *g_TCPNetworkEngine = NULL;				    //socket::server
ITCPSocketEngine		   *g_TCPSocketEngine = NULL;					//socker::client -- Ŀ�������Ϊ Э����
ITimerEngine			   *g_TimerEngine = NULL;						//��ʱ��


//���캯��
CServiceUnits::CServiceUnits(std::string dll_name)
{
	//���ö���
	if (g_pServiceUnits==NULL) g_pServiceUnits=this;

	m_AttemperEngine = NULL;
	m_TCPNetworkEngine = NULL;
	m_TCPSocketEngine = NULL;
	m_TimerEngine = NULL;

	m_subgame_dll_name = dll_name;

	//�������
	ZeroMemory(&m_GameServiceAttrib,sizeof(m_GameServiceAttrib));
	ZeroMemory(&m_GameServiceOption,sizeof(m_GameServiceOption));
}

//��������
CServiceUnits::~CServiceUnits()
{
	ConcludeService();
}

//����ģ��
int CServiceUnits::InitializeService()
{
	/***************************************************  �������� *************************************************/
	m_AttemperEngine = static_cast<IAttemperEngine*>(CWHModule::AttemperEngine());
	m_TCPNetworkEngine = static_cast<ITCPNetworkEngine*>(CWHModule::TCPNetworkEngine());
	m_TCPSocketEngine = static_cast<ITCPSocketEngine*>(CWHModule::TCPSocketEngine());
	m_TimerEngine = static_cast<ITimerEngine*>(CWHModule::TimerEngine());

	ITableFrameSink *SubGameDll = static_cast<ITableFrameSink*>(CWHModule::SubGame(m_subgame_dll_name)); //�˴�ֻ��У��

	if(m_AttemperEngine == NULL) return 1;
	if(m_TCPNetworkEngine == NULL) return 2;
	if(m_TCPSocketEngine == NULL) return 3;
	if(m_TimerEngine == NULL) return 4;
	if(SubGameDll == NULL) return 5;

	g_AttemperEngine = m_AttemperEngine;
	g_TCPNetworkEngine = m_TCPNetworkEngine;
	g_TCPSocketEngine = m_TCPSocketEngine;
	g_TimerEngine = m_TimerEngine;

	//�ص�����
	IUnknownEx * pIAttemperEngineSink=static_cast<IAttemperEngineSink*>(&m_AttemperEngineSink);
	IUnknownEx * pIDataBaseEngineSink=static_cast<IUnknownEx*>(&m_DataBaseEngineSink);


	/***************************************************  AttemperEngine ������Ϣ *************************************************/
	//AttemperEngine���� Attemper����
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return 6;

	//AttemperEngine���� DB����
	if (m_AttemperEngine->SetDataBaseEngineSink(pIDataBaseEngineSink)==false) return 7;

	/***************************************************  socket::client ������Ϣ *************************************************/
	//Э������
	if (m_TCPSocketEngine->SetServiceID(NETWORK_CORRESPOND)==false) return 11;

	/***************************************************  socket::server ������Ϣ *************************************************/
	//��������
	if(m_TCPNetworkEngine->SetServiceParameter(m_GameServiceOption.wGameServerPort)==false) return 12;

	/***************************************************  log ������Ϣ *************************************************/
	//���ǵ���Ϸ�������ڲ���֪��ServerID, ���ֻ�ܽ�log��������Ϣд������
	//std::string log_nam= "GameServer-" + std::to_string(m_GameServiceOption.dwServerID) + ".log";

	std::string log_nam= "GameServer.log";
	CLog::Init(log_nam.c_str());

	CLog::Log(log_debug, "service create success");
	return 0;
}

//��������
bool CServiceUnits::StartService()
{
	//���÷���
	int iRet = InitializeService();
	if (iRet != 0)
	{
		CLog::Log(log_error, "CServiceUnits::InitializeService %d", iRet);
		ConcludeService();
		return false;
	}

	//�����ں�
	iRet = StartKernelService();
	if (iRet != 0)
	{
		CLog::Log(log_error, "CServiceUnits::InitializeService %d", iRet);
		ConcludeService();
		return false;
	}

	//����Э��
	if (m_TCPSocketEngine !=NULL)
	{
		m_TCPSocketEngine->Connect(_CPD_SERVER_ADDR, PORT_CENTER);
	}

	return true;
}

//ֹͣ����
bool CServiceUnits::ConcludeService()
{
	//�������
	if (m_TimerEngine !=NULL) m_TimerEngine->ConcludeService();
	if (m_AttemperEngine!=NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPSocketEngine!=NULL) m_TCPSocketEngine->ConcludeService();
	if (m_TCPNetworkEngine!=NULL) m_TCPNetworkEngine->ConcludeService();

	return true;
}

//�����ں�
int CServiceUnits::StartKernelService()
{
	//��������
	if (m_AttemperEngine->StartService()==false)
	{
		return 1;
	}

	//ʱ������
	if (m_TimerEngine->StartService()==false)
	{
		return 2;
	}

	//Э������
	if (m_TCPSocketEngine->StartService()==false)
	{
		return 3;
	}

	return 0;
}

//��������
bool CServiceUnits::StartNetworkService()
{
	//��������
	if (m_TCPNetworkEngine->StartService()==false)
	{
		return false;
	}

	return true;
}
