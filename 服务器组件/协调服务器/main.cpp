#include <stdio.h>
#include <tchar.h>
#include <SDKDDKVer.h>
#include <iostream>

#include "ServiceUnits.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//ÏÔÊ¾´°¿Ú
	CServiceUnits	m_ServiceUnits;	
	m_ServiceUnits.StartService();

	while(true)
	{
		Sleep(10000000000);
	}

	return 0;
}
