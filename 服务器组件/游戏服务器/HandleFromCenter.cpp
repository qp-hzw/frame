#include "HandleFromCenter.h"
#include "GameCtrl.h"
#include "TableManager.h"

bool CHandleFromCenter::HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, WORD wServiceID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_REGISTER_CPD:		//ע�����
		{
			return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
		}
	case CPD_MDM_TRANSFER:		//��ת����
		{
			return OnTCPSocketMainTransfer(Command.wSubCmdID,pData,wDataSize);	
		}
	}

	return true;
}

//ע���¼�
bool CHandleFromCenter::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case CPO_REGISTER_SUCESS:		//ע�����
		{
			//У��
			if (sizeof(WORD) != wDataSize)
				return false;

			//��ȡ����
			WORD* pServerItem = (WORD* )pData;

			//���ö˿�
			if (0 != g_GameCtrl->SetNetworkPort(*pServerItem))
			{
				g_GameCtrl->ConcludeService();
				return true;
			}

			//����socket::server����
			if(!g_GameCtrl->StartNetworkService())
			{
				g_GameCtrl->ConcludeService();
				return true;
			}

			return true;
		}
	case CPO_REGISTER_FAILURE:		//ע��ʧ��
		{
			g_GameCtrl->ConcludeService();

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
	case CPO_PG_CLUB_TABLE_DISSOLVE: //��¼��֪ͨЭ����, Э����֪ͨ��Ϸ�� ��ɢ����
		{
			//У������
			if (wDataSize != sizeof(STR_CPO_PG_CLUB_TABLE_DISSOLVE)) return false;
			
			STR_CPO_PG_CLUB_TABLE_DISSOLVE* pCPO = (STR_CPO_PG_CLUB_TABLE_DISSOLVE *)pData;

			//������Ǳ�����������Ϣ, ����˵�
			if(pCPO->dwGameID != (g_GameCtrl->GetServerID() & 0xFFFFFF00))
			{
				return true;
			}

			CTableFrame *pTableFrame = CTableManager::FindTableByTableID( pCPO->dwTableID);
			if (NULL == pTableFrame)
					return true;

			//���÷��䴦�ڽ�ɢ״̬
			pTableFrame->SetDismissState(true);

			//֪ͨ�ͻ��˵�����ֽ������
			STR_CMD_GR_FRAME_DISMISS_RESULT DismissResult;
			ZeroMemory(&DismissResult, sizeof(DismissResult));
			DismissResult.cbDismiss = 1;
			//DismissResult.cbAgree = 1;					
			if (pTableFrame->GetGameStatus() != GAME_STATUS_FREE)
			{
				//DismissResult.cbClubQuit = 1;
			}

			//֪ͨ����Ϸ��ɢ����
			//����״̬ ֱ�ӽ�ɢ
			if(GAME_STATUS_FREE == pTableFrame->GetGameStatus())
			{
				pTableFrame->HandleDJGameEnd(GAME_CONCLUDE_NORMAL);
			}
			//��Ϸ״̬�� ֪ͨ����Ϸ
			else
			{
				pTableFrame->OnEventClubDismissRoom();
			}

			return true;
		}

	case CPR_PG_OFFLINE_PLAYERQUERY: //��ѯ�������
		{			
			//У������
			if (wDataSize != sizeof(STR_CPR_PG_OFFLINE_PLAYERQUERY)) return false;
			STR_CPR_PG_OFFLINE_PLAYERQUERY* pCPO = (STR_CPR_PG_OFFLINE_PLAYERQUERY *)pData;

			STR_CPO_GP_OFFLINE_FINISH cmd;
			cmd.dwSocketID = pCPO->dwSocketID;
			cmd.dwUserID = pCPO->dwUserID;

			CPlayer *player =  CPlayerManager::FindPlayerByID(pCPO->dwUserID);
			if(!player)
			{
				cmd.bOffline = 0;
				g_TCPSocketEngine->SendData(CPD_MDM_TRANSFER,CPO_GP_OFFLINE_FINISH, &cmd, sizeof(cmd));
				return true;
			}
		
			if(player->GetTableID() != INVALID_TABLE)
			{
				cmd.bOffline = 1;
			}
			else
			{
				cmd.bOffline = 0;
			}
			g_TCPSocketEngine->SendData(CPD_MDM_TRANSFER,CPO_GP_OFFLINE_FINISH, &cmd, sizeof(cmd));
			return true;
		}
	}

	return true;
}
