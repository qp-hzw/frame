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
#include "../../../../share/������/ȫ�ֶ���/Macro.h"
#include "../../ȫ�ֶ���/Define_base.h"
#include "../../ȫ�ֶ���/Define.h"

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

/*** Other ***/
//��Ϣ����
#include "..\..\��Ϣ����\CMD_Correspond.h"
#include "..\..\��Ϣ����\CMD_GameServer.h"
#include "..\..\��Ϣ����\CMD_LogonServer.h"

//�������
#include "..\..\���������\��Ϸ����\GameServiceHead.h"
#include "..\..\���������\��������\MatchServiceHead.h"

#include "../../../../share\������\�������\ServiceCoreHead.h"
#include "../../../../share\������\�������\WHIniData.h"
#include "../../../../share\������\�������\WHDataLocker.h"
#include "../../../../share\������\�������\WHService.h"
#include "../../../../share\������\�������\WHEncrypt.h"

#include "../../../../share\������\�ں�����\KernelEngineHead.h"
#include "../../../../share\������\�ں�����\DataBaseAide.h"
#include "../../../../share\������\�ں�����\log.h"

#include "..\..\���������\ģ�����\ModuleManagerHead.h"



//////////////////////////////////////////////////////////////////////////////////
//���Ӵ���
#pragma comment (lib,"../../../../share/������/���ӿ�/ServiceCore.lib")
#pragma comment (lib,"../../������/���ӿ�/GameService.lib")
#pragma comment (lib,"../../../../share/������/���ӿ�/KernelEngine.lib")
#pragma comment (lib,"../../������/���ӿ�/ModuleManager.lib")


//����̨
#include "ConsoleAdapter.h"
//CConsoleAdapter g_ConsoleAdpter;

//////////////////////////////////////////////////////////////////////////////////
