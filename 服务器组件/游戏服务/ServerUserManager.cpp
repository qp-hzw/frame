#include "StdAfx.h"
#include "ServerUserManager.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CServerUserItem::CServerUserItem()
{
	//附加变量
	m_dwUserRight=0L;
	m_dwMasterRight=0L;

	//托管变量
	m_lTrusteeScore=0L;
	m_lRestrictScore=0L;
	m_lFrozenedScore=0L;
	m_lFrozenedScore=0L;

	//登录信息
	m_dwDBQuestID=0L;
	m_dwLogonTime=0L;
	m_dwInoutIndex=INVALID_DWORD;

	//客户类型

	m_bAndroidUser=false;

	m_bVirtualUser=false;

	//组件接口
	m_pIServerUserItemSink=NULL;

	//系统属性
	m_wBindIndex=INVALID_WORD;
	m_dwClientAddr=INADDR_NONE;
	ZeroMemory(m_szMachineID,sizeof(m_szMachineID));

	//辅助变量
	m_bClientReady=false;
	m_bModifyScore=false;
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));

	//属性变量
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	ZeroMemory(&m_UserRule,sizeof(m_UserRule));
	ZeroMemory(&m_ScoreFormer,sizeof(m_ScoreFormer));
	ZeroMemory(&m_UserProperty,sizeof(m_UserProperty));
	m_UserInfo.wTableID=INVALID_TABLE;
	m_UserInfo.wChairID=INVALID_CHAIR;

	//记录信息
	ZeroMemory(&m_RecordInfo,sizeof(m_RecordInfo));
	ZeroMemory(&m_VariationInfo,sizeof(m_VariationInfo));

	//手机定义
	m_wMobileUserRule=0;
	m_wMobileUserRule |= VIEW_MODE_PART;
	m_wMobileUserRule |= VIEW_INFO_LEVEL_1;
	m_wDeskPos=0;
	m_wDeskCount=1;

	m_bVirtualUser = false;

	m_dwOfflineGameID = 0;

	return;
}

//析构函数
CServerUserItem::~CServerUserItem()
{
}

//接口查询
VOID * CServerUserItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServerUserItem,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserService,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IServerUserItem,Guid,dwQueryVer);
	return NULL;
}

//用户胜率
WORD CServerUserItem::GetUserWinRate()
{
	//计算胜率
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwWinCount*10000L/dwPlayCount);

	return 0;
}

//用户输率
WORD CServerUserItem::GetUserLostRate()
{
	//计算输率
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwLostCount*10000L/dwPlayCount);

	return 0;
}

//用户和率
WORD CServerUserItem::GetUserDrawRate()
{
	//计算和率
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwDrawCount*10000L/dwPlayCount);

	return 0;
}

//用户逃率
WORD CServerUserItem::GetUserFleeRate()
{
	//计算逃率
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwFleeCount*10000L/dwPlayCount);

	return 0;
}

//对比帐号
bool CServerUserItem::ContrastNickName(LPCTSTR pszNickName)
{
	//效验参数
	ASSERT(pszNickName!=NULL);
	if (pszNickName==NULL) return false;

	//长度对比
	INT nContrastLen=lstrlen(pszNickName);
	INT nSourceLen=lstrlen(m_UserInfo.szNickName);

	//字符对比
	if (nContrastLen!=nSourceLen) return false;
	if (CompareString(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,pszNickName,nContrastLen,m_UserInfo.szNickName,nSourceLen)!=CSTR_EQUAL) return false;

	return true;
}

//设置状态
bool CServerUserItem::SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID, bool bNotify)
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//记录信息
	WORD wOldTableID=m_UserInfo.wTableID;
	WORD wOldChairID=m_UserInfo.wChairID;

	//设置变量
	m_UserInfo.wTableID=wTableID;
	m_UserInfo.wChairID=wChairID;
	m_UserInfo.cbUserStatus=cbUserStatus;

	//发送状态
	if (m_pIServerUserItemSink!=NULL && bNotify) m_pIServerUserItemSink->OnEventUserItemStatus(this,wOldTableID,wOldChairID);

	return true;
}

