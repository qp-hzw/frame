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
	//注册事件
	static bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//中转服务
	static bool OnTCPSocketMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//汇总事件
	static bool OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//具体处理
public:

};

#endif