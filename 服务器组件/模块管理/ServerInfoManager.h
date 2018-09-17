#ifndef SERVER_INFO_MANAGER_HEAD_FILE
#define SERVER_INFO_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "ModuleManagerHead.h"
#include "..\..\������\�ں�����\DataBaseAide.h"

//////////////////////////////////////////////////////////////////////////////////

//���鶨��
typedef CWHArray<tagGameRoomItem *> CGameServerInfoArray;

//��������
typedef CMap<DWORD,DWORD,tagGameRoomItem *,tagGameRoomItem *> CGameServerInfoMap;

//////////////////////////////////////////////////////////////////////////////////

//ģ������
class MODULE_MANAGER_CLASS CServerItemManager
{
	//��������
public:
	CGameServerInfoMap				m_GameServerInfoMap;				//ģ������
	CGameServerInfoArray			m_GameServerInfoArray;				//ģ������

	//��������
public:
	//���캯��
	CServerItemManager();
	//��������
	virtual ~CServerItemManager();

	//������
public:
	//��������
	bool ResetServerInfo();
	//ɾ������
	bool DeleteServerInfo(DWORD wServerID);
	//��������
	bool InsertServerInfo(tagGameRoomItem * pGameServerInfo);

	//��Ϣ����
public:
	//��ȡ��Ŀ
	DWORD GetServerInfoCount();
	//��������
	tagGameRoomItem * SearchServerInfo(DWORD wServerID);

	//�ڲ�����
private:
	//��������
	tagGameRoomItem * CreateServerInfo();
};

//////////////////////////////////////////////////////////////////////////////////

//ģ����Ϣ
class MODULE_MANAGER_CLASS CServerInfoManager
{
	//��������
public:
	//���캯��
	CServerInfoManager();
	//��������
	virtual ~CServerInfoManager();

	//���ط���
public:
	//���ط���
	bool LoadGameServerInfo(TCHAR szMachineID[LEN_MACHINE_ID], CServerItemManager & ServerInfoBuffer);
	//���ط���		TODO ��ʱ��û�õ����������������вŵ��ã���Ӧ�Ĵ�������߼�Ҳ��Ҫ�޸�
	bool LoadGameServerInfo(TCHAR szMachineID[LEN_MACHINE_ID], DWORD dwServerID, tagGameRoomItem & GameServerResult);

	//ע�����
public:
	//ɾ������
	bool DeleteGameServer(DWORD wServerID);
	//���뷿��
	bool InsertGameServer(tagGameServiceOption * pGameServerCreate);
	//�޸ķ���
	bool ModifyGameServer(tagSQL_In_InsertGameRoom * pGameServerCreate, tagGameRoomItem & GameServerResult);

	//�ڲ�����
private:
	//��������
	bool ConnectPlatformDB(CDataBaseHelper & PlatformDBModule);
	//��ȡ����
	bool ReadGameServerInfo(CDataBaseAide & PlatformDBAide, tagGameRoomItem & GameServerResult);
};

//////////////////////////////////////////////////////////////////////////////////

#endif