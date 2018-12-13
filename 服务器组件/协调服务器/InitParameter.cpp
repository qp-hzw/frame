#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////////////

#define REG_ITEM_NAME				TEXT("Information")					//注册表项

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CInitParameter::CInitParameter() 
{ 
	InitParameter();
}

//析构函数
CInitParameter::~CInitParameter()
{
}

//初始化
VOID CInitParameter::InitParameter()
{

	return;
}

//加载配置
VOID CInitParameter::LoadInitParameter()
{
	//重置参数
	InitParameter();
	//读取跑马灯配置
	LoadMsgParameter();

	return;
}

//加载配置(跑马灯)yang
VOID CInitParameter::LoadMsgParameter()
{
	//初始化
	m_wMarqueeMsgPlayCount = 3;
	m_wMarqueeMsgApartTime = 10;

	//获取路径
	TCHAR szWorkDirM[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szWorkDirM,CountArray(szWorkDirM));

	//构造路径
	TCHAR szIniFileM[MAX_PATH]=TEXT("");
	_sntprintf_s(szIniFileM,CountArray(szIniFileM),TEXT("%s\\GlobalParameter.ini"),szWorkDirM);

	//读取配置
	CWHIniData IniDataM;
	IniDataM.SetIniFilePath(szIniFileM);

	//读取配置
	m_wMarqueeMsgPlayCount=IniDataM.ReadInt(TEXT("MarqueeMsgConfig"),TEXT("PlayCount"),m_wMarqueeMsgPlayCount);
	m_wMarqueeMsgApartTime=IniDataM.ReadInt(TEXT("MarqueeMsgConfig"),TEXT("ApartTime"),m_wMarqueeMsgApartTime);
	m_wSYSPlayTime = IniDataM.ReadInt(TEXT("MarqueeMsgConfig"),TEXT("SYSPlayTime "),m_wSYSPlayTime);
	m_wGamePlayTime = IniDataM.ReadInt(TEXT("MarqueeMsgConfig"),TEXT("GamePlayTime "),m_wGamePlayTime);

	return;
}

//////////////////////////////////////////////////////////////////////////////////
