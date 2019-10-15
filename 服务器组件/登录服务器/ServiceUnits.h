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
	//�������
private:
	CWHDataQueue					m_DataQueue;						//���ݶ���
	CCriticalSection				m_CriticalSection;					//ͬ������

	//�������
protected:
	CAttemperEngineSink				m_AttemperEngineSink;				//Attemper����
	CDataBaseEngineSink				m_DataBaseEngineSink;			    //DB����

	//�ں����
public:
	IAttemperEngine			       *m_AttemperEngine;					//��������
	ITCPNetworkEngine			   *m_TCPNetworkEngine;				    //socket::server
	ITCPSocketEngine			   *m_TCPSocketEngine;					//socker::client -- Ŀ�������Ϊ Э����
	ITimerEngine				   *m_TimerEngine;						//��ʱ��

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
	int StartNetworkService();

	//��������
protected:
	//�������
	int InitializeService();
	//�����ں�
	int StartKernelService();
};

extern CServiceUnits *			g_pServiceUnits;					//����ָ��

//////////////////////////////////////////////////////////////////////////////////

#endif