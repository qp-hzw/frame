#include "StdAfx.h"
#include "ModuleListControl.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CModuleListControl::CModuleListControl()
{
}

//析构函数
CModuleListControl::~CModuleListControl()
{
}

//配置列表
VOID CModuleListControl::InitListControl()
{
	//变量定义
	INT nColIndex=0;

	//配置列表
	InsertColumn(nColIndex++,TEXT("游戏ID"),LVCFMT_CENTER,80);  // 0
	InsertColumn(nColIndex++,TEXT("游戏名字"),LVCFMT_CENTER,80);  // 1
	InsertColumn(nColIndex++,TEXT("组件状态"),LVCFMT_CENTER,80);  // 2
	InsertColumn(nColIndex++,TEXT("游戏服地址"),LVCFMT_LEFT,100); // 3
	InsertColumn(nColIndex++,TEXT("组件名称"),LVCFMT_LEFT,140);   // 4 

	return;
}

//子项排序
INT CModuleListControl::SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex)
{
	//变量定义
	tagGameGame * pGameGameItem1=(tagGameGame *)lParam1;
	tagGameGame * pGameGameItem2=(tagGameGame *)lParam2;

	//安装排序
	bool bInstall1=(pGameGameItem1->dwNativeVersion!=0L);
	bool bInstall2=(pGameGameItem2->dwNativeVersion!=0L);
	if (bInstall1!=bInstall2) return (bInstall1==true)?1:-1;

	//子项排序
	switch (wColumnIndex)
	{
	case 1:		//游戏名字
		{
			return lstrcmp(pGameGameItem1->szGameName,pGameGameItem2->szGameName);
		}
	case 2:		//组件状态
		{
			//组件状态
			BYTE cbStatus1=0;
			if (pGameGameItem1->dwNativeVersion!=0)
			{
				cbStatus1 = 1;
			}

			//组件状态
			BYTE cbStatus2=0;
			if (pGameGameItem2->dwNativeVersion!=0)
			{
				cbStatus2= 1;
			}
			return (cbStatus1>cbStatus2)?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 3:		//游戏服地址
		{
			return lstrcmp(pGameGameItem1->szGameServerAddr,pGameGameItem2->szGameServerAddr);
		}
	case 4:		//服务器名
		{
			return lstrcmp(pGameGameItem1->szDLLName,pGameGameItem2->szDLLName);
		}
	}

	return 0;
}

//获取颜色
VOID CModuleListControl::GetListItemColor(LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor)
{
	//变量定义
	ASSERT(lItemParam!=NULL);
	tagGameGame * pGameGameItem=(tagGameGame *)lItemParam;

	//没有安装
	if (pGameGameItem->dwNativeVersion==0L)
	{
		//设置颜色
		ListItemColor.rcTextColor=RGB(125,125,125);
		ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(50,50,50):CR_NORMAL_BK;

		return;
	}

	//设置颜色
	ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(0,0,128):CR_NORMAL_BK;
	ListItemColor.rcTextColor=(uItemStatus&ODS_SELECTED)?RGB(255,255,255):RGB(0,0,0);


	return;
}

//插入列表
bool CModuleListControl::InsertModuleInfo(tagGameGame * pGameGameItem)
{
	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pGameGameItem;

	//存在判断
	INT nInsertItem=FindItem(&FindInfo);
	if (nInsertItem!=LB_ERR) return true;

	//插入列表
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		if (i==0)
		{
			//插入首项
			INT nIndex=GetInsertIndex(pGameGameItem);
			LPCTSTR pszDescribe=GetDescribeString(pGameGameItem,i);
			nInsertItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,pszDescribe,0,0,0,(LPARAM)pGameGameItem);
		}
		else
		{
			//字符子项
			SetItem(nInsertItem,i,LVIF_TEXT,GetDescribeString(pGameGameItem,i),0,0,0,0);
		}
	}

	return true;
}

//插入索引
WORD CModuleListControl::GetInsertIndex(tagGameGame * pGameGameItem)
{
	//变量定义
	INT nItemCount=GetItemCount();
	tagGameGame * pGameModuleTemp=NULL;

	//获取位置
	for (INT i=0;i<nItemCount;i++)
	{
		//获取数据
		pGameModuleTemp=(tagGameGame *)GetItemData(i);
	
		//名字判断
		if (lstrcmp(pGameGameItem->szGameName,pGameModuleTemp->szGameName)<0)
		{
			return i;
		}
	}

	return nItemCount;
}

//描述字符
LPCTSTR CModuleListControl::GetDescribeString(tagGameGame * pGameGameItem, WORD wColumnIndex)
{
	//构造字符
	switch (wColumnIndex)
	{
	case 0:     //游戏索引
		{
			CString temp;
			temp.Format(_T("%d"),pGameGameItem->wGameID);
			LPCTSTR pStr = LPCTSTR(temp);
			return pStr;
		}
	case 1:		//游戏名字
		{
			return pGameGameItem->szGameName;
		}
	case 2:		//组件状态
		{
			if (pGameGameItem->dwNativeVersion==0L) return TEXT("没有安装");
			return TEXT("已经安装");
		}
	case 3:		//游戏服地址	modified by lizhihu
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
	case 4:		//服务器名
		{
			return pGameGameItem->szDLLName;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////
