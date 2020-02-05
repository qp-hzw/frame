#include "GoldRoomManager.h"

std::map<BYTE,  STR_CMD_GC_USER_GOLD_INFO> CGoldRoomManager::s_RoomInfo;

void CGoldRoomManager::Init(int kindid)
{
	//打开文件
	string file_path = std::to_string(kindid)+ ".gold";
	int iRet = CWHCfg::Instance()->OpenFile(file_path);
	if(iRet != 0 )
	{
		return ;
	}

	for(int i=0; i<20; i++)
	{
		char psz[20];
		sprintf(psz, "ROOM_LEVEL_%d", i);

		STR_CMD_GC_USER_GOLD_INFO roomInfo;
		roomInfo.bGoldMod = i;
		iRet = CWHCfg::Instance()->GetItemValue(psz, "difen", roomInfo.dwScore);
		if(iRet != 0) continue;
		iRet = CWHCfg::Instance()->GetItemValue(psz, "enterGold", roomInfo.dwGold);
		if(iRet != 0) continue;

		memcpy(&s_RoomInfo[i], &roomInfo, sizeof(STR_CMD_GC_USER_GOLD_INFO));
	}

	for(auto item : s_RoomInfo)
	{
		CLog::Log(log_debug, "level %d,  score: %d,  gold: %d", item.second.bGoldMod, item.second.dwScore, item.second.dwGold);
	}

	//关闭文件
	CWHCfg::Instance()->CloseFile();
}


std::vector<STR_CMD_GC_USER_GOLD_INFO>  CGoldRoomManager::GetAllRoomInfo()
{
	std::vector<STR_CMD_GC_USER_GOLD_INFO> vec;
	for(auto item : s_RoomInfo)
	{
		vec.push_back(item.second);
	}

	return vec;
}

STR_CMD_GC_USER_GOLD_INFO CGoldRoomManager::GetRoomInfo(BYTE level)
{
	return s_RoomInfo[level];
}