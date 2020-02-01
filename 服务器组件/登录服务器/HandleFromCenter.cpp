#include "HandleFromCenter.h"
#include "GameCtrl.h"
#include "PlayerManager.h"

//��Ϣ�ַ�������
bool CHandleFromCenter::HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, WORD wServiceID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_REGISTER_CPD:		//ע��ģ��
		{
			return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
		}
	case CPD_MDM_USER:	//�б�����
		{
			return OnTCPSocketMainServiceInfo(Command.wSubCmdID,pData,wDataSize);
		}
	case CPD_MDM_TRANSFER:	//��ת����
		{
			return OnTCPSocketMainTransfer(Command.wSubCmdID,pData,wDataSize);	
		}
	}

	return false;
}


//ע��ģ��
bool CHandleFromCenter::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case CPO_REGISTER_SUCESS:	//ע��ɹ�
		{
			//����socket::server����
			if(g_GameCtrl->StartNetworkService() != 0)
			{
				g_GameCtrl->ConcludeService();
				return true;
			}

			return true;
		}

	case CPO_REGISTER_FAILURE:		//ע��ʧ��
		{
			//�¼�֪ͨ 
			g_GameCtrl ->ConcludeService();
			return true;
		}
	}

	return true;
}
//�б�����
bool CHandleFromCenter::OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_USER_OFFLINE_B: //�û�����
		{
			//Ч������
			if (wDataSize!=sizeof(tagOfflineUser)) return true;
			tagOfflineUser * pOffline = (tagOfflineUser*) pData;

			CLog::Log(log_debug, "2, 15 %d", pOffline->byMask);

			if(pOffline->byMask == 1)//���Ӷ����û�
			{
				CPlayerManager::AddOfflinePlayer(pOffline->dwUserID, pOffline->dwServerID);
			}
			else if(pOffline->byMask == 2) //ɾ�������û�
			{
				CPlayerManager::DeleteOfflinePlayer(pOffline->dwUserID);
			}

			return true;
		}
	}

	return true;
}
//��ת����
bool CHandleFromCenter::OnTCPSocketMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case CPO_PL_CLUB_TABLE_INFO:  //���ֲ�������Ϣ
		{
			//TODONOW ��ʱ�ȸ�Ϊ֪ͨ����client, �����Ϊ֪ͨ�ڸ÷����ڵ���
			//added by WangChengQing 2018/5/21
			//Ч������
			if (wDataSize!=sizeof(STR_CMD_LC_CLUB_TABLE_LIST)) return true;

			//����֪ͨ -- ȫ���ͻ���
			g_GameCtrl->SendDataBatch(MDM_CLUB,CMD_LC_CLUB_TABLE_LIST_TABLE,pData,wDataSize);
			return true;
		}
	case CPO_PL_CLUB_PLAYER_INFO: //���ֲ������Ϣ
		{
			//TODONOW ��ʱ�ȸ�Ϊ֪ͨ����client, �����Ϊ֪ͨ�ڸ÷����ڵ���
			//added by WangChengQing 2018/5/21
			//Ч������
			if (wDataSize!=sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST)) return true;

			//����֪ͨ -- ȫ���ͻ���
			g_GameCtrl->SendDataBatch(MDM_CLUB, CMD_LC_CLUB_TABLE_LIST_USER,pData,wDataSize);
			return true;
		}
	case CPO_PL_CREATE_TABLE:	//�������� ��ѯ���õ�GameID
		{
			//Ч������
			if (wDataSize!=sizeof(STR_CPO_PL_CREATE_TABLE)) return true;
			STR_CPO_PL_CREATE_TABLE *pCPO = (STR_CPO_PL_CREATE_TABLE*)pData;
			
			STR_CMD_LC_GAME_QUERY_GAMEID CMD;
			CMD.dwGameID = pCPO->dwGameID;
			CMD.dwResultCode = pCPO->dwResultCode;
			memcpy(CMD.szDescribeString, pCPO->szDescribeString, sizeof(CMD.szDescribeString));
			CopyMemory(CMD.szIPAddr, pCPO->szIPAddr, sizeof(CMD.szIPAddr));
			CMD.dwPort = pCPO->dwPort;

			g_GameCtrl->SendData(pCPO->dwSocketID, MDM_GAME, CMD_LC_GAME_QUERY_GAMEID, &CMD, sizeof(CMD));
			return true;
		}
	}

	return true;
}
