#ifndef MATCH_SERVICE_HEAD_HEAD_FILE
#define MATCH_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//��Ϣ����
#include "..\..\��Ϣ����\CMD_Commom.h"
#include "..\..\��Ϣ����\CMD_Correspond.h"
#include "..\..\��Ϣ����\CMD_GameServer.h"

//ƽ̨�ļ�
#include "..\..\���������\��Ϸ����\GameServiceHead.h"
#include "//www.45quyou.com/share\������\�������\ServiceCoreHead.h"
#include "//www.45quyou.com/share\������\�ں�����\KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef MATCH_SERVICE_CLASS
	#ifdef  MATCH_SERVICE_DLL
		#define MATCH_SERVICE_CLASS _declspec(dllexport)
	#else
		#define MATCH_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define MATCH_SERVICE_DLL_NAME	TEXT("MatchService.dll")			//�������
#else
	#define MATCH_SERVICE_DLL_NAME	TEXT("MatchServiceD.dll")			//�������
#endif

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
DECLARE_MODULE_HELPER(GameMatchServiceManager,MATCH_SERVICE_DLL_NAME,"CreateGameMatchServiceManager")

//////////////////////////////////////////////////////////////////////////////////

#endif