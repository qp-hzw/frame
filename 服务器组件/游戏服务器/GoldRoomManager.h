#ifndef GOLD_ROOM_MANAGER_H
#define GOLD_ROOM_MANAGER_H
#include <vector>
#include "stdafx.h"
#include <map>

class CGoldRoomManager
{
private:
	static std::map<BYTE,  STR_CMD_GC_USER_GOLD_INFO> s_RoomInfo;   //�������� [����ȼ�, ������Ϣ> 

public:
	//��ʼ��
	static void Init(int kindid);

public:
	//��ȡ���з�������
	static std::vector<STR_CMD_GC_USER_GOLD_INFO>  GetAllRoomInfo();

	//��ȡָ���ķ���ȼ��� ��������
	static STR_CMD_GC_USER_GOLD_INFO GetRoomInfo(BYTE level);

};

#endif