//更新用户财富信息
bool CServerUserItem::ModifyUserTreasure(DWORD dwTableID, BYTE byTableMode, BYTE byRound, SCORE lUserTreasuse, BYTE byWin) 
{
	//发送状态
	if (m_pIServerUserItemSink!=NULL) 
	{
		m_pIServerUserItemSink->OnEventModifyUserTreasure(this, dwTableID, byTableMode, byRound, lUserTreasuse, byWin);
	}

	return true;
}

//查询记录
bool CServerUserItem::QueryRecordInfo(tagVariationInfo & RecordInfo)
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//设置变量
	RecordInfo=m_RecordInfo;

	return true;
}

//提取变更
bool CServerUserItem::DistillVariation(tagVariationInfo & VariationInfo)
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//修改判断
	if (m_bModifyScore==true)
	{
		//设置变量
		VariationInfo=m_VariationInfo;
	}
	else
	{
		//设置变量
		ZeroMemory(&VariationInfo,sizeof(VariationInfo));
	}

	//还原变量
	m_bModifyScore=false;
	ZeroMemory(&m_VariationInfo,sizeof(m_VariationInfo));

	return true;
}

//解除绑定
bool CServerUserItem::DetachBindStatus()
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//用户属性
	m_bClientReady=false;

	//连接属性
	m_wBindIndex=INVALID_WORD;

	return true;
}

//银行操作
bool CServerUserItem::ModifyUserInsure(SCORE lScore, SCORE lInsure, SCORE lRevenue)
{
	//平衡效验
	/*ASSERT((m_bAndroidUser==true)||(((lScore+lInsure+lRevenue)==0L)&&(lRevenue>=0L)));
	if ((m_bAndroidUser==false)&&(((lScore+lInsure+lRevenue)!=0L)||(lRevenue<0L))) return false;*/

	//效验状态
	/*ASSERT((m_UserInfo.dwUserID!=0L)&&((m_UserInfo.lScore+m_lTrusteeScore+lScore)>=0L));
	if ((m_UserInfo.dwUserID==0L)||((m_UserInfo.lScore+m_lTrusteeScore+lScore)<0L)) return false;*/

	//设置积分
	m_UserInfo.lScore+=lScore;

	//记录信息
	m_RecordInfo.lScore+=lScore;
	m_RecordInfo.lInsure+=lInsure;
	m_RecordInfo.lRevenue+=lRevenue;

	//托管积分
	if (m_lRestrictScore>0L)
	{
		//变量定义
		SCORE lTotalScore=m_UserInfo.lScore+m_lTrusteeScore;

		//托管调整
		if (lTotalScore>m_lRestrictScore)
		{
			m_UserInfo.lScore=m_lRestrictScore;
			m_lTrusteeScore=lTotalScore-m_lRestrictScore;
		}
		else
		{
			m_lTrusteeScore=0L;
			m_UserInfo.lScore=lTotalScore;
		}
	}

	//发送状态
	ASSERT(m_pIServerUserItemSink!=NULL);
	//if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemScore(this, SCORE_REASON_INSURE);

	return true;
}

//设置参数
bool CServerUserItem::SetUserParameter(DWORD dwClientAddr, WORD wBindIndex, TCHAR szMachineID[LEN_MACHINE_ID], bool bAndroidUser, 
									   bool bClientReady, const double &dLongitude, const double &dLatitude)
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//用户属性
	m_bAndroidUser=bAndroidUser;
	m_bClientReady=bClientReady;

	//连接属性
	m_wBindIndex=wBindIndex;
	m_dwClientAddr=dwClientAddr;
	lstrcpyn(m_szMachineID,szMachineID,CountArray(m_szMachineID));

	//位置属性
	m_UserInfo.dLatitude = dLatitude;
	m_UserInfo.dLongitude = dLongitude;

	return true;
}

//修改权限
VOID CServerUserItem::ModifyUserRight( DWORD dwAddRight, DWORD dwRemoveRight, bool bGameRight)
{
	//添加权限
	m_dwUserRight |= dwAddRight;

	//删除权限
	m_dwUserRight &= ~dwRemoveRight;

	//发送状态
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemRight(this,dwAddRight,dwRemoveRight,bGameRight);

	return;
}

