#include "StdAfx.h"
#include "ServerListControl.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CServerListControl::CServerListControl()
{
}

//��������
CServerListControl::~CServerListControl()
{
}

//�����б�
VOID CServerListControl::InitListControl()
{
	//��������
	INT nColIndex=0;

	//�����б�
	InsertColumn(nColIndex++,TEXT("�����ʶ"),LVCFMT_CENTER,80);   // 0
	InsertColumn(nColIndex++,TEXT("��������"),LVCFMT_LEFT,170);    // 1
	InsertColumn(nColIndex++,TEXT("�����˿�"),LVCFMT_CENTER,70);   // 2
	InsertColumn(nColIndex++,TEXT("�������"),LVCFMT_LEFT,140);    // 3

	return;
}

//��������
INT CServerListControl::SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex)
{
	//��������
	tagGameRoomItem * pGameServerInfo1=(tagGameRoomItem *)lParam1;
	tagGameRoomItem * pGameServerInfo2=(tagGameRoomItem *)lParam2;

	//��װ����
	bool bInstall1=(pGameServerInfo1->dwNativeVersion!=0L);
	bool bInstall2=(pGameServerInfo2->dwNativeVersion!=0L);
	if (bInstall1!=bInstall2) return (bInstall1==true)?1:-1;

	//��������
	switch (wColumnIndex)
	{
	case 0:		//�����ʶ
		{
			return (pGameServerInfo1->dwServerID>pGameServerInfo2->dwServerID)?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 1:		//��������
		{
			return lstrcmp(pGameServerInfo1->szServerName,pGameServerInfo2->szServerName);
		}
	case 2:		//�����˿�
		{
			return (pGameServerInfo1->wGameServerPort>pGameServerInfo2->wGameServerPort)?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 3:	//�������
		{
			return lstrcmp(pGameServerInfo1->szDLLName,pGameServerInfo2->szDLLName);
		}
	}

	return 0;
}

//��ȡ��ɫ
VOID CServerListControl::GetListItemColor(LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor)
{
	//��������
	ASSERT(lItemParam!=NULL);
	tagGameRoomItem * pGameServerInfo=(tagGameRoomItem *)lItemParam;

	//û�а�װ
	if (pGameServerInfo->dwNativeVersion==0L)
	{
		//������ɫ
		ListItemColor.rcTextColor=RGB(125,125,125);
		ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(50,50,50):CR_NORMAL_BK;

		return;
	}

	//������ɫ
	ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(0,0,128):CR_NORMAL_BK;
	ListItemColor.rcTextColor=(uItemStatus&ODS_SELECTED)?RGB(255,255,255):RGB(0,0,0);

	return;
}

//�����б�
bool CServerListControl::InsertServerInfo(tagGameRoomItem * pGameServerInfo)
{
	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pGameServerInfo;

	//�����ж�
	INT nInsertItem=FindItem(&FindInfo);
	if (nInsertItem!=LB_ERR) return true;

	//�����б�
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		if (i==0)
		{
			//��������
			INT nIndex=GetInsertIndex(pGameServerInfo);
			LPCTSTR pszDescribe=GetDescribeString(pGameServerInfo,i);
			nInsertItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,pszDescribe,0,0,0,(LPARAM)pGameServerInfo);
		}
		else
		{
			//�ַ�����
			SetItem(nInsertItem,i,LVIF_TEXT,GetDescribeString(pGameServerInfo,i),0,0,0,0);
		}
	}

	return true;
}

//��������
WORD CServerListControl::GetInsertIndex(tagGameRoomItem * pGameServerInfo)
{
	//��������
	INT nItemCount=GetItemCount();
	tagGameRoomItem * pGameServerTemp=NULL;

	//��ȡλ��
	for (INT i=0;i<nItemCount;i++)
	{
		//��ȡ����
		pGameServerTemp=(tagGameRoomItem *)GetItemData(i);

		//��װ�ж�
		if ((pGameServerInfo->dwNativeVersion==0)&&(pGameServerTemp->dwNativeVersion!=0))
		{
			continue;
		}

		//��������
		if (lstrcmp(pGameServerInfo->szServerName,pGameServerTemp->szServerName)<0)
		{
			return i;
		}
	}

	return nItemCount;
}

//�����ַ�
LPCTSTR CServerListControl::GetDescribeString(tagGameRoomItem * pGameServerInfo, WORD wColumnIndex)
{
	//��������
	static TCHAR szDescribe[128]=TEXT("");

	//�����ַ�
	switch (wColumnIndex)
	{
	case 0:		//�����ʶ
		{
			return _itot(pGameServerInfo->dwServerID,szDescribe,10);
		}
	case 1:		//��������
		{
			return pGameServerInfo->szServerName;
		}
	case 2:		//�����˿�
		{
			if (pGameServerInfo->wGameServerPort==0) return TEXT("-");
			return _itot(pGameServerInfo->wGameServerPort,szDescribe,10);
		}
	case 3:	//��������
		{
			return pGameServerInfo->szDLLName;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////
