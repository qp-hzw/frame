#ifndef GAME_SERVER_SERVER_DLG_HEAD_FILE
#define GAME_SERVER_SERVER_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ServiceUnits.h"

//////////////////////////////////////////////////////////////////////////////////

//主对话框
class CGameServerDlg : public CDialog, public IServiceUnitsSink
{
	//组件变量
protected:
	CServiceUnits					m_ServiceUnits;						//服务单元

	//配置参数
protected:
	bool							m_bAutoControl;						//自动控制
	bool							m_bOptionSuccess;					//配置标志
	bool							m_bQuit;							//退出标识
	tagModuleInitParameter			m_ModuleInitParameter;				//配置参数

	//函数定义
public:
	//构造函数
	CGameServerDlg();
	//析构函数
	virtual ~CGameServerDlg();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual VOID OnOK();
	//取消函数
	virtual VOID OnCancel();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//服务接口
public:
	//服务状态
	virtual VOID OnServiceUnitsStatus(enServiceStatus ServiceStatus);

	//辅助函数
protected:
	//更新标题
	VOID UpdateServerTitle(enServiceStatus ServiceStatus);
	//更新状态
	VOID UpdateParameterStatus(tagModuleInitParameter & ModuleInitParameter);

	//按钮消息
protected:
	//启动服务
	VOID OnBnClickedStartService();
	//停止服务
	VOID OnBnClickedStopService();
	//加载房间
	VOID OnBnClickedOpenServer();
	//创建房间
	VOID OnBnClickedCreateServer();

	//消息映射
public:
	//关闭询问
	BOOL OnQueryEndSession();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif