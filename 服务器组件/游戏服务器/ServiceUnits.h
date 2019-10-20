#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

#include "Stdafx.h"
#include "AttemperEngineSink.h"
#include "DataBaseEngineSink.h"

//////////////////////////////////////////////////////////////////////////////////

//�����ʾ
#define NETWORK_CORRESPOND			1									//Э������



//����Ԫ
class CServiceUnits : public CWnd
{
	//�������
public:
	ITimerEngine				    *m_TimerEngine;						//ʱ������
	IAttemperEngine					*m_AttemperEngine;					//��������
	ITCPNetworkEngine				*m_TCPNetworkEngine;				//socket::server
	ITCPSocketEngine				*m_TCPSocketEngine;					//socket::client -> Ŀ������� Э����

	//�ص�����
public:
	CAttemperEngineSink				m_AttemperEngineSink;				//���ȹ���
	CDataBaseEngineSink				m_DataBaseEngineSink;	  		    //���ݹ���

	//�������
protected:
	tagGameServiceAttrib			m_GameServiceAttrib;				//��������
	tagGameServiceOption			m_GameServiceOption;				//��������

	//subgame_dll_name
protected:
	std::string			            m_subgame_dll_name;				    //����Ϸdll����
	

	//��������
public:
	//���캯��
	CServiceUnits(std::string dll_name);
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
	//�������
	int InitializeService();
	//�����ں�
	int StartKernelService();

};

extern CServiceUnits               *g_pServiceUnits;                     
extern IAttemperEngine			   *g_AttemperEngine;					//��������
extern ITCPNetworkEngine		   *g_TCPNetworkEngine;				    //socket::server
extern ITCPSocketEngine			   *g_TCPSocketEngine;					//socker::client -- Ŀ�������Ϊ Э����
extern ITimerEngine				   *g_TimerEngine;						//��ʱ��


#endif