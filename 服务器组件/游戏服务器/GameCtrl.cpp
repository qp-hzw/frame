#include "Stdafx.h"
#include "GameCtrl.h"
#include <iostream>

#define MDM_CM_SYSTEM				1000								//ϵͳ����
#define SUB_CM_SYSTEM_MESSAGE		1									//ϵͳ��Ϣ


//ϵͳ��Ϣ
struct CMD_CM_SystemMessage
{
	WORD							wType;								//��Ϣ����
	WORD							wLength;							//��Ϣ����
	TCHAR							szString[1024];						//��Ϣ����
};

//ȫ�ֱ���
CGameCtrl                  *g_GameCtrl = NULL; 
ITCPNetworkEngine		   *g_TCPNetworkEngine = NULL;				    //socket::server
ITCPSocketEngine		   *g_TCPSocketEngine = NULL;					//socker::client -- Ŀ�������Ϊ Э����


//���캯��
CGameCtrl::CGameCtrl(std::string dll_name)
{
	//���ö���
	if (g_GameCtrl==NULL) g_GameCtrl=this;

	m_AttemperEngine = NULL;
	m_TCPNetworkEngine = NULL;
	m_TCPSocketEngine = NULL;
	m_TimerEngine = NULL;

	m_subgame_dll_name = dll_name;
	m_ServerID = 0;
}

//��������
CGameCtrl::~CGameCtrl()
{
	ConcludeService();
}

//��������
bool CGameCtrl::StartService()
{
	//���÷���
	int iRet = InitializeService();
	if (iRet != 0)
	{
		CLog::Log(log_error, "CGameCtrl::InitializeService %d", iRet);
		ConcludeService();
		return false;
	}

	//�����ں�
	iRet = StartKernelService();
	if (iRet != 0)
	{
		CLog::Log(log_error, "CGameCtrl::InitializeService %d", iRet);
		ConcludeService();
		return false;
	}

	//����Э��
	if (m_TCPSocketEngine !=NULL)
	{
		m_TCPSocketEngine->Connect(_CPD_SERVER_ADDR, PORT_CENTER);
	}

	return true;
}
//ֹͣ����
bool CGameCtrl::ConcludeService()
{
	//�������
	if (m_TimerEngine !=NULL) m_TimerEngine->ConcludeService();
	if (m_AttemperEngine!=NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPSocketEngine!=NULL) m_TCPSocketEngine->ConcludeService();
	if (m_TCPNetworkEngine!=NULL) m_TCPNetworkEngine->ConcludeService();

	return true;
}

//����ģ��
int CGameCtrl::InitializeService()
{
	/***************************************************  �������� *************************************************/
	m_AttemperEngine = static_cast<IAttemperEngine*>(CWHModule::AttemperEngine());
	m_TCPNetworkEngine = static_cast<ITCPNetworkEngine*>(CWHModule::TCPNetworkEngine());
	m_TCPSocketEngine = static_cast<ITCPSocketEngine*>(CWHModule::TCPSocketEngine());
	m_TimerEngine = static_cast<ITimerEngine*>(CWHModule::TimerEngine());

	if(m_AttemperEngine == NULL) return 1;
	if(m_TCPNetworkEngine == NULL) return 2;
	if(m_TCPSocketEngine == NULL) return 3;
	if(m_TimerEngine == NULL) return 4;

	g_TCPNetworkEngine = m_TCPNetworkEngine;
	g_TCPSocketEngine = m_TCPSocketEngine;

	//�ص�����
	IUnknownEx * pIAttemperEngineSink=static_cast<IAttemperEngineSink*>(&m_AttemperEngineSink);
	IUnknownEx * pIDataBaseEngineSink=static_cast<IUnknownEx*>(&m_DataBaseEngineSink);

	/***************************************************  AttemperEngine ������Ϣ *************************************************/
	//AttemperEngine���� Attemper����
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return 6;

	//AttemperEngine���� DB����
	if (m_AttemperEngine->SetDataBaseEngineSink(pIDataBaseEngineSink)==false) return 7;

	/***************************************************  socket::client ������Ϣ *************************************************/
	//Э������
	if (m_TCPSocketEngine->SetServiceID(NETWORK_CORRESPOND)==false) return 11;

	/***************************************************  log ������Ϣ *************************************************/
	std::string log_nam= "GameServer.log";
	CLog::Init(log_nam.c_str());

	/***************************************************  SubGame ������Ϣ *************************************************/
	//��ȡ����Ϸ�����ļ�
	DWORD dwKindId = static_cast<DWORD >(CWHModule::SubGameCfg(m_subgame_dll_name)); //�˴�ֻ��У��
	if (0 == dwKindId)	return 8;
	SetServerID(dwKindId << 16);


	//��ȡ������������ļ�
	//ReadFrameRoomRule();
	//ReadSubGameRoomRule(dwKindId);
	memcpy(m_rule_arry.ItemArry[0].szHeadName, "����", 15);
	memcpy(m_rule_arry.ItemArry[0].szItemValue[0], "1", 10);
	memcpy(m_rule_arry.ItemArry[0].szItemValue[1], "2", 10);
	memcpy(m_rule_arry.ItemArry[0].szItemValue[2], "8", 10);
	
	memcpy(m_rule_arry.ItemArry[1].szHeadName, "�����", 15);
	memcpy(m_rule_arry.ItemArry[1].szItemValue[0], "2", 10);
	memcpy(m_rule_arry.ItemArry[1].szItemValue[1], "3", 10);
	memcpy(m_rule_arry.ItemArry[1].szItemValue[2], "4", 10);

	return 0;
}
//�����ں�
int CGameCtrl::StartKernelService()
{
	//��������
	if (m_AttemperEngine->StartService()==false)
	{
		return 1;
	}

	//ʱ������
	if (m_TimerEngine->StartService()==false)
	{
		return 2;
	}

	//Э������
	if (m_TCPSocketEngine->StartService()==false)
	{
		return 3;
	}

	return 0;
}
//��������
bool CGameCtrl::StartNetworkService()
{
	//��������
	if (m_TCPNetworkEngine->StartService()==false)
	{
		return false;
	}

	return true;
}

