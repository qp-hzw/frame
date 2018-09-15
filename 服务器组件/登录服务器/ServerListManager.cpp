#include "StdAfx.h"
#include "ServerListManager.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameTypeItem::CGameTypeItem()
{
	//���ñ���
	ZeroMemory(&m_GameType,sizeof(m_GameType));

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameKindItem::CGameKindItem()
{
	//���ñ���
	ZeroMemory(&m_GameKind,sizeof(m_GameKind));

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameNodeItem::CGameNodeItem()
{
	//���ñ���
	ZeroMemory(&m_GameNode,sizeof(m_GameNode));

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameServerItem::CGameServerItem()
{
	//���ñ���
	ZeroMemory(&m_GameServer,sizeof(m_GameServer));

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CServerListManager::CServerListManager()
{
	//��������
	m_TypeItemMap.InitHashTable(PRIME_TYPE);
	m_KindItemMap.InitHashTable(PRIME_KIND);
	m_NodeItemMap.InitHashTable(PRIME_NODE);
	m_ServerItemMap.InitHashTable(PRIME_SERVER);

	return;
}

//��������
CServerListManager::~CServerListManager()
{
	//��������
	DWORD wKey=0;

	//ɾ������
	CGameTypeItem * pGameTypeItem=NULL;
	POSITION Position=m_TypeItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_TypeItemMap.GetNextAssoc(Position,wKey,pGameTypeItem);
		SafeDelete(pGameTypeItem);
	}
	for (INT_PTR i=0;i<m_TypeItemBuffer.GetCount();i++)
	{
		pGameTypeItem=m_TypeItemBuffer[i];
		SafeDelete(pGameTypeItem);
	}
	m_TypeItemMap.RemoveAll();
	m_TypeItemBuffer.RemoveAll();

	//ɾ������
	CGameKindItem * pGameKindItem=NULL;
	Position=m_KindItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_KindItemMap.GetNextAssoc(Position,wKey,pGameKindItem);
		SafeDelete(pGameKindItem);
	}
	for (INT_PTR i=0;i<m_KindItemBuffer.GetCount();i++)
	{
		pGameKindItem=m_KindItemBuffer[i];
		SafeDelete(pGameKindItem);
	}
	m_KindItemMap.RemoveAll();
	m_KindItemBuffer.RemoveAll();

	//ɾ���ڵ�
	CGameNodeItem * pGameNodeItem=NULL;
	Position=m_NodeItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_NodeItemMap.GetNextAssoc(Position,wKey,pGameNodeItem);
		SafeDelete(pGameNodeItem);
	}
	for (INT_PTR i=0;i<m_NodeItemBuffer.GetCount();i++)
	{
		pGameNodeItem=m_NodeItemBuffer[i];
		SafeDelete(pGameNodeItem);
	}
	m_NodeItemMap.RemoveAll();
	m_NodeItemBuffer.RemoveAll();

	//ɾ������
	CGameServerItem * pGameServerItem=NULL;
	Position=m_ServerItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_ServerItemMap.GetNextAssoc(Position,wKey,pGameServerItem);
		SafeDelete(pGameServerItem);
	}
	for (INT_PTR i=0;i<m_ServerItemBuffer.GetCount();i++)
	{
		pGameServerItem=m_ServerItemBuffer[i];
		SafeDelete(pGameServerItem);
	}
	m_ServerItemMap.RemoveAll();
	m_ServerItemBuffer.RemoveAll();

	return;
}

//�����б�
VOID CServerListManager::ResetServerList()
{
	//�����б�
	DisuseKernelItem();
	DisuseServerItem();

	//�����б�
	CleanKernelItem();
	CleanServerItem();

	return;
}

