#ifndef ROOM_RULE_MANAGER
#define ROOM_RULE_MANAGER
#include <string>
#include "Stdafx.h"
using std::string;

class RoomRuleManager
{
public:
	static rule_arry                    m_rule_arry;						//�������
	static BYTE							m_frame_rule_count;					//frame������� ����
	static BYTE							m_subgame_rule_count;				//����Ϸ������� ����

public:
	//��ȡ��������ѡ��
	static rule_arry GetRoomRuleSetting(){return m_rule_arry;}
	//��ȡ�������
	static tagTableRule GetRoomRule(byte value[20]);

public:
	//��ȡͨ�÷��������ļ�
	static void ReadFrameRoomRule();
	//��ȡ����Ϸ���������ļ�
	static void ReadSubGameRoomRule(int kindid);

	//��������
private:
	//��ȡ�ֶ� ��Ӧ������
	static string GetDescribe(string key_name);
	//�����ֶ�����, Ϊ�ṹ���Ӧ�ֶθ�ֵ
	static void SetRoomRule(tagTableRule &roomrule, string key, byte value);


	//�����ֶ�����, �ҵ���Ӧ���ֶ�ֵ
	static string GetRoomValByKey(string key, byte value);
};

#endif