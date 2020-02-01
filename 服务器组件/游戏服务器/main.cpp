#include "Stdafx.h"
#include "GameCtrl.h"


int main(int argc, char **argv)
{
	//设置异常接口
	SET_DEFULTER_HANDLER();

	//显示窗口
	CGameCtrl	GameCtrl("1.so");	
	GameCtrl.StartService();
	while(true)
	{
		Sleep(1000000);
	}

	return 0;
}