#pragma once



#ifndef VC_EXTRALEAN

#define VC_EXTRALEAN		// �� Windows ͷ���ų�����ʹ�õ�����

#endif



// ���������ʹ��������ָ����ƽ̨֮ǰ��ƽ̨�����޸�����Ķ��塣

// �йز�ͬƽ̨����Ӧֵ��������Ϣ����ο� MSDN��

#ifndef WINVER				// ����ʹ���ض��� Windows 95 �� Windows NT 4 ����߰汾�Ĺ��ܡ�

#define WINVER 0x0500		// ���˸���Ϊ����� Windows 98 �� Windows 2000 ����߰汾�ĺ��ʵ�ֵ��

#endif



#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows NT 4 ����߰汾�Ĺ��ܡ�

#define _WIN32_WINNT 0x0500	// ���˸���Ϊ����� Windows 2000 ����߰汾�ĺ��ʵ�ֵ��

#endif						



#ifndef _WIN32_WINDOWS		// ����ʹ���ض��� Windows 98 ����߰汾�Ĺ��ܡ�

#define _WIN32_WINDOWS 0x0500 // ���˸���Ϊ����� Windows Me ����߰汾�ĺ��ʵ�ֵ��

#endif



#ifndef _WIN32_IE			// ����ʹ���ض��� IE 4.0 ����߰汾�Ĺ��ܡ�

#define _WIN32_IE 0x0400	// ���˸���Ϊ����� IE 5.0 ����߰汾�ĺ��ʵ�ֵ��

#endif



#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ĳЩ CString ���캯����Ϊ��ʽ��



#include <afxwin.h>         // MFC ��������ͱ�׼���

#include <afxext.h>         // MFC ��չ



#ifndef _AFX_NO_OLE_SUPPORT

#include <afxole.h>         // MFC OLE ��

#include <afxodlgs.h>       // MFC OLE �Ի�����

#include <afxdisp.h>        // MFC �Զ�����

#endif // _AFX_NO_OLE_SUPPORT



#include <afxdtctl.h>		// MFC �� Internet Explorer 4 �����ؼ���֧��

#ifndef _AFX_NO_AFXCMN_SUPPORT

#include <afxcmn.h>			// MFC �� Windows �����ؼ���֧��

#endif // _AFX_NO_AFXCMN_SUPPORT


/***ȫ���ļ�***/
//�����ļ�
#include "//www.45quyou.com/share/������/ȫ�ֶ���/Macro.h"
#include "//www.45quyou.com/share/������/ȫ�ֶ���/Define.h"

//�ṹ�ļ�
#include "../../ȫ�ֶ���/Struct.h"
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
#include "../../ȫ�ֶ���/Version.h"

//////////////////////////////////////////////////////////////////////////
//���ͷ�ļ�
#include "..\..\���������\��Ϸ����\GameServiceHead.h"
#include "//www.45quyou.com/share\������\�ں�����\DataBaseAide.h"


//////////////////////////////////////////////////////////////////////////////////
//���Ӵ���

#ifndef _DEBUG
		#pragma comment (lib,"//www.45quyou.com/share/������/���ӿ�/ServiceCore.lib")
		#pragma comment (lib,"../../������/���ӿ�/GameService.lib")
		#pragma comment (lib,"//www.45quyou.com/share/������/���ӿ�/KernelEngine.lib")
#else
		#pragma comment (lib,"//www.45quyou.com/share/������/���ӿ�/ServiceCoreD.lib")
		#pragma comment (lib,"../../������/���ӿ�/GameServiceD.lib")
		#pragma comment (lib,"//www.45quyou.com/share/������/���ӿ�/KernelEngineD.lib")
#endif

//////////////////////////////////////////////////////////////////////////