//�����ں�
VOID CServerListManager::CleanKernelItem()
{
	//��������
	POSITION PositionType=m_TypeItemMap.GetStartPosition();
	POSITION PositionKind=m_KindItemMap.GetStartPosition();
	POSITION PositionNode=m_NodeItemMap.GetStartPosition();

	//��������
	while (PositionType!=NULL)
	{
		//��������
		DWORD wTypeID=0;
		CGameTypeItem * pGameTypeItem=NULL;
		m_TypeItemMap.GetNextAssoc(PositionType,wTypeID,pGameTypeItem);

		//ɾ���ж�
		if (pGameTypeItem->m_bDisuse==true)
		{
			m_TypeItemMap.RemoveKey(wTypeID);
			m_TypeItemBuffer.Add(pGameTypeItem);
		}
	}

	//��������
	while (PositionKind!=NULL)
	{
		//��������
		DWORD wKindID=0;
		CGameKindItem * pGameKindItem=NULL;
		m_KindItemMap.GetNextAssoc(PositionKind,wKindID,pGameKindItem);

		//ɾ���ж�
		if (pGameKindItem->m_bDisuse==true)
		{
			m_KindItemMap.RemoveKey(wKindID);
			m_KindItemBuffer.Add(pGameKindItem);
		}
	}

	//����ڵ�
	while (PositionNode!=NULL)
	{
		//��������
		DWORD wNodeID=0;
		CGameNodeItem * pGameNodeItem=NULL;
		m_NodeItemMap.GetNextAssoc(PositionNode,wNodeID,pGameNodeItem);

		//ɾ���ж�
		if (pGameNodeItem->m_bDisuse==true)
		{
			m_NodeItemMap.RemoveKey(wNodeID);
			m_NodeItemBuffer.Add(pGameNodeItem);
		}
	}

	return;
}

//������
VOID CServerListManager::CleanServerItem()
{
	//��������
	POSITION PositionServer=m_ServerItemMap.GetStartPosition();

	//ɾ������
	while (PositionServer!=NULL)
	{
		//��������
		DWORD wServerID=0;
		CGameServerItem * pGameServerItem=NULL;
		m_ServerItemMap.GetNextAssoc(PositionServer,wServerID,pGameServerItem);

		//ɾ���ж�
		if (pGameServerItem->m_bDisuse==true)
		{
			//ɾ������
			m_ServerItemMap.RemoveKey(wServerID);
			m_ServerItemBuffer.Add(pGameServerItem);

			//��������
			CGameKindItem * pGameKindItem=NULL;
			WORD KindID = pGameServerItem->m_GameServer.wServerID >> 16;
			if (m_KindItemMap.Lookup(KindID, pGameKindItem)==TRUE)
			{
				tagGameServer * pGameServer=&pGameServerItem->m_GameServer;
				//pGameKindItem->m_GameKind.dwOnLineCount=__max(pGameKindItem->m_GameKind.dwOnLineCount-pGameServer->dwOnLineCount,0);
				//pGameKindItem->m_GameKind.dwFullCount=__max(pGameKindItem->m_GameKind.dwFullCount-pGameServer->dwFullCount,0);
			}
		}
	}

	return;
}

//�����ں�
VOID CServerListManager::DisuseKernelItem()
{
	//��������
	DWORD wKey=0;
	//��������
	CGameTypeItem * pGameTypeItem=NULL;
	POSITION Position=m_TypeItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_TypeItemMap.GetNextAssoc(Position,wKey,pGameTypeItem);
		pGameTypeItem->m_bDisuse=true;
	}

	//��������
	CGameKindItem * pGameKindItem=NULL;
	Position=m_KindItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_KindItemMap.GetNextAssoc(Position,wKey,pGameKindItem);
		pGameKindItem->m_bDisuse=true;
	}

	//�����ڵ�
	CGameNodeItem * pGameNodeItem=NULL;
	Position=m_NodeItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_NodeItemMap.GetNextAssoc(Position,wKey,pGameNodeItem);
		pGameNodeItem->m_bDisuse=true;
	}

	return;
}

//��������
VOID CServerListManager::DisuseServerItem()
{
	//��������
	DWORD wKey=0;
	CGameServerItem * pGameServerItem=NULL;

	//��������
	POSITION Position=m_ServerItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_ServerItemMap.GetNextAssoc(Position,wKey,pGameServerItem);
		pGameServerItem->m_bDisuse=true;
	}

	return;
}

