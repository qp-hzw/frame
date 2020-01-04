#include "StdAfx.h"
#include "GameCtrl.h"
#include "AttemperEngineSink.h"
#include "TableManager.h"
#include "HandleFromCenter.h"
#include "HandleFromGate.h"
#include <vector>
#include <algorithm>


#include "../../ȫ�ֶ���/Define.h"

CAttemperEngineSink * g_AttemperEngineSink = NULL;

//////////////////////////////////////////////////////////////////////////////////
#define CT_CONNECT_CORRESPOND		100									//����Э��
#define CT_LOAD_SERVICE_CONFIG		101									//��������
#define CT_TRY_TO_STOP_SERVICE		102									//ֹͣ����

//////////////////////////////////////////////////////////////////////////////////
//ʱ���ʶ

#define IDI_LOAD_ANDROID_USER			(IDI_MAIN_MODULE_START+1)			//������Ϣ
//#define IDI_REPORT_SERVER_INFO			(IDI_MAIN_MODULE_START+2)			//��������
#define IDI_CONNECT_CORRESPOND			(IDI_MAIN_MODULE_START+3)			//����ʱ��
#define IDI_GAME_SERVICE_PULSE			(IDI_MAIN_MODULE_START+4)			//��������
#define IDI_DBCORRESPOND_NOTIFY			(IDI_MAIN_MODULE_START+6)			//����֪ͨ

#define IDI_LOAD_SENSITIVE_WORD			(IDI_MAIN_MODULE_START+9)			//�������д�

#define IDI_WRITE_STOCK_SCORE			(IDI_MAIN_MODULE_START+14)			//д����¼

#define TIME_CONNECT				30									//����ʱ��
//////////////////////////////////////////////////////////////////////////////////
//ʱ�䶨�� ��

#define TIME_LOAD_ANDROID_USER				600L								//���ػ���
#define TIME_DISTRIBUTE_ANDROID				15L									//�����û�
#define TIME_REPORT_SERVER_INFO				30L									//�ϱ�ʱ��
#define TIME_DBCORRESPOND_NOTIFY			3L									//����֪ͨʱ��
#define TIME_LOAD_SYSTEM_MESSAGE			3600L								//ϵͳ��Ϣʱ��
#define TIME_SEND_SYSTEM_MESSAGE			10L									//ϵͳ��Ϣʱ��
#define TIME_CONTROL_ROOM_VALUE_SAVE		120000L								//�������ֵʱ��yang
#define TIME_READ_ROOM_CONFIG				2000L								//��ȡ��������yang
#define TIME_WRITE_STOCK_SCORE				120000L								//д����¼
#define TIME_WAIT_SERVICE_STOP				5000L								//�ӳ�ֹͣ
#define TIME_CONNECT_CORRESPOND				30*1000L									//����ʱ��

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	if(g_AttemperEngineSink == NULL) g_AttemperEngineSink =this;
}


//�����¼�
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	/*  TODONOW �ô��ᵼ�³������
	//����ʱ��
	g_GameCtrl->SetTimer(IDI_GAME_SERVICE_PULSE,1000L,TIMES_INFINITY,NULL);
	g_GameCtrl->SetTimer(IDI_LOAD_ANDROID_USER,TIME_LOAD_ANDROID_USER*1000L,TIMES_INFINITY,NULL);
	g_GameCtrl->SetTimer(IDI_DBCORRESPOND_NOTIFY,TIME_DBCORRESPOND_NOTIFY*1000L,TIMES_INFINITY,NULL);
	*/
	return true;
}

