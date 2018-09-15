#ifndef DLG_SERVER_WIZARD_HEAD_FILE
#define DLG_SERVER_WIZARD_HEAD_FILE

#pragma once

#include "ModuleListControl.h"
#include "ModuleInfoManager.h"
#include "ServerInfoManager.h"
#include "ModuleManagerHead.h"


/* *******************************************************************************
** ��Ҫ����:  ������  ��Ӧ  '��������'��ť֮��ĶԻ�����
** ��ϸ����:  1. ��Ϸ�б�
**            2. ������Ϣ
** �� �� ��:  WangChengQing
** �޸ļ�¼:  1. �޸�����dll����, ʹ֮��Ӧ���°汾
**            
** ******************************************************************************/

//��˵��
class CDlgServerWizard;

//�򵼻��� -- CDlgServerWizardItem1 �� CDlgServerWizardItem2�Ļ���
class MODULE_MANAGER_CLASS CDlgServerWizardItem : public CDialog
{
	//��Ԫ����
	friend class CDlgServerWizard;

	//��������
protected:
	CDlgServerWizard *				m_pDlgServerWizard;					//��ָ��
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������

	//��������
protected:
	//���캯��
	CDlgServerWizardItem(UINT nIDTemplate);
	//��������
	virtual ~CDlgServerWizardItem();

	//���غ���
protected:
	//ȷ������
	virtual VOID OnOK();
	//ȡ����Ϣ
	virtual VOID OnCancel();

	//�ӿں���
protected:
	//��������
	virtual bool SaveInputInfo()=NULL;
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight)=NULL;

	//���ܺ���
private:
	//��������
	bool SaveItemData();
	//������
	bool ShowWizardItem(const CRect & rcRect, CWnd * pParentWnd);

	//��Ϣ����
private:
	//λ����Ϣ
	VOID OnSize(UINT nType, INT cx, INT cy);

	DECLARE_MESSAGE_MAP()
};

//��Ϸѡ�� -- ���������һ��ҳ��  CDlgServerWizard�ؼ� ѡ�������Ϸ
class MODULE_MANAGER_CLASS CDlgServerWizardItem1 : public CDlgServerWizardItem
{
	//�б����
protected:
	CStatic							m_StaticPrompt;						//��ʾ�ؼ�
	CGameItemMap					m_GameItemMap;						//ģ����Ϣ
	CModuleListControl				m_ModuleListControl;				//ģ���б�
	CModuleInfoManager				m_ModuleInfoManager;				//ģ�����

	//�������
protected:
	CGameServiceManagerHelper		m_GameServiceManager;				//��Ϸģ��

	//��������
public:
	//���캯��
	CDlgServerWizardItem1();
	//��������
	virtual ~CDlgServerWizardItem1(){};

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��ʼ����������ʼ����Ϸ����б�
	virtual BOOL OnInitDialog();

	//���غ���
public:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//���ܺ���
protected:
	//����ģ��
	bool LoadDBModuleItem();
	//�Զ���ȡServerID
	DWORD GetServerID();
	//�Զ���ȡServerPort
	WORD GetServerPort();
	//�ؼ���Ϣ
protected:
	//˫���б�
	VOID OnNMDblclkModuleList(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//�������� -- ��������ڶ���ҳ��  CDlgServerWizard�ؼ� ��Ϸ���ý���
class MODULE_MANAGER_CLASS CDlgServerWizardItem2 : public CDlgServerWizardItem
{
	//��������
public:
	//���캯��
	CDlgServerWizardItem2();
	//��������
	virtual ~CDlgServerWizardItem2(){};

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();

	//���غ���
public:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//���غ���
protected:
	//ȷ������
	virtual VOID OnOK();
	//ȡ����Ϣ
	virtual VOID OnCancel();

	//���ܺ���
private:
	//��������
	bool SaveItemData();
	//��ʾ����
	bool ShowOptionItem(const CRect & rcRect, CWnd * pParentWnd);

	//���ܺ���
protected:
	//����ؼ�
	VOID InitCtrlWindow();

	//�ؼ���Ϣ
protected:
	//������Ϣ
	VOID OnSetFocus(CWnd * pNewWnd);

	DECLARE_MESSAGE_MAP()
};

//�������� ��Dlg
class MODULE_MANAGER_CLASS CDlgServerWizard : public CDialog
{
	//��Ԫ����
	friend class CDlgServerWizardItem2;

	//��������
protected:
	WORD							m_wActiveIndex;						//�����
	CDlgServerWizardItem *			m_pWizardItem[2];					//��ָ��

	//������Ϣ
public:
	tagModuleInitParameter			m_ModuleInitParameter;				//���ò���

	//�ؼ�����
protected:
	CDlgServerWizardItem1			m_ServerWizardItem1;				//���ò���
	CDlgServerWizardItem2			m_ServerWizardItem2;				//���ò���

	//�ӿڱ���
protected:
	IGameServiceManager *			m_pIGameServiceManager;				//�������
	IGameServiceCustomRule *		m_pIGameServiceCustomRule;			//�Զ�����

	//��������
public:
	//���캯��
	CDlgServerWizard();
	//��������
	virtual ~CDlgServerWizard(){};

	//���غ���
public:
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ������
	virtual VOID OnOK();

	//���ú���
public:
	//��������
	bool CreateGameServer();
	//��������
	VOID SetWizardParameter(IGameServiceManager * pIGameServiceManager, tagGameServiceOption * pGameServiceOption, TCHAR* GameName = NULL);

	//��������
private:
	//������
	bool ActiveWizardItem(WORD wIndex);

	//��Ϣӳ��
public:
	//��һ��
	VOID OnBnClickedLast();
	//��һ��
	VOID OnBnClickedNext();
	//��ɰ�ť
	VOID OnBnClickedFinish();

	DECLARE_MESSAGE_MAP()
};

#endif