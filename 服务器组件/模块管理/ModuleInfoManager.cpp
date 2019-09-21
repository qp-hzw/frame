#include "StdAfx.h"
#include "ModuleInfoManager.h"

//////////////////////////////////////////////////////////////////////////////////
#include <WinSock2.h>
#include <iostream>
#include <regex> 
using namespace std;
//#pragma comment(lib, "ws2_32.lib");


//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameItemMap::CGameItemMap()
{
}

//析构函数
CGameItemMap::~CGameItemMap()
{
	//变量定义
	DWORD wModuleID=0;
	tagGameGame * pGameGameItem=NULL;
	POSITION Position=m_GameModuleInfoMap.GetStartPosition();

	//删除索引
	while (Position!=NULL)
	{
		m_GameModuleInfoMap.GetNextAssoc(Position,wModuleID,pGameGameItem);
		SafeDelete(pGameGameItem);
	}

	//删除数组
	for (INT_PTR i=0;i<m_GameModuleInfoArray.GetCount();i++)
	{
		pGameGameItem=m_GameModuleInfoArray[i];
		SafeDelete(pGameGameItem);
	}

	//删除引用
	m_GameModuleInfoMap.RemoveAll();
	m_GameModuleInfoArray.RemoveAll();

	return;
}

//重置数据
bool CGameItemMap::ResetModuleInfo()
{
	//变量定义
	DWORD wModuleID=0;
	tagGameGame * pGameGameItem=NULL;
	POSITION Position=m_GameModuleInfoMap.GetStartPosition();

	//删除对象
	while (Position!=NULL)
	{
		m_GameModuleInfoMap.GetNextAssoc(Position,wModuleID,pGameGameItem);
		m_GameModuleInfoArray.Add(pGameGameItem);
	}

	//删除索引
	m_GameModuleInfoMap.RemoveAll();

	return true;
}

//删除数据
bool CGameItemMap::DeleteModuleInfo(DWORD wModuleID)
{
	//查找类型
	tagGameGame * pGameGameItem=NULL;
	if (m_GameModuleInfoMap.Lookup(wModuleID,pGameGameItem)==FALSE) return false;

	//删除数据
	m_GameModuleInfoMap.RemoveKey(wModuleID);
	m_GameModuleInfoArray.Add(pGameGameItem);

	//设置变量
	ZeroMemory(pGameGameItem,sizeof(tagGameGame));

	return true;
}

