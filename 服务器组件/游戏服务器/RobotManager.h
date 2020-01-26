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
	//查
	static CPlayer * FindRobotByID(DWORD dwUserID);
	//查
	static CPlayer * FindRobotByIndex(WORD index);
	//查总数
	static DWORD RobotCount();
	//设置机器人自动加入房间定时器
	static void SetRobotTimer();
	//设置机器人自动加入房间
	static bool On_ANDROID_JOIN_GAME();
	//查找符合条件的机器人用户
	static CPlayer* GetAndroidUser(CTableFrame *pTableFrame);
};

# endif