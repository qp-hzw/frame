#include "HandleFromGame.h"
#include "GameCtrl.h"
#include "ServerItemManager.h"

//��Ϣ�ַ�������
bool CHandleFromGame::HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case CPD_MDM_TRANSFER:	//��ת����
		{
			return OnTCPNetworkMainTransfer(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	}
	return false;
}

#pragma region  һ���ַ�����
//��ת����
bool CHandleFromGame::OnTCPNetworkMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case CPR_GP_CLUB_TABLE_INFO: //club���ֲ�������Ϣ����
		{
			//Ч������
			if (wDataSize!=sizeof(STR_CMD_LC_CLUB_TABLE_LIST)) return true;
			
			//����֪ͨ -- ȫ����¼��
			g_GameCtrl->SendDataBatch(CPD_MDM_TRANSFER,CPO_PL_CLUB_TABLE_INFO,pData,wDataSize);
			return true;
		}
	case CPR_GP_CLUB_PLAYER_INFO: //club���ֲ������Ϣ����
		{
			//Ч������
			if (wDataSize!=sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST)) return false;
			
			//����֪ͨ -- ȫ����¼��
			g_GameCtrl->SendDataBatch(CPD_MDM_TRANSFER,CPO_PL_CLUB_PLAYER_INFO,pData,wDataSize);
			return true;
		}
	case CPO_GP_OFFLINE_FINISH:
		{
			//У������
			if (wDataSize != sizeof(STR_CPO_GP_OFFLINE_FINISH)) return false;
			STR_CPO_GP_OFFLINE_FINISH* pCPO = (STR_CPO_GP_OFFLINE_FINISH *)pData;

			static int recv_num = 0;
			recv_num++;

			if(pCPO->bOffline == 1)
			{
				tagServerItem* item =  CServerItemManager::FindItemBySocketID(dwSocketID);
				if(!item)
				{
					//���͸�logon
					STR_CPO_PL_OFFLINE_FiNISH cmd;
					cmd.dwUserID = pCPO->dwUserID;
					cmd.dwGameID = item->dwServerID;
					cmd.dwSocketID = pCPO->dwSocketID;
					
					g_GameCtrl->SendData(pCPO->dwSocketID, CPD_MDM_TRANSFER, CPO_PL_OFFLINE_FiNISH,&cmd,sizeof(cmd));
					return true;
				}
			}

			//�������ݽ������
			if(recv_num == CServerItemManager::FindAllGameServer().size())
			{
				recv_num = 0;

				//���͸�logon
				STR_CPO_PL_OFFLINE_FiNISH cmd;
				cmd.dwUserID = pCPO->dwUserID;
				cmd.dwGameID = 0;
				cmd.dwSocketID = pCPO->dwSocketID;

				g_GameCtrl->SendData(pCPO->dwSocketID, CPD_MDM_TRANSFER, CPO_PL_OFFLINE_FiNISH,&cmd,sizeof(cmd));
				return true;
			}

			return true;
		}
	}

	return false;
}

#pragma endregion 