//ʱ���¼�
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	//����ʱ��
	if ((dwTimerID>=IDI_MAIN_MODULE_START)&&(dwTimerID<=IDI_MAIN_MODULE_FINISH))
	{
		//ʱ�䴦��
		switch (dwTimerID)
		{
		case IDI_LOAD_ANDROID_USER:		//���ػ���
			{
				//���ػ���
				g_GameCtrl->PostDataBaseRequest(DBR_GR_LOAD_ANDROID_USER,0L,NULL,0L);

				return true;
			}
		case IDI_CONNECT_CORRESPOND:	//����Э��
			{
				g_TCPSocketEngine->Connect(_CPD_SERVER_ADDR, PORT_CENTER);
				return true;
			}
		case IDI_GAME_SERVICE_PULSE:	//����ά��
			{
				return true;
			}

		case IDI_LOAD_SENSITIVE_WORD:	//�������д�
			{
				//Ͷ������
				//g_GameCtrl->PostDataBaseRequest(DBR_GR_LOAD_SENSITIVE_WORDS,0,NULL,0);				
				return true;
			}

		}
	}

	//����ʱ��
	if ((dwTimerID>=IDI_TABLE_MODULE_START)&&(dwTimerID<=IDI_TABLE_MODULE_FINISH))
	{
		//���Ӻ���
		DWORD dwTableTimerID=dwTimerID-IDI_TABLE_MODULE_START;
		DWORD wTableIndex=(DWORD)(dwTableTimerID/TIME_TABLE_MODULE_RANGE);

		//ʱ��֪ͨ
		CTableFrame * pTableFrame= CTableManager::FindTableByTableID(wTableIndex);
		if(pTableFrame != NULL)
			return pTableFrame->OnEventTimer(dwTableTimerID%TIME_TABLE_MODULE_RANGE,wBindParam);
	}

	return false;
}

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBaseResult(WORD wRequestID, DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//TODONOW ��ʱ�ŵ�CHandleFromGate��, ֮���ӵ�CHandleFromDB
	return CHandleFromGate::HandlePacketDB(wRequestID, dwScoketID, pData, wDataSize);
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
		ServerItem.dwServerID = g_GameCtrl->GetServerID();
		ServerItem.byServerType = GAME_TYPE;
		//lstrcpyn(RegisterServer.szServerName, m_pGameServiceOption->szServerName, CountArray(RegisterServer.szServerName));
		lstrcpyn(ServerItem.szServerAddr,TEXT("127.20.10.4"),CountArray(ServerItem.szServerAddr));
		ServerItem.wServerPort = 0;

		CLog::Log(log_debug, "ServerID:%d", ServerItem.dwServerID);

		//��������
		g_TCPSocketEngine->SendData(MDM_REGISTER,CPR_REGISTER_SERVER,&ServerItem,sizeof(ServerItem));

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
		//ɾ��ʱ��
		//g_GameCtrl->KillTimer(IDI_REPORT_SERVER_INFO);

		//����ʱ��
		g_GameCtrl->SetTimer(IDI_CONNECT_CORRESPOND, TIME_CONNECT_CORRESPOND, 1, 0);

		return true;
	}

	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	return CHandleFromCenter::HandlePacket(Command,pData, wDataSize, wServiceID);
	/*
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
	}*/
}

//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	return true;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//��ȡ�û�
	CPlayer * pPlayer=  CPlayerManager::FindPlayerBySocketID(dwSocketID);

	//���Ӵ���
	if (pPlayer != NULL)
	{
		DWORD passWord = pPlayer->GetTableID();

		CTableFrame *pTableFrame = CTableManager::FindTableByTableID(passWord);
		if (pTableFrame == NULL)
			return false;

		//����뿪
		pTableFrame->PlayerLeaveTable(pPlayer);
	}

	//�û�����
	if (pPlayer!=NULL)
	{
		bool bIsExsit = false; //�Ƿ����ڶ���

		//��������
		DWORD wTableID=pPlayer->GetTableID();

		//���ߴ���
		if (wTableID!=INVALID_TABLE)
		{
			//�Ƿ���Ҫ�����������
			CTableFrame* pTableFrame = CTableManager::FindTableByTableID(wTableID);
			if (( pTableFrame != NULL) && ( pTableFrame->OnEventUserOffLine(pPlayer)))
			{
				DWORD dwServerID = (g_GameCtrl->GetServerID()) &0xFFFFFF00;

				//��Ϸ��ֱ���޸� TODONOW ��ʱ����
				pPlayer->SetOfflineGameID(dwServerID);

				//���͸�Э����, ��Э����֪ͨ��¼��
				tagOfflineUser data;
				data.dwUserID = pPlayer->GetUserID();
				data.dwServerID = dwServerID;
				data.byMask = 1; //��ʾ���Ӷ����û�
				g_TCPSocketEngine->SendData(MDM_USER,SUB_CS_C_USER_OFFLINE,&data,sizeof(tagOfflineUser));

				bIsExsit = true;
			}
		}
		else
		{
			//ɾ���û�
			CPlayerManager::DeletePlayer(pPlayer);
			delete pPlayer;
			return true;
		}

		//���״̬�仯
		/* ������ ��Ϸ�� socket�ϵ���ʱ��
		**	1. �ж���Ϸ�б���  �Ƿ����
		**	�������, ���¼���϶������ˣ�Ӧ������Ϊ��������
		**	�����, ������Ϊ��Ϸ����
		*/
		DBR_GP_UserQuitInfo quitInfo;
		quitInfo.dwUserID = pPlayer->GetUserID();
		if(bIsExsit)
		{
			quitInfo.byOnlineMask = 3;
		}
		else
		{
			quitInfo.byOnlineMask = 1;
		}
		g_GameCtrl->PostDataBaseRequest(DBR_GP_GAME_USER_STATE,dwSocketID, &quitInfo,sizeof(quitInfo));
	}

	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	return CHandleFromGate::HandlePacket(Command, pData, wDataSize, dwSocketID);
}
