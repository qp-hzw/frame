#ifndef CORRESPOND_SERVER_DLG_HEAD_FILE
#define CORRESPOND_SERVER_DLG_HEAD_FILE

#include "Stdafx.h"
#include "ServiceUnits.h"

//���Ի���
class CCorrespondServerDlg : public CDialog
{
	//�������
protected:
	CServiceUnits					m_ServiceUnits;						//����Ԫ
	CTraceServiceControl			m_TraceServiceControl;				//׷�ٴ���

	//��������
public:
	//���캯��
	CCorrespondServerDlg();
	//��������
	virtual ~CCorrespondServerDlg();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
};

#endif