//������Ϸ������������
DWORD CServerListManager::CollectOnlineInfo()
{
	//��������
	DWORD dwOnLineCount=0L;
	POSITION PositionServer=m_ServerItemMap.GetStartPosition();

	//ö�ٷ���
	while (PositionServer!=NULL)
	{
		//��������
		DWORD wServerID=0;
		CGameServerItem * pGameServerItem=NULL;
		m_ServerItemMap.GetNextAssoc(PositionServer,wServerID,pGameServerItem);

		//ͳ������
		if (pGameServerItem != NULL)
		{
			dwOnLineCount += pGameServerItem->m_GameServer.dwOnlineCount;
		}
	}

	return dwOnLineCount;
}

//��������
bool CServerListManager::InsertGameType(tagGameType * pGameType)
{
	//Ч�����
	ASSERT(pGameType!=NULL);
	if (pGameType==NULL) return false;

	//�����ִ�
	CGameTypeItem * pGameTypeItem=NULL;
	if (m_TypeItemMap.Lookup(pGameType->wTypeID,pGameTypeItem)==FALSE)
	{
		//��������
		try
		{
			INT_PTR nStroeCount=m_TypeItemBuffer.GetCount();
			if (nStroeCount>0)
			{
				pGameTypeItem=m_TypeItemBuffer[nStroeCount-1];
				m_TypeItemBuffer.RemoveAt(nStroeCount-1);
			}
			else
			{
				pGameTypeItem=new CGameTypeItem;
				if (pGameTypeItem==NULL) return false;
			}
		}
		catch (...) { return false; }

		//���ñ���
		ZeroMemory(pGameTypeItem,sizeof(CGameTypeItem));
	}

	//��������
	pGameTypeItem->m_bDisuse=false;
	CopyMemory(&pGameTypeItem->m_GameType,pGameType,sizeof(tagGameType));

	//��������
	m_TypeItemMap[pGameType->wTypeID]=pGameTypeItem;

	return true;
}

//��������
bool CServerListManager::InsertGameKind(tagGameKind * pGameKind)
{
	//Ч�����
	ASSERT(pGameKind!=NULL);
	if (pGameKind==NULL) return false;

	//�����ִ�
	CGameKindItem * pGameKindItem=NULL;
	if (m_KindItemMap.Lookup(pGameKind->wKindID,pGameKindItem)==FALSE)
	{
		//��������
		try
		{
			INT_PTR nStroeCount=m_KindItemBuffer.GetCount();
			if (nStroeCount>0)
			{
				pGameKindItem=m_KindItemBuffer[nStroeCount-1];
				m_KindItemBuffer.RemoveAt(nStroeCount-1);
			}
			else
			{
				pGameKindItem=new CGameKindItem;
				if (pGameKindItem==NULL) return false;
			}
		}
		catch (...) { return false; }

		//���ñ���
		ZeroMemory(pGameKindItem,sizeof(CGameKindItem));
	}

	//��������
	pGameKindItem->m_bDisuse=false;
	CopyMemory(&pGameKindItem->m_GameKind,pGameKind,sizeof(tagGameKind));

	//��������
	m_KindItemMap[pGameKind->wKindID]=pGameKindItem;

	return true;
}

//����ڵ�
bool CServerListManager::InsertGameNode(tagGameNode * pGameNode)
{
	//Ч�����
	ASSERT(pGameNode!=NULL);
	if (pGameNode==NULL) return false;

	//TOODNOW added by WangChengQing 
	//1. Ϊȥ�����ݿ���PlatformDB ��GameNodeItem(��)��NodeID��Ψһkey
	//2. �޸���ͼΪ�㼶�ṹ,ȡ�����Ҵ���, ʹKindIDΪΨһ��־,
	// �� �޸�m_NodeItemMap 
	DWORD mapKey = (pGameNode->wKindID << 16) + (pGameNode->wNodeID << 8);

	//�����ִ�
	CGameNodeItem * pGameNodeItem=NULL;
	if (m_NodeItemMap.Lookup(mapKey,pGameNodeItem)==FALSE) 
	{
		//��������
		try
		{
			INT_PTR nStroeCount=m_NodeItemBuffer.GetCount();
			if (nStroeCount>0)
			{
				pGameNodeItem=m_NodeItemBuffer[nStroeCount-1];
				m_NodeItemBuffer.RemoveAt(nStroeCount-1);
			}
			else
			{
				pGameNodeItem=new CGameNodeItem;
				if (pGameNodeItem==NULL) return false;
			}
		}
		catch (...) { return false; }

		//���ñ���
		ZeroMemory(pGameNodeItem,sizeof(CGameNodeItem));
	}

	//��������
	pGameNodeItem->m_bDisuse=false;
	CopyMemory(&pGameNodeItem->m_GameNode,pGameNode,sizeof(tagGameNode));

	//��������
	m_NodeItemMap[mapKey]=pGameNodeItem;

	return true;
}

//���뷿�� -- ����ֵfalse��ʾ����Ҫ֪ͨclient,  ture��ʾ��Ҫ֪ͨ�ͻ���
byte CServerListManager::InsertGameServer(tagGameServer * pGameServer)
{
	//Ч�����
	ASSERT(pGameServer!=NULL);
	if (pGameServer==NULL) return 0;

	//���ҷ���
	CGameServerItem * pGameServerItem=NULL;

	//��ȡ����
	if (m_ServerItemMap.Lookup(pGameServer->wServerID,pGameServerItem)==FALSE) //��������
	{
		//��������
		try
		{
			INT_PTR nStroeCount=m_ServerItemBuffer.GetCount();
			if (nStroeCount>0)
			{
				pGameServerItem=m_ServerItemBuffer[nStroeCount-1];
				m_ServerItemBuffer.RemoveAt(nStroeCount-1);
			}
			else
			{
				pGameServerItem=new CGameServerItem;
				if (pGameServerItem==NULL) return 0;
			}
		}
		catch (...) { return 0; }

		//���ñ���
		ZeroMemory(pGameServerItem,sizeof(CGameServerItem));
	}
	else
	{
		//�������, ��ʹ�����µ�ֵ
		//return 1;
	}

	//��������
	pGameServerItem->m_bDisuse=false;
	CopyMemory(&pGameServerItem->m_GameServer,pGameServer,sizeof(tagGameServer));

	//��������
	m_ServerItemMap[pGameServer->wServerID]=pGameServerItem;

	return 2;
}

//ɾ������
bool CServerListManager::DeleteGameType(WORD wTypeID)
{
	//��������
	CGameTypeItem * pGameTypeItem=NULL;
	if (m_TypeItemMap.Lookup(wTypeID,pGameTypeItem)==FALSE) return false;

	//ɾ������
	m_TypeItemMap.RemoveKey(wTypeID);
	m_TypeItemBuffer.Add(pGameTypeItem);

	return true;
}

//ɾ������
bool CServerListManager::DeleteGameKind(WORD wKindID)
{
	//��������
	CGameKindItem * pGameKindItem=NULL;
	if (m_KindItemMap.Lookup(wKindID,pGameKindItem)==FALSE) return false;

	//ɾ������
	m_KindItemMap.RemoveKey(wKindID);
	m_KindItemBuffer.Add(pGameKindItem);

	//���ñ���
	ZeroMemory(pGameKindItem,sizeof(CGameKindItem));

	return true;
}

//ɾ���ڵ�
bool CServerListManager::DeleteGameNode(WORD wNodeID)
{
	//���ҽڵ�
	CGameNodeItem * pGameNodeItem=NULL;
	if (m_NodeItemMap.Lookup(wNodeID,pGameNodeItem)==FALSE) return false;

	//ɾ������
	m_NodeItemMap.RemoveKey(wNodeID);
	m_NodeItemBuffer.Add(pGameNodeItem);

	//���ñ���
	ZeroMemory(pGameNodeItem,sizeof(CGameNodeItem));

	return false;
}

