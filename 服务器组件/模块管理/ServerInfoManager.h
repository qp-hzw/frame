#ifndef SERVER_INFO_MANAGER_HEAD_FILE
#define SERVER_INFO_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "ModuleManagerHead.h"
#include "..\..\依赖项\内核引擎\DataBaseAide.h"

//////////////////////////////////////////////////////////////////////////////////

//数组定义
typedef CWHArray<tagGameRoomItem *> CGameServerInfoArray;

//索引定义
typedef CMap<DWORD,DWORD,tagGameRoomItem *,tagGameRoomItem *> CGameServerInfoMap;

//////////////////////////////////////////////////////////////////////////////////

//模块数据
class MODULE_MANAGER_CLASS CServerItemManager
{
	//变量定义
public:
	CGameServerInfoMap				m_GameServerInfoMap;				//模块索引
	CGameServerInfoArray			m_GameServerInfoArray;				//模块数组

	//函数定义
public:
	//构造函数
	CServerItemManager();
	//析构函数
	virtual ~CServerItemManager();

	//管理函数
public:
	//重置数据
	bool ResetServerInfo();
	//删除数据
	bool DeleteServerInfo(DWORD wServerID);
	//插入数据
	bool InsertServerInfo(tagGameRoomItem * pGameServerInfo);

	//信息函数
public:
	//获取数目
	DWORD GetServerInfoCount();
	//查找数据
	tagGameRoomItem * SearchServerInfo(DWORD wServerID);

	//内部函数
private:
	//创建对象
	tagGameRoomItem * CreateServerInfo();
};

//////////////////////////////////////////////////////////////////////////////////

//模块信息
class MODULE_MANAGER_CLASS CServerInfoManager
{
	//函数定义
public:
	//构造函数
	CServerInfoManager();
	//析构函数
	virtual ~CServerInfoManager();

	//加载房间
public:
	//加载房间
	bool LoadGameServerInfo(TCHAR szMachineID[LEN_MACHINE_ID], CServerItemManager & ServerInfoBuffer);
	//加载房间		TODO 暂时都没用到，在命令行启动中才调用，对应的储存过程逻辑也需要修改
	bool LoadGameServerInfo(TCHAR szMachineID[LEN_MACHINE_ID], DWORD dwServerID, tagGameRoomItem & GameServerResult);

	//注册管理
public:
	//删除房间
	bool DeleteGameServer(DWORD wServerID);
	//插入房间
	bool InsertGameServer(tagGameServiceOption * pGameServerCreate);
	//修改房间
	bool ModifyGameServer(tagSQL_In_InsertGameRoom * pGameServerCreate, tagGameRoomItem & GameServerResult);

	//内部函数
private:
	//连接数据
	bool ConnectPlatformDB(CDataBaseHelper & PlatformDBModule);
	//读取房间
	bool ReadGameServerInfo(CDataBaseAide & PlatformDBAide, tagGameRoomItem & GameServerResult);
};

//////////////////////////////////////////////////////////////////////////////////

#endif