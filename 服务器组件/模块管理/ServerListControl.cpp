#include "StdAfx.h"
#include "ServerListControl.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CServerListControl::CServerListControl()
{
}

//析构函数
CServerListControl::~CServerListControl()
{
}

//配置列表
VOID CServerListControl::InitListControl()
{
	//变量定义
	INT nColIndex=0;

	//配置列表
	InsertColumn(nColIndex++,TEXT("房间标识"),LVCFMT_CENTER,80);   // 0
	InsertColumn(nColIndex++,TEXT("房间名字"),LVCFMT_LEFT,170);    // 1
	InsertColumn(nColIndex++,TEXT("监听端口"),LVCFMT_CENTER,70);   // 2
	InsertColumn(nColIndex++,TEXT("组件名称"),LVCFMT_LEFT,140);    // 3

	return;
}

//子项排序
INT CServerListControl::SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex)
{
	//变量定义
	tagGameRoomItem * pGameServerInfo1=(tagGameRoomItem *)lParam1;
	tagGameRoomItem * pGameServerInfo2=(tagGameRoomItem *)lParam2;

	//安装排序
	bool bInstall1=(pGameServerInfo1->dwNativeVersion!=0L);
	bool bInstall2=(pGameServerInfo2->dwNativeVersion!=0L);
	if (bInstall1!=bInstall2) return (bInstall1==true)?1:-1;

	//子项排序
	switch (wColumnIndex)
	{
	case 0:		//房间标识
		{
			return (pGameServerInfo1->dwServerID>pGameServerInfo2->dwServerID)?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 1:		//房间名字
		{
			return lstrcmp(pGameServerInfo1->szServerName,pGameServerInfo2->szServerName);
		}
	case 2:		//监听端口
		{
			return (pGameServerInfo1->wGameServerPort>pGameServerInfo2->wGameServerPort)?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 3:	//组件名称
		{
			return lstrcmp(pGameServerInfo1->szDLLName,pGameServerInfo2->szDLLName);
		}
	}

	return 0;
}

//获取颜色
VOID CServerListControl::GetListItemColor(LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor)
{
	//变量定义
	ASSERT(lItemParam!=NULL);
	tagGameRoomItem * pGameServerInfo=(tagGameRoomItem *)lItemParam;

	//没有安装
	if (pGameServerInfo->dwNativeVersion==0L)
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
bool CServerListControl::InsertServerInfo(tagGameRoomItem * pGameServerInfo)
{
	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pGameServerInfo;

	//存在判断
	INT nInsertItem=FindItem(&FindInfo);
	if (nInsertItem!=LB_ERR) return true;

	//插入列表
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		if (i==0)
		{
			//插入首项
			INT nIndex=GetInsertIndex(pGameServerInfo);
			LPCTSTR pszDescribe=GetDescribeString(pGameServerInfo,i);
			nInsertItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,pszDescribe,0,0,0,(LPARAM)pGameServerInfo);
		}
		else
		{
			//字符子项
			SetItem(nInsertItem,i,LVIF_TEXT,GetDescribeString(pGameServerInfo,i),0,0,0,0);
		}
	}

	return true;
}

//插入索引
WORD CServerListControl::GetInsertIndex(tagGameRoomItem * pGameServerInfo)
{
	//变量定义
	INT nItemCount=GetItemCount();
	tagGameRoomItem * pGameServerTemp=NULL;

	//获取位置
	for (INT i=0;i<nItemCount;i++)
	{
		//获取数据
		pGameServerTemp=(tagGameRoomItem *)GetItemData(i);

		//安装判断
		if ((pGameServerInfo->dwNativeVersion==0)&&(pGameServerTemp->dwNativeVersion!=0))
		{
			continue;
		}

		//房间名字
		if (lstrcmp(pGameServerInfo->szServerName,pGameServerTemp->szServerName)<0)
		{
			return i;
		}
	}

	return nItemCount;
}

//描述字符
LPCTSTR CServerListControl::GetDescribeString(tagGameRoomItem * pGameServerInfo, WORD wColumnIndex)
{
	//变量定义
	static TCHAR szDescribe[128]=TEXT("");

	//构造字符
	switch (wColumnIndex)
	{
	case 0:		//房间标识
		{
			return _itot(pGameServerInfo->dwServerID,szDescribe,10);
		}
	case 1:		//房间名字
		{
			return pGameServerInfo->szServerName;
		}
	case 2:		//监听端口
		{
			if (pGameServerInfo->wGameServerPort==0) return TEXT("-");
			return _itot(pGameServerInfo->wGameServerPort,szDescribe,10);
		}
	case 3:	//服务器名
		{
			return pGameServerInfo->szDLLName;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////
