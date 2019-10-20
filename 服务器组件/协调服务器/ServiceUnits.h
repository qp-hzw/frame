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
	IAttemperEngine			       *m_AttemperEngine;					//��������
    ITCPNetworkEngine			   *m_TCPNetworkEngine;					//��������


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
	int InitializeService();
	//�����ں�
	int StartKernelService();
};
           
extern IAttemperEngine			   *g_AttemperEngine;					//��������
extern ITCPNetworkEngine		   *g_TCPNetworkEngine;				    //socket::server

#endif