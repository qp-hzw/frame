# ifndef _ROBOT_MANAGER_H
# define _ROBOT_MANAGER_H
# include <vector>
# include "TableFrame.h"

class CPlayer;

class CRobotManager
{
	//�û�����
public:
	static std::vector<CPlayer *>			s_RobotArray;			//����������

	//���Һ���
public:
	//��
	static bool InsertRobot(tagUserInfo & UserInfo);
	//ɾ
	static bool DeleteRobot(CPlayer * pPlayer);
	//��
	static CPlayer * FindRobotByID(DWORD dwUserID);
	//��
	static CPlayer * FindRobotByIndex(WORD index);
	//������
	static DWORD RobotCount();
	//���û������Զ����뷿�䶨ʱ��
	static void SetRobotTimer();
	//���û������Զ����뷿��
	static bool On_ANDROID_JOIN_GAME();
	//���ҷ��������Ļ������û�
	static CPlayer* GetAndroidUser(CTableFrame *pTableFrame);
};

# endif