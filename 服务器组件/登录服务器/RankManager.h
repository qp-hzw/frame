#pragma once

#pragma pack(1)

#include "AttemperEngineSink.h"
#include <vector>

using std::vector;

#define RANK_COUNT  12  //排行榜类别数量

#define RANK_VEC	vector<tagRankInfo*>

//排行榜管理类
class RankManager
{
public:
	RankManager();
	~RankManager(void);

	static RankManager* Instance();
	void Init();

protected:
	static RankManager* s_instance;

	RANK_VEC vecTodayWast;		//今日消耗房卡
	RANK_VEC vecTodayWin;		//今日胜场
	RANK_VEC vecYseterdatWast; //昨日消耗房卡
	RANK_VEC vecYesterdayWin;	//昨日胜场
	RANK_VEC vecWeekWast;		//本周消耗房卡
	RANK_VEC vecWeekWin;		//本周胜场
	RANK_VEC vecMonthWast;		//本月消耗房卡
	RANK_VEC vecMonthWin;		//本月胜场

	RANK_VEC vecRecharge;		//充值榜

	RANK_VEC vecPlayGoldJuCount;	//完成金币场局数
	RANK_VEC vecPlayJjcCount;		//完成竞技场游戏场数
	RANK_VEC vecPlayFkCount;		//完成房卡游戏场数

protected:

	bool		m_bRecived[RANK_COUNT];						//榜首是否领取
	RANK_VEC*	m_vecArry[RANK_COUNT];						//列表指针数组

	HANDLE		m_ArrayMutex;			//m_vecArry使用互斥量

public:
	//增加今日消耗
	void AddTodayWast(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);
	//增加今日胜场
	void AddTodayWin(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);
	//增加昨日消耗
	void AddYseterdatWast(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);
	//增加昨日胜场
	void AddYseterdatWin(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);
	//增加本周消耗
	void AddWeekWast(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);
	//增加本周胜场
	void AddWeekWin(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);
	//增加本月消耗
	void AddMonthWast(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);
	//增加本月胜场
	void AddMonthWin(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);

	//增加充值金额
	void AddRecharge(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);
	//增加已玩金币场局数
	void AddPlayGoldJuCount(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);
	//增加已玩竞技场场数
	void AddPlayJjcCount(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);
	//增加已玩房卡场场数
	void AddPlayFkCount(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);

	//通用添加方法
	void AddRankCount(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl, WORD index);

	
public:
	//从数据库获取排行榜列表
	void InitRankList(IDataBase *DataBase);
	//排序
	void SortRankList();
	//单个列表排序
	void SortRankListByIndex(int index);
	//获取制定榜单数量
	WORD GetRankCountByIndex(int index);
	//获取制定排行榜的对应名次信息
	tagRankInfo* GetRankItemByIndex(int index, int byRankNum);
	//领取榜首奖励
	int GetRankReward(int index, DWORD dwUserID, TCHAR szDescribe[], DWORD dwContextID);

protected:
	void Lock() //占用互斥量
	{
		// 等待互斥对象通知
		WaitForSingleObject(m_ArrayMutex, INFINITE);
	};

	void UnLock() //释放互斥量
	{
		ReleaseMutex(m_ArrayMutex);
	};

};

