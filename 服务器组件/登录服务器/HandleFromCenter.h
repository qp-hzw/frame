#ifndef HANDLE_FROM_CENTER_HEAD_FILE_L
#define HANDLE_FROM_CENTER_HEAD_FILE_L

#include "Stdafx.h"

class CHandleFromCenter
{
	//总分发函数
public:
	static bool HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, WORD wServiceID = 0);

	//一级分发函数
public:
	//注册模块
	static bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//列表命令
	static bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//中转服务
	static bool OnTCPSocketMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//具体处理
public:

};

#endif