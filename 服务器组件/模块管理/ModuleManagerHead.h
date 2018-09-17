#ifndef MODULE_MANAGER_HEAD_HEAD_FILE
#define MODULE_MANAGER_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//组件定义
#include "..\..\依赖项\服务核心\ServiceCoreHead.h"
#include "..\..\依赖项\内核引擎\KernelEngineHead.h"

#include "..\..\服务器组件\游戏服务\GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////
//导出定义

//导出定义
#ifndef MODULE_MANAGER_CLASS
	#ifdef  MODULE_MANAGER_DLL
		#define MODULE_MANAGER_CLASS _declspec(dllexport)
	#else
		#define MODULE_MANAGER_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define MODULE_MANAGER_DLL_NAME	TEXT("ModuleManager.dll")			//组件 DLL 名字
#else
	#define MODULE_MANAGER_DLL_NAME	TEXT("ModuleManagerD.dll")			//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////////////
//结构定义

//房间信息 GameRoomItem
struct tagSQL_In_InsertGameRoom
{
	//索引变量
	DWORD							dwServerID;							//房间号码

	//税收配置
	LONG							lCellScore;							//单位积分
	WORD							wRevenueRatio;						//税收比例
	SCORE							lServiceScore;						//服务费用

	//限制配置
	SCORE							lRestrictScore;						//限制积分
	SCORE							lMinTableScore;						//最低积分
	SCORE							lMinEnterScore;						//最低积分
	SCORE							lMaxEnterScore;						//最高积分

	//会员限制
	BYTE							cbMinEnterMember;					//最低会员
	BYTE							cbMaxEnterMember;					//最高会员

	//房间配置
	DWORD							dwServerRule;						//房间规则
	DWORD							dwAttachUserRight;					//附加权限

	//房间属性
	WORD							wMaxPlayer;							//最大数目
	WORD							wTableCount;						//桌子数目
	WORD							wGameServerPort;					//游戏服端口
	WORD							wServerType;						//房间类型
	TCHAR							szServerName[LEN_SERVER];			//房间名称

	//分组设置
	BYTE							cbDistributeRule;					//分组规则
	WORD							wMinDistributeUser;					//最少人数
	WORD							wMaxDistributeUser;					//最多人数
	WORD							wDistributeTimeSpace;				//分组间隔
	WORD							wDistributeDrawCount;				//分组局数
	WORD							wDistributeStartDelay;				//开始延时

	//数据库信息
	TCHAR							szDataBaseName[LEN_DB_NAME];		//数据库名（财富库）
	TCHAR							szDataBaseAddr[LEN_DB_ADDR];		//数据库连接地址
	WORD							wDataBasePort;						//数据库端口
	TCHAR							szDataBaseAccount[LEN_ACCOUNTS];	//数据库登录帐号
	TCHAR							szDataBasePassword[LEN_MD5];		//数据库登录密码

	//数据设置
	BYTE							cbCustomRule[1024];					//自定规则
	TCHAR							szServiceMachine[LEN_MACHINE_ID];	//机器序列
};

//////////////////////////////////////////////////////////////////////////////////

//配置参数
struct tagModuleInitParameter
{
	tagGameServiceAttrib			GameServiceAttrib;					//服务属性
	tagGameServiceOption			GameServiceOption;					//服务配置
};

//////////////////////////////////////////////////////////////////////////////////

//导出文件
#ifndef INCLUDE_IMPORT_FILES
	#ifndef MODULE_MANAGER_DLL
		#include "ListControl.h"
		#include "DlgServerItem.h"
		#include "DlgServerWizard.h"
	
		#include "ModuleListControl.h"
		#include "ModuleDBParameter.h"
		#include "ModuleInfoManager.h"

		#include "ServerCustomRule.h"
		#include "ServerListControl.h"
		#include "ServerInfoManager.h"
	#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif