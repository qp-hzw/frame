#pragma once

//////////////////////////////////////////////////////////////////////////////////
//MFC �ļ�
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
#include "../../ȫ�ֶ���/Setting.h"
#include "../../ȫ�ֶ���/Define.h"
#include "../../ȫ�ֶ���/Struct.h"

//�ṹ�ļ�
#include "../../../../share/������/ȫ�ֶ���/Packet.h"

//ģ���ļ�
#include "../../../../share/������/ȫ�ֶ���/Array.h"

/*** Other ***/
//��Ϣ����
#include "..\..\��Ϣ����\CMD_Correspond.h"
#include "..\..\��Ϣ����\CMD_GameServer.h"
#include "..\..\��Ϣ����\CMD_LogonServer.h"

//�������
#include "../../../../share\������\�������\ServiceCoreHead.h"
#include "../../../../share\������\�������\WHModule.h"
#include "../../../../share\������\�������\log.h"
#include "../../../../share\������\�������\ip.h"
#include "../../../../share\������\�������\cfg.h"
#include "../../../../share\������\�������\BaseException.h"

#include "../../../../share\������\�ں�����\KernelEngineHead.h"


//////////////////////////////////////////////////////////////////////////////////
//���Ӵ���
#pragma comment (lib,"../../../../share/������/���ӿ�/ServiceCore.lib")
#pragma comment (lib,"../../../../share/������/���ӿ�/KernelEngine.lib")
