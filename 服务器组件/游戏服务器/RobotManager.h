# ifndef _ROBOT_MANAGER_H
# define _ROBOT_MANAGER_H
# include <vector>
# include "TableFrame.h"

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
	//获取空闲机器人
	static CPlayer* GetFreeAndroid();

	//设置机器人自动加入房间定时器
	static void SetRobotTimer();
	//设置机器人自动加入房间
	static void On_ANDROID_JOIN_GAME();
};

# endif