#pragma once

//////////////////////////////////////////////////////////////////////////////////
//MFC 文件

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WINVER
#define WINVER 0x0500
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif						

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0500
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0400
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

#define _AFX_ALL_WARNINGS

#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxDisp.h>
#include <AfxDtctl.h>

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <AfxCmn.h>
#endif

//////////////////////////////////////////////////////////////////////////////////
//包含文件

/***全局文件***/
//定义文件
#include "//www.45quyou.com/share/依赖项/全局定义/Macro.h"
#include "//www.45quyou.com/share/依赖项/全局定义/Define.h"
#include "../../全局定义/Define.h"

//结构文件
#include "../../全局定义/Struct.h"
#include "//www.45quyou.com/share/依赖项/全局定义/Packet.h"
#include "../../全局定义/Property.h"

//模板文件
#include "//www.45quyou.com/share/依赖项/全局定义/Array.h"
#include "//www.45quyou.com/share/依赖项/全局定义/Module.h"
#include "//www.45quyou.com/share/依赖项/全局定义/PacketAide.h"
#include "../../全局定义/ServerRule.h"
#include "../../全局定义/RightDefine.h"

//配置文件
#include "//www.45quyou.com/share/依赖项/全局定义/Version.h"
#include "../../全局定义/Version.h"

/*** Other ***/
//消息定义
#include "..\..\消息定义\CMD_Correspond.h"
#include "..\..\消息定义\CMD_GameServer.h"
#include "..\..\消息定义\CMD_LogonServer.h"

//组件定义
#include "..\..\服务器组件\游戏服务\GameServiceHead.h"
#include "..\..\服务器组件\比赛服务\MatchServiceHead.h"

#include "//www.45quyou.com/share\依赖项\服务核心\ServiceCoreHead.h"
#include "//www.45quyou.com/share\依赖项\服务核心\WHIniData.h"
#include "//www.45quyou.com/share\依赖项\服务核心\WHDataLocker.h"

#include "//www.45quyou.com/share\依赖项\内核引擎\KernelEngineHead.h"
#include "//www.45quyou.com/share\依赖项\内核引擎\DataBaseAide.h"
#include "//www.45quyou.com/share\依赖项\内核引擎\TraceService.h"

#include "..\..\服务器组件\模块管理\ModuleManagerHead.h"



//////////////////////////////////////////////////////////////////////////////////
//链接代码

#ifndef _DEBUG
	#pragma comment (lib,"//www.45quyou.com/share/依赖项/链接库/ServiceCore.lib")
	#pragma comment (lib,"../../依赖项/链接库/GameService.lib")
	#pragma comment (lib,"//www.45quyou.com/share/依赖项/链接库/KernelEngine.lib")
	#pragma comment (lib,"../../依赖项/链接库/ModuleManager.lib")
#else
	#pragma comment (lib,"//www.45quyou.com/share/依赖项/链接库/ServiceCoreD.lib")
	#pragma comment (lib,"../../依赖项/链接库/GameServiceD.lib")
	#pragma comment (lib,"//www.45quyou.com/share/依赖项/链接库/KernelEngineD.lib")
	#pragma comment (lib,"../../依赖项/链接库/ModuleManagerD.lib")
#endif

//控制台
#include "ConsoleAdapter.h"
//CConsoleAdapter g_ConsoleAdpter;

//////////////////////////////////////////////////////////////////////////////////
