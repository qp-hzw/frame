#ifndef HANDLE_FROM_CENTER_HEAD_FILE_L
#define HANDLE_FROM_CENTER_HEAD_FILE_L

#include "Stdafx.h"

class CHandleFromCenter
{
	//�ַܷ�����
public:
	static bool HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, WORD wServiceID = 0);

	//һ���ַ�����
public:
	//ע��ģ��
	static bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�б�����
	static bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��ת����
	static bool OnTCPSocketMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//���崦��
public:

};

#endif