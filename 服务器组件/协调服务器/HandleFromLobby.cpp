#include "HandleFromLobby.h"
#include "GameCtrl.h"
#include "ServerItemManager.h"

//��Ϣ�ַ�������
bool CHandleFromLobby::HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_TRANSFER:	//��ת����
		{
			return OnTCPNetworkMainTransfer(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	}
	return false;
}

#pragma region  һ���ַ�����
//��ת����
bool CHandleFromLobby::OnTCPNetworkMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case CPR_LP_CLUB_TABLE_DISSOLVE :	//��¼��֪ͨЭ����, Э����֪ͨ��Ϸ�� ��ɢ����
		{
			//У������
			if (wDataSize!=sizeof(STR_CPR_LP_CLUB_TABLE_DISSOLVE)) return true;

			//֪ͨ�ض���Ϸ�� ��GameIDΪ�ж�����
			//TOODNOW added by WangChengQing ��Ϊ��֪��Э�������ͨ��gmaeID�ҵ���Ϸ����socketID
			//���Դ˴���֪ͨ������Ϸ����Ȼ����Ϸ������У��
			//֮��ĵ�

			STR_CPR_LP_CLUB_TABLE_DISSOLVE *pCPR = (STR_CPR_LP_CLUB_TABLE_DISSOLVE*) pData;

			STR_CPO_PG_CLUB_TABLE_DISSOLVE CPO;
			CPO.dwGameID = pCPR->dwGameID;
			CPO.dwTableID = pCPR->dwTableID;

			//����֪ͨ -- ȫ����Ϸ��
			g_GameCtrl->SendDataBatch(MDM_TRANSFER,CPO_PG_CLUB_TABLE_DISSOLVE,&CPO,sizeof(CPO));

			return true;
		}
		case CPR_LP_CREATE_TABLE:	//�������� ��ѯ���õ�GameID 
		{
			//У������
			if (wDataSize!=sizeof(STR_CPR_LP_CREATE_TABLE)) return true;
			STR_CPR_LP_CREATE_TABLE *pCPR = (STR_CPR_LP_CREATE_TABLE*) pData;


			DWORD dwMatchGameID = 0;
			DWORD dwMinOnlineCount = 60*4; //ÿ�������������Ϊ240 ����Ժ��޸�, ����Ҳ��Ҫ�޸� added by WangChengQing

			//��ȡ����Ϸ��KindIDƥ�������
			tagServerItem *pItem = CServerItemManager::FindItemByKindID(pCPR->dwKindID); 

			//CPO����
			STR_CPO_PL_CREATE_TABLE CPO;
			ZeroMemory(&CPO, sizeof(CPO));

			if(NULL == pItem)
			{
				CPO.dwResultCode = 1;
				memcpy(CPO.szDescribeString, TEXT("δ�ҵ����ʵ���Ϸ������"), sizeof(CPO.szDescribeString));
				CPO.dwSocketID = pCPR->dwSocketID;
			}
			else
			{
				CPO.dwResultCode = 0;
				CPO.dwGameID = pItem->dwServerID & 0x00FFFF00;
				CPO.dwSocketID = pCPR->dwSocketID;
			}

			//������Ϣ
			g_GameCtrl->SendData(dwSocketID,MDM_TRANSFER,CPO_PL_CREATE_TABLE, &CPO, sizeof(CPO));
			return true;
		}

	}

	return false;
}
#pragma endregion 


/*
//�����б�
bool CAttemperEngineSink::SendServerListItem(DWORD dwSocketID)
{
	//��������
	WORD wPacketSize=0L;
	POSITION Position=NULL;
	BYTE cbBuffer[SOCKET_TCP_PACKET];

	for(auto item : CServerItemManager::FindAllGameServer())
	{
		//��������
		if ((wPacketSize+sizeof(tagServerItem))>sizeof(cbBuffer))
		{
			g_GameCtrl->SendData(dwSocketID,MDM_CPD_LIST,CPO_PL_LIST_INSERT_GAME_LIST,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		tagServerItem * pGameServer=(tagServerItem *)(cbBuffer+wPacketSize);
		if(item != NULL)
		{
			wPacketSize+=sizeof(tagServerItem);
			CopyMemory(pGameServer, item, sizeof(tagServerItem));
		}
	}

	//��������
	if (wPacketSize>0) 
		g_GameCtrl->SendData(dwSocketID,MDM_CPD_LIST,CPO_PL_LIST_INSERT_GAME_LIST,cbBuffer,wPacketSize);

	return true;
}
*/