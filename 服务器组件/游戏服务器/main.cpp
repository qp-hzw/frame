#include "Stdafx.h"
#include "GameCtrl.h"


int main(int argc, char **argv)
{
	//�����쳣�ӿ�
	SET_DEFULTER_HANDLER();

	//��ʾ����
	CGameCtrl	GameCtrl("1.so");	
	GameCtrl.StartService();
	while(true)
	{
		Sleep(1000000);
	}

	return 0;
}