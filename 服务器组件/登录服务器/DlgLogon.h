#pragma once

#include "ServiceUnits.h"

// DlgLogon �Ի���

class DlgLogon : public CDialog
{
	DECLARE_DYNAMIC(DlgLogon)

	//�������
protected:
	CServiceUnits					m_ServiceUnits;						//����Ԫ
	CTraceServiceControl			m_TraceServiceControl;				//׷�ٴ���

public:
	DlgLogon(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgLogon();

// �Ի�������
	enum { IDD = IDD_DLGLOGON };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