//插入数据
bool CGameItemMap::InsertModuleInfo(tagGameGame * pGameGameItem)
{
	//效验参数
	ASSERT(pGameGameItem!=NULL);
	if (pGameGameItem==NULL) return false;

	//查找现存
	DWORD wGameID=pGameGameItem->wGameID;
	tagGameGame * pGameModuleInsert=SearchModuleInfo(wGameID);

	//创建判断
	if (pGameModuleInsert==NULL)
	{
		//创建对象
		pGameModuleInsert=CreateModuleInfo();

		//结果判断
		if (pGameModuleInsert==NULL)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//设置数据
	m_GameModuleInfoMap[wGameID]=pGameModuleInsert;
	CopyMemory(pGameModuleInsert,pGameGameItem,sizeof(tagGameGame));

	return true;
}

//获取数目
DWORD CGameItemMap::GetModuleInfoCount()
{
	return (DWORD)(m_GameModuleInfoMap.GetCount());
}

//查找数据
tagGameGame * CGameItemMap::SearchModuleInfo(DWORD wModuleID)
{
	tagGameGame * pGameGameItem=NULL;
	m_GameModuleInfoMap.Lookup(wModuleID,pGameGameItem);
	return pGameGameItem;
}

//创建对象
tagGameGame * CGameItemMap::CreateModuleInfo()
{
	//变量定义
	tagGameGame * pGameGameItem=NULL;

	//创建对象
	try
	{
		INT_PTR nArrayCount=m_GameModuleInfoArray.GetCount();
		if (nArrayCount>0)
		{
			pGameGameItem=m_GameModuleInfoArray[nArrayCount-1];
			m_GameModuleInfoArray.RemoveAt(nArrayCount-1);
		}
		else
		{
			pGameGameItem=new tagGameGame;
			if (pGameGameItem==NULL) return NULL;
		}
	}
	catch (...) { return false; }

	//设置变量
	ZeroMemory(pGameGameItem,sizeof(tagGameGame));

	return pGameGameItem;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CModuleInfoManager::CModuleInfoManager()
{
}

//析构函数
CModuleInfoManager::~CModuleInfoManager()
{
}

//注册模块
bool CModuleInfoManager::RegisterGameModule(LPCTSTR pszModuleName)
{
	return true;
}

//注销模块
bool CModuleInfoManager::UnRegisterGameModule(LPCTSTR pszModuleName)
{
	return true;
}

int GetInternetIP( TCHAR *szInernet_ip)
{
	//下载脚本
	TCHAR szTempPath[_MAX_PATH] = {0}, szTempFile[MAX_PATH] = {0};
	GetTempPath(MAX_PATH, szTempPath);

	UINT nResult = GetTempFileName(szTempPath, _T("ex"), 0, szTempFile);
	int ret=URLDownloadToFile(NULL,_T("http://www.net.cn/static/customercare/yourip.asp"),szTempFile,BINDF_GETNEWESTVERSION,NULL);
	if (ret == S_FALSE)
		return 1;

	//判断脚本是否下载成功
	FILE *fp;
	if (_wfopen_s(&fp,szTempFile,_T("r"))!=0)
		return 2;


	fseek(fp,0,SEEK_END);//得到文件大小
	int ilength=ftell(fp);
	fseek(fp,0,SEEK_SET);


	//读取脚本中的ip地址
	if(ilength>0)
	{ 
		std::string buffer;
		buffer.resize(ilength);
		int iReadNum = fread(&buffer[0],sizeof(TCHAR),ilength,fp);
		fclose(fp);
		DeleteFile(szTempFile);

		std::regex re ("(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})");
 
		std::regex_iterator<std::string::const_iterator> begin(buffer.cbegin(), buffer.cend(), re);
		for (auto iter = begin; iter != std::sregex_iterator(); iter++)
		{
			MultiByteToWideChar(CP_ACP, 0,  &(iter->str())[0], -1, szInernet_ip, 32);

			//构造提示
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("外网IP: %s"),szInernet_ip);
			//提示消息
			CLog::Log(szDescribe,log_debug);
			//std::cout << iter->length() << ": " << iter->str() << std::endl;
		}
	}
	else
	{
		fclose(fp);
		return 4;
	}

	return 5;
}


//加载模块
bool CModuleInfoManager::LoadGameModuleInfo(CGameItemMap & ModuleInfoBuffer)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	TCHAR szInernet_ip[32] = TEXT("0.0.0.0");
	GetInternetIP(szInernet_ip);

	//创建对象
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("LoadGameModuleInfo 创建 PlatformDBModule 对象失败"),MB_ICONERROR);
		return false;
	}

	//连接数据库
	try
	{
		//设置连接
		PlatformDBModule->SetConnectionInfo(1, _TEST);

		//发起连接
		PlatformDBModule->OpenConnection();
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//读取列表
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("byMystery"), _MYSTERY);
		PlatformDBAide.AddParameter(TEXT("IP"), szInernet_ip);
		//CLog::Log(szInernet_ip, log_debug);
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadGameGameItem"),true)==DB_SUCCESS)
		{
			//清空列表
			ModuleInfoBuffer.ResetModuleInfo();

			//读取列表
			while (PlatformDBModule->IsRecordsetEnd()==false)
			{
				//变量定义
				tagGameGame GameModuleInfo;
				ZeroMemory(&GameModuleInfo,sizeof(GameModuleInfo));

				//模块属性
				GameModuleInfo.wGameID=PlatformDBAide.GetValue_DWORD(TEXT("GameID"));
				PlatformDBAide.GetValue_String(TEXT("GameName"),GameModuleInfo.szGameName,CountArray(GameModuleInfo.szGameName));

				//游戏服地址
				PlatformDBAide.GetValue_String(TEXT("GameServerAddr"),GameModuleInfo.szGameServerAddr,CountArray(GameModuleInfo.szGameServerAddr));

				//游戏属性
				PlatformDBAide.GetValue_String(TEXT("ServerDLLName"),GameModuleInfo.szDLLName,CountArray(GameModuleInfo.szDLLName));
	
				//本地版本
				LPCTSTR pszServerDLLName=GameModuleInfo.szDLLName;
				CWHService::GetModuleVersion(pszServerDLLName,GameModuleInfo.dwNativeVersion);

				//列表处理
				ModuleInfoBuffer.InsertModuleInfo(&GameModuleInfo);

				//移动记录
				PlatformDBModule->MoveToNext();
			}	
		}
		
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CLog::Log(pszDescribe,log_error);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

	return false;
}

//模块属性
bool CModuleInfoManager::GetGameServiceAttrib(LPCTSTR pszModuleName, tagGameServiceAttrib & GameServiceAttrib)
{
	//设置变量
	ZeroMemory(&GameServiceAttrib,sizeof(GameServiceAttrib));

	//游戏模块
	CGameServiceManagerHelper GameServiceManager;
	GameServiceManager.SetModuleCreateInfo(pszModuleName,GAME_SERVICE_CREATE_NAME);

	//加载模块
	if (GameServiceManager.CreateInstance()==true)
	{
		GameServiceManager->GetServiceAttrib(GameServiceAttrib);
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////
