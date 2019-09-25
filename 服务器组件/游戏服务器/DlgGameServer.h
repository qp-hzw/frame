#ifndef GAME_SERVER_SERVER_DLG_HEAD_FILE
#define GAME_SERVER_SERVER_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ServiceUnits.h"

//////////////////////////////////////////////////////////////////////////////////

//���Ի���
class CGameServerDlg : public CDialog
{
	//�������
protected:
	CServiceUnits					m_ServiceUnits;						//����Ԫ

	//���ò���
protected:
	tagModuleInitParameter			m_ModuleInitParameter;				//���ò���

	//��������
public:
	//���캯��
	CGameServerDlg();
	//��������
	virtual ~CGameServerDlg();

	//��������
protected:
	//����״̬
	VOID UpdateParameterStatus(tagModuleInitParameter & ModuleInitParameter);

	//��ť��Ϣ
protected:
	//���ط���
	VOID OnBnClickedOpenServer();
	//��������
	VOID OnBnClickedCreateServer();

public:
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif