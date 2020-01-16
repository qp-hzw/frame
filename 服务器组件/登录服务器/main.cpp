#include "Stdafx.h"
#include "GameCtrl.h"


int main(int argc, char **argv)
{
	//设置异常接口
	SET_DEFULTER_HANDLER();

	//显示窗口
	CGameCtrl	GameCtrl;	
	GameCtrl.StartService();

	while(true)
	{
		Sleep(10000000000);
	}

	return 0;
}
