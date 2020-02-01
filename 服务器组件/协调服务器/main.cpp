#include <stdio.h>
#include <tchar.h>
#include <SDKDDKVer.h>
#include <iostream>

#include "GameCtrl.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//设置异常接口
	SET_DEFULTER_HANDLER();

	//显示窗口
	CGameCtrl	GameCtrl;	
	GameCtrl.StartService();

	while(true)
	{
		Sleep(1000);
	}

	return 0;
}