//��ȡͨ�÷��������ļ�
void CGameCtrl::ReadFrameRoomRule()
{
	//��ʼ������
	ZeroMemory(&m_rule_arry, sizeof(m_rule_arry));
	m_frame_rule_count = 0;
	m_subgame_rule_count = 0;

	//���ļ�
	string file_path = "frame.rule";
	int iRet = CWHCfg::Instance()->OpenFile(file_path);
	if(iRet != 0 )
	{
		return ;
	}

	BYTE card_group_num = 0;
	for(int i=0; i<20; i++)
	{
		char psz[20];
		sprintf(psz, "RULE_%d", i);
		string strTemp;
		iRet = CWHCfg::Instance()->GetItemValue(psz, "head", strTemp);
		if(iRet != 0) continue;
		std::cout << strTemp << std::endl;
		std::cout << strTemp.c_str() << std::endl;

		m_frame_rule_count ++;
		//swprintf(m_rule_arry.ItemArry[i].szHeadName, 15, L"%S", strTemp.c_str());

		for(int j=0; j<4; j++)
		{
			char value[20];
			sprintf(value, "value_%d", j);
			iRet = CWHCfg::Instance()->GetItemValue(psz, value, strTemp);
			if(iRet != 0) continue;
			//swprintf(m_rule_arry.ItemArry[i].szItemValue[j], 10, L"%S", strTemp.c_str());
		}
	}

	//�ر��ļ�
	CWHCfg::Instance()->CloseFile();
}
//��ȡ����Ϸ���������ļ�
void CGameCtrl::ReadSubGameRoomRule(int kindid)
{
	//���ļ�
	string file_path = std::to_string(kindid)+ ".rule";
	int iRet = CWHCfg::Instance()->OpenFile(file_path);
	if(iRet != 0 )
	{
		return ;
	}

	BYTE card_group_num = 0;
	for(int i=0; i<20; i++)
	{
		char psz[20];
		sprintf(psz, "RULE_%d", i);
		string strTemp;
		iRet = CWHCfg::Instance()->GetItemValue(psz, "head", strTemp);
		if(iRet != 0) continue;
		m_subgame_rule_count ++;
		//swprintf(m_rule_arry.ItemArry[i].szHeadName, 15, L"%S", strTemp.c_str());

		for(int j=0; j<4; j++)
		{
			char value[20];
			sprintf(value, "value_%d", j);
			iRet = CWHCfg::Instance()->GetItemValue(psz, value, strTemp);
			if(iRet != 0) continue;
			//swprintf(m_rule_arry.ItemArry[i].szItemValue[j], 10, L"%S", strTemp.c_str());
		}
	}

	//�ر��ļ�
	CWHCfg::Instance()->CloseFile();
}

//���ö˿�
int CGameCtrl::SetNetworkPort(WORD Port)
{
	if (m_TCPNetworkEngine->SetServiceParameter(Port) == false)
		return -1;

	return 0;
}

