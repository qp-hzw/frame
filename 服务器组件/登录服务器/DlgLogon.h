#pragma once

#include "ServiceUnits.h"

// DlgLogon 对话框

class DlgLogon : public CDialog
{
	DECLARE_DYNAMIC(DlgLogon)

	//组件变量
protected:
	CServiceUnits					m_ServiceUnits;						//服务单元
	CTraceServiceControl			m_TraceServiceControl;				//追踪窗口

public:
	DlgLogon(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgLogon();

// 对话框数据
	enum { IDD = IDD_DLGLOGON };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
