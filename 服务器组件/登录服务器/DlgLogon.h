#pragma once

#include "ServiceUnits.h"

// DlgLogon 对话框

class DlgLogon : public CDialog
{
	DECLARE_DYNAMIC(DlgLogon)

	//组件变量
protected:
	CServiceUnits					m_ServiceUnits;						//服务单元

public:
	DlgLogon(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgLogon();

// 对话框数据
	enum { IDD = IDD_DLGLOGON };

	DECLARE_MESSAGE_MAP()
};
