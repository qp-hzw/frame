#include "Stdafx.h"
#include "GameCtrl.h"


int main(int argc, char **argv)
{
	//�����쳣�ӿ�
	SET_DEFULTER_HANDLER();

	//��ʾ����
	CGameCtrl	GameCtrl;	
	GameCtrl.StartService();

	while(true)
	{
		Sleep(10000);
	}

	return 0;
}
