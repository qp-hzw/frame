#pragma once

#include "Stdafx.h"

//程序对象
class CLogonServerApp : public CWinApp
{
	//函数定义
public:
	//构造函数 
	CLogonServerApp(){}

	//重载函数
public:
	//启动函数
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

//程序对象
extern CLogonServerApp theApp;
