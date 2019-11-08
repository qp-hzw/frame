#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#include "Stdafx.h"
#include "TableFrame.h"
#include "Player.h"
#include "PlayerManager.h"
#include "DataBasePacket.h"

//�󶨲���
struct tagBindParameter
{
	//��������
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	DWORD							dwActiveTime;						//����ʱ��
	//�û�����
	CPlayer *				        pCPlayer;					        //�û��ӿ�
};

//ϵͳ��Ϣ
struct tagSystemMessage
{
	DWORD							dwLastTime;						   //����ʱ��
	DBR_GR_SystemMessage            SystemMessage;                     //ϵͳ��Ϣ
};


//////////////////////////////////////////////////////////////////////////////////

//���ȹ���
class CAttemperEngineSink : public IAttemperEngineSink
{
#pragma region ��ʼ������
	//��������
public:
	//���캯��
	CAttemperEngineSink();
	//��������
	~CAttemperEngineSink(){}

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
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx){return true;}

public:
	//�����¼�
	virtual bool OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize);

	//��ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);
	//���ݿ��¼�
	virtual bool OnEventDataBaseResult(WORD wRequestID, DWORD dwScoketID, VOID * pData, WORD wDataSize);

	//socket::client
public:
	//�����¼�
	virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//socket::server
public:
	//Ӧ���¼�
	virtual bool OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID);
	//�ر��¼�
	virtual bool OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID);
	//��ȡ�¼�
	virtual bool OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

#pragma endregion

};

extern CAttemperEngineSink * g_AttemperEngineSink;

//////////////////////////////////////////////////////////////////////////////////





#endif