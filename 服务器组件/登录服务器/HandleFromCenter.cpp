#include "HandleFromCenter.h"
#include "GameCtrl.h"
#include "PlayerManager.h"
#include "Player.h"

//��Ϣ�ַ�������
bool CHandleFromCenter::HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, WORD wServiceID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_REGISTER_CPD:		//ע��ģ��
		{
			return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
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
			//g_GameCtrl->SendDataBatch(MDM_CLUB, CMD_LC_CLUB_TABLE_LIST_USER,pData,wDataSize);
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
	case CPO_PL_OFFLINE_FiNISH: //��ѯ����
		{
			//Ч������
			if (wDataSize!=sizeof(STR_CPO_PL_OFFLINE_FiNISH)) return true;
			STR_CPO_PL_OFFLINE_FiNISH *pCPO = (STR_CPO_PL_OFFLINE_FiNISH*)pData;

			CPlayer* player = CPlayerManager::FindPlayerByID(pCPO->dwUserID);
			if(!player) return true;

			//���͵�¼�ɹ� 
			STR_CMD_LC_LOGON_PLATFORM  cmd;
			cmd.dwResultCode = 0;
			memcpy(&cmd.useInfo, player->GetUserInfo(), sizeof(cmd.useInfo));
			cmd.dwOffLineGameID  = pCPO->dwGameID;
			g_GameCtrl->SendData(player->GetSocketID(), MDM_LOGON, CMD_LC_LOGON_ACCOUNTS, &cmd, sizeof(cmd));


			//��ѯ����ұ���, ��ע����Ϣ
			g_GameCtrl->PostDataBaseRequest(DBR_CL_BAG_QUERY, 0, &(pCPO->dwUserID),sizeof(DWORD));

			return true;
		}
	}

	return true;
}
