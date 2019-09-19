#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE


#include "Stdafx.h"
#include "AttemperEngineSink.h"


//����Ԫ
class CServiceUnits
{
	//�������
protected:
	CAttemperEngineSink				m_AttemperEngineSink;				//���ȹ���

	//�ں����
protected:
	CAttemperEngineHelper			m_AttemperEngine;					//��������
	CTCPNetworkEngineHelper			m_TCPNetworkEngine;					//��������


	//��������
public:
	//���캯��
	CServiceUnits() {} 
	//��������
	virtual ~CServiceUnits() {}

	//�������
public:
	//��������
	bool StartService();
	//ֹͣ����
	bool ConcludeService();

	//��������
protected:
	//�������
	bool InitializeService();
	//�����ں�
	bool StartKernelService();
};

#endif