//��ȡITableFrameSink
ITableFrameSink* CGameCtrl::GetITableFrameSink()
{
	ITableFrameSink *SubGameDll = static_cast<ITableFrameSink*>(CWHModule::SubGame(m_subgame_dll_name)); //�˴�ֻ��У��
	return SubGameDll;
}

/************************************************��Ϣ����*********************************************************/
//��������
bool CGameCtrl::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	m_TCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);

	return true;
}

//��������
bool CGameCtrl::SendData(CPlayer * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (pIServerUserItem==NULL) return false;

	//��������
	m_TCPNetworkEngine->SendData(pIServerUserItem->GetSocketID(),wMainCmdID,wSubCmdID,pData,wDataSize);
	
	return true;
}

//������Ϣ
bool CGameCtrl::SendRoomMessage(LPCTSTR lpszMessage, WORD wType)
{
	//��������
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	m_TCPNetworkEngine->SendDataBatch(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

	return true;
}

//��Ϸ��Ϣ
bool CGameCtrl::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//��������
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	m_TCPNetworkEngine->SendDataBatch(MDM_G_FRAME,CMD_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

	return true;
}

//������Ϣ
bool CGameCtrl::SendRoomMessage(CPlayer * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//Ч�����
	if (pIServerUserItem==NULL) return false;

	//��������
	if (pIServerUserItem->GetBindIndex()!=INVALID_WORD)
	{
		//��������
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//��������
		SystemMessage.wType=wType;
		SystemMessage.wLength=lstrlen(lpszMessage)+1;
		lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

		//��������
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//��������
		if (pIServerUserItem->IsAndroidUser()==true)
		{

		}
		else
		{
			//�����û�
			m_TCPNetworkEngine->SendData(pIServerUserItem->GetSocketID(),MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
		}

		return true;
	}

	return false;
}

//��Ϸ��Ϣ
bool CGameCtrl::SendGameMessage(CPlayer * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	if ((pIServerUserItem->GetBindIndex()!=INVALID_WORD))
	{
		//��������
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//��������
		SystemMessage.wType=wType;
		SystemMessage.wLength=lstrlen(lpszMessage)+1;
		lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

		//��������
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//��������
		if (pIServerUserItem->IsAndroidUser()==true)
		{

		}
		else
		{
			//�����û�
			m_TCPNetworkEngine->SendData(pIServerUserItem->GetSocketID(),MDM_G_FRAME,CMD_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
		}

		return true;
	}

	return false;
}

//������Ϣ
bool CGameCtrl::SendRoomMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType, bool bAndroid)
{
	//��������
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	if (bAndroid)
	{
		//�����û�
		//m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	}
	else
	{
		//�����û�
		m_TCPNetworkEngine->SendData(dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	}

	return true;
}

//�����������Ϣ
bool CGameCtrl::SendMessageLobbyAndAllGame( LPCTSTR lpszMessage, WORD wType ,WORD MsgID)
{
	/*
	//������Ϣ
	CMD_CS_C_SendMarquee pSendLobbyMessage;
	ZeroMemory(&pSendLobbyMessage,sizeof(pSendLobbyMessage));
	lstrcpyn(pSendLobbyMessage.szMessage,lpszMessage,CountArray(pSendLobbyMessage.szMessage));
	pSendLobbyMessage.MsgType=wType;
	pSendLobbyMessage.MsgID = MsgID;
	//���͸�Э��������
	m_TCPSocketEngine->SendData(MDM_WEB,CPR_WP_WEB_MARQUEE,&pSendLobbyMessage,sizeof(CMD_CS_C_SendMarquee));
	*/
	return true;
}


/***************************************************  ��Ϣ����  ->DB  *************************************************/
bool CGameCtrl::PostDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	return m_AttemperEngine->PostDataBaseRequest(wRequestID, dwContextID, pData, wDataSize);
}


/***************************************************     Timer       *************************************************/
//���ö�ʱ��
bool CGameCtrl::SetTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	return m_TimerEngine->SetTimer(dwTimerID, dwElapse, dwRepeat, dwBindParameter);
}
//ɾ����ʱ��
bool CGameCtrl::KillTimer(DWORD dwTimerID)
{
	return m_TimerEngine->KillTimer(dwTimerID);
}
//ɾ����ʱ��
bool CGameCtrl::KillAllTimer()
{
	return m_TimerEngine->KillAllTimer();
}
//��ȡ��ʱ��ʣ��ʱ�䣨���룩
DWORD CGameCtrl::GetTimerLeftTickCount(DWORD dwTimerID)
{
	return m_TimerEngine->GetTimerLeftTickCount(dwTimerID);
}