#include "StdAfx.h"
#include "ModuleInfoManager.h"

//////////////////////////////////////////////////////////////////////////////////
#include <WinSock2.h>
#include <iostream>
using namespace std;
//#pragma comment(lib, "ws2_32.lib");


//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameItemMap::CGameItemMap()
{
}

//��������
CGameItemMap::~CGameItemMap()
{
	//��������
	DWORD wModuleID=0;
	tagGameGame * pGameGameItem=NULL;
	POSITION Position=m_GameModuleInfoMap.GetStartPosition();

	//ɾ������
	while (Position!=NULL)
	{
		m_GameModuleInfoMap.GetNextAssoc(Position,wModuleID,pGameGameItem);
		SafeDelete(pGameGameItem);
	}

	//ɾ������
	for (INT_PTR i=0;i<m_GameModuleInfoArray.GetCount();i++)
	{
		pGameGameItem=m_GameModuleInfoArray[i];
		SafeDelete(pGameGameItem);
	}

	//ɾ������
	m_GameModuleInfoMap.RemoveAll();
	m_GameModuleInfoArray.RemoveAll();

	return;
}

//��������
bool CGameItemMap::ResetModuleInfo()
{
	//��������
	DWORD wModuleID=0;
	tagGameGame * pGameGameItem=NULL;
	POSITION Position=m_GameModuleInfoMap.GetStartPosition();

	//ɾ������
	while (Position!=NULL)
	{
		m_GameModuleInfoMap.GetNextAssoc(Position,wModuleID,pGameGameItem);
		m_GameModuleInfoArray.Add(pGameGameItem);
	}

	//ɾ������
	m_GameModuleInfoMap.RemoveAll();

	return true;
}

//ɾ������
bool CGameItemMap::DeleteModuleInfo(DWORD wModuleID)
{
	//��������
	tagGameGame * pGameGameItem=NULL;
	if (m_GameModuleInfoMap.Lookup(wModuleID,pGameGameItem)==FALSE) return false;

	//ɾ������
	m_GameModuleInfoMap.RemoveKey(wModuleID);
	m_GameModuleInfoArray.Add(pGameGameItem);

	//���ñ���
	ZeroMemory(pGameGameItem,sizeof(tagGameGame));

	return true;
}

//��������
bool CGameItemMap::InsertModuleInfo(tagGameGame * pGameGameItem)
{
	//Ч�����
	ASSERT(pGameGameItem!=NULL);
	if (pGameGameItem==NULL) return false;

	//�����ִ�
	DWORD wGameID=pGameGameItem->wGameID;
	tagGameGame * pGameModuleInsert=SearchModuleInfo(wGameID);

	//�����ж�
	if (pGameModuleInsert==NULL)
	{
		//��������
		pGameModuleInsert=CreateModuleInfo();

		//����ж�
		if (pGameModuleInsert==NULL)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//��������
	m_GameModuleInfoMap[wGameID]=pGameModuleInsert;
	CopyMemory(pGameModuleInsert,pGameGameItem,sizeof(tagGameGame));

	return true;
}

//��ȡ��Ŀ
DWORD CGameItemMap::GetModuleInfoCount()
{
	return (DWORD)(m_GameModuleInfoMap.GetCount());
}

//��������
tagGameGame * CGameItemMap::SearchModuleInfo(DWORD wModuleID)
{
	tagGameGame * pGameGameItem=NULL;
	m_GameModuleInfoMap.Lookup(wModuleID,pGameGameItem);
	return pGameGameItem;
}

//��������
tagGameGame * CGameItemMap::CreateModuleInfo()
{
	//��������
	tagGameGame * pGameGameItem=NULL;

	//��������
	try
	{
		INT_PTR nArrayCount=m_GameModuleInfoArray.GetCount();
		if (nArrayCount>0)
		{
			pGameGameItem=m_GameModuleInfoArray[nArrayCount-1];
			m_GameModuleInfoArray.RemoveAt(nArrayCount-1);
		}
		else
		{
			pGameGameItem=new tagGameGame;
			if (pGameGameItem==NULL) return NULL;
		}
	}
	catch (...) { return false; }

	//���ñ���
	ZeroMemory(pGameGameItem,sizeof(tagGameGame));

	return pGameGameItem;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CModuleInfoManager::CModuleInfoManager()
{
}

//��������
CModuleInfoManager::~CModuleInfoManager()
{
}

//ע��ģ��
bool CModuleInfoManager::RegisterGameModule(LPCTSTR pszModuleName)
{
	return true;
}

//ע��ģ��
bool CModuleInfoManager::UnRegisterGameModule(LPCTSTR pszModuleName)
{
	return true;
}

//����ģ��
bool CModuleInfoManager::LoadGameModuleInfo(CGameItemMap & ModuleInfoBuffer)
{
	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//��������
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("LoadGameModuleInfo ���� PlatformDBModule ����ʧ��"),MB_ICONERROR);
		return false;
	}

	//�������ݿ�
	try
	{
		//��������
		PlatformDBModule->SetConnectionInfo(1, _TEST);

		//��������
		PlatformDBModule->OpenConnection();
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//��ȡ�б�
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("byMystery"), _MYSTERY);
		PlatformDBAide.AddParameter(TEXT("IP"), _GAME_SERVER_ADDR);
		CTraceService::TraceString(_GAME_SERVER_ADDR, TraceLevel_Normal);
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadGameGameItem"),true)==DB_SUCCESS)
		{
			//����б�
			ModuleInfoBuffer.ResetModuleInfo();

			//��ȡ�б�
			while (PlatformDBModule->IsRecordsetEnd()==false)
			{
				//��������
				tagGameGame GameModuleInfo;
				ZeroMemory(&GameModuleInfo,sizeof(GameModuleInfo));

				//ģ������
				GameModuleInfo.wGameID=PlatformDBAide.GetValue_DWORD(TEXT("GameID"));
				PlatformDBAide.GetValue_String(TEXT("GameName"),GameModuleInfo.szGameName,CountArray(GameModuleInfo.szGameName));

				//��Ϸ����ַ
				PlatformDBAide.GetValue_String(TEXT("GameServerAddr"),GameModuleInfo.szGameServerAddr,CountArray(GameModuleInfo.szGameServerAddr));

				//��Ϸ����
				PlatformDBAide.GetValue_String(TEXT("ServerDLLName"),GameModuleInfo.szDLLName,CountArray(GameModuleInfo.szDLLName));
	
				//���ذ汾
				LPCTSTR pszServerDLLName=GameModuleInfo.szDLLName;
				CWHService::GetModuleVersion(pszServerDLLName,GameModuleInfo.dwNativeVersion);

				//�б���
				ModuleInfoBuffer.InsertModuleInfo(&GameModuleInfo);

				//�ƶ���¼
				PlatformDBModule->MoveToNext();
			}	
		}
		
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

	return false;
}

//ģ������
bool CModuleInfoManager::GetGameServiceAttrib(LPCTSTR pszModuleName, tagGameServiceAttrib & GameServiceAttrib)
{
	//���ñ���
	ZeroMemory(&GameServiceAttrib,sizeof(GameServiceAttrib));

	//��Ϸģ��
	CGameServiceManagerHelper GameServiceManager;
	GameServiceManager.SetModuleCreateInfo(pszModuleName,GAME_SERVICE_CREATE_NAME);

	//����ģ��
	if (GameServiceManager.CreateInstance()==true)
	{
		GameServiceManager->GetServiceAttrib(GameServiceAttrib);
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////
