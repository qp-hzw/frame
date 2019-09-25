#ifndef GAME_SERVER_SERVER_DLG_HEAD_FILE
#define GAME_SERVER_SERVER_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ServiceUnits.h"

//////////////////////////////////////////////////////////////////////////////////

//主对话框
class CGameServerDlg : public CDialog
{
	//组件变量
protected:
	CServiceUnits					m_ServiceUnits;						//服务单元

	//配置参数
protected:
	tagModuleInitParameter			m_ModuleInitParameter;				//配置参数

	//函数定义
public:
	//构造函数
	CGameServerDlg();
	//析构函数
	virtual ~CGameServerDlg();

	//辅助函数
protected:
	//更新状态
	VOID UpdateParameterStatus(tagModuleInitParameter & ModuleInitParameter);

	//按钮消息
protected:
	//加载房间
	VOID OnBnClickedOpenServer();
	//创建房间
	VOID OnBnClickedCreateServer();

public:
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif