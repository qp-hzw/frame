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
#include "../../������/ȫ�ֶ���/Macro.h"
#include "../../������/ȫ�ֶ���/Define.h"
#include "../../ȫ�ֶ���/Define.h"

//�ṹ�ļ�
#include "../../ȫ�ֶ���/Struct.h"
#include "../../������/ȫ�ֶ���/Struct.h"
#include "../../������/ȫ�ֶ���/Packet.h"
#include "../../ȫ�ֶ���/Property.h"

//ģ���ļ�
#include "../../������/ȫ�ֶ���/Array.h"
#include "../../������/ȫ�ֶ���/Module.h"
#include "../../������/ȫ�ֶ���/PacketAide.h"
#include "../../ȫ�ֶ���/ServerRule.h"
#include "../../ȫ�ֶ���/RightDefine.h"

//�����ļ�
#include "../../������/ȫ�ֶ���/Version.h"


/*** Other ***/
//ƽ̨����
#include "..\..\��Ϣ����\CMD_Correspond.h"
#include "..\..\��Ϣ����\CMD_LogonServer.h"

//�������
#include "..\..\������\�������\ServiceCoreHead.h"
#include "..\..\������\�������\WHIniData.h"
#include "..\..\������\�������\WHDataLocker.h"

#include "..\..\������\�ں�����\KernelEngineHead.h"
#include "..\..\������\�ں�����\DataBaseAide.h"
#include "..\..\������\�ں�����\TraceService.h"

//////////////////////////////////////////////////////////////////////////////////
//���Ӵ���

#ifndef _DEBUG
	#pragma comment (lib,"../../������/���ӿ�/ServiceCore.lib")
	#pragma comment (lib,"../../������/���ӿ�/KernelEngine.lib")
#else

	#pragma comment (lib,"../../������/���ӿ�/ServiceCoreD.lib")
	#pragma comment (lib,"../../������/���ӿ�/KernelEngineD.lib")
#endif

//////////////////////////////////////////////////////////////////////////////////


//����̨
#include "ConsoleAdapter.h"
//CConsoleAdapter g_ConsoleAdpter;