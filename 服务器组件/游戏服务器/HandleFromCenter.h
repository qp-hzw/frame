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
	//ע���¼�
	static bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��ת����
	static bool OnTCPSocketMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�����¼�
	static bool OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//���崦��
public:

};

#endif