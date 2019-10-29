#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

#include <string>
#include "Stdafx.h"
#include "AttemperEngineSink.h"
#include "DataBaseEngineSink.h"

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

	//��ȡITableFrameSink
	ITableFrameSink* GetITableFrameSink();

	//��������
protected:
	//�������
	int InitializeService();
	//�����ں�
	int StartKernelService();


	//��Ϣ�ӿ� 1
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
};

extern CGameCtrl                   *g_GameCtrl;                     
extern IAttemperEngine			   *g_AttemperEngine;					//��������
extern ITCPNetworkEngine		   *g_TCPNetworkEngine;				    //socket::server
extern ITCPSocketEngine			   *g_TCPSocketEngine;					//socker::client -- Ŀ�������Ϊ Э����
extern ITimerEngine				   *g_TimerEngine;						//��ʱ��


#endif