#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GlobalInfoManager.h"

#pragma region �����ṹ��
//��������
enum enServiceKind
{
	ServiceKind_None,				//��Ч����
	ServiceKind_Game,				//��Ϸ��
	ServiceKind_Logon,				//��¼��
};

//�󶨲���
struct tagBindParameter
{
	//��������
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	DWORD							dwActiveTime;						//����ʱ��

	//��������
	DWORD							dwServiceID;						//ServerID
	enServiceKind					ServiceKind;						//��������
};

//���鶨��
typedef CWHArray<WORD> CWordArrayTemplate;
#pragma endregion


//���ȹ���
class CAttemperEngineSink : public IAttemperEngineSink
{
	friend class CServiceUnits;

#pragma region ��Ա����
	//״̬����
protected:
	WORD							m_wCollectItem;						//��������
	CWordArrayTemplate				m_WaitCollectItemArray;				//���ܵȴ�

	//��������
protected:
    tagBindParameter *				m_pBindParameter;					//��������

	//�������
protected:
	CGlobalInfoManager				m_GlobalInfoManager;				//ȫ�ֹ���

	//����ӿ�
protected:
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//��������
#pragma endregion

#pragma region ��ʼ������
	//��������
public:
	//���캯��
	CAttemperEngineSink();
	//��������
	virtual ~CAttemperEngineSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);
#pragma endregion

#pragma region �¼��ӿ�
	//�첽�ӿ�
public:
	//�����¼�
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx);

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
	virtual bool OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID);
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

#pragma region Socketϵͳ����
	//����Socket�¼� ��ȡ�¼� ��������
protected:
	//ע��ģ��
	bool OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�б�����
	bool OnTCPNetworkMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ת����
	bool OnTCPNetworkMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û�����
	bool OnTCPNetworkMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��̨����
	bool OnTCPNetworkMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
#pragma endregion

#pragma region ���ͺ���
	//��������
protected:
	//�����б�
	bool SendServerListItem(DWORD dwSocketID);

	//���ݷ���
protected:
	//���䷢��
	bool SendDataToGame(DWORD wServerID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�������� ����wSocketID���ж���Ⱥ�� ���Ƿ��͸��ض��Ĵ���
	bool SendDataToPlaza(WORD wSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�û����� TODONOW �ӿڴ�ʵ��, ����ͨ�����͸����е�client �� gameserverʵ��
	bool SendDataToUser(DWORD wServerID, DWORD dwUserID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//���͸���Ϸ�����з���
	bool SendDataToGameAllRoom(WORD wKindID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
#pragma endregion

};

//////////////////////////////////////////////////////////////////////////////////

#endif