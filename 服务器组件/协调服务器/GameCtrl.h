#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

#include "Stdafx.h"
#include "AttemperEngineSink.h"

//����Ԫ
class CGameCtrl
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
	CGameCtrl();
	//��������
	virtual ~CGameCtrl() {}

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

	//��Ϣ���ͺ���
public:
	//���ͺ���
	bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID);
	//���ͺ���
	bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	bool SendDataBatch(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
};
           
extern CGameCtrl                   *g_GameCtrl;                          
extern IAttemperEngine			   *g_AttemperEngine;					//��������
extern ITCPNetworkEngine		   *g_TCPNetworkEngine;				    //socket::server

#endif