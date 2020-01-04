#include "Stdafx.h"
#include "GameCtrl.h"


int main(int argc, char **argv)
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
