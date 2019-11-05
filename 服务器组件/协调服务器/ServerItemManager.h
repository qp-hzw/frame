#ifndef GLOBAL_USER_MANAGER_HEAD_FILE
#define GLOBAL_USER_MANAGER_HEAD_FILE

#include <vector>
#include "Stdafx.h"

//ȫ����Ϣ
class CServerItemManager
{
	//��Ա����
private:
	static std::vector<tagServerItem*>     s_server_list;                     //����������

	//
public:
	//��
	static int InsertItem(tagServerItem& ServerItem);
	//ɾ 
	static bool DeleteItem(DWORD dwSocketID);

	//��
	static tagServerItem* FindItemBySocketID(DWORD dwSocketID);
	//��
	static tagServerItem* FindItemByServerID(DWORD dwServerID);
	//��
	static tagServerItem* FindItemByKindID(DWORD dwKindID);
	
	//������
	static std::vector<tagServerItem*> FindAllGameServer();
	//������
	static std::vector<tagServerItem*> FindAllLobbyServer();
};

#endif