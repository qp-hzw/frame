#include "Stdafx.h"
#include "ServiceUnits.h"


int main(int argc, char **argv)
{
	//��ʾ����
	CServiceUnits	m_ServiceUnits;	
	m_ServiceUnits.StartService();

	while(true)
	{
		Sleep(10000000000);
	}

	return 0;
}
