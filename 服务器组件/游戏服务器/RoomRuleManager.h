#ifndef ROOM_RULE_MANAGER
#define ROOM_RULE_MANAGER
#include <string>
#include "Stdafx.h"
using std::string;

class RoomRuleManager
{
public:
	static rule_arry                    m_rule_arry;						//房间规则
	static BYTE							m_frame_rule_count;					//frame房间规则 条数
	static BYTE							m_subgame_rule_count;				//子游戏房间规则 条数

public:
	//获取房间配置选项
	static rule_arry GetRoomRuleSetting(){return m_rule_arry;}
	//获取房间规则
	static tagTableRule GetRoomRule(byte value[20]);

public:
	//读取通用房间配置文件
	static void ReadFrameRoomRule();
	//读取子游戏房间配置文件
	static void ReadSubGameRoomRule(int kindid);

	//辅助函数
private:
	//获取字段 对应的描述
	static string GetDescribe(string key_name);
	//根据字段名字, 为结构体对应字段赋值
	static void SetRoomRule(tagTableRule &roomrule, string key, byte value);


	//根据字段名字, 找到对应的字段值
	static string GetRoomValByKey(string key, byte value);
};

#endif