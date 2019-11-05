#ifndef GLOBAL_USER_MANAGER_HEAD_FILE
#define GLOBAL_USER_MANAGER_HEAD_FILE

#include <vector>
#include "Stdafx.h"

//全局信息
class CServerItemManager
{
	//成员变量
private:
	static std::vector<tagServerItem*>     s_server_list;                     //服务器连接

	//
public:
	//增
	static int InsertItem(tagServerItem& ServerItem);
	//删 
	static bool DeleteItem(DWORD dwSocketID);

	//查
	static tagServerItem* FindItemBySocketID(DWORD dwSocketID);
	//查
	static tagServerItem* FindItemByServerID(DWORD dwServerID);
	//查
	static tagServerItem* FindItemByKindID(DWORD dwKindID);
	
	//查所有
	static std::vector<tagServerItem*> FindAllGameServer();
	//查所有
	static std::vector<tagServerItem*> FindAllLobbyServer();
};

#endif