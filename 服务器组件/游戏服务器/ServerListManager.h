#ifndef SERVER_LIST_MANAGER_HEAD_FILE
#define SERVER_LIST_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"

//////////////////////////////////////////////////////////////////////////////////

//列表管理
namespace NServerListManager
{

	//////////////////////////////////////////////////////////////////////////////////

	//游戏子项
	class CGameListItem
	{
		//变量定义
	public:
		bool							m_bDisuse;							//废弃标志

		//函数定义
	protected:
		//构造函数
		CGameListItem() { m_bDisuse=false; }
	};

	//////////////////////////////////////////////////////////////////////////////////

	//种类结构
	class CGameTypeItem : public CGameListItem
	{
		//变量定义
	public:
		tagGameType						m_GameType;							//种类信息

		//函数定义
	public:
		//构造函数
		CGameTypeItem();
	};

	//////////////////////////////////////////////////////////////////////////////////

	//类型结构
	class CGameKindItem : public CGameListItem
	{
		//变量定义
	public:
		tagGameKind						m_GameKind;							//类型信息

		//函数定义
	public:
		//构造函数
		CGameKindItem();
	};

	//////////////////////////////////////////////////////////////////////////////////

	//节点结构
	class CGameNodeItem : public CGameListItem
	{
		//变量定义
	public:
		tagGameNode						m_GameNode;							//节点信息

		//函数定义
	public:
		//构造函数
		CGameNodeItem();
	};

	//////////////////////////////////////////////////////////////////////////////////

	//房间结构
	class CGameServerItem : public CGameListItem
	{
		//变量定义
	public:
		tagGameServer					m_GameServer;						//房间信息

		//函数定义
	public:
		//构造函数
		CGameServerItem();
	};

	//////////////////////////////////////////////////////////////////////////////////

	//数组说明
	typedef CWHArray<CGameTypeItem *> CGameTypeItemArray;
	typedef CWHArray<CGameKindItem *> CGameKindItemArray;
	typedef CWHArray<CGameNodeItem *> CGameNodeItemArray;
	typedef CWHArray<CGameServerItem *>	CGameServerItemArray;

	//索引说明
	typedef CMap<WORD,WORD,CGameTypeItem *,CGameTypeItem * &> CTypeItemMap;
	typedef CMap<WORD,WORD,CGameKindItem *,CGameKindItem * &> CKindItemMap;
	typedef CMap<WORD,WORD,CGameNodeItem *,CGameNodeItem * &> CNodeItemMap;
	typedef CMap<DWORD,DWORD,CGameServerItem *,CGameServerItem * &> CServerItemMap;

	//////////////////////////////////////////////////////////////////////////////////

	//列表服务
	class CServerListManager
	{
		//索引变量
	protected:
		CTypeItemMap					m_TypeItemMap;						//种类索引 最大为255 byte即可
		CKindItemMap					m_KindItemMap;						//类型索引 kind 8位  最大为255
		CNodeItemMap					m_NodeItemMap;						//节点索引 16位 kind + byte 最大为255
		CServerItemMap					m_ServerItemMap;					//房间索引 server 32位 

		//存储变量
	protected:
		CGameTypeItemArray				m_TypeItemBuffer;					//种类数组
		CGameKindItemArray				m_KindItemBuffer;					//类型数组
		CGameNodeItemArray				m_NodeItemBuffer;					//节点数组
		CGameServerItemArray			m_ServerItemBuffer;					//房间数组

		//函数定义
	public:
		//构造函数
		CServerListManager();
		//析构函数
		virtual ~CServerListManager();

		//管理接口
	public:
		//重置列表
		VOID ResetServerList();
		//清理内核
		VOID CleanKernelItem();
		//清理房间
		VOID CleanServerItem();
		//废弃内核
		VOID DisuseKernelItem();
		//废弃房间
		VOID DisuseServerItem();

		//插入接口
	public:
		//插入种类
		bool InsertGameType(tagGameType * pGameType);
		//插入类型
		bool InsertGameKind(tagGameKind * pGameKind);
		//插入节点
		bool InsertGameNode(tagGameNode * pGameNode);
		//插入房间
		bool InsertGameServer(tagGameServer * pGameServer);

		//删除接口
	public:
		//删除种类
		bool DeleteGameType(WORD wTypeID);
		//删除类型
		bool DeleteGameKind(WORD wKindID);
		//删除节点
		bool DeleteGameNode(WORD wNodeID, WORD wKindID);
		//删除房间
		bool DeleteGameServer(DWORD wServerID);

		//枚举接口
	public:
		//枚举种类
		CGameTypeItem * EmunGameTypeItem(POSITION & Position);
		//枚举类型
		CGameKindItem * EmunGameKindItem(POSITION & Position);
		//枚举节点
		CGameNodeItem * EmunGameNodeItem(POSITION & Position);
		//枚举房间
		CGameServerItem * EmunGameServerItem(POSITION & Position);

		//查找接口
	public:
		//查找种类
		CGameTypeItem * SearchGameType(WORD wTypeID);
		//查找类型
		CGameKindItem * SearchGameKind(WORD wKindID);
		//查找节点
		CGameNodeItem * SearchGameNode(WORD wNodeID);
		//查找房间
		CGameServerItem * SearchGameServer(DWORD wServerID);

		//数目接口
	public:
		//种类数目
		DWORD GetGameTypeCount() { return (DWORD)m_TypeItemMap.GetCount(); }
		//类型数目
		DWORD GetGameKindCount() { return (DWORD)m_KindItemMap.GetCount(); }
		//节点数目
		DWORD GetGameNodeCount() { return (DWORD)m_NodeItemMap.GetCount(); }
		//房间数目
		DWORD GetGameServerCount() { return (DWORD)m_ServerItemMap.GetCount(); }
	};

	//////////////////////////////////////////////////////////////////////////////////

};

using namespace NServerListManager;

#endif