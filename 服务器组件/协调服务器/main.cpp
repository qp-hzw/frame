#include <stdio.h>
#include <tchar.h>
#include <SDKDDKVer.h>
#include <iostream>

#include "GameCtrl.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//�����쳣�ӿ�
	SET_DEFULTER_HANDLER();

	//��ʾ����
	CGameCtrl	GameCtrl;	
	GameCtrl.StartService();

	while(true)
	{
		Sleep(1000);
	}

	return 0;
}
