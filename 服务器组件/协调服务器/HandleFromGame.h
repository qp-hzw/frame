#ifndef HANDLE_FROM_GAME_HEAD_FILE
#define HANDLE_FROM_GAME_HEAD_FILE
#include "Stdafx.h"

class CHandleFromGame
{
	//��Ϣ�ַ�����
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