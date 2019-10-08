#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

#include "Stdafx.h"
#include "AttemperEngineSink.h"
#include "DataBaseEngineSink.h"
#include "DBCorrespondManager.h"

//////////////////////////////////////////////////////////////////////////////////

//�����ʾ
#define NETWORK_CORRESPOND			1									//Э������



//����Ԫ
class CServiceUnits : public CWnd
{
	//�������
protected:
	tagGameServiceAttrib			m_GameServiceAttrib;				//��������
	tagGameServiceOption			m_GameServiceOption;				//��������

	//�������
private:
	CWHDataQueue					m_DataQueue;						//���ݶ���
	CCriticalSection				m_CriticalSection;					//ͬ������

	//�������
public:
	CAttemperEngineSink				m_AttemperEngineSink;				//���ȹ���
	CDataBaseEngineSink				m_RecordDataBaseSink[4];			//���ݹ���
	CDataBaseEngineSink				m_KernelDataBaseSink[4];			//���ݹ���
	
	//�������
public:
	CDBCorrespondManager            m_DBCorrespondManager;				//Э������
	CDataBaseEngineHelper			m_RecordDataBaseEngine;				//��������
	CDataBaseEngineHelper			m_KernelDataBaseEngine;				//��������

	//�ں����
public:
	CTimerEngineHelper				m_TimerEngine;						//ʱ������
	CAttemperEngineHelper			m_AttemperEngine;					//��������
	CTCPNetworkEngineHelper			m_TCPNetworkEngine;					//socket::server
	CTCPSocketEngineHelper			m_TCPSocketEngine;					//socket::client -> Ŀ������� Э����
	CGameServiceManagerHelper		m_GameServiceManager;				//����Ϸģ�� ����

	//��������
public:
	//���캯��
	CServiceUnits();
	//��������
	virtual ~CServiceUnits();


	//�������
public:
	//��������
	bool StartService();
	//ֹͣ����
	bool ConcludeService();

public:
	//����socket::server
	bool StartNetworkService();

	//��������
protected:
	//����ģ��
	int CreateServiceDLL();
	//�������
	int InitializeService();
	//�����ں�
	int StartKernelService();

};

extern   CServiceUnits *			g_pServiceUnits;					//����ָ��

#endif