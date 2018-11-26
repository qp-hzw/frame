#ifndef MATCH_SERVICE_HEAD_HEAD_FILE
#define MATCH_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//消息定义
#include "..\..\消息定义\CMD_Commom.h"
#include "..\..\消息定义\CMD_Correspond.h"
#include "..\..\消息定义\CMD_GameServer.h"

//平台文件
#include "..\..\服务器组件\游戏服务\GameServiceHead.h"
#include "//www.45quyou.com/share\依赖项\服务核心\ServiceCoreHead.h"
#include "//www.45quyou.com/share\依赖项\内核引擎\KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////////////
//导出定义

//导出定义
#ifndef MATCH_SERVICE_CLASS
	#ifdef  MATCH_SERVICE_DLL
		#define MATCH_SERVICE_CLASS _declspec(dllexport)
	#else
		#define MATCH_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define MATCH_SERVICE_DLL_NAME	TEXT("MatchService.dll")			//组件名字
#else
	#define MATCH_SERVICE_DLL_NAME	TEXT("MatchServiceD.dll")			//组件名字
#endif

//////////////////////////////////////////////////////////////////////////////////

//游戏服务
DECLARE_MODULE_HELPER(GameMatchServiceManager,MATCH_SERVICE_DLL_NAME,"CreateGameMatchServiceManager")

//////////////////////////////////////////////////////////////////////////////////

#endif