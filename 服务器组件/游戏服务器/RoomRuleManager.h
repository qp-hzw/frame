#ifndef ROOM_RULE_MANAGER
#define ROOM_RULE_MANAGER
#include <string>
#include <map>
#include <vector>
#include "Stdafx.h"
using std::string;


class CPlayer;
class RoomRuleManager
{
private:
	static ISubRoomRuleManager*            m_SubRoomRuleManager;      //����Ϸ �������ӿ�

	static std::map<BYTE,  STR_CMD_GC_USER_GOLD_INFO> s_RoomInfo;     //��ҳ� ��������  <����ȼ�, ������Ϣ> 
	static rule_arry						m_rule_arry;			  //������ ��������ѡ��
	static string							m_roomRuleMsg;			  //�������������

public:
	//��ʼ��
	static void Init(int kindid, string dll_name);

private:
	//��ʼ�� ������
	static void InitFK(int kindid);
	//��ʼ�� ��ҳ�
	static void InitGold(int kindid);

	//������
public:
	//��ȡ������ ��������ѡ��
	static rule_arry GetRoomRuleSetting();
	//��ȡ������ �������
	static tagTableRule GetFKRoomRule(byte value[20], byte GameMode);
	//��ȡ������ �������������
	static string GetFKRoomRuleMsg();

	//��ҳ�
public:
	//��ȡ��ҳ� �������
	static tagTableRule GetGoldRoomRule(BYTE byType);
	//��ȡ��ҳ� ���з�������
	static std::vector<STR_CMD_GC_USER_GOLD_INFO>  GetAllRoomInfo();
	//��ȡ��ҳ� �ض�����ȼ��� ��������
	static STR_CMD_GC_USER_GOLD_INFO GetRoomInfo(BYTE level);

	//������
public:
	//���ñ���������
	static void SetMatchRule(tagTableRule& roomRule, MATCH_CONFIG *config, WORD wstage);

	//��Ʊ���
public:
	//������ ��Ʊ�Ƿ��������
	static int CheckTickt(tagTableRule* rule, CPlayer* player);

	//������� �洢��DB
public:
	//����value -> DB�ַ���
	static string GetRuleHexString(STR_SUB_CG_USER_SET_ROOM_RULE rule_setting);
	//DB�ַ���  -> ����value
	static STR_SUB_CG_USER_SET_ROOM_RULE GetRuleSettingValue(string hex_string);

	//��������
private:
	//��ȡ�ֶ� ��Ӧ������
	static string GetDescribe(string key_name);
	//�����ֶ�����, Ϊ�ṹ���Ӧ�ֶθ�ֵ
	static void SetRoomRule(tagTableRule &roomrule, string key, byte value);

	//�����ֶ�����, �ҵ���Ӧ���ֶ�ֵ
	static string GetRoomValByKey(string key, byte value);

	//��������2
public:
	//�жϵ�ǰ��ѡ����, �ǵڴ���
	static int GetCountType(byte value);
};

#endif