#ifndef GOLD_ROOM_MANAGER_H
#define GOLD_ROOM_MANAGER_H
#include <vector>
#include "stdafx.h"
#include <map>

class CGoldRoomManager
{
private:
	static std::map<BYTE,  STR_CMD_GC_USER_GOLD_INFO> s_RoomInfo;   //房间数据 [房间等级, 房间信息> 

public:
	//初始化
	static void Init(int kindid);

public:
	//获取所有房间数据
	static std::vector<STR_CMD_GC_USER_GOLD_INFO>  GetAllRoomInfo();

	//获取指定的房间等级的 房间数据
	static STR_CMD_GC_USER_GOLD_INFO GetRoomInfo(BYTE level);

};

#endif