#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#include "Stdafx.h"

//���ȹ���
class CAttemperEngineSink : public IAttemperEngineSink
{
#pragma region ��ʼ������
	//��������
public:
	//���캯��
	CAttemperEngineSink(){}
	//��������
	virtual ~CAttemperEngineSink(){}

	//����ӿ�
public:
	//��������
	virtual bool StartService(){return true;}
	//ֹͣ����
	virtual bool ConcludeService(){return true;}
#pragma endregion

#pragma region �ص��ӿ�
public:
	//�����¼�
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx){return true;}
	//ֹͣ�¼�
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx){return true;}

	//�¼��ӿ�
public:
	//�����¼�
	virtual bool OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize){return false;}

	//socket::client
public:
	//�����¼�
    virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode){return false;}
	//�ر��¼�
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason){return false;}
	//��ȡ�¼�
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize){return true;}

	//socket::server
public:
	//Ӧ���¼�
	virtual bool OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID) {return true;}
	//�ر��¼�
	virtual bool OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID);
	//��ȡ�¼�
	virtual bool OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//
public:
	//��ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam){return false;}
	//���ݿ��¼�
	virtual bool OnEventDataBaseResult(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize){return false;}
#pragma endregion

#pragma region ��������
public:
	//����ע��
	virtual bool DoServerRegister(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);
#pragma endregion
};

//////////////////////////////////////////////////////////////////////////////////

#endif