#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AttemperEngineSink.h"
#include "DataBaseEngineSink.h"
//////////////////////////////////////////////////////////////////////////////////

//�����ʾ
#define NETWORK_CORRESPOND			1									//��¼����

//��Ϣ����
#define WM_UICONTROL_REQUEST		(WM_USER+100)						//��������


//////////////////////////////////////////////////////////////////////////////////

//����Ԫ
class CServiceUnits : public CWnd
{
	//��Ԫ����
	friend class CAttemperEngineSink;
	friend class CDataBaseEngineSink;

	//�������
private:
	CWHDataQueue					m_DataQueue;						//���ݶ���
	CCriticalSection				m_CriticalSection;					//ͬ������

	//�������
protected:
	CAttemperEngineSink				m_AttemperEngineSink;				//���ȹ���
	CDataBaseEngineSink				m_DataBaseEngineSink[4];			//���ݹ���

	//�ں����
protected:
	CTimerEngineHelper				m_TimerEngine;						//ʱ������
	CAttemperEngineHelper			m_AttemperEngine;					//��������
	CDataBaseEngineHelper			m_DataBaseEngine;					//��������
	CTCPNetworkEngineHelper			m_TCPNetworkEngine;					//��������
	CTCPSocketEngineHelper			m_TCPSocketEngine;					//Э������ -- Ŀ�������Ϊ Э����

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
	//Ͷ������
	bool PostControlRequest(WORD wIdentifier, VOID * pData, WORD wDataSize);

	//��������
protected:
	//�������
	int InitializeService();
	//�����ں�
	int StartKernelService();
	//�������� -- Э���� ���سɹ��Ż�����
	int StartNetworkService();

	//��Ϣӳ��
protected:
	//������Ϣ
	LRESULT OnUIControlRequest(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

extern CServiceUnits *			g_pServiceUnits;					//����ָ��

//////////////////////////////////////////////////////////////////////////////////

#endif