//ɾ������
bool CServerListManager::DeleteGameServer(DWORD wServerID)
{
	//���ҷ���
	CGameServerItem * pGameServerItem=NULL;
	if (m_ServerItemMap.Lookup(wServerID,pGameServerItem)==FALSE) return false;

	//ɾ������
	m_ServerItemMap.RemoveKey(wServerID);
	m_ServerItemBuffer.Add(pGameServerItem);

	//��������
	CGameKindItem * pGameKindItem=NULL;
	WORD wGameKindID = (pGameServerItem->m_GameServer.wServerID >> 16);
	if (m_KindItemMap.Lookup(wGameKindID, pGameKindItem)==TRUE)
	{
		tagGameServer * pGameServer=&pGameServerItem->m_GameServer;
		//pGameKindItem->m_GameKind.dwOnLineCount= __max(pGameKindItem->m_GameKind.dwOnLineCount-pGameServer->dwOnLineCount, 0);
		//pGameKindItem->m_GameKind.dwFullCount = __max(pGameKindItem->m_GameKind.dwFullCount-pGameServer->dwFullCount, 0);
	}

	return false;
}

//ö������
CGameTypeItem * CServerListManager::EmunGameTypeItem(POSITION & Position)
{
	//��������
	DWORD wKey=0;
	CGameTypeItem * pGameTypeItem=NULL;

	//��ȡ����
	if (Position==NULL) Position=m_TypeItemMap.GetStartPosition();
	if (Position!=NULL) m_TypeItemMap.GetNextAssoc(Position,wKey,pGameTypeItem);

	return pGameTypeItem;
}

//ö������
CGameKindItem * CServerListManager::EmunGameKindItem(POSITION & Position)
{
	//��������
	DWORD wKey=0;
	CGameKindItem * pGameKindItem=NULL;

	//��ȡ����
	if (Position==NULL) Position=m_KindItemMap.GetStartPosition();
	if (Position!=NULL) m_KindItemMap.GetNextAssoc(Position,wKey,pGameKindItem);

	return pGameKindItem;
}

//ö�ٽڵ�
CGameNodeItem * CServerListManager::EmunGameNodeItem(POSITION & Position)
{
	//��������
	DWORD wKey=0;
	CGameNodeItem * pGameNodeItem=NULL;

	//��ȡ����
	if (Position==NULL) Position=m_NodeItemMap.GetStartPosition();
	if (Position!=NULL) m_NodeItemMap.GetNextAssoc(Position,wKey,pGameNodeItem);

	return pGameNodeItem;
}

//ö�ٷ���
CGameServerItem * CServerListManager::EmunGameServerItem(POSITION & Position)
{
	//��������
	DWORD wKey=0;
	CGameServerItem * pGameServerItem=NULL;

	//��ȡ����
	if (Position==NULL) Position=m_ServerItemMap.GetStartPosition();
	if (Position!=NULL) m_ServerItemMap.GetNextAssoc(Position,wKey,pGameServerItem);

	return pGameServerItem;
}

//��������
CGameTypeItem * CServerListManager::SearchGameType(WORD wTypeID)
{
	CGameTypeItem * pGameTypeItem=NULL;
	m_TypeItemMap.Lookup(wTypeID,pGameTypeItem);
	return pGameTypeItem;
}

//��������
CGameKindItem * CServerListManager::SearchGameKind(WORD wKindID)
{
	CGameKindItem * pGameKindItem=NULL;
	m_KindItemMap.Lookup(wKindID,pGameKindItem);
	return pGameKindItem;
}

//���ҽڵ�
CGameNodeItem * CServerListManager::SearchGameNode(WORD wNodeID)
{
	CGameNodeItem * pGameNodeItem=NULL;
	m_NodeItemMap.Lookup(wNodeID,pGameNodeItem);
	return pGameNodeItem;
}

//���ҷ���
CGameServerItem * CServerListManager::SearchGameServer(DWORD wServerID)
{
	CGameServerItem * pGameServerItem=NULL;
	m_ServerItemMap.Lookup(wServerID,pGameServerItem);
	return pGameServerItem;
}

//////////////////////////////////////////////////////////////////////////////////