//重置数据
VOID CServerUserItem::ResetUserItem()
{
	//附加变量
	m_dwUserRight=0L;
	m_dwMasterRight=0L;

	//托管变量
	m_lTrusteeScore=0L;
	m_lRestrictScore=0L;
	m_lFrozenedScore=0L;
	m_lFrozenedScore=0L;

	//登录信息
	m_dwDBQuestID=0L;
	m_dwLogonTime=0L;
	m_dwInoutIndex=INVALID_DWORD;

	//系统属性
	m_wBindIndex=INVALID_WORD;
	m_dwClientAddr=INADDR_ANY;
	ZeroMemory(m_szMachineID,sizeof(m_szMachineID));

	//客户类型

	m_bAndroidUser=false;

	//辅助变量
	m_bClientReady=false;
	m_bModifyScore=false;
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));

	//属性变量
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	ZeroMemory(&m_UserRule,sizeof(m_UserRule));
	ZeroMemory(&m_ScoreFormer,sizeof(m_ScoreFormer));
	//ZeroMemory(&m_UserProperty,sizeof(m_UserProperty));
	m_UserInfo.wTableID=INVALID_TABLE;
	m_UserInfo.wChairID=INVALID_CHAIR;

	//记录信息
	ZeroMemory(&m_RecordInfo,sizeof(m_RecordInfo));
	ZeroMemory(&m_VariationInfo,sizeof(m_VariationInfo));

	//手机定义
	m_wMobileUserRule=0;
	m_wMobileUserRule |= VIEW_MODE_PART;
	m_wMobileUserRule |= VIEW_INFO_LEVEL_1;
	m_wDeskPos=0;
	m_wDeskCount=1;

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CServerUserManager::CServerUserManager()
{
	//组件接口
	m_pIServerUserItemSink=NULL;

	//设置质数
	m_UserIDMap.InitHashTable(PRIME_SERVER_USER);

	return;
}

//析构函数
CServerUserManager::~CServerUserManager()
{
	//释放用户
	for (INT_PTR i=0;i<m_UserItemStore.GetCount();i++) m_UserItemStore[i]->Release();
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++) m_UserItemArray[i]->Release();

	//删除数据
	m_UserIDMap.RemoveAll();
	m_UserItemStore.RemoveAll();
	m_UserItemArray.RemoveAll();

	return;
}

//接口查询
VOID * CServerUserManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServerUserManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IServerUserManager,Guid,dwQueryVer);
	return NULL;
}

//设置接口
bool CServerUserManager::SetServerUserItemSink(IUnknownEx * pIUnknownEx)
{
	//设置接口
	if (pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IServerUserItemSink)!=NULL);
		m_pIServerUserItemSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IServerUserItemSink);

		//成功判断
		if (m_pIServerUserItemSink==NULL) return false;
	}
	else m_pIServerUserItemSink=NULL;

	return true;
}

//枚举用户
IServerUserItem * CServerUserManager::EnumUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_UserItemArray.GetCount()) return NULL;
	return m_UserItemArray[wEnumIndex];
}

//查找用户
IServerUserItem * CServerUserManager::SearchUserItem(DWORD dwUserID)
{
	return m_UserIDMap[dwUserID];
}

//查找用户
IServerUserItem * CServerUserManager::SearchUserItem(LPCTSTR pszNickName)
{
	//变量定义
	CServerUserItem * pServerUserItem=NULL;

	//搜索用户
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		pServerUserItem=m_UserItemArray[i];
		if (pServerUserItem->ContrastNickName(pszNickName)==true) return pServerUserItem;
	}

	return NULL;
}

//删除用户
bool CServerUserManager::DeleteUserItem()
{
	//存储对象
	m_UserItemStore.Append(m_UserItemArray);

	//删除对象
	m_UserIDMap.RemoveAll();
	m_UserItemArray.RemoveAll();

	return true;
}

