#include "HandleFromGame.h"
#include "GameCtrl.h"

//��Ϣ�ַ�������
bool CHandleFromGame::HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_TRANSFER:	//��ת����
		{
			return OnTCPNetworkMainTransfer(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_USER://�û�����
		{
			return OnTCPNetworkMainUserCollect(Command.wSubCmdID,pData,wDataSize,dwSocketID);
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
			g_GameCtrl->SendDataBatch(MDM_TRANSFER,CPO_PL_CLUB_TABLE_INFO,pData,wDataSize);
			return true;
		}
	case CPR_GP_CLUB_PLAYER_INFO: //club���ֲ������Ϣ����
		{
			//Ч������
			if (wDataSize!=sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST)) return false;
			
			//����֪ͨ -- ȫ����¼��
			g_GameCtrl->SendDataBatch(MDM_TRANSFER,CPO_PL_CLUB_PLAYER_INFO,pData,wDataSize);
			return true;
		}
	}

	return false;
}

//�û�����
bool CHandleFromGame::OnTCPNetworkMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_USER_OFFLINE:	//�û�����
		{
			//Ч������
			if (wDataSize!=sizeof(STR_SUB_CS_C_USER_OFFLINE)) return false;

			//��Ϣ����
			STR_SUB_CS_C_USER_OFFLINE * pUserOffLine=(STR_SUB_CS_C_USER_OFFLINE *)pData;

			STR_SUB_CS_C_USER_OFFLINE data;
			data.dwUserID = pUserOffLine->dwUserID;
			data.dwGameID = pUserOffLine->dwGameID;
			data.byMask = pUserOffLine->byMask;
			
			//����֪ͨ -- ȫ����¼��
			g_GameCtrl->SendDataBatch(MDM_USER,SUB_CS_C_USER_OFFLINE_B,&data,sizeof(STR_SUB_CS_C_USER_OFFLINE));
			
			return true;
		}
	}

	return false;
}
#pragma endregion 
