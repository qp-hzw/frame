#pragma once

//////////////////////////////////////////////////////////////////////////////////

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

#define _ATL_ATTRIBUTES
#define _AFX_ALL_WARNINGS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

//////////////////////////////////////////////////////////////////////////////////

//MFC 文件
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <AfxDisp.h>


/***全局文件***/
//定义文件
#include "../../../../share/依赖项/全局定义/Macro.h"
#include "../../../../share/依赖项/全局定义/Define.h"

//结构文件
#include "../../全局定义/Struct.h"
#include "../../../../share/依赖项/全局定义/Packet.h"
#include "../../全局定义/Property.h"

//模板文件
#include "../../../../share/依赖项/全局定义/Array.h"
#include "../../../../share/依赖项/全局定义/Module.h"
#include "../../../../share/依赖项/全局定义/PacketAide.h"
#include "../../全局定义/ServerRule.h"
#include "../../全局定义/RightDefine.h"

//配置文件
#include "../../../../share/依赖项/全局定义/Version.h"
#include "../../全局定义/Version.h"


//组件定义
#include "../../../../share\依赖项\服务核心\ServiceCoreHead.h"
#include "../../../../share\依赖项\服务核心\WHIniData.h"
#include "../../../../share\依赖项\服务核心\WHDataLocker.h"
#include "../../../../share\依赖项\服务核心\WHDataQueue.h"
#include "../../../../share\依赖项\服务核心\WHService.h"


#include "../../../../share\依赖项\内核引擎\KernelEngineHead.h"
#include "../../../../share\依赖项\内核引擎\DataBaseAide.h"
#include "../../../../share\依赖项\内核引擎\TraceService.h"
//////////////////////////////////////////////////////////////////////////////////
//链接代码

#ifndef _DEBUG
	#pragma comment (lib,"../../../../share/依赖项/链接库/ServiceCore.lib")
	#pragma comment (lib,"../../../../share/依赖项/链接库/KernelEngine.lib")
#else
	#pragma comment (lib,"../../../../share/依赖项/链接库/ServiceCoreD.lib")
	#pragma comment (lib,"../../../../share/依赖项/链接库/KernelEngineD.lib")
#endif

//////////////////////////////////////////////////////////////////////////////////
