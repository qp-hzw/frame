#pragma once

#include "Stdafx.h"

//�������
class CCorrespondServerApp : public CWinApp
{
	//��������
public:
	//���캯�� 
	CCorrespondServerApp() {}

	//���غ���
public:
	//��������
	virtual BOOL InitInstance();
};

//�������
extern CCorrespondServerApp theApp;
