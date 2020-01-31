#ifndef ROOM_RULE_MANAGER
#define ROOM_RULE_MANAGER
#include <string>
#include "Stdafx.h"
using std::string;


//���ӽӿ�
class ISubRoomRuleManager : public IUnknownEx
{
	//����ӿ�
public:
	//��������
	virtual bool StartService(){return true;}
	//ֹͣ����
	virtual bool ConcludeService(){return true;}

	//
public:
	//��ȡ�ֶ� ��Ӧ������
	virtual string GetDescribe(string key_name)= NULL;
	//�����ֶ�����, Ϊ�ṹ���Ӧ�ֶθ�ֵ
	virtual void SetRoomRule(string key, string value)= NULL;
	//
	virtual void Init() = NULL;
};


class RoomRuleManager
{
private:
	static RoomRuleManager*         _instance;  
	ISubRoomRuleManager*            m_SubRoomRuleManager;
	rule_arry						m_rule_arry;						//�������
	BYTE							m_frame_rule_count;					//frame������� ����
	BYTE							m_subgame_rule_count;				//����Ϸ������� ����

public:
	static  RoomRuleManager* Instance();

public:
	//��ȡ��������ѡ��
	rule_arry GetRoomRuleSetting();
	//��ȡ�������
	void GetRoomRule(tagTableRule& roomrule, byte value[20]);

public:
	//��ȡ����Ϸ���������ļ�
	void ReadSubGameRoomRule(int kindid);

	//��������
private:
	//��ȡ�ֶ� ��Ӧ������
	virtual string GetDescribe(string key_name);

	//�����ֶ�����, Ϊ�ṹ���Ӧ�ֶθ�ֵ
	virtual void SetRoomRule(tagTableRule &roomrule, string key, byte value);
	//�����ֶ�����, �ҵ���Ӧ���ֶ�ֵ
	string GetRoomValByKey(string key, byte value);
};

#endif