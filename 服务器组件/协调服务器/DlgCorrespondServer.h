#ifndef CORRESPOND_SERVER_DLG_HEAD_FILE
#define CORRESPOND_SERVER_DLG_HEAD_FILE

#include "Stdafx.h"
#include "ServiceUnits.h"

//主对话框
class CCorrespondServerDlg : public CDialog
{
	//组件变量
protected:
	CServiceUnits					m_ServiceUnits;						//服务单元
	CTraceServiceControl			m_TraceServiceControl;				//追踪窗口

	//函数定义
public:
	//构造函数
	CCorrespondServerDlg();
	//析构函数
	virtual ~CCorrespondServerDlg();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
};

#endif