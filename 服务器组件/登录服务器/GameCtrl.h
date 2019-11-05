#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

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
class CGameCtrl
{
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
	CGameCtrl();
	//��������
	virtual ~CGameCtrl();

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


	/*********************** ��Ϣ���� Socket::Server -> Socket::Client ***********************/
public:
	//���ͺ���
	bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID);
	//���ͺ���
	bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	bool SendDataBatch(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);


	/***********************           ��Ϣ����  -> DB                ***********************/
public:
	bool PostDataBaseRequest(WORD wRequestID, DWORD dwScoketID, VOID * pData, WORD wDataSize);


	/***********************            Timer                         ***********************/
public:
	//���ö�ʱ��
	bool SetTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter); 
	//ɾ����ʱ��
	bool KillTimer(DWORD dwTimerID);
	//ɾ����ʱ��
	bool KillAllTimer();
	//��ȡ��ʱ��ʣ��ʱ�䣨���룩
	DWORD GetTimerLeftTickCount(DWORD dwTimerID);
};

extern CGameCtrl                   *g_GameCtrl;                     
extern ITCPNetworkEngine		   *g_TCPNetworkEngine;				    //socket::server
extern ITCPSocketEngine			   *g_TCPSocketEngine;					//socker::client -- Ŀ�������Ϊ Э����

 
//////////////////////////////////////////////////////////////////////////////////

#endif