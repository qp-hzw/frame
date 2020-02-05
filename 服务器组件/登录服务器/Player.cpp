#include "Player.h"
#include "DataBaseEngineSink.h"
#include<algorithm>

std::map< DWORD, std::vector<tagUserProperty>>	CPlayer::m_ArrPrp;

//构造函数
CPlayer::CPlayer(DWORD dwSocketID, tagUserInfo& UserInfo)
{
	ZeroMemory(&m_UserInfo, sizeof(tagUserInfo));
	CopyMemory(&m_UserInfo, &UserInfo, sizeof(tagUserInfo));

	m_dwSocketID = dwSocketID;
}

//析构函数
CPlayer::~CPlayer()
{
}

//初始化道具
void CPlayer::InitProp(tagUserProperty prop)
{
	for(auto item :m_ArrPrp[GetUserID()])
	{
		if(item.dwPropID == prop.dwPropID)
		{
			item.dwPropCount += prop.dwPropCount;
			return;
		}
	}

	m_ArrPrp[GetUserID()].push_back(prop);
}
//添加道具
void CPlayer::AddProp(tagUserProperty prop)
{
	InitProp(prop);

	//保存到数据库
	g_AccountsDB->ResetParameter();
	g_AccountsDB->AddParameter(TEXT("@UserID"), GetUserID());
	g_AccountsDB->AddParameter(TEXT("@GoodsID"), prop.dwPropID);
	g_AccountsDB->AddParameter(TEXT("@GoodsNum"), prop.dwPropCount);
	LONG lResultCode = g_AccountsDB->ExecuteProcess(TEXT("GSP_CL_BAG_ADD"), true);	
	if(lResultCode != DB_SUCCESS)
	{
		CLog::Log(log_error, "[%ld] insert prop failed  [%ld:%ld]", GetUserID(), prop.dwPropID, prop.dwPropCount);
	}

	CLog::Log(log_error, "[%ld] insert prop  [%ld:%ld]", GetUserID(), prop.dwPropID, prop.dwPropCount);
}

//获取玩家道具
std::vector<tagUserProperty> CPlayer::GetProps()
{
	std::vector<tagUserProperty> vec = m_ArrPrp.at(GetUserID());
	
	sort(vec.begin(), vec.end(), 
		[](tagUserProperty a, tagUserProperty b){
		return a.dwPropID < b.dwPropID;}
	);

	for(size_t i=0; i < vec.size(); i++)
	{
		CLog::Log(log_debug, "i %d   id: %ld", i, vec.at(i).dwPropID);
	}
	return vec;
}