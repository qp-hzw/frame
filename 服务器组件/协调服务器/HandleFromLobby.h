#ifndef HANDLE_FROM_LOBBY_HEAD_FILE
#define HANDLE_FROM_LOBBY_HEAD_FILE

#include "Stdafx.h"

class CHandleFromLobby
{
	//总分发函数
public:
	static bool HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//一级分发函数
public:
	//中转服务
	static bool OnTCPNetworkMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//具体处理
public:

};

#endif