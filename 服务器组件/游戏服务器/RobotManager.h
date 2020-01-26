# ifndef _ROBOT_MANAGER_H
# define _ROBOT_MANAGER_H
# include <vector>

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
};

# endif