#pragma once

//////////////////////////////////////////////////////////////////////////////////
//MFC 文件
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
#include "../../../../share/依赖项/全局定义/Macro.h"
#include "../../全局定义/Setting.h"
#include "../../全局定义/Define.h"
#include "../../全局定义/Struct.h"

//结构文件
#include "../../../../share/依赖项/全局定义/Packet.h"

//模板文件
#include "../../../../share/依赖项/全局定义/Array.h"

//配置文件
#include "../../../../share/依赖项/全局定义/Version.h"

/*** Other ***/
//消息定义
#include "..\..\消息定义\CMD_Correspond.h"
#include "..\..\消息定义\CMD_GameServer.h"
#include "..\..\消息定义\CMD_LogonServer.h"

//组件定义
#include "../../../../share\依赖项\服务核心\ServiceCoreHead.h"
#include "../../../../share\依赖项\服务核心\WHIniData.h"
#include "../../../../share\依赖项\服务核心\WHModule.h"
#include "../../../../share\依赖项\服务核心\log.h"

#include "../../../../share\依赖项\内核引擎\KernelEngineHead.h"


//////////////////////////////////////////////////////////////////////////////////
//链接代码
#pragma comment (lib,"../../../../share/依赖项/链接库/ServiceCore.lib")
#pragma comment (lib,"../../../../share/依赖项/链接库/KernelEngine.lib")
