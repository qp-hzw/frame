#include "HandleFromCenter.h"
#include "GameCtrl.h"
#include "TableManager.h"

bool CHandleFromCenter::HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, WORD wServiceID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_REGISTER:		//ע�����
		{
			return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
		}
	case MDM_TRANSFER:		//��ת����
		{
			return OnTCPSocketMainTransfer(Command.wSubCmdID,pData,wDataSize);	
		}
	}
}

//ע���¼�
bool CHandleFromCenter::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case CPO_REGISTER_SUCESS:		//ע�����
		{
			//У��
			if (sizeof(tagServerItem) != wDataSize)
				return false;

			//��ȡ����
			tagServerItem *pServerItem = (tagServerItem *)pData;

			CLog::Log(log_debug, "port:%d", pServerItem->wServerPort);

			//���ö˿�
			if (0 != g_GameCtrl->SetNetworkPort(pServerItem->wServerPort))
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

			//����Ǳ�����������Ϣ, ��֪ͨ����Ϸ��ɢ��Ϸ
			for(int i = 0; i < CTableManager::TableCount(); i++)
			{
				//У��
				CTableFrame *pTableFrame = CTableManager::FindTableByIndex(i);
				if (NULL == pTableFrame)
					continue;

				//�ҵ���ͬ���Ӻŵģ� Ȼ���ɢ
				if(pTableFrame->GetTableID() == pCPO->dwTableID)
				{
					//���÷��䴦�ڽ�ɢ״̬
					pTableFrame->SetDismissState(true);

					//֪ͨ�ͻ��˵�����ֽ������
					STR_CMD_GR_FRAME_DISMISS_RESULT DismissResult;
					ZeroMemory(&DismissResult, sizeof(DismissResult));
					DismissResult.cbDismiss = 1;
					DismissResult.cbAgree = 1;					
					if (pTableFrame->GetGameStatus() != GAME_STATUS_FREE)
					{
						DismissResult.cbClubQuit = 1;
					}
					
					pTableFrame->SendTableData(INVALID_CHAIR, CMD_GR_FRAME_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_G_FRAME);
					
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
					break;
				}
			}

			return true;
		}
	}

	return true;
}
