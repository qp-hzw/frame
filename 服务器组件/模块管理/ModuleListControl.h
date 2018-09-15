#ifndef MODULE_LIST_CONTROL_HEAD_FILE
#define MODULE_LIST_CONTROL_HEAD_FILE

#pragma once

#include "ListControl.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////////////

/*
** '��������'��ť�� ��һ�����  CDlgServerWizardItem1���·��ؼ�
*/
//ģ���б�
class MODULE_MANAGER_CLASS CModuleListControl : public CListControl
{
	//��������
public:
	//���캯��
	CModuleListControl();
	//��������
	virtual ~CModuleListControl();

	//���غ���
public:
	//�����б�
	virtual VOID InitListControl();
	//��������
	virtual INT SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex);
	//��ȡ��ɫ
	virtual VOID GetListItemColor(LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor);

	//���ܺ���
public:
	//�����б�
	bool InsertModuleInfo(tagGameGame * pGameGameItem);

	//��������
private:
	//��������
	WORD GetInsertIndex(tagGameGame * pGameGameItem);
	//�����ַ�
	LPCTSTR GetDescribeString(tagGameGame * pGameGameItem, WORD wColumnIndex);
};

//////////////////////////////////////////////////////////////////////////////////

#endif