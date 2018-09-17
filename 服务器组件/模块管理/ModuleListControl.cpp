#include "StdAfx.h"
#include "ModuleListControl.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CModuleListControl::CModuleListControl()
{
}

//��������
CModuleListControl::~CModuleListControl()
{
}

//�����б�
VOID CModuleListControl::InitListControl()
{
	//��������
	INT nColIndex=0;

	//�����б�
	InsertColumn(nColIndex++,TEXT("��ϷID"),LVCFMT_CENTER,80);  // 0
	InsertColumn(nColIndex++,TEXT("��Ϸ����"),LVCFMT_CENTER,80);  // 1
	InsertColumn(nColIndex++,TEXT("���״̬"),LVCFMT_CENTER,80);  // 2
	InsertColumn(nColIndex++,TEXT("��Ϸ����ַ"),LVCFMT_LEFT,100); // 3
	InsertColumn(nColIndex++,TEXT("�������"),LVCFMT_LEFT,140);   // 4 

	return;
}

//��������
INT CModuleListControl::SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex)
{
	//��������
	tagGameGame * pGameGameItem1=(tagGameGame *)lParam1;
	tagGameGame * pGameGameItem2=(tagGameGame *)lParam2;

	//��װ����
	bool bInstall1=(pGameGameItem1->dwNativeVersion!=0L);
	bool bInstall2=(pGameGameItem2->dwNativeVersion!=0L);
	if (bInstall1!=bInstall2) return (bInstall1==true)?1:-1;

	//��������
	switch (wColumnIndex)
	{
	case 1:		//��Ϸ����
		{
			return lstrcmp(pGameGameItem1->szGameName,pGameGameItem2->szGameName);
		}
	case 2:		//���״̬
		{
			//���״̬
			BYTE cbStatus1=0;
			if (pGameGameItem1->dwNativeVersion!=0)
			{
				cbStatus1 = 1;
			}

			//���״̬
			BYTE cbStatus2=0;
			if (pGameGameItem2->dwNativeVersion!=0)
			{
				cbStatus2= 1;
			}
			return (cbStatus1>cbStatus2)?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 3:		//��Ϸ����ַ
		{
			return lstrcmp(pGameGameItem1->szGameServerAddr,pGameGameItem2->szGameServerAddr);
		}
	case 4:		//��������
		{
			return lstrcmp(pGameGameItem1->szDLLName,pGameGameItem2->szDLLName);
		}
	}

	return 0;
}

//��ȡ��ɫ
VOID CModuleListControl::GetListItemColor(LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor)
{
	//��������
	ASSERT(lItemParam!=NULL);
	tagGameGame * pGameGameItem=(tagGameGame *)lItemParam;

	//û�а�װ
	if (pGameGameItem->dwNativeVersion==0L)
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
bool CModuleListControl::InsertModuleInfo(tagGameGame * pGameGameItem)
{
	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pGameGameItem;

	//�����ж�
	INT nInsertItem=FindItem(&FindInfo);
	if (nInsertItem!=LB_ERR) return true;

	//�����б�
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		if (i==0)
		{
			//��������
			INT nIndex=GetInsertIndex(pGameGameItem);
			LPCTSTR pszDescribe=GetDescribeString(pGameGameItem,i);
			nInsertItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,pszDescribe,0,0,0,(LPARAM)pGameGameItem);
		}
		else
		{
			//�ַ�����
			SetItem(nInsertItem,i,LVIF_TEXT,GetDescribeString(pGameGameItem,i),0,0,0,0);
		}
	}

	return true;
}

//��������
WORD CModuleListControl::GetInsertIndex(tagGameGame * pGameGameItem)
{
	//��������
	INT nItemCount=GetItemCount();
	tagGameGame * pGameModuleTemp=NULL;

	//��ȡλ��
	for (INT i=0;i<nItemCount;i++)
	{
		//��ȡ����
		pGameModuleTemp=(tagGameGame *)GetItemData(i);
	
		//�����ж�
		if (lstrcmp(pGameGameItem->szGameName,pGameModuleTemp->szGameName)<0)
		{
			return i;
		}
	}

	return nItemCount;
}

//�����ַ�
LPCTSTR CModuleListControl::GetDescribeString(tagGameGame * pGameGameItem, WORD wColumnIndex)
{
	//�����ַ�
	switch (wColumnIndex)
	{
	case 0:     //��Ϸ����
		{
			CString temp;
			temp.Format(_T("%d"),pGameGameItem->wGameID);
			LPCTSTR pStr = LPCTSTR(temp);
			return pStr;
		}
	case 1:		//��Ϸ����
		{
			return pGameGameItem->szGameName;
		}
	case 2:		//���״̬
		{
			if (pGameGameItem->dwNativeVersion==0L) return TEXT("û�а�װ");
			return TEXT("�Ѿ���װ");
		}
	case 3:		//��Ϸ����ַ	modified by lizhihu
		{
			TCHAR DBAddr[LEN_DB_ADDR]=TEXT("");
			if(_T('\0') !=  pGameGameItem->szGameServerAddr[0])
			{
				memcpy(DBAddr, pGameGameItem->szGameServerAddr, LEN_DB_NAME);
			}
			else
			{
				memcpy(DBAddr, TEXT("127.0.0.1"), LEN_DB_ADDR);
			}
			CString temp;
			temp.Format(_T("%s"),DBAddr);
			LPCTSTR pStr = LPCTSTR(temp);
			return pStr;
		}
	case 4:		//��������
		{
			return pGameGameItem->szDLLName;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////
