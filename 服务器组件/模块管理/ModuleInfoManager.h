#ifndef MODULE_INFO_MANAGER_HEAD_FILE
#define MODULE_INFO_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////////////

//数组定义
typedef CWHArray<tagGameGame *> CGameModuleInfoArray;

//索引定义
typedef CMap<DWORD,DWORD,tagGameGame *,tagGameGame *> CGameModuleInfoMap;

//////////////////////////////////////////////////////////////////////////////////

//模块数据
class MODULE_MANAGER_CLASS CGameItemMap
{
	//变量定义
public:
	CGameModuleInfoMap				m_GameModuleInfoMap;				//模块索引
	CGameModuleInfoArray			m_GameModuleInfoArray;				//模块数组

	//函数定义
public:
	//构造函数
	CGameItemMap();
	//析构函数
	virtual ~CGameItemMap();

	//管理函数
public:
	//重置数据
	bool ResetModuleInfo();
	//删除数据
	bool DeleteModuleInfo(DWORD wModuleID);
	//插入数据
	bool InsertModuleInfo(tagGameGame * pGameGameItem);

	//信息函数
public:
	//获取数目
	DWORD GetModuleInfoCount();
	//查找数据
	tagGameGame * SearchModuleInfo(DWORD wModuleID);

	//内部函数
private:
	//创建对象
	tagGameGame * CreateModuleInfo();
};

//////////////////////////////////////////////////////////////////////////////////

//模块信息
class MODULE_MANAGER_CLASS CModuleInfoManager
{
	//函数定义
public:
	//构造函数
	CModuleInfoManager();
	//析构函数
	virtual ~CModuleInfoManager();

	//注册管理
public:
	//创建模块
	bool RegisterGameModule(LPCTSTR pszModuleName);
	//注销模块
	bool UnRegisterGameModule(LPCTSTR pszModuleName);

	//数据管理
public:
	//加载模块
	bool LoadGameModuleInfo(CGameItemMap & ModuleInfoBuffer);

	//模块信息
public:
	//模块属性
	bool GetGameServiceAttrib(LPCTSTR pszModuleName, tagGameServiceAttrib & GameServiceAttrib);
};

//////////////////////////////////////////////////////////////////////////////////

#endif