//删除用户
bool CServerUserManager::DeleteUserItem(IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserStatus()==US_NULL));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetUserStatus()!=US_NULL)) return false;

	//变量定义
	CServerUserItem * pTempUserItem=NULL;
	DWORD dwUserID=pIServerUserItem->GetUserID();

	//寻找对象
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		//获取用户
		pTempUserItem=m_UserItemArray[i];
		if (pIServerUserItem!=pTempUserItem) continue;

		//重置对象
		pTempUserItem->ResetUserItem();

		//删除对象
		m_UserItemArray.RemoveAt(i);
		m_UserIDMap.RemoveKey(dwUserID);
		m_UserItemStore.Add(pTempUserItem);

		return true;
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//插入用户
bool CServerUserManager::InsertUserItem(IServerUserItem * * pIServerUserResult, tagUserInfo & UserInfo, tagUserInfoPlus & UserInfoPlus)
{
	//变量定义
	CServerUserItem * pServerUserItem=NULL;

	//获取指针
	if (m_UserItemStore.GetCount()>0)
	{
		INT_PTR nItemPostion=m_UserItemStore.GetCount()-1;
		pServerUserItem=m_UserItemStore[nItemPostion];
		m_UserItemStore.RemoveAt(nItemPostion);
	}
	else
	{
		try
		{
			pServerUserItem=new CServerUserItem;
			pServerUserItem->m_pIServerUserItemSink=m_pIServerUserItemSink;
		}
		catch (...)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//原始变量
	pServerUserItem->m_ScoreFormer.lScore=UserInfo.lScore;
	pServerUserItem->m_ScoreFormer.lOpenRoomCard=UserInfo.lOpenRoomCard;
	pServerUserItem->m_ScoreFormer.dwWinCount=UserInfo.dwWinCount;
	pServerUserItem->m_ScoreFormer.dwLostCount=UserInfo.dwLostCount;
	pServerUserItem->m_ScoreFormer.dwDrawCount=UserInfo.dwDrawCount;
	pServerUserItem->m_ScoreFormer.dwFleeCount=UserInfo.dwFleeCount;
	pServerUserItem->m_ScoreFormer.dwExperience=UserInfo.dwExperience;

	//属性变量
	CopyMemory(&pServerUserItem->m_UserInfo,&UserInfo,sizeof(UserInfo));
	ZeroMemory(&pServerUserItem->m_RecordInfo,sizeof(pServerUserItem->m_RecordInfo));
	ZeroMemory(&pServerUserItem->m_VariationInfo,sizeof(pServerUserItem->m_VariationInfo));

	//登录信息
	pServerUserItem->m_dwLogonTime=UserInfoPlus.dwLogonTime;
	pServerUserItem->m_dwInoutIndex=UserInfoPlus.dwInoutIndex;

	//用户权限
	pServerUserItem->m_dwUserRight=UserInfoPlus.dwUserRight;
	pServerUserItem->m_dwMasterRight=UserInfoPlus.dwMasterRight;
	pServerUserItem->m_lRestrictScore=UserInfoPlus.lRestrictScore;

	//连接信息
	pServerUserItem->m_wBindIndex=UserInfoPlus.wBindIndex;
	pServerUserItem->m_dwClientAddr=UserInfoPlus.dwClientAddr;
	lstrcpyn(pServerUserItem->m_szMachineID,UserInfoPlus.szMachineID,CountArray(pServerUserItem->m_szMachineID));

	//辅助变量
	pServerUserItem->m_bClientReady=false;
	pServerUserItem->m_bModifyScore=false;
	pServerUserItem->m_bAndroidUser=UserInfoPlus.bAndroidUser;
	lstrcpyn(pServerUserItem->m_szLogonPass,UserInfoPlus.szPassword,CountArray(pServerUserItem->m_szLogonPass));

	//托管调整
	if ((UserInfoPlus.lRestrictScore>0L)&&(UserInfo.lScore>UserInfoPlus.lRestrictScore))
	{
		pServerUserItem->m_UserInfo.lScore=UserInfoPlus.lRestrictScore;
		pServerUserItem->m_lTrusteeScore=UserInfo.lScore-UserInfoPlus.lRestrictScore;
	}

	//插入用户
	m_UserItemArray.Add(pServerUserItem);
	m_UserIDMap[UserInfo.dwUserID]=pServerUserItem;

	//设置变量
	*pIServerUserResult=pServerUserItem;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
