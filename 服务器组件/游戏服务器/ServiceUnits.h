#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AttemperEngineSink.h"
#include "DataBaseEngineSink.h"
#include "DBCorrespondManager.h"

//////////////////////////////////////////////////////////////////////////////////

//�����ʾ
#define NETWORK_CORRESPOND			1									//Э������

//��Ϣ����
#define WM_UICONTROL_REQUEST		(WM_USER+100)						//��������


//����Ԫ
class CServiceUnits : public CWnd
{
	//��Ԫ����
	friend class CAttemperEngineSink;
	friend class CDataBaseEngineSink;


	//�������
protected:
	tagGameParameter				m_GameParameter;					//���ò���
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
	CTCPNetworkEngineHelper			m_TCPNetworkEngine;					//��������
	CTCPSocketServiceHelper			m_TCPSocketService;					//�������
	CGameServiceManagerHelper		m_GameServiceManager;				//��Ϸģ��
	CGameMatchServiceManagerHelper	m_GameMatchServiceManager;			//��������

	//��̬����
public:
	static CServiceUnits *			g_pServiceUnits;					//����ָ��

	//��������
public:
	//���캯��
	CServiceUnits();
	//��������
	virtual ~CServiceUnits();


	//�������
public:
	//Ͷ������
	bool PostControlRequest(WORD wIdentifier, VOID * pData, WORD wDataSize);

	//�������
public:
	//��������
	bool StartService();
	//ֹͣ����
	bool ConcludeService();
	//��Ϸ����
	bool CollocateService(LPCTSTR pszGameModule, tagGameServiceOption & GameServiceOption);

	//��������
protected:
	//����ģ��
	bool CreateServiceDLL();
	//�������
	bool InitializeService();
	//�����ں�
	bool StartKernelService();
	//��������
	bool StartNetworkService();
	//��������
	bool RectifyServiceParameter();

	//��Ϣӳ��
protected:
	//������Ϣ
	LRESULT OnUIControlRequest(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif