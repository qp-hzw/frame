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

//MFC �ļ�
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <AfxDisp.h>


/***ȫ���ļ�***/
//�����ļ�
#include "../../../../share/������/ȫ�ֶ���/Macro.h"
#include "../../../../share/������/ȫ�ֶ���/Define.h"

//�ṹ�ļ�
#include "../../ȫ�ֶ���/Struct.h"
#include "../../../../share/������/ȫ�ֶ���/Packet.h"
#include "../../ȫ�ֶ���/Property.h"

//ģ���ļ�
#include "../../../../share/������/ȫ�ֶ���/Array.h"
#include "../../../../share/������/ȫ�ֶ���/Module.h"
#include "../../../../share/������/ȫ�ֶ���/PacketAide.h"
#include "../../ȫ�ֶ���/ServerRule.h"
#include "../../ȫ�ֶ���/RightDefine.h"

//�����ļ�
#include "../../../../share/������/ȫ�ֶ���/Version.h"
#include "../../ȫ�ֶ���/Version.h"


//�������
#include "../../../../share\������\�������\ServiceCoreHead.h"
#include "../../../../share\������\�������\WHIniData.h"
#include "../../../../share\������\�������\WHDataLocker.h"
#include "../../../../share\������\�������\WHDataQueue.h"
#include "../../../../share\������\�������\WHService.h"


#include "../../../../share\������\�ں�����\KernelEngineHead.h"
#include "../../../../share\������\�ں�����\DataBaseAide.h"
#include "../../../../share\������\�ں�����\TraceService.h"
//////////////////////////////////////////////////////////////////////////////////
//���Ӵ���

#ifndef _DEBUG
	#pragma comment (lib,"../../../../share/������/���ӿ�/ServiceCore.lib")
	#pragma comment (lib,"../../../../share/������/���ӿ�/KernelEngine.lib")
#else
	#pragma comment (lib,"../../../../share/������/���ӿ�/ServiceCoreD.lib")
	#pragma comment (lib,"../../../../share/������/���ӿ�/KernelEngineD.lib")
#endif

//////////////////////////////////////////////////////////////////////////////////
