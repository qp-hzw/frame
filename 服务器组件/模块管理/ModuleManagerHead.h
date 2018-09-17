#ifndef MODULE_MANAGER_HEAD_HEAD_FILE
#define MODULE_MANAGER_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//�������
#include "..\..\������\�������\ServiceCoreHead.h"
#include "..\..\������\�ں�����\KernelEngineHead.h"

#include "..\..\���������\��Ϸ����\GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef MODULE_MANAGER_CLASS
	#ifdef  MODULE_MANAGER_DLL
		#define MODULE_MANAGER_CLASS _declspec(dllexport)
	#else
		#define MODULE_MANAGER_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define MODULE_MANAGER_DLL_NAME	TEXT("ModuleManager.dll")			//��� DLL ����
#else
	#define MODULE_MANAGER_DLL_NAME	TEXT("ModuleManagerD.dll")			//��� DLL ����
#endif

//////////////////////////////////////////////////////////////////////////////////
//�ṹ����

//������Ϣ GameRoomItem
struct tagSQL_In_InsertGameRoom
{
	//��������
	DWORD							dwServerID;							//�������

	//˰������
	LONG							lCellScore;							//��λ����
	WORD							wRevenueRatio;						//˰�ձ���
	SCORE							lServiceScore;						//�������

	//��������
	SCORE							lRestrictScore;						//���ƻ���
	SCORE							lMinTableScore;						//��ͻ���
	SCORE							lMinEnterScore;						//��ͻ���
	SCORE							lMaxEnterScore;						//��߻���

	//��Ա����
	BYTE							cbMinEnterMember;					//��ͻ�Ա
	BYTE							cbMaxEnterMember;					//��߻�Ա

	//��������
	DWORD							dwServerRule;						//�������
	DWORD							dwAttachUserRight;					//����Ȩ��

	//��������
	WORD							wMaxPlayer;							//�����Ŀ
	WORD							wTableCount;						//������Ŀ
	WORD							wGameServerPort;					//��Ϸ���˿�
	WORD							wServerType;						//��������
	TCHAR							szServerName[LEN_SERVER];			//��������

	//��������
	BYTE							cbDistributeRule;					//�������
	WORD							wMinDistributeUser;					//��������
	WORD							wMaxDistributeUser;					//�������
	WORD							wDistributeTimeSpace;				//������
	WORD							wDistributeDrawCount;				//�������
	WORD							wDistributeStartDelay;				//��ʼ��ʱ

	//���ݿ���Ϣ
	TCHAR							szDataBaseName[LEN_DB_NAME];		//���ݿ������Ƹ��⣩
	TCHAR							szDataBaseAddr[LEN_DB_ADDR];		//���ݿ����ӵ�ַ
	WORD							wDataBasePort;						//���ݿ�˿�
	TCHAR							szDataBaseAccount[LEN_ACCOUNTS];	//���ݿ��¼�ʺ�
	TCHAR							szDataBasePassword[LEN_MD5];		//���ݿ��¼����

	//��������
	BYTE							cbCustomRule[1024];					//�Զ�����
	TCHAR							szServiceMachine[LEN_MACHINE_ID];	//��������
};

//////////////////////////////////////////////////////////////////////////////////

//���ò���
struct tagModuleInitParameter
{
	tagGameServiceAttrib			GameServiceAttrib;					//��������
	tagGameServiceOption			GameServiceOption;					//��������
};

//////////////////////////////////////////////////////////////////////////////////

//�����ļ�
#ifndef INCLUDE_IMPORT_FILES
	#ifndef MODULE_MANAGER_DLL
		#include "ListControl.h"
		#include "DlgServerItem.h"
		#include "DlgServerWizard.h"
	
		#include "ModuleListControl.h"
		#include "ModuleDBParameter.h"
		#include "ModuleInfoManager.h"

		#include "ServerCustomRule.h"
		#include "ServerListControl.h"
		#include "ServerInfoManager.h"
	#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif