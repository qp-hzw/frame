#pragma once

//////////////////////////////////////////////////////////////////////////////////
//MFC �ļ�

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
//�����ļ�
/***ȫ���ļ�***/
//�����ļ�
#include "//www.45quyou.com/share/������/ȫ�ֶ���/Macro.h"
#include "//www.45quyou.com/share/������/ȫ�ֶ���/Define.h"
#include "../../ȫ�ֶ���/Define.h"

//�ṹ�ļ�
#include "../../ȫ�ֶ���/Struct.h"
#include "//www.45quyou.com/share/������/ȫ�ֶ���/Struct.h"
#include "//www.45quyou.com/share/������/ȫ�ֶ���/Packet.h"
#include "../../ȫ�ֶ���/Property.h"

//ģ���ļ�
#include "//www.45quyou.com/share/������/ȫ�ֶ���/Array.h"
#include "//www.45quyou.com/share/������/ȫ�ֶ���/Module.h"
#include "//www.45quyou.com/share/������/ȫ�ֶ���/PacketAide.h"
#include "../../ȫ�ֶ���/ServerRule.h"
#include "../../ȫ�ֶ���/RightDefine.h"

//�����ļ�
#include "//www.45quyou.com/share/������/ȫ�ֶ���/Version.h"


/*** Other ***/
//ƽ̨����
#include "..\..\��Ϣ����\CMD_Correspond.h"
#include "..\..\��Ϣ����\CMD_LogonServer.h"

//�������
#include "//www.45quyou.com/share\������\�������\ServiceCoreHead.h"
#include "//www.45quyou.com/share\������\�������\WHIniData.h"
#include "//www.45quyou.com/share\������\�������\WHDataLocker.h"

#include "//www.45quyou.com/share\������\�ں�����\KernelEngineHead.h"
#include "//www.45quyou.com/share\������\�ں�����\DataBaseAide.h"
#include "//www.45quyou.com/share\������\�ں�����\TraceService.h"

//////////////////////////////////////////////////////////////////////////////////
//���Ӵ���

#ifndef _DEBUG
	#pragma comment (lib,"//www.45quyou.com/share/������/���ӿ�/ServiceCore.lib")
	#pragma comment (lib,"//www.45quyou.com/share/������/���ӿ�/KernelEngine.lib")
#else

	#pragma comment (lib,"//www.45quyou.com/share/������/���ӿ�/ServiceCoreD.lib")
	#pragma comment (lib,"//www.45quyou.com/share/������/���ӿ�/KernelEngineD.lib")
#endif

//////////////////////////////////////////////////////////////////////////////////


//����̨
#include "ConsoleAdapter.h"
//CConsoleAdapter g_ConsoleAdpter;