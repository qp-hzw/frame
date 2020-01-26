# ifndef _ROBOT_MANAGER_H
# define _ROBOT_MANAGER_H
# include <vector>

class CPlayer;

class CRobotManager
{
	//用户变量
public:
	static std::vector<CPlayer *>			s_RobotArray;			//机器人数组

	//查找函数
public:
	//增
	static bool InsertRobot(tagUserInfo & UserInfo);
	//删
	static bool DeleteRobot(CPlayer * pPlayer);
	//查
	static CPlayer * FindRobotByID(DWORD dwUserID);
	//查
	static CPlayer * FindRobotByIndex(WORD index);
	//查总数
	static DWORD RobotCount();
};

# endif