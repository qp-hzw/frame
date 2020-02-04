#include "GameCtrl.h"
#include "AttemperEngineSink.h"
#include "HandleFromCenter.h"
#include "HandleFromGate.h"
#include "PlayerManager.h"


CAttemperEngineSink* g_AttemperEngineSink = NULL;
//////////////////////////////////////////////////////////////////////////////////

#define  IDI_CONNECT_CORRESPOND		2									//������ʶ
#define	 TIME_CONNECT_CORRESPOND	30*1000L							//����ʱ��

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	if(!g_AttemperEngineSink)
		g_AttemperEngineSink = this;

	return;
}

//ʱ���¼�
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	switch (dwTimerID)
	{
	case IDI_CONNECT_CORRESPOND:	//����Э��
		{
			g_GameCtrl->KillTimer(IDI_CONNECT_CORRESPOND);
			//��������
			g_TCPSocketEngine->Connect(_CPD_SERVER_ADDR, PORT_CENTER);

			return true;
		}
	}

	return false;
}

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBaseResult(WORD wRequestID, DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//TODOLATER ��ʱ�ŵ�HandleFromGate��, ֮��ŵ�HandleFromDB��

	return CHandleFromGate::HandlePacketDB(wRequestID, dwScoketID,  pData, wDataSize);
}

//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	return true;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	CPlayerManager::DeletePlayerBySocketID (dwSocketID);
	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	return CHandleFromGate::HandlePacket(Command, pData, wDataSize, dwSocketID);
}

//�����¼�
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//�����ж�
		if (nErrorCode!=0)
		{
			//����ʱ��
			g_GameCtrl->SetTimer(IDI_CONNECT_CORRESPOND, TIME_CONNECT_CORRESPOND, 1, 0);
			return false;
		}

		//��������
		tagServerItem ServerItem;
		ZeroMemory(&ServerItem,sizeof(ServerItem));

		//��������
		ServerItem.dwServerID = 0; //TODOLATER lobby.serverid = (0 << 16)  + nodeid;  //lobby.kind = 0
		ServerItem.byServerType = LOBBY_TYPE;
		//lstrcpyn(ServerItem.szServerName,pCPR->szServerName,CountArray(ServerItem.szServerName));
		ServerItem.wServerPort = PORT_LOGON;

		//���ñ���
		TCHAR szInernet_ip[32] = TEXT("127.0.0.1");
		//CWHIP::GetInternetIP(szInernet_ip);
		lstrcpyn(ServerItem.szServerAddr,szInernet_ip ,CountArray(ServerItem.szServerAddr));

		//��������
		g_TCPSocketEngine->SendData(MDM_REGISTER_CPD,CPR_REGISTER_SERVER,&ServerItem,sizeof(ServerItem));

		return true;
	}

	return true;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//����ʱ��
		g_GameCtrl->SetTimer(IDI_CONNECT_CORRESPOND, TIME_CONNECT_CORRESPOND, 1, 0);
		return true;
	}

	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	return CHandleFromCenter::HandlePacket(Command, pData, wDataSize, wServiceID);
}








