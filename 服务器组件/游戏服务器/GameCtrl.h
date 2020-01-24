#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

#include <string>
#include "Stdafx.h"
#include "AttemperEngineSink.h"
#include "DataBaseEngineSink.h"
#include <map>

//////////////////////////////////////////////////////////////////////////////////

//�����ʾ
#define NETWORK_CORRESPOND			1									//Э������

//����Ԫ
class CGameCtrl
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

	//subgame_dll_name
protected:
	std::string			            m_subgame_dll_name;				    //����Ϸdll����
	DWORD                           m_ServerID;                         //serverid

	rule_arry                       m_rule_arry;						//�������
	BYTE							m_frame_rule_count;					//frame������� ����
	BYTE							m_subgame_rule_count;				//����Ϸ������� ����

	//��������
public:
	//���캯��
	CGameCtrl(std::string dll_name);
	//��������
	virtual ~CGameCtrl();

	//�������
public:
	//��������
	bool StartService();
	//ֹͣ����
	bool ConcludeService();

	//����socket::server
	bool StartNetworkService();

	//���ö˿�
	int SetNetworkPort(WORD Port);

	//��ȡITableFrameSink
	ITableFrameSink* GetITableFrameSink();

	//��ȡServerID
	DWORD GetServerID(){return m_ServerID; }
	void SetServerID(DWORD serverid){m_ServerID = serverid;}
	//��ȡKindID
	DWORD GetKindID(){return (m_ServerID & 0xFFFF0000) >> 16;}

	//��ȡ������������ļ�
	rule_arry GetRoomRule(){return m_rule_arry;}

	//��������
protected:
	//�������
	int InitializeService();
	//�����ں�
	int StartKernelService();

	//��ȡͨ�÷��������ļ�
	void ReadFrameRoomRule();
	//��ȡ����Ϸ���������ļ�
	void ReadSubGameRoomRule(int kindid);

	/*********************** ��Ϣ���� Socket::Server -> Socket::Client ***********************/
public:
	//��������
	virtual bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendData(CPlayer * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//��Ϣ�ӿ� 2
public:
	//������Ϣ
	virtual bool SendRoomMessage(LPCTSTR lpszMessage, WORD wType);
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType);
	//������Ϣ
	virtual bool SendRoomMessage(CPlayer * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(CPlayer * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);

	//������Ϣ
	virtual bool SendRoomMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType, bool bAndroid);
	//���͵�������������Ϸ ��ǰҪ���ؿ���( ���ᷢ�͸��������½�����������. ���ᷢ�͸����к���Ϸ�����������)
	virtual bool SendMessageLobbyAndAllGame(LPCTSTR lpszMessage, WORD wType,WORD MsgID);


	/***********************           ��Ϣ����  -> DB                ***********************/
public:
	bool PostDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);


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


#endif