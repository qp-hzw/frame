#ifndef MODULE_INFO_MANAGER_HEAD_FILE
#define MODULE_INFO_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////////////

//���鶨��
typedef CWHArray<tagGameGame *> CGameModuleInfoArray;

//��������
typedef CMap<DWORD,DWORD,tagGameGame *,tagGameGame *> CGameModuleInfoMap;

//////////////////////////////////////////////////////////////////////////////////

//ģ������
class MODULE_MANAGER_CLASS CGameItemMap
{
	//��������
public:
	CGameModuleInfoMap				m_GameModuleInfoMap;				//ģ������
	CGameModuleInfoArray			m_GameModuleInfoArray;				//ģ������

	//��������
public:
	//���캯��
	CGameItemMap();
	//��������
	virtual ~CGameItemMap();

	//������
public:
	//��������
	bool ResetModuleInfo();
	//ɾ������
	bool DeleteModuleInfo(DWORD wModuleID);
	//��������
	bool InsertModuleInfo(tagGameGame * pGameGameItem);

	//��Ϣ����
public:
	//��ȡ��Ŀ
	DWORD GetModuleInfoCount();
	//��������
	tagGameGame * SearchModuleInfo(DWORD wModuleID);

	//�ڲ�����
private:
	//��������
	tagGameGame * CreateModuleInfo();
};

//////////////////////////////////////////////////////////////////////////////////

//ģ����Ϣ
class MODULE_MANAGER_CLASS CModuleInfoManager
{
	//��������
public:
	//���캯��
	CModuleInfoManager();
	//��������
	virtual ~CModuleInfoManager();

	//ע�����
public:
	//����ģ��
	bool RegisterGameModule(LPCTSTR pszModuleName);
	//ע��ģ��
	bool UnRegisterGameModule(LPCTSTR pszModuleName);

	//���ݹ���
public:
	//����ģ��
	bool LoadGameModuleInfo(CGameItemMap & ModuleInfoBuffer);

	//ģ����Ϣ
public:
	//ģ������
	bool GetGameServiceAttrib(LPCTSTR pszModuleName, tagGameServiceAttrib & GameServiceAttrib);
};

//////////////////////////////////////////////////////////////////////////////////

#endif