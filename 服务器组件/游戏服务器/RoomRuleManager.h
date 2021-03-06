#ifndef ROOM_RULE_MANAGER
#define ROOM_RULE_MANAGER
#include <string>
#include <map>
#include <vector>
#include "Stdafx.h"
#include "TableFrame.h"
using std::string;

//单条房间规则
struct rule_all_item
{
	TCHAR	                       szHeadName[15];						//房间头
	TCHAR	                       szItemValue[4][10];				    //房间选择值
	TCHAR	                       szItemValueForC[4][10];				//房间选择值 -- 展现给客户端看的
};
//房间规则
struct rule_all_arry
{
	rule_all_item	                   ItemArry[20];						//房间规则
};

class CPlayer;
class RoomRuleManager
{
private:
	static ISubRoomRuleManager*            m_SubRoomRuleManager;      //子游戏 房间规则接口

	static std::map<BYTE,  STR_CMD_GC_USER_GOLD_INFO> s_RoomInfo;     //金币场 房间数据  <房间等级, 房间信息> 
	static rule_all_arry						m_rule_arry;			  //房卡场 房间配置选项

public:
	//初始化
	static void Init(int kindid, string dll_name);

private:
	//初始化 房卡场
	static void InitFK(int kindid);
	//初始化 金币场
	static void InitGold(int kindid);

	//房卡场
public:
	//获取房卡场 房间配置选项
	static rule_arry GetRoomRuleSetting();
	//获取房卡场 房间规则
	static tagTableRule GetFKRoomRule(byte value[20], byte GameMode);

	//金币场
public:
	//获取金币场 房间规则
	static tagTableRule GetGoldRoomRule(BYTE byType);
	//获取金币场 所有房间数据
	static std::vector<STR_CMD_GC_USER_GOLD_INFO>  GetAllRoomInfo();
	//获取金币场 特定房间等级的 房间数据
	static STR_CMD_GC_USER_GOLD_INFO GetRoomInfo(BYTE level);

	//比赛场
public:
	//设置比赛场规则
	static void SetMatchRule(tagTableRule& roomRule, MATCH_CONFIG *config, WORD wstage);

	//工会
public:
	//获取工会 房间规则
	static tagTableRule GetClubComRoomRule(DWORD dwClubID, DWORD dwRoomID);
	//获取工会 俱乐部规则
	static tagClubRoomRule GetClubRoomRule(DWORD dwClubID, DWORD dwRoomID, DWORD dwPassw);
	//门票检测
public:
	//检测玩家 门票是否符合条件
	static int CheckTickt(tagTableRule* rule, CPlayer* player, CTableFrame *table = NULL);

	//房间规则 存储在DB
public:
	//房间value -> DB字符串
	static string GetRuleHexString(BYTE value[20]);

	//辅助函数
private:
	//根据字段名字, 为结构体对应字段赋值
	static void SetRoomRule(tagTableRule &roomrule, string key, byte value);

	//根据字段名字, 找到对应的字段值
	static string GetRoomValByKey(string key, byte value);

	//辅助函数2
public:
	//判断当前所选局数, 是第大类
	static int GetCountType(byte value);
};

#endif