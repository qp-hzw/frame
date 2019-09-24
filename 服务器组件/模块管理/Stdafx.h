#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WINVER				// 允许使用特定于 Windows 95 和 Windows NT 4 或更高版本的功能。
#define WINVER 0x0500		// 将此更改为针对于 Windows 98 和 Windows 2000 或更高版本的合适的值。
#endif

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows NT 4 或更高版本的功能。
#define _WIN32_WINNT 0x0500	// 将此更改为针对于 Windows 2000 或更高版本的合适的值。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用特定于 Windows 98 或更高版本的功能。
#define _WIN32_WINDOWS 0x0500 // 将此更改为针对于 Windows Me 或更高版本的合适的值。
#endif

#ifndef _WIN32_IE			// 允许使用特定于 IE 4.0 或更高版本的功能。
#define _WIN32_IE 0x0400	// 将此更改为针对于 IE 5.0 或更高版本的合适的值。
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将为显式的

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE 类
#include <afxodlgs.h>       // MFC OLE 对话框类
#include <afxdisp.h>        // MFC 自动化类
#endif // _AFX_NO_OLE_SUPPORT

#include <afxdtctl.h>		// MFC 对 Internet Explorer 4 公共控件的支持
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

/***全局文件***/
//定义文件
#include "../../../../share/依赖项/全局定义/Macro.h"
#include "../../全局定义/Define.h"
#include "../../全局定义/Define.h"

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
#include "../../../../share\依赖项\内核引擎\log.h"

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
