#ifndef HANDLE_FROM_LOBBY_HEAD_FILE
#define HANDLE_FROM_LOBBY_HEAD_FILE

#include "Stdafx.h"

class CHandleFromLobby
{
	//�ַܷ�����
public:
	static bool HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//һ���ַ�����
public:
	//��ת����
	static bool OnTCPNetworkMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���崦��
public:

};

#endif