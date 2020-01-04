#include <stdio.h>
#include <tchar.h>
#include <SDKDDKVer.h>
#include <iostream>

#include "GameCtrl.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//ÏÔÊ¾´°¿Ú
	CGameCtrl	GameCtrl;	
	GameCtrl.StartService();

	while(true)
	{
		Sleep(10000000000);
	}

	return 0;
}
