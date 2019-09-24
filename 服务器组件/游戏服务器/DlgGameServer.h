#ifndef GAME_SERVER_SERVER_DLG_HEAD_FILE
#define GAME_SERVER_SERVER_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ServiceUnits.h"

//////////////////////////////////////////////////////////////////////////////////

//���Ի���
class CGameServerDlg : public CDialog, public IServiceUnitsSink
{
	//�������
protected:
	CServiceUnits					m_ServiceUnits;						//����Ԫ

	//���ò���
protected:
	bool							m_bAutoControl;						//�Զ�����
	bool							m_bOptionSuccess;					//���ñ�־
	bool							m_bQuit;							//�˳���ʶ
	tagModuleInitParameter			m_ModuleInitParameter;				//���ò���

	//��������
public:
	//���캯��
	CGameServerDlg();
	//��������
	virtual ~CGameServerDlg();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ����Ϣ
	virtual VOID OnOK();
	//ȡ������
	virtual VOID OnCancel();
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//����ӿ�
public:
	//����״̬
	virtual VOID OnServiceUnitsStatus(enServiceStatus ServiceStatus);

	//��������
protected:
	//���±���
	VOID UpdateServerTitle(enServiceStatus ServiceStatus);
	//����״̬
	VOID UpdateParameterStatus(tagModuleInitParameter & ModuleInitParameter);

	//��ť��Ϣ
protected:
	//��������
	VOID OnBnClickedStartService();
	//ֹͣ����
	VOID OnBnClickedStopService();
	//���ط���
	VOID OnBnClickedOpenServer();
	//��������
	VOID OnBnClickedCreateServer();

	//��Ϣӳ��
public:
	//�ر�ѯ��
	BOOL OnQueryEndSession();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif