#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"
#include "RankManager.h"
#include "ServiceUnits.h"

#include <WinSock2.h>
#include <iostream>
#include <regex> 
using namespace std;

CAttemperEngineSink* g_AttemperEngineSink = NULL;
//////////////////////////////////////////////////////////////////////////////////
# pragma region 定时器
//时间标识
#define  IDI_LOAD_GAME_LIST			1									//加载列表
#define  IDI_CONNECT_CORRESPOND		2									//重连标识
#define  IDI_COLLECT_ONLINE_INFO	3									//统计在线人数 -- 写入到数据库
#define  IDI_UPDATA_ONLINE_COUNT	5									//更新在线人数 -- 发送给客户端
#define  IDI_UPDATA_MARQUEE			6									//加载跑马灯消息
#define	 IDI_PLAY_MARQUEE			7									//通知client滚动跑马灯消息

#define  TIME_COLLECT_ONLINE_INFO   1800L								//统计在线人数 -- 写入到数据库
#define  TIME_UPDATA_ONLINE_COUNT   30L									//更新在线人数 -- 发送给客户端
#define  TIME_UPDATA_MARQUEE	    6L									//加载跑马灯消息
#define	 TIME_CONNECT_CORRESPOND	30*1000L							//连接时间
#define  TIME_RELOAD_LIST			6									//加载列表
#define  TIME_LOAD_LIST				120									//加载列表
#define  TIME_CONNECT				30									//重连时间

#pragma endregion

#pragma region 事件接口
//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//状态变量
	m_bShowServerStatus=false;

	//状态变量
	m_pBindParameter=NULL;

	//组件变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketEngine=NULL;

	m_pRankManager = NULL;

	if(!g_AttemperEngineSink)
		g_AttemperEngineSink = this;

	return;
}

//析构函数
CAttemperEngineSink::~CAttemperEngineSink()
{
}

//接口查询
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//绑定参数
	m_pBindParameter=new tagBindParameter[MAX_CONTENT];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*MAX_CONTENT);

	//更新在线人数 -- 通知database
	//m_pITimerEngine->SetTimer(IDI_COLLECT_ONLINE_INFO, TIME_COLLECT_ONLINE_INFO*1000L,TIMES_INFINITY,0);
	//更新在线人数 -- 通知client
	//m_pITimerEngine->SetTimer(IDI_UPDATA_ONLINE_COUNT,TIME_UPDATA_ONLINE_COUNT*1000L,TIMES_INFINITY, 0);
	//加载跑马灯消息
	//m_pITimerEngine->SetTimer(IDI_UPDATA_MARQUEE,TIME_UPDATA_MARQUEE*1000L,1, 0);

	//获取目录
	TCHAR szPath[MAX_PATH]=TEXT("");
	CString szFileName;
	GetModuleFileName(AfxGetInstanceHandle(),szPath,sizeof(szPath));
	szFileName=szPath;
	int nIndex = szFileName.ReverseFind(TEXT('\\'));
	szFileName = szFileName.Left(nIndex);
	szFileName += TEXT("\\PlazaOptionConfig.ini");

	//读取配置
	m_bShowServerStatus = (GetPrivateProfileInt(TEXT("ServerStatus"),TEXT("ShowServerStatus"),0,szFileName) != 0);

	//获取目录
	TCHAR szServerAddr[LEN_SERVER]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);

	return true;
}

//停止事件
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{
	//组件变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketEngine=NULL;

	//删除数据
	SafeDeleteArray(m_pBindParameter);

	//列表组件
	m_ServerListManager.ResetServerList();

	if(m_pRankManager != NULL)
	{
		delete m_pRankManager;
		m_pRankManager = NULL;
	}

	return true;
}

//控制事件
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{
	}

	return false;
}

//时间事件
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	switch (dwTimerID)
	{
	case IDI_LOAD_GAME_LIST:		//加载列表
		{
			m_pITimerEngine->KillTimer(IDI_LOAD_GAME_LIST);
			//加载列表
			m_ServerListManager.DisuseKernelItem();
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_GAME_LIST,0,NULL,0);

			return true;
		}
	case IDI_CONNECT_CORRESPOND:	//连接协调
		{
			m_pITimerEngine->KillTimer(IDI_CONNECT_CORRESPOND);
			//发起连接
			m_pITCPSocketEngine->Connect(_CPD_SERVER_ADDR, PORT_CENTER);

			return true;
		}
		/*
	case IDI_COLLECT_ONLINE_INFO:		//统计在线 -- 发给数据库
		{
			//变量定义
			DBR_GP_OnLineCountInfo OnLineCountInfo;
			ZeroMemory(&OnLineCountInfo,sizeof(OnLineCountInfo));

			//获取总数
			OnLineCountInfo.dwOnLineCountSum=m_ServerListManager.CollectOnlineInfo();

			//获取类型
			POSITION ServerPosition=NULL;
			do
			{
				CGameServerItem * pGameServerItem=m_ServerListManager.EmunGameServerItem(ServerPosition);
				if (pGameServerItem != NULL)
				{
					DWORD dwGameID = (pGameServerItem->m_GameServer.wServerID) & 0xFFFFFFFF;
					bool bIsExit = false;

					//判断是否在list中已存在gameID
					for(DWORD i = 0; i <= OnLineCountInfo.dwGameCount; i ++)
					{
						//如果存在
						if( (dwGameID == OnLineCountInfo.OnLineCountGame[i].dwGameID))
						{
							bIsExit = true;
							break;		
						}
					}

					//如果不存在
					if(!bIsExit)
					{
						//最大类型数目
						OnLineCountInfo.dwGameCount++;
					}
					if( OnLineCountInfo.dwGameCount > 0)
					{
						//类型标识 && 在线人数
						OnLineCountInfo.OnLineCountGame[OnLineCountInfo.dwGameCount-1].dwGameID=dwGameID;			
						OnLineCountInfo.OnLineCountGame[OnLineCountInfo.dwGameCount-1].dwOnLineCount += pGameServerItem->m_GameServer.dwOnlineCount;	
					}
				}

				//溢出判断
				if (OnLineCountInfo.dwGameCount > CountArray(OnLineCountInfo.OnLineCountGame))
				{
					//提示消息
					CLog::Log(log_warn, "统计在线人数 溢出 IDI_COLLECT_ONLINE_INFO");
					break;
				}

			} while (ServerPosition!=NULL);

			//发送请求
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_ONLINE_COUNT_INFO,0,&OnLineCountInfo,sizeof(OnLineCountInfo));

			return true;
		}
	case  IDI_UPDATA_ONLINE_COUNT: //发送给客户端
		{
			//网络数据
			WORD wSendSize=0;
			BYTE cbDataBuffer[SOCKET_TCP_PACKET];

			//枚举数据
			POSITION Position=NULL;
			CGameServerItem * pGameServerItem=NULL;

			STR_CMD_LC_LIST_ROOM_ONLINE pServerOnLine;
			ZeroMemory( &pServerOnLine, sizeof(pServerOnLine));

			//枚举数据
			for (DWORD i=0;i<m_ServerListManager.GetGameServerCount();i++)
			{
				//获取数据
				pGameServerItem=m_ServerListManager.EmunGameServerItem(Position);
				if (pGameServerItem==NULL) break;

				pServerOnLine.dwServerID = pGameServerItem->m_GameServer.wServerID;
				pServerOnLine.dwOnlineCount = pGameServerItem->m_GameServer.dwOnlineCount;

				//拷贝数据
				CopyMemory( cbDataBuffer+wSendSize, &pServerOnLine,sizeof(STR_CMD_LC_LIST_ROOM_ONLINE));
				wSendSize += sizeof(STR_CMD_LC_LIST_ROOM_ONLINE);
			}

			//查找用户连接,发送消息
			for (int i = 0; i < MAX_CONTENT; i++)
			{
				tagBindParameter * pBindParameter=(m_pBindParameter+i);
				if ( pBindParameter && pBindParameter->dwUserID != 0 && pBindParameter->dwSocketID != 0)
				{
					m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,MDM_LIST,CMD_LC_LIST_ROOM_ONLINE,&pServerOnLine, sizeof(pServerOnLine));
				}
			}
			return true;
		}
	case IDI_UPDATA_MARQUEE:	//加载跑马灯消息
		{
			m_pITimerEngine->KillTimer(IDI_UPDATA_MARQUEE);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_UPDATA_MARQUEE,0,NULL,0);
			return true;
		}
	case IDI_PLAY_MARQUEE:		//通知client滚动跑马灯消息   
		{
			//TODONOW 跑马灯消息号
			//added by WangChengQing 2018/6/2
			//1. 应该放在协调服
			//2. 可以在client登录的时候, 只发送一次给client。  没必要服务端时刻发送给所有客户端

			static DWORD s_MarqueeID;

			//1. 结束时间 < 当前时间的  去除掉
			for(int i = 0; i < m_MarqueeMsgListManager.GetCount() ; i++)
			{

				STR_DBO_UPDATA_MARQUEE strMarquee = m_MarqueeMsgListManager.GetAt(i);

				//从 s_MarqueeID 开始判断,  s_MarqueeID是递增的
				if(strMarquee.dwMarqueeID <= s_MarqueeID)
				{
					if(i != (m_MarqueeMsgListManager.GetCount()-1))
					{
						continue;
					}
					else //循环完一遍了
					{
						s_MarqueeID = 0;
						//最后一遍的时候 需要重启该定时器
						m_pITimerEngine->SetTimer(IDI_PLAY_MARQUEE,3*1000L,1,0);
						continue;
					}
				}

				//时间判断
				SYSTEMTIME nowTime;
				 GetLocalTime(&nowTime);

				 COleDateTime dTimeStart(strMarquee.timeStartTime); 
				 COleDateTime dTimeEnd(strMarquee.timeEndTime); 
				 COleDateTime dTimeNow(nowTime);

				 COleDateTimeSpan dTimeSpan;
				 double nSecnonSpan = 0;

				 //如果现在时间 大于 结束时间 -- 删除该信息
				 dTimeSpan = dTimeNow - dTimeEnd;
				 nSecnonSpan = dTimeSpan.GetTotalSeconds();
				 if(nSecnonSpan > 0)
				 {
					 m_MarqueeMsgListManager.GetAt(i).byMask = 2; //删除
					 continue;
				 }

				 //如果现在时间 小于 开始时间 -- 忽略该信息
				 dTimeSpan = dTimeStart - dTimeNow;
				 nSecnonSpan = dTimeSpan.GetTotalSeconds();
				 if(nSecnonSpan > 0)
				 {
					 m_MarqueeMsgListManager.GetAt(i).byMask = 3;//忽略
					 continue;
				 } 

				  m_MarqueeMsgListManager.GetAt(i).byMask = 4; //播放

				  //记录当前发送的MarqueeID
				  s_MarqueeID = strMarquee.dwMarqueeID;

				 //定时器间隔时间
				 DWORD wTimerTime =  static_cast<DWORD> (10 + _tcslen(strMarquee.szMarqueeMsg)*0.45);
				 wTimerTime += strMarquee.dwMaruqeeTime;
				 m_pITimerEngine->SetTimer(IDI_PLAY_MARQUEE,wTimerTime*1000L,1,0);

				 //发送数据
				 m_pITCPNetworkEngine->SendDataBatch(MDM_SERVICE, CMD_LC_SERVICE_MARQUEE, &strMarquee, sizeof(strMarquee));
				 break;
			}


			//删除过时的 跑马灯消息
			for(int i = 0; i < m_MarqueeMsgListManager.GetCount() ; i++)
			{
				if(2 == m_MarqueeMsgListManager.GetAt(i).byMask)
				{
					m_MarqueeMsgListManager.RemoveAt(i);
				}
			}

			return true;
		}
		*/

	}

	return false;
}

//数据库事件
bool CAttemperEngineSink::OnEventDataBaseResult(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
#pragma region 登录模块
	case DBO_CL_LOGON_ACCOUNTS:			//账号登录
		{
			return On_CMD_LC_Logon_Account(dwContextID,pData,wDataSize);
		}
	case DBO_CL_LOGON_PLATFORM:			//第三方登陆
		{
			return On_CMD_LC_Logon_Platform(dwContextID, pData, wDataSize);
		}
#pragma endregion

	case DBO_CL_SERVICE_USER_FEEDBACK:			//玩家反馈返回
		{
			return On_CMD_LC_Service_UserFeedBack(dwContextID, pData);
		}
	case DBO_CL_SERVICE_REFRESH_USER_INFO:		//刷新用户信息返回
		{
			return On_CMD_LC_Service_RefreshUserInfo(dwContextID, pData);
		}
	case DBO_CL_SERVICE_QUERY_ROOM_LIST:		//查询开房列表返回
		{
			return On_CMD_LC_Service_QueryRoomList(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_RICH_LIST:			//获取富豪榜返回
		{
			return On_CMD_LC_Service_GetRichList(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_USER_RECORD_LIST:	//获取用户录像列表返回（大局）
		{
			return On_CMD_LC_Service_GetUserRecordList(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_SPECIFIC_RECORD:	//获取指定ID录像返回（小局）
		{
			return On_CMD_LC_Service_GetSpecificRecord(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_ONLINE_REWARD:			//获取在线奖励返回
		{
			return On_CMD_LC_Service_OnlineReward(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_TASK_LIST:			//获取任务列表返回
		{
			return On_CMD_LC_Service_GetTaskList(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_TASK_REWARD:		//领取任务奖励返回
		{
			return On_CMD_LC_Service_GetTaskReward(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_REQUEST_LOTTERY:		//请求抽奖返回
		{
			return On_CMD_LC_Service_RequestLottery(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_MODIFY_PERSONAL_INFO:	//修改个人资料返回
		{
			return On_CMD_LC_Service_ModifyPersonalInfo(dwContextID,pData,wDataSize);
		}
	case DBO_CL_USER_QUERY_SCORE_INFO:			//查询(变更)金币房卡
		{
			return On_CMD_LC_Service_QueryScoreInfo(dwContextID,pData,wDataSize);
		}
#pragma region 启动命令
	case DBO_GP_GAME_LIST_RESULT:		//加载结果
		{
			return OnDBPCGameListResult(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GAME_TYPE_ITEM:			//type 
		{
			return OnDBPCGameTypeItem(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GAME_KIND_ITEM:			//kind
		{
			return OnDBPCGameKindItem(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GAME_NODE_ITEM:			//node
		{
			return OnDBPCGameNodeItem(dwContextID,pData,wDataSize);
		}
	case DBO_UPDATA_MARQUEE:			//加载跑马灯 返回
		{
			return On_DBO_UPDATA_MARQUEE(dwContextID,pData,wDataSize);
		}
	case DBO_UPDATA_MARQUEE_FINISH:		//加载跑马灯 结束
		{
			return On_DBO_UPDATA_MARQUEE_FINISH(dwContextID,pData,wDataSize);
		}
#pragma endregion
	
	case DBO_CL_USER_COMMAND_RESULT:		//公共操作结果
		{
			return On_CMD_LC_CommonOperateResult(dwContextID,pData,wDataSize);
		}
#pragma region MDM_GIFT 礼物道具
	case DBO_CL_GIFT_GIVE_PROPS:			//道具赠送 返回
		{
			return On_CMD_LC_GIFT_GIVE_PROPS(dwContextID,pData,wDataSize);
		}
	case DBO_CL_GIFT_GIVE_PROPS_SHOW:		//道具赠送 通知接收人
		{
			return On_CMD_LC_GIFT_GIVE_PROPS_SHOW(dwContextID,pData,wDataSize);
		}
#pragma endregion
	case DBO_CL_USER_RECHARGE_INFO:			//充值信息返回
		{
			return On_CMD_LC_Other_RechargeInfo(dwContextID,pData,wDataSize);
		}
	case DBO_CL_USER_EXCHANGE_INFO:			//兑换道具返回
		{
			return On_CMD_LC_Other_ExchangeInfo(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_RANK_REWARD:		//获得排行榜奖励	
		{
			return OnDBRankRewardResult(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_STANDING_LIST:		//pure大厅排行榜 返回
		{
			return On_CMD_LC_SERVICE_PURE_STANDING_LIST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_STANDING_FINISH:	//pure大厅排行榜 结束
		{
			return On_CMD_LC_SERVICE_PURE_STANDING_FINISH(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_RECORD_LIST:		//大局战绩
		{
			return On_CMD_LC_SERVICE_PURE_RECORD_LIST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_RECORD_LIST_PINFO:	//大局玩家信息
		{
			return On_CMD_LC_SERVICE_PURE_RECORD_LIST_PINFO(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_RECORD_FINISH://大局战绩结束
		{
			return On_CMD_LC_SERVICE_PURE_RECORD_FINISH(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_XJ_RECORD_LIST:		//小局战绩
		{
			return On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO:	//小局玩家信息
		{
			return On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_XJ_RECORD_PLAYBACK:			//小局录像回放
		{
			return On_CMD_LC_Service_XJRecordPlayback(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_CUSTOMER_MESSEGE:		//客服消息
		{
			return On_CMD_LC_SERVICE_CUSTOMER_MESSEGE(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_XJ_RECORD_FINISH://小局战绩结束
		{
			return On_CMD_LC_SERVICE_PURE_XJ_RECORD_FINISH(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_GOLD_INFO:		//请求金币大厅 返回
		{
			return On_CMD_LC_SERVICE_GOLD_INFO(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_GOLD_INFO_FINISH:	//请求金币大厅 结束
		{
			return On_CMD_LC_SERVICE_GOLD_INFO_FINISH(dwContextID,pData,wDataSize);
		}
			 
#pragma region MDM_CLUB 牌友圈
	case DBO_LC_CLUB_ALL_CLUB_INFO_LIST:		//查询牌友圈列表 返回
		{
			return On_CMD_LC_CLUB_ALL_CLUB_INFO_LIST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ALL_INFO_FINISH:		//查询牌友圈列表 结束
		{
			return On_CMD_LC_CLUB_ALL_INFO_FINISH(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ROOM_LIST: //查询指定牌友圈房间列表 返回
		{
			return On_CMD_LC_CLUB_ROOM_LIST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ROOM_LIST_FINISH: //查询指定牌友圈房间列表 结束
		{
			return On_CMD_LC_CLUB_ROOM_LIST_FINISH(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_RANDOM_CLUB_LIST : //查询未满员随机牌友圈 返回
		{
			return On_CMD_LC_CLUB_RANDOM_CLUB_LIST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_RANDOM_CLUB_LIST_FINISH : //查询未满员随机牌友圈  结束
		{
			return On_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_JOIN_CLUB: //申请加入牌友圈
		{
			return On_CMD_LC_CLUB_JOIN_CLUB(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_JOIN_CLUB_BDCAST: //申请加入牌友圈 广播
		{
			return On_CMD_LC_CLUB_JOIN_CLUB_BDCAST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_JOIN_CLUB_RE:	//申请加入牌友圈 客户端实时刷新俱乐部
		{
			return On_CMD_LC_CLUB_JOIN_CLUB_RE(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_DISS_CLUB : //解散牌友圈
		{
			return On_CMD_LC_CLUB_DISS_CLUB(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ROOM_SETTING: //房间设置
		{
			return On_CMD_LC_CLUB_ROOM_SETTING(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ROOM_QUERY_SETTING: //请求房间设置
		{
			return On_CMD_LC_CLUB_ROOM_QUERY_SETTING(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ROOM_DISSOLVE: //解散房间请求 返回
		{
			return On_CMD_LC_CLUB_ROOM_DISSOLVE(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_TABLE_DISSOLVE: //解散桌子请求 返回
		{
			return On_CMD_LC_CLUB_TABLE_DISSOLVE(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_NOTICE: //牌友圈公告
		{
			return On_CMD_LC_CLUB_NOTICE(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_MESSAGE: //牌友圈简介
		{
			return On_CMD_LC_CLUB_MESSAGE(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_CONTRIBUTE_FK://贡献房卡
		{
			return On_CMD_LC_CLUB_CONTRIBUTE_FK(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_AUTO_AGREE://牌友圈设置
		{
			return On_CMD_LC_CLUB_AUTO_AGREE(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_INVITE://邀请他人加入牌友圈
		{
			return On_CMD_LC_CLUB_INVITE(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_INVITE_REMIND://被邀请人的提醒
		{
			return On_CMD_LC_CLUB_INVITE_REMIND(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_INVITE_RESULT: //被邀请人 回复
		{
			return On_CMD_LC_CLUB_INVITE_RESULT(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_INQUERY_LIST: //被邀请人查看自己的邀请列表 返回
		{
			return On_CMD_LC_CLUB_INQUERY_LIST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_APPLICANT_LIST://申请人列表 返回
		{
			return On_CMD_LC_CLUB_APPLICANT_LIST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_APPLICANT_LIST_FINISH://申请人列表 结束
		{
			return On_CMD_LC_CLUB_APPLICANT_LIST_FINISH(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_APPOINTMENT: //职务任免
		{
			return On_CMD_LC_CLUB_APPOINTMENT(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_APPOINTMENT_NOTE: //职务任免 提醒
		{
			return On_CMD_LC_CLUB_APPOINTMENT_NOTE(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CHAT_ALL://俱乐部聊天 返回
		{
			return On_CMD_LC_CLUB_CHAT(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CHAT_ALL_BDCAST://俱乐部聊天 广播
		{
			return On_CMD_LC_CLUB_CHAT_BDCAST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_STICKY_POST:	//牌友圈置顶返回
		{
			return On_CMD_LC_CLUB_STICKY_POST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_INQUERY_LIST_FINISH: //被邀请人查看自己的邀请列表 结束
		{
			return On_CMD_LC_CLUB_INQUERY_LIST_FINISH(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_TABLE_LIST_TABLE: //桌子列表
		{
			return On_CMD_LC_CLUB_TABLE_LIST_TABLE(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_TABLE_LIST_USER:	//桌子玩家列表
		{
			return On_CMD_LC_CLUB_TABLE_LIST_USER(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_CREATE_CLUB://创建牌友圈 返回
		{
			return On_CMD_LC_CLUB_CREATE_CLUB(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_JOIN_ROOM: //申请加入房间 返回
		{
			return On_CMD_LC_CLUB_JOIN_ROOM(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_APPLICANT_RESULT: //群主|管理对申请消息的答复(同意|拒绝) 返回
		{
			return On_CMD_LC_CLUB_APPLICANT_RESULT(dwContextID,pData,wDataSize);
		}
		/*
	case DBO_LC_CLUB_JOIN_CLUB_MESSAGE_FINISH: //群主|管理对申请消息的答复(同意|拒绝) 结束
		{
			return On_CMD_LC_CLUB_APPLICANT_LIST_FINISH(dwContextID,pData,wDataSize);
		}
		*/
	case DBO_LC_CLUB_MEMBER_MANAGER://请求成员数据 返回
		{
			return On_CMD_LC_CLUB_MEMBER_MANAGER(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_MEMBER_MANAGER_FINISH://请求成员数据 结束
		{
			return On_CMD_LC_CLUB_MEMBER_MANAGER_FINISH(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_DATA ://工会基本信息
		{
			return On_CMD_LC_CLUB_DATA(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_QUIT://踢出退出请求 返回
		{
			return On_CMD_LC_CLUB_QUIT(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_RECORD_LIST: //工会战绩统计 返回
		{
			return On_CMD_LC_CLUB_RECORD_LIST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_RECORD_FINISH: //工会战绩统计 结束
		{
			return On_CMD_LC_CLUB_RECORD_FINISH(dwContextID,pData,wDataSize);
		}
#pragma endregion
#pragma region MDM_SHOP 商城道具
	case DBO_CL_SHOP_QUERY:		//查询商城
		{
			return On_CMD_LC_SHOP_QUERY_RESULT(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SHOP_QUERY_FINISH: //查询商城结束
		{
			return On_CMD_LC_SHOP_QUERY_FINISH(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SHOP_DIAMOND : //钻石购买道具
		{
			return On_CMD_LC_SHOP_DIAMOND_RESULT(dwContextID,pData,wDataSize);
		}
	case DBO_CL_BAG_QUERY: //背包物品查询
		{
			return On_CMD_LC_BAG_RESULT(dwContextID,pData,wDataSize);
		}
	case DBO_CL_BAG_FINISH://背包物品查询完成
		{
			return On_CMD_LC_BAG_FINISH(dwContextID,pData,wDataSize);
		}
#pragma endregion
	}

	return false;
}

#pragma endregion

#pragma region Socket事件
//应答事件
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	//获取索引
	if (LOWORD(dwSocketID)>=MAX_CONTENT) return false;

	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//设置变量
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientAddr=dwClientAddr;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);
	return true;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	///Socket连接关闭 数据库处理
	if(pBindParameter->dwUserID != 0)
	{
		/*
		** 情形四 登录服 socket断掉的时候
		** 判断是否在断线列表中, 如果存在，则设置为游戏断线； 如果不存在， 则设置为离线
		*/
		bool bIsExsit = false;
		for(int i=0; i < m_OfflineUserListManager.GetCount(); i++)
		{
			if(m_OfflineUserListManager.GetAt(i).dwUserID == pBindParameter->dwUserID)
			{
				bIsExsit = true;
			}
		}

		DBR_GP_UserQuitInfo quitInfo;
		quitInfo.dwUserID = pBindParameter->dwUserID;

		//1大厅在线 ，2正在游戏，3游戏断线，4离线
		if(bIsExsit)
		{
			quitInfo.byOnlineMask = 3;
		}
		else
		{
			quitInfo.byOnlineMask = 4;
		}
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOGON_USER_STATE,dwSocketID, &quitInfo,sizeof(quitInfo));
	}

	//清除信息
	//WORD wBindIndex=LOWORD(dwSocketID);
	ZeroMemory((m_pBindParameter+wBindIndex),sizeof(tagBindParameter));

	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_LOGON:				//登录命令
		{
			return OnTCPNetworkMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_SERVICE:			//服务命令
		{
			return OnTCPNetworkMainService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CLUB:				//牌友圈(版本2)
		{
			return On_MDM_CLUB(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GIFT:				//礼物道具	
		{
			return OnTCPNetworkMainOther(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_SHOP:				//商城道具
		{
			return On_MDM_SHOP(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GAME:				//游戏服相关
		{
			return On_MDM_GAME(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}

	}

	//登录服是短连接, 心跳包无效
	return false;
}



//获取外网ip
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

			//提示消息
			CLog::Log(log_debug, "外网IP: %s", szInernet_ip);
		}
	}
	else
	{
		fclose(fp);
		return 4;
	}

	return 5;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//协调连接
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//设置时间
		m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND, TIME_CONNECT_CORRESPOND, 1, 0);
		return true;
	}

	return false;
}

//连接事件
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//协调连接
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//错误判断
		if (nErrorCode!=0)
		{
			//设置时间
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND, TIME_CONNECT_CORRESPOND, 1, 0);
			return false;
		}

		//变量定义
		STR_CPR_LP_REGISTER_LOGON CPR;
		ZeroMemory(&CPR,sizeof(CPR));

		//设置变量
		TCHAR szInernet_ip[32] = TEXT("0.0.0.0");
		//GetInternetIP(szInernet_ip);
		//lstrcpyn(CPR.szServerAddr,szInernet_ip ,CountArray(CPR.szServerAddr));

		lstrcpyn(CPR.szServerAddr,TEXT("127.0.0.1"),CountArray(CPR.szServerAddr));

		//发送数据
		m_pITCPSocketEngine->SendData(MDM_REGISTER,CPR_LP_REGISTER_LOGON,&CPR,sizeof(CPR));

		return true;
	}

	return true;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//协调连接
	if (wServiceID==NETWORK_CORRESPOND)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_REGISTER:		//注册模块
			{
				return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CPD_LIST:	//列表命令
			{
				return OnTCPSocketMainServiceInfo(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_TRANSFER:	//中转服务
			{
				return OnTCPSocketMainTransfer(Command.wSubCmdID,pData,wDataSize);	
			}
		case MDM_WEB: //后台服务
			{
				return OnTCPSocketMainWeb(Command.wSubCmdID,pData,wDataSize);
			}
		}
	}

	return true;
}

#pragma endregion

#pragma region  启动命令
/*********************************************【启动登陆服时，服务单元发送的消息处理函数】*********************************************************/
//游戏种类
bool CAttemperEngineSink::OnDBPCGameTypeItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize%sizeof(tagGameType)==0);
	if (wDataSize%sizeof(tagGameType)!=0) return false;

	//变量定义
	WORD wItemCount=wDataSize/sizeof(tagGameType);
	tagGameType * pGameType=(tagGameType *)pData;

	//更新数据
	for (WORD i=0;i<wItemCount;i++)
	{
		//变量定义
		tagGameType GameType;
		ZeroMemory(&GameType,sizeof(GameType));

		//构造数据
		GameType.wTypeID=(pGameType+i)->wTypeID;
		lstrcpyn(GameType.szTypeName,(pGameType+i)->szTypeName,CountArray(GameType.szTypeName));

		//插入列表
		m_ServerListManager.InsertGameType(&GameType);
	}

	return true;
}

//游戏类型
bool CAttemperEngineSink::OnDBPCGameKindItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize%sizeof(tagGameKind)==0);
	if (wDataSize%sizeof(tagGameKind)!=0) return false;

	//变量定义
	WORD wItemCount=wDataSize/sizeof(tagGameKind);
	tagGameKind * pGameKind=(tagGameKind *)pData;

	//更新数据
	for (WORD i=0;i<wItemCount;i++)
	{
		//变量定义
		tagGameKind GameKind;
		ZeroMemory(&GameKind,sizeof(GameKind));

		//构造数据
		GameKind.wKindID=(pGameKind+i)->wKindID;
		GameKind.wTypeID=(pGameKind+i)->wTypeID;
		lstrcpyn(GameKind.szKindName,(pGameKind+i)->szKindName,CountArray(GameKind.szKindName));

		//插入列表
		m_ServerListManager.InsertGameKind(&GameKind);
	}

	return true;
}

//游戏节点
bool CAttemperEngineSink::OnDBPCGameNodeItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize%sizeof(tagGameNode)==0);
	if (wDataSize%sizeof(tagGameNode)!=0) return false;

	//变量定义
	WORD wItemCount=wDataSize/sizeof(tagGameNode);
	tagGameNode * pGameNode=(tagGameNode *)pData;

	//更新数据
	for (WORD i=0;i<wItemCount;i++)
	{
		//变量定义
		tagGameNode GameNode;
		ZeroMemory(&GameNode,sizeof(GameNode));

		//构造数据
		GameNode.wKindID=(pGameNode+i)->wKindID;
		GameNode.wNodeID=(pGameNode+i)->wNodeID;
		lstrcpyn(GameNode.szNodeName,(pGameNode+i)->szNodeName,CountArray(GameNode.szNodeName));

		//插入列表
		m_ServerListManager.InsertGameNode(&GameNode);
	}

	return true;
}

//游戏列表
bool CAttemperEngineSink::OnDBPCGameListResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	if (wDataSize!=sizeof(DBO_GP_GameListResult)) return false;

	//变量定义
	DBO_GP_GameListResult * pGameListResult=(DBO_GP_GameListResult *)pData;

	//消息处理
	if (pGameListResult->cbSuccess==TRUE)
	{
		//清理列表
		//m_ServerListManager.CleanKernelItem();

		//事件通知
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess=ER_SUCCESS;

	}
	else
	{
		//提示消息
		CLog::Log(log_warn, "服务器列表加载失败，%ld 秒后将重新加载", TIME_RELOAD_LIST);

		//设置时间
		m_pITimerEngine->SetTimer(IDI_LOAD_GAME_LIST,TIME_RELOAD_LIST*1000L,1,0);
	}

	return true;
}

//加载跑马灯消息 返回
bool CAttemperEngineSink::On_DBO_UPDATA_MARQUEE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	WORD size = sizeof(STR_DBO_UPDATA_MARQUEE);
	if ((wDataSize < size) || ( (wDataSize % size) != 0))  return false;

	//变量定义
	WORD wItemCount=wDataSize/size;
	STR_DBO_UPDATA_MARQUEE * pMarquee=(STR_DBO_UPDATA_MARQUEE *)pData;

	//更新数据
	for (WORD i=0;i<wItemCount;i++)
	{
		STR_DBO_UPDATA_MARQUEE strMarquee;
		strMarquee.byMask = (pMarquee+i)->byMask;
		strMarquee.dwMarqueeID =  (pMarquee+i)->dwMarqueeID;
		strMarquee.dwMaruqeeTime = (pMarquee+i)->dwMaruqeeTime;
		memcpy(&strMarquee.timeStartTime, &(pMarquee+i)->timeStartTime, sizeof(SYSTEMTIME));
		memcpy(&strMarquee.timeEndTime, &(pMarquee+i)->timeEndTime, sizeof(SYSTEMTIME));
		memcpy(strMarquee.szMarqueeMsg, (pMarquee+i)->szMarqueeMsg, sizeof(strMarquee.szMarqueeMsg));

		//插入列表
		m_MarqueeMsgListManager.Add(strMarquee);
	}

	return true;
}

//加载跑马灯消息 结束
bool CAttemperEngineSink::On_DBO_UPDATA_MARQUEE_FINISH(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//开启定时器,  通知客户端 滚动跑马灯
	m_pITimerEngine->SetTimer(IDI_PLAY_MARQUEE,1*1000L,1, 0);

	return true;
}


#pragma endregion

#pragma region 协调服 主消息号分发处理
//注册模块
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case CPO_PGPL_REGISTER_SUCESS:	//注册成功
		{
			//开启socket::server服务
			if(g_pServiceUnits->StartNetworkService() != 0)
			{
				g_pServiceUnits->ConcludeService();
				return true;
			}

			//读取数据库 加载列表
			m_ServerListManager.DisuseKernelItem();
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_GAME_LIST,0,NULL,0);

			//注册完成，在这里初始化排行榜
			if(NULL == m_pRankManager)
				m_pRankManager = new RankManager(m_pIDataBaseEngine);

			return true;
		}

	case CPO_PGPL_REGISTER_FAILURE:		//注册失败
		{
			//事件通知 
			g_pServiceUnits ->ConcludeService();
			return true;
		}
	}

	return true;
}

//列表命令
bool CAttemperEngineSink::OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SERVER_INFO:		//房间信息
		{
			//废弃列表
			m_ServerListManager.DisuseServerItem();

			return true;
		}
	case CPO_PL_LIST_GAME_ONLINE:	//房间人数
		{
			//效验参数
			if (wDataSize!=sizeof(STR_CPR_PL_LIST_GAME_ONLINE)) return false;

			//变量定义
			STR_CPR_PL_LIST_GAME_ONLINE * pServerOnLine=(STR_CPR_PL_LIST_GAME_ONLINE *)pData;

			//查找房间
			CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(pServerOnLine->wServerID);
			if(pGameServerItem == NULL) return true;

			pGameServerItem->m_GameServer.dwOnlineCount = pServerOnLine->dwOnLineCount;
			return true;
		}
	case CPO_PL_LIST_INSERT_GAME:	//房间插入
		{
			//效验参数
			if (wDataSize != sizeof(tagGameServer)) return false;

			//变量定义
			tagGameServer * pGameServer=(tagGameServer *)pData;

			//插入房间
			byte byRet = m_ServerListManager.InsertGameServer(pGameServer);

			// 如果是新房间, 需要通知所有client
			if(byRet == 2)
			{
				pGameServer->byMask = 0;
				m_pITCPNetworkEngine->SendDataBatch(MDM_LIST, CMD_LC_LIST_ROOM, pGameServer, sizeof(tagGameServer));
			}

			return true;
		}
	case CPO_PL_LIST_INSERT_GAME_LIST:	//房间插入(列表)
		{
			//效验参数
			if ((wDataSize < sizeof(tagGameServer))
				||(wDataSize%sizeof(tagGameServer)!=0)) return false;

			//变量定义
			WORD wItemCount=wDataSize/sizeof(tagGameServer);
			tagGameServer * pGameServer=(tagGameServer *)pData;

			//更新数据
			for (WORD i=0;i<wItemCount;i++)
			{
				//list时候无须通知client
				m_ServerListManager.InsertGameServer(pGameServer++);
			}

			return true;
		}
	case CPO_PL_LIST_REMOVE_GAME:	//房间删除
		{
			//效验参数
			if (wDataSize!=sizeof(STR_CPO_PL_LIST_REMOVE_GAME)) return false;

			//变量定义
			STR_CPO_PL_LIST_REMOVE_GAME * pServerRemove=(STR_CPO_PL_LIST_REMOVE_GAME *)pData;

			//变量定义
			m_ServerListManager.DeleteGameServer(pServerRemove->dwServerID);

			//同时删除 与此 ServerID相关的断线用户
			for(int i=0; i < m_OfflineUserListManager.GetCount(); i++)
			{
				if(m_OfflineUserListManager.GetAt(i).dwServerID == pServerRemove->dwServerID)
				{
					m_OfflineUserListManager.RemoveAt(i);
				}
			}

			//通知client, 有个游戏服掉线了
			tagGameServer GameServer;
			GameServer.wServerID = pServerRemove->dwServerID;
			GameServer.byMask = 1; //删除标志
			m_pITCPNetworkEngine->SendDataBatch(MDM_LIST, CMD_LC_LIST_ROOM, &GameServer, sizeof(tagGameServer));

			return true;
		}
	case SUB_CS_C_USER_OFFLINE_B: //用户断线
		{
			//效验参数
			if (wDataSize!=sizeof(tagOfflineUser)) return false;

			//变量定义
			tagOfflineUser * pOfflineUser=(tagOfflineUser *)pData;
			tagOfflineUser OfflineUser;
			OfflineUser.dwUserID = pOfflineUser->dwUserID;
			OfflineUser.dwServerID = pOfflineUser->dwServerID;
			OfflineUser.byMask = pOfflineUser->byMask;

			if(1 == OfflineUser.byMask)
			{
				m_OfflineUserListManager.Add(OfflineUser);

				//通知数据库, 更改玩家在线状态
				DBR_GP_UserQuitInfo quitInfo;
				quitInfo.dwUserID = OfflineUser.dwUserID ;
				quitInfo.byOnlineMask = 3;
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOGON_USER_STATE,0, &quitInfo,sizeof(quitInfo));
				
			}else if(2 == OfflineUser.byMask)
			{
				for(int i=0; i < m_OfflineUserListManager.GetCount(); i++)
				{
					if(m_OfflineUserListManager.GetAt(i).dwUserID == OfflineUser.dwUserID)
					{
						m_OfflineUserListManager.RemoveAt(i);
					}
				}
			}
			
			return true;
		}
	}

	return true;
}

//中转服务
bool CAttemperEngineSink::OnTCPSocketMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case CPO_PL_CLUB_TABLE_INFO:  //俱乐部桌子信息
		{
			//TODONOW 暂时先改为通知所有client, 后面改为通知在该房间内的人
			//added by WangChengQing 2018/5/21
			//效验数据
			if (wDataSize!=sizeof(STR_CMD_LC_CLUB_TABLE_LIST)) return true;

			//发送通知 -- 全部客户端
			m_pITCPNetworkEngine->SendDataBatch(MDM_CLUB,CMD_LC_CLUB_TABLE_LIST_TABLE,pData,wDataSize);
			return true;
		}
	case CPO_PL_CLUB_PLAYER_INFO: //俱乐部玩家信息
		{
			//TODONOW 暂时先改为通知所有client, 后面改为通知在该房间内的人
			//added by WangChengQing 2018/5/21
			//效验数据
			if (wDataSize!=sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST)) return true;

			//发送通知 -- 全部客户端
			m_pITCPNetworkEngine->SendDataBatch(MDM_CLUB, CMD_LC_CLUB_TABLE_LIST_USER,pData,wDataSize);
			return true;
		}
	case CPO_PL_CREATE_TABLE:	//创建桌子 查询可用的GameID
		{
			//效验数据
			if (wDataSize!=sizeof(STR_CPO_PL_CREATE_TABLE)) return true;
			STR_CPO_PL_CREATE_TABLE *pCPO = (STR_CPO_PL_CREATE_TABLE*)pData;
			
			STR_CMD_LC_GAME_QUERY_GAMEID CMD;
			CMD.dwGameID = pCPO->dwGameID;
			CMD.dwResultCode = pCPO->dwResultCode;
			memcpy(CMD.szDescribeString, pCPO->szDescribeString, sizeof(CMD.szDescribeString));

			m_pITCPNetworkEngine->SendData(pCPO->dwSocketID, MDM_GAME, CMD_LC_GAME_QUERY_GAMEID, &CMD, sizeof(CMD));
			return true;
		}
	}

	return true;
}

//后台管理
bool CAttemperEngineSink::OnTCPSocketMainWeb( WORD wSubCmdID, VOID * pData, WORD wDataSize )
{
	switch (wSubCmdID)
	{
		case CPO_PL_WEB_MARQUEE: //跑马灯信息读取
		{
			if (wDataSize!=sizeof(STR_CPR_WP_WEB_MARQUEE)) return true;

			//TODONOW added by WangChengQing 2018/8/14 如果程序崩溃, 请查看此处
			//1. 先清空跑马灯列表
			m_MarqueeMsgListManager.RemoveAll();

			//2. 重新读取数据库
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_UPDATA_MARQUEE,0,NULL,0);
			return true;
		}
		case CPO_PL_WEB_SYSTEM_MESSAGE: //聊天消息 -- 系统消息
		{
			//效验大小
			if (wDataSize!=sizeof(STR_CPR_WP_WEB_SYSTEM_MESSAGE))
			{
				return true;
			}
			STR_CPR_WP_WEB_SYSTEM_MESSAGE *pSub = (STR_CPR_WP_WEB_SYSTEM_MESSAGE*) pData;

			//结构体构造 
			STR_CMD_LC_CLUB_CHAT_BDCAST CMD;
			ZeroMemory(&CMD, sizeof(CMD));
			CMD.byChatMode = 2;
			memcpy(CMD.szUserNickName, TEXT("系统管理员"), sizeof(CMD.szUserNickName));
			memcpy(CMD.szChat, pSub->szMessage, sizeof(CMD.szChat));

			m_pITCPNetworkEngine->SendDataBatch(MDM_CLUB, CMD_LC_CLUB_CHAT_BDCAST, &CMD, sizeof(CMD));
			return true;
		}
	}
	return true;
}

#pragma endregion

#pragma region 客户端 主消息号分发处理
/*********************************************服务器<->客户端消息响应函数*********************************************************/

//登录模块
bool CAttemperEngineSink::OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CL_LOGON_ACCOUNTS:		//帐号登录
		{
			return On_SUB_CL_Logon_Accounts(pData,wDataSize,dwSocketID);
		}
	case SUB_CL_LOGON_REGISTER:		//帐号注册
		{
			return On_SUB_CL_Logon_Register(pData,wDataSize,dwSocketID);
		}
	case SUB_CL_LOGON_PLATFORM:		//平台登录
		{
			return On_SUB_CL_Logon_Platform(pData,wDataSize,dwSocketID);
		}
	}
	return false;
}

//用户服务
bool CAttemperEngineSink::OnTCPNetworkMainService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CL_SERVICE_USER_FEEDBACK:			//玩家反馈
		{
			return On_SUB_CL_Service_UserFeedBack(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_REFRESH_USER_INFO:		//刷新用户信息
		{
			return On_SUB_CL_Service_RefreshUserInfo(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_QUERY_ROOM_LIST:		//查询开房信息列表
		{
			return On_SUB_CL_Service_QueryRoomList(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_MODIFY_PERSONAL_INFO:	//修改个人资料
		{
			return On_SUB_CL_Service_ModifyPersonalInfo(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_RICH_LIST:			//获取富豪榜
		{
			return On_SUB_CL_Service_GetRichList(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_DJ_RECORD_LIST:	//获取用户录像列表
		{
			return On_SUB_CL_Service_GetUserRecordList(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_XJ_RECORD_LIST:	//获取指定ID录像
		{
			return On_SUB_CL_Service_GetSpecificRecord(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_ONLINE_REWARD:			//获取在线奖励
		{
			return On_SUB_CL_Service_OnlineReward(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_TASK_LIST:			//获取任务列表
		{
			return On_SUB_CL_Service_GetTaskList(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_TASK_REWARD:		//领取任务奖励
		{
			return On_SUB_CL_Service_GetTaskReward(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_RANK_LIST:			//获取排行榜
		{
			return On_SUB_CL_Service_GetRankList(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_REQUEST_LOTTERY:		//请求抽奖
		{
			return On_SUB_CL_Service_RequestLottery(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_PURE_STANDING_LIST:		//pure大厅排行版
		{
			return On_SUB_CL_SERVICE_PURE_STANDING_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_PURE_RECORD_LIST:		//大局战绩
		{
			return On_SUB_CL_SERVICE_PURE_RECORD_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_PURE_XJ_RECORD_LIST:	//小局战绩
		{
			return On_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GOLD_INFO: //金币场信息
		{
			return On_SUB_CL_SERVICE_GOLD_INFO(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_XJ_RECORD_PLAYBACK:	//小局录像回放
		{
			return On_SUB_CL_Service_XJRecordPlayback(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_CUSTOMER_MESSEGE:	//客服消息
		{
			return On_SUB_CL_SERVICE_CUSTOMER_MESSEGE(pData, wDataSize, dwSocketID);
		}
	}

	return false;
}

//牌友圈
bool CAttemperEngineSink::On_MDM_CLUB(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch(wSubCmdID)
	{
	case SUB_CL_CLUB_ALL_CLUB_INFO_LIST:	 //查询牌友圈列表
		{
			return On_SUB_CL_CLUB_ALL_CLUB_INFO_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_ROOM_LIST:  //查询指定牌友圈房间列表
		{
			return On_SUB_CL_CLUB_ROOM_LIST(pData, wDataSize, dwSocketID);	
		}
	case SUB_CL_CLUB_RANDOM_CLUB_LIST: //查询未满员, 随机牌友圈(最大9个)
		{
			return On_SUB_CL_CLUB_RANDOM_CLUB_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_JOIN_CLUB: //申请加入牌友圈
		{
			return On_SUB_CL_CLUB_JOIN_CLUB(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_DISS_CLUB: //解散牌友圈
		{
			return On_SUB_CL_CLUB_DISS_CLUB(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_CREATE_CLUB:	 //创建牌友圈
		{
			return On_SUB_CL_CLUB_CREATE_CLUB(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_JOIN_ROOM: //申请加入房间
		{
			return On_SUB_CL_CLUB_JOIN_ROOM(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_ROOM_SETTING: //房间设置
		{
			return On_SUB_CL_CLUB_ROOM_SETTING(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_ROOM_QUERY_SETTING://请求房间设置
		{
			return On_SUB_CL_CLUB_ROOM_QUERY_SETTING(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_ROOM_USER_LEAVE://玩家离开房间
		{
			return On_SUB_CL_CLUB_ROOM_USER_LEAVE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_ROOM_DISSOLVE:	//解散房间
		{
			return On_SUB_CL_CLUB_ROOM_DISSOLVE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_TABLE_DISSOLVE:	//解散桌子
		{
			return On_SUB_CL_CLUB_TABLE_DISSOLVE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_NOTICE: //牌友圈公告
		{
			return On_SUB_CL_CLUB_NOTICE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_MESSAGE: //牌友圈简介
		{
			return On_SUB_CL_CLUB_MESSAGE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_CONTRIBUTE_FK: //贡献房卡
		{
			return On_SUB_CL_CLUB_CONTRIBUTE_FK(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_AUTO_AGREE: //牌友圈设置
		{
			return On_SUB_CL_CLUB_AUTO_AGREE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_CHAT://牌友圈 聊天
		{
			return On_SUB_CL_CLUB_CHAT(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_STICKY_POST:	//牌友圈置顶
		{
			return On_SUB_CL_CLUBSTICKY_POST(pData, wDataSize, dwSocketID);
		}
		/*
	case SUB_CL_CLUB_CREATE_CLUB_ROOM_APPLY:  //创建牌友圈房间
		{
			return On_SUB_CL_CLUB_CREATE_CLUB_ROOM_APPLY(pData, wDataSize, dwSocketID);	
		}
		*/
	case SUB_CL_CLUB_APPLICANT_RESULT:	 //群主|管理对申请消息的答复(同意|拒绝)
		{
			return On_SUB_CL_CLUB_APPLICANT_RESULT(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_MEMBER_MANAGER:  //请求成员数据
		{
			return On_SUB_CL_CLUB_MEMBER_MANAGER(pData, wDataSize, dwSocketID);	
		}
	case SUB_CL_CLUB_INVITE://邀请他人加入牌友圈
		{
			return On_SUB_CL_CLUB_INVITE(pData, wDataSize, dwSocketID);	
		}
	case SUB_CL_CLUB_INVITE_RESULT:// 被邀请人 回复
		{
			return On_SUB_CL_CLUB_INVITE_RESULT(pData, wDataSize, dwSocketID);	
		}
	case SUB_CL_CLUB_INQUERY_LIST://被邀请人查看自己的邀请列表
		{
			return On_SUB_CL_CLUB_INQUERY_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_APPLICANT_LIST: //申请人列表
		{
			return On_SUB_CL_CLUB_APPLICANT_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_QUIT: //踢出退出请求
		{
			return On_SUB_CL_CLUB_QUIT(pData, wDataSize, dwSocketID);
		}
	case  SUB_CL_CLUB_APPOINTMENT://职务任免
		{
			return On_SUB_CL_CLUB_APPOINTMENT(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_RECORD_LIST:	//工会战绩统计
		{
			return On_SUB_CL_CLUB_RECORD_LIST(pData, wDataSize, dwSocketID);
		}
	}

	return false;
}

//礼物道具
bool CAttemperEngineSink::OnTCPNetworkMainOther(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CL_GIFT_GIVE_PROPS:				//赠送道具
		{
			return On_SUB_CL_GIFT_GIVE_PROPS(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_OTHERS_RECHARGE_INFO:				//充值信息
		{
			return On_SUB_CL_Other_ReChargeInfo(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_OTHERS_EXCHANGE_INFO:				//兑换道具信息
		{
			return On_SUB_CL_Other_ExchangeInfo(pData, wDataSize, dwSocketID);
		}
	}

	return false;
}

//商城道具
bool CAttemperEngineSink::On_MDM_SHOP(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch(wSubCmdID)
	{
	case SUB_CL_SHOP_QUERY:	 //查询商城
		{
			return On_SUB_CL_SHOP_QUERY(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SHOP_MONEY:  //金钱购买道具
		{
			//return On_SUB_CL_SHOP_MONEY(pData, wDataSize, dwSocketID);	
		}
	case SUB_CL_SHOP_DIAMOND: //钻石购买道具
		{
			return On_SUB_CL_SHOP_DIAMOND(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_BAG_QUERY: //背包物品查询
		{
			return On_SUB_CL_BAG_QUERY(pData, wDataSize, dwSocketID);
		}	
	}

	return false;
}

//游戏服相关
bool CAttemperEngineSink::On_MDM_GAME(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch(wSubCmdID)
	{
	case SUB_CL_GAME_QUERY_GAMEID:	 //创建房间之前, 先查询可用的GameID
		{
			//校验数据
			if (wDataSize!=sizeof(STR_SUB_CL_GAME_QUERY_GAMEID)) return true;

			STR_SUB_CL_GAME_QUERY_GAMEID *pSub = (STR_SUB_CL_GAME_QUERY_GAMEID*)pData;
			
			STR_CPR_LP_CREATE_TABLE CPR;
			CPR.dwKindID = pSub->dwKindID;
			CPR.dwSocketID = dwSocketID;

			m_pITCPSocketEngine->SendData(MDM_TRANSFER, CPR_LP_CREATE_TABLE, &CPR, sizeof(CPR));

			return true;
		}
	}

	return false;
}

#pragma endregion

#pragma region MDM_LOGON 登录模块
/***************************************** 【登录处理函数-主消息1】 *******************************************/
//帐号登录
bool CAttemperEngineSink::On_SUB_CL_Logon_Accounts(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	if (wDataSize != sizeof(STR_SUB_CL_LOGON_ACCOUNTS))
	{
		return false;
	}

	//处理消息
	STR_SUB_CL_LOGON_ACCOUNTS * pSUBLogonAccounts=(STR_SUB_CL_LOGON_ACCOUNTS *)pData;

	//版本判断（版本不对，直接退出）
	if ( On_CMD_LC_Logon_UpdateNotify(pSUBLogonAccounts->dwVersionCheck, dwSocketID) )
	{
		return true;
	}

	//变量构造
	STR_DBR_CL_LOGON_ACCOUNTS DBRLogonAccounts;
	ZeroMemory(&DBRLogonAccounts,sizeof(DBRLogonAccounts));
	//Socket校验
	DBRLogonAccounts.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));
	//数据赋值
	lstrcpyn(DBRLogonAccounts.szAccounts,pSUBLogonAccounts->szAccounts,CountArray(DBRLogonAccounts.szAccounts));
	lstrcpyn(DBRLogonAccounts.szPassword,pSUBLogonAccounts->szPassword,CountArray(DBRLogonAccounts.szPassword));
	lstrcpyn(DBRLogonAccounts.szMachineID,pSUBLogonAccounts->szMachineID,CountArray(DBRLogonAccounts.szMachineID));
	DBRLogonAccounts.dwProxyID = pSUBLogonAccounts->dwProxyID;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_LOGON_ACCOUNTS,dwSocketID,&DBRLogonAccounts,sizeof(DBRLogonAccounts));
	return true;
}

//账号登录返回
bool CAttemperEngineSink::On_CMD_LC_Logon_Account(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;
	if(wDataSize != sizeof(STR_CMD_LC_LOGON_PLATFORM)) return false;

	//变量定义
	STR_CMD_LC_LOGON_PLATFORM * pCMD=(STR_CMD_LC_LOGON_PLATFORM *)pData;

	//如果登录失败, 直接返回
	if(DB_SUCCESS != pCMD->dwResultCode)
	{
		//发送数据
		m_pITCPNetworkEngine->SendData(dwContextID, MDM_LOGON, CMD_LC_LOGON_ACCOUNTS, pCMD, sizeof(STR_CMD_LC_LOGON_PLATFORM));

		return true;
	}

	//重复登录踢出
	On_CMD_LC_Logon_RepeatLogon( pCMD->dwUserID, dwContextID );

	//断线重连的处理 1.CMD结构体赋值  2.list中清除itme 
	bool bIsExist = false; //是否存在断线列表中
	for(int i=0; i < m_OfflineUserListManager.GetCount(); i++)
	{
		if(m_OfflineUserListManager.GetAt(i).dwUserID == pCMD->dwUserID)
		{
			pCMD->dwOffLineGameID = m_OfflineUserListManager.GetAt(i).dwServerID;
			bIsExist = true;
			break;
		}
	}

	//设置连接
	(m_pBindParameter+LOWORD(dwContextID))->dwUserID = pCMD->dwUserID;	//记录此连接的用户ID

	//发送登录成功
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_LOGON, CMD_LC_LOGON_ACCOUNTS, pCMD, sizeof(STR_CMD_LC_LOGON_PLATFORM));


	//发送列表
	On_CMD_LC_List_Type(dwContextID);
	On_CMD_LC_List_Kind(dwContextID);
	On_CMD_LC_List_Node(dwContextID);
	On_CMD_LC_List_Room(dwContextID);

	//登录奖励
	On_CMD_LC_Logon_Logon_Reward(dwContextID, pCMD->LasLogonDate);

	// 更改用户在线标志
	CheckUserState_Logon(pCMD->dwUserID, bIsExist);

	return true;
}

void CAttemperEngineSink::CheckUserState_Logon(DWORD dwUserID, bool bIsExist)
{
	/*
	 ** 情形一  登录服 登录的时候
     ** 1. 与 游戏服肯定没有连接
     ** 2. 与 登录服肯定连接了
     ** 3. 判断断线列表 -- 如果存在, 则不发送; 如果不存在, 则为大厅在线 
	 */

	if(!bIsExist)
	{
		//TODONOWW 通知数据库更新玩家状态为 "大厅在线"
		DBR_GP_UserQuitInfo quitInfo;
		quitInfo.dwUserID = dwUserID;
		quitInfo.byOnlineMask =  1; //1大厅在线 ，2正在游戏，3游戏断线，4离线

		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOGON_USER_STATE,0, &quitInfo,sizeof(quitInfo));
	}
}

//帐号注册
bool CAttemperEngineSink::On_SUB_CL_Logon_Register(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	if (wDataSize != sizeof(STR_SUB_CL_LOGON_REGISTER))
	{
		return false;
	}
	
	//处理消息
	STR_SUB_CL_LOGON_REGISTER * pSUBLogonRegister=(STR_SUB_CL_LOGON_REGISTER *)pData;

	//效验版本
	if ( On_CMD_LC_Logon_UpdateNotify(pSUBLogonRegister->dwVersionCheck, dwSocketID) )
	{
		return true;
	}
	

	//变量定义
	//TODONOW 增加代理ID  added by WangChengQing 2018/8/13
	STR_DBR_CL_LOGON_REGISTER DBRLogonRegister;
	ZeroMemory(&DBRLogonRegister,sizeof(DBRLogonRegister));
	DBRLogonRegister.pBindParameter = (m_pBindParameter+LOWORD(dwSocketID));
	//数据赋值
	lstrcpyn(DBRLogonRegister.szAccounts,pSUBLogonRegister->szAccounts,CountArray(DBRLogonRegister.szAccounts));
	lstrcpyn(DBRLogonRegister.szPassword,pSUBLogonRegister->szPassword,CountArray(DBRLogonRegister.szPassword));
	lstrcpyn(DBRLogonRegister.szNickName,pSUBLogonRegister->szNickName,CountArray(DBRLogonRegister.szNickName));
	DBRLogonRegister.cbGender = pSUBLogonRegister->cbGender;
	lstrcpyn(DBRLogonRegister.strMobilePhone,pSUBLogonRegister->strMobilePhone,CountArray(DBRLogonRegister.strMobilePhone));
	lstrcpyn(DBRLogonRegister.szMachineID,pSUBLogonRegister->szMachineID,CountArray(DBRLogonRegister.szMachineID));
	DBRLogonRegister.dwProxyID = pSUBLogonRegister->dwProxyID;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_LOGON_REGISTER,dwSocketID,&DBRLogonRegister,sizeof(DBRLogonRegister));

	return true;
}

//平台登录
bool CAttemperEngineSink::On_SUB_CL_Logon_Platform(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	if (wDataSize != sizeof(STR_SUB_CL_LOGON_PLATFORM))
	{
		return false;
	}

	//处理消息
	STR_SUB_CL_LOGON_PLATFORM * pSUBLogonPlatform=(STR_SUB_CL_LOGON_PLATFORM *)pData;

	//效验版本
	if ( On_CMD_LC_Logon_UpdateNotify(pSUBLogonPlatform->dwVersionCheck, dwSocketID) )
	{
		return true;
	}

	//变量构造
	STR_DBR_CL_LOGON_PLATFORM DBRLogonPlatform;
	ZeroMemory(&DBRLogonPlatform,sizeof(DBRLogonPlatform));
	//Socket校验
	DBRLogonPlatform.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));
	DBRLogonPlatform.cbGender = pSUBLogonPlatform->cbGender;
	lstrcpyn(DBRLogonPlatform.szNickName, pSUBLogonPlatform->szNickName, CountArray(DBRLogonPlatform.szNickName));
	lstrcpyn(DBRLogonPlatform.strHeadUrl, pSUBLogonPlatform->strHeadUrl, CountArray(DBRLogonPlatform.strHeadUrl));
	lstrcpyn(DBRLogonPlatform.szOpenID, pSUBLogonPlatform->szOpenID, CountArray(DBRLogonPlatform.szOpenID));
	lstrcpyn(DBRLogonPlatform.szUnionID, pSUBLogonPlatform->szUnionID, CountArray(DBRLogonPlatform.szUnionID));
	lstrcpyn(DBRLogonPlatform.szMachineID,pSUBLogonPlatform->szMachineID,CountArray(DBRLogonPlatform.szMachineID));
	DBRLogonPlatform.dwProxyID = pSUBLogonPlatform->dwProxyID;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_LOGON_PLATFORM, dwSocketID, &DBRLogonPlatform, sizeof(DBRLogonPlatform));

	return true;}

//平台登录返回
bool CAttemperEngineSink::On_CMD_LC_Logon_Platform(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	STR_CMD_LC_LOGON_PLATFORM * pDBOLogonPlatform = (STR_CMD_LC_LOGON_PLATFORM *)pData;

	//构造数据
	STR_CMD_LC_LOGON_PLATFORM pCMDLogonPlatform;
	ZeroMemory(&pCMDLogonPlatform, sizeof(STR_CMD_LC_LOGON_PLATFORM));
	memcpy_s(&pCMDLogonPlatform, sizeof(STR_CMD_LC_LOGON_PLATFORM), pDBOLogonPlatform, sizeof(STR_CMD_LC_LOGON_PLATFORM));

	//如果登录失败, 直接返回
	if(DB_SUCCESS != pDBOLogonPlatform->dwResultCode)
	{
		m_pITCPNetworkEngine->SendData(dwContextID, MDM_LOGON, CMD_LC_LOGON_ACCOUNTS, &pCMDLogonPlatform, sizeof(STR_CMD_LC_LOGON_PLATFORM));
		return true;
	}

	//重复登录踢出
	On_CMD_LC_Logon_RepeatLogon( pCMDLogonPlatform.dwUserID ,dwContextID);

	//断线重连的处理 1.CMD结构体赋值  2.list中清除itme 
	bool bIsExist = false; //是否存在断线列表中
	for(int i=0; i < m_OfflineUserListManager.GetCount(); i++)
	{
		if(m_OfflineUserListManager.GetAt(i).dwUserID == pCMDLogonPlatform.dwUserID)
		{
			pCMDLogonPlatform.dwOffLineGameID = m_OfflineUserListManager.GetAt(i).dwServerID;
			bIsExist = true;
			break;
		}
	}
	
	//设置连接
	(m_pBindParameter+LOWORD(dwContextID))->dwUserID = pCMDLogonPlatform.dwUserID;//记录此连接的用户ID

	//发送登录成功
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_LOGON,CMD_LC_LOGON_PLATFORM, &pCMDLogonPlatform, sizeof(STR_CMD_LC_LOGON_PLATFORM));

	//发送列表
	On_CMD_LC_List_Type(dwContextID);
	On_CMD_LC_List_Kind(dwContextID);
	On_CMD_LC_List_Node(dwContextID);
	On_CMD_LC_List_Room(dwContextID);

	//TODONOW 第三方登陆，登陆奖励和老玩家奖励应该放在这里

	//更改用户在线标志
	CheckUserState_Logon(pCMDLogonPlatform.dwUserID, bIsExist);

	return true;
}

//重复登录踢出
bool CAttemperEngineSink::On_CMD_LC_Logon_RepeatLogon(DWORD UserID, DWORD dwContextID)
{
	//重复登录
	bool bIsRepeatLogon = false;

	//遍历所有连接
	for ( int i = 0; i <  MAX_CONTENT; i++ )
	{
		tagBindParameter* p_tempBind = (m_pBindParameter+LOWORD(i));

		//判断用户是否已经登录
		if (p_tempBind->dwUserID == UserID)
		{
			STR_CMD_LC_LOGON_REPEAT_LOGON pCMDLogonRepeatLogon;
			ZeroMemory(&pCMDLogonRepeatLogon, sizeof(STR_CMD_LC_LOGON_REPEAT_LOGON));

			//提示当前用户账号已登录，无法登录
			_snwprintf(pCMDLogonRepeatLogon.szDescribe, sizeof(pCMDLogonRepeatLogon.szDescribe), TEXT("账号已经在其它地方登录"));
			m_pITCPNetworkEngine->SendData(p_tempBind->dwSocketID, MDM_LOGON, CMD_LC_LOGON_REPEAT_LOGON, &pCMDLogonRepeatLogon, sizeof(STR_CMD_LC_LOGON_REPEAT_LOGON));


			//提示已登录的用户
			_snwprintf(pCMDLogonRepeatLogon.szDescribe, sizeof(pCMDLogonRepeatLogon.szDescribe), TEXT("账号在其它地方登录"));
			m_pITCPNetworkEngine->SendData(dwContextID, MDM_LOGON, CMD_LC_LOGON_REPEAT_LOGON, &pCMDLogonRepeatLogon, sizeof(STR_CMD_LC_LOGON_REPEAT_LOGON));

			//关闭连接 --  关闭之前的连接
			//m_pITCPNetworkEngine->ShutDownSocket(p_tempBind->dwSocketID);
			m_pITCPNetworkEngine->CloseSocket(p_tempBind->dwSocketID);
			
			//清空数据
			WORD wBindIndex = LOWORD(i);
			ZeroMemory( p_tempBind,sizeof(tagBindParameter));

			bIsRepeatLogon = true;
		}
	}

	return bIsRepeatLogon;
}

//升级提示（版本校验）
bool CAttemperEngineSink::On_CMD_LC_Logon_UpdateNotify(DWORD dwVersionCheck, DWORD dwSocketID)
{
	DWORD serverFrameVersion = Get_Framework_Version(PLATFORM_VERSION);  //服务端 frame 版本
	DWORD clientFrameVersion = dwVersionCheck; //client  Hall 版本

	byte ret = Compate_Hall_LogonServer(clientFrameVersion, serverFrameVersion);
	CLog::Log(log_debug, "版本校验结果:%d  服务器版本:%ld  客户端版本:%ld", ret, serverFrameVersion, clientFrameVersion);


	//版本匹配, 则直接退出, 不需要发送消息
	if(0 == ret )
	{
		return false;
	}

	//构造数据
	STR_CMD_LC_LOGON_UPDATE_NOTIFY UpdateNotify;
	ZeroMemory(&UpdateNotify,sizeof(UpdateNotify));
	UpdateNotify.cbUpdateNotify = ret;
	UpdateNotify.dwCurrentServerVersion = serverFrameVersion;
	//发送消息
	m_pITCPNetworkEngine->SendData(dwSocketID, MDM_LOGON, CMD_LC_LOGON_UPDATE_NOTIFY, &UpdateNotify, sizeof(UpdateNotify));

	return true;
}

//登录奖励
bool CAttemperEngineSink::On_CMD_LC_Logon_Logon_Reward(DWORD dwContextID, SYSTEMTIME LasLogonDate)
{ 
	// 1. 奖励应该是读取数据库
	// 2. 登录奖励 -- 1)节日奖励  2)老玩家奖励等
	
	//获得系统时间
	SYSTEMTIME systime;
	GetLocalTime(&systime);

	CTime tmLeft(systime.wYear, systime.wMonth, systime.wDay, 0, 0, 0);
	CTime tmRight(LasLogonDate.wYear, LasLogonDate.wMonth, LasLogonDate.wDay, 0, 0, 0);

	//获得上次登录时间
	CTimeSpan sp = tmLeft - tmRight;
	long MillisecondsL = (systime.wHour*3600 + systime.wMinute*60 + systime.wSecond)*1000 + systime.wMilliseconds;  
	long MillisecondsR = (LasLogonDate.wHour*3600 + LasLogonDate.wMinute*60 + LasLogonDate.wSecond)*1000 + LasLogonDate.wMilliseconds;  

	//计算时间间隔
	LONGLONG seconds =  ((__int64)sp.GetDays()*86400000 + (MillisecondsL - MillisecondsR))/1000;//此处返回秒

	//计算是否满30天,奖励50房卡
	if(seconds > 30*24*3600)
	{
		//变更用户财富
		DBR_GP_ModifyUserInsure modifyUserInsure;
		ZeroMemory(&modifyUserInsure,sizeof(DBR_GP_ModifyUserInsure));
		modifyUserInsure.lOpenRoomCard = 50;
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_USER_INSURE, dwContextID, &modifyUserInsure, sizeof(DBR_GP_ModifyUserInsure));

		//老玩家奖励返回
		STR_CMD_LC_LOGON_LOGON_REWARD OldBackReward;
		OldBackReward.byRewardType = 2;
		OldBackReward.dwRewardCount = 50;
		lstrcpyn( OldBackReward.szDescribe,TEXT("欢迎回来，您已获得老玩家回归奖励50房卡!"),CountArray(OldBackReward.szDescribe));

		m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_LOGON_LOGON_REWARD, &OldBackReward, sizeof(STR_CMD_LC_LOGON_LOGON_REWARD));
	}
	
	return true;
}

#pragma endregion

#pragma region MDM_LIST 列表命令
/***************************************** 【列表处理函数-主消息2】 *******************************************/
//发送游戏类型（棋牌、麻将等）
VOID CAttemperEngineSink::On_CMD_LC_List_Type(DWORD dwSocketID)
{
	//网络数据
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//枚举数据
	POSITION Position=NULL;
	CGameTypeItem * pGameTypeItem=NULL;		//数据库Platform中的GameTypeItem（棋牌/休闲...）

	//枚举数据
	for (DWORD i=0;i<m_ServerListManager.GetGameTypeCount();i++)
	{
		//发送数据(防止数据太多，一批批发送)
		if ((wSendSize+sizeof(tagGameType))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_LIST,CMD_LC_LIST_TYPE,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//获取数据
		pGameTypeItem=m_ServerListManager.EmunGameTypeItem(Position);
		if (pGameTypeItem==NULL) break;

		//拷贝数据
		CopyMemory(cbDataBuffer+wSendSize,&pGameTypeItem->m_GameType,sizeof(tagGameType));
		wSendSize+=sizeof(tagGameType);
	}

	//发送剩余
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_LIST,CMD_LC_LIST_TYPE,cbDataBuffer,wSendSize);

	return;
}

//发送游戏种类（斗地主、炸金花等）
VOID CAttemperEngineSink::On_CMD_LC_List_Kind(DWORD dwSocketID)
{
	//网络数据
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//枚举数据
	POSITION Position=NULL;
	CGameKindItem * pGameKindItem=NULL;

	//枚举数据
	for (DWORD i=0;i<m_ServerListManager.GetGameKindCount();i++)
	{
		//发送数据
		if ((wSendSize+sizeof(tagGameKind))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_LIST, CMD_LC_LIST_KIND, cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//获取数据
		pGameKindItem=m_ServerListManager.EmunGameKindItem(Position);
		if (pGameKindItem==NULL) break;

		//拷贝数据
		CopyMemory(cbDataBuffer+wSendSize,&pGameKindItem->m_GameKind,sizeof(tagGameKind));
		wSendSize+=sizeof(tagGameKind);
	}

	//发送剩余
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID, MDM_LIST, CMD_LC_LIST_KIND, cbDataBuffer, wSendSize);

	return;
}

//发送节点（电信区、网通区等）
VOID CAttemperEngineSink::On_CMD_LC_List_Node(DWORD dwSocketID)
{
	//网络数据
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//枚举数据
	POSITION Position=NULL;
	CGameNodeItem * pGameNodeItem=NULL;

	//枚举数据
	for (DWORD i=0;i<m_ServerListManager.GetGameNodeCount();i++)
	{
		//发送数据
		if ((wSendSize+sizeof(tagGameNode))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_LIST,CMD_LC_LIST_NODE,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//获取数据
		pGameNodeItem=m_ServerListManager.EmunGameNodeItem(Position);
		if (pGameNodeItem==NULL) break;

		CopyMemory(cbDataBuffer+wSendSize,&pGameNodeItem->m_GameNode,sizeof(tagGameNode));
		wSendSize+=sizeof(tagGameNode);
	}

	//发送剩余
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_LIST,CMD_LC_LIST_NODE,cbDataBuffer,wSendSize);

	return;
}

//发送房间（游戏服房间）
VOID CAttemperEngineSink::On_CMD_LC_List_Room(DWORD dwSocketID)
{
	//网络数据
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//枚举数据
	POSITION Position=NULL;
	CGameServerItem * pGameServerItem=NULL;	

	//枚举数据
	for (DWORD i=0;i<m_ServerListManager.GetGameServerCount();i++)
	{
		//发送数据
		if ((wSendSize+sizeof(tagGameServer))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_LIST,CMD_LC_LIST_ROOM,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//获取列表数据
		pGameServerItem=m_ServerListManager.EmunGameServerItem(Position);
		if (pGameServerItem==NULL) break;

		//拷贝数据
		CopyMemory(cbDataBuffer+wSendSize,&pGameServerItem->m_GameServer,sizeof(tagGameServer));
		wSendSize+=sizeof(tagGameServer);
	}

	//发送剩余
	if (wSendSize>0)
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_LIST, CMD_LC_LIST_ROOM, cbDataBuffer, wSendSize);

	return;
}

#pragma endregion

#pragma region MDM_SERVICE 用户服务
/***************************************** 【服务处理函数-主消息3】 *******************************************/
//玩家反馈
bool CAttemperEngineSink::On_SUB_CL_Service_UserFeedBack(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	ASSERT( wDataSize == sizeof(STR_SUB_CL_SERVICE_FEEDBACK));
	if(wDataSize != sizeof(STR_SUB_CL_SERVICE_FEEDBACK)) return false;

	//处理消息
	STR_SUB_CL_SERVICE_FEEDBACK * pUserSuggestion = (STR_SUB_CL_SERVICE_FEEDBACK *)pData;

	//构造玩家反馈数据
	STR_DBR_CL_SERVICE_FEEDBACK UserSuggestion;
	ZeroMemory(&UserSuggestion,sizeof(UserSuggestion));
	UserSuggestion.dwUserID = pUserSuggestion->dwUserID;
	lstrcpyn(UserSuggestion.szFB_Title,pUserSuggestion->szFB_Title,CountArray(UserSuggestion.szFB_Title));
	lstrcpyn(UserSuggestion.szContact,pUserSuggestion->szContact,CountArray(UserSuggestion.szContact));
	lstrcpyn(UserSuggestion.szFB_Content,pUserSuggestion->szFB_Content,CountArray(UserSuggestion.szFB_Content));

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_USER_FEEDBACK, dwSocketID, &UserSuggestion, sizeof(UserSuggestion));

	return true;
}

//玩家反馈返回
bool CAttemperEngineSink::On_CMD_LC_Service_UserFeedBack( DWORD dwContextID, VOID * pData)
{
	//判断在线
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	STR_CMD_LC_SERVICE_FEEDBACK FeedBack;
	ZeroMemory(&FeedBack, sizeof(FeedBack));

	//变量定义
	STR_DBO_CL_SERVICE_FEEDBACK *pOperate = (STR_DBO_CL_SERVICE_FEEDBACK *)pData;

	//构造数据
	FeedBack.lResultCode=pOperate->lResultCode;
	lstrcpyn(FeedBack.szDescribeString, pOperate->szDescribeString, CountArray(FeedBack.szDescribeString));

	//发送数据
	WORD wDataSize = sizeof(FeedBack);
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_SERVICE_USER_FEEDBACK, &FeedBack, wDataSize);

	return true;
}

//刷新用户信息
bool CAttemperEngineSink::On_SUB_CL_Service_RefreshUserInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	ASSERT( wDataSize == sizeof(STR_SUB_CL_SERVICE_REFRESH_INFO));
	if(wDataSize != sizeof(STR_SUB_CL_SERVICE_REFRESH_INFO)) return false;

	//处理消息
	STR_SUB_CL_SERVICE_REFRESH_INFO *pUserRequest = (STR_SUB_CL_SERVICE_REFRESH_INFO *)pData;
	//定义变量
	STR_DBR_CL_SERCIVR_REFRESH_INFO UserRequest;
	ZeroMemory(&UserRequest,sizeof(UserRequest));

	UserRequest.dwUserID = pUserRequest->dwUserID;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_REFRESH_USER_INFO, dwSocketID, &UserRequest, sizeof(UserRequest));
	return true;
}

//刷新用户信息返回
bool CAttemperEngineSink::On_CMD_LC_Service_RefreshUserInfo( DWORD dwContextID, VOID * pData )
{
	//判断在线
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	STR_CMD_LC_SERVICE_REFRESH_INFO UserInfo;
	ZeroMemory(&UserInfo,sizeof(UserInfo));

	//变量定义
	STR_DBO_CL_SERCIVR_REFRESH_INFO * pUserInfo=(STR_DBO_CL_SERCIVR_REFRESH_INFO *)pData;

	//构造数据
	memcpy_s(&UserInfo, sizeof(STR_CMD_LC_SERVICE_REFRESH_INFO), pUserInfo, sizeof(STR_CMD_LC_SERVICE_REFRESH_INFO));

	//发送数据
	WORD wDataSize = sizeof(STR_CMD_LC_SERVICE_REFRESH_INFO);
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_CL_SERVICE_REFRESH_USER_INFO, &UserInfo, wDataSize);

	return true;
}

//查询开房信息列表
bool CAttemperEngineSink::On_SUB_CL_Service_QueryRoomList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT( wDataSize == sizeof(STR_SUB_CL_SERVICE_QUERY_ROOMLIST));
	if(wDataSize != sizeof(STR_SUB_CL_SERVICE_QUERY_ROOMLIST)) return false;

	STR_SUB_CL_SERVICE_QUERY_ROOMLIST* pTableInfoList = (STR_SUB_CL_SERVICE_QUERY_ROOMLIST*)pData;

	
	STR_DBR_CL_SERCIVR_QUERY_ROOMLIST GetTableInfoList;
	GetTableInfoList.dwUserID = pTableInfoList->dwUserID;

	//查询开房列表
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_QUERY_ROOM_LIST,dwSocketID,&GetTableInfoList,sizeof(STR_DBR_CL_SERCIVR_QUERY_ROOMLIST));

	return true;
}

//查询开房列表返回
bool CAttemperEngineSink::On_CMD_LC_Service_QueryRoomList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_QUERY_ROOMLIST));
	STR_DBO_CL_SERCIVR_QUERY_ROOMLIST *pRecord = (STR_DBO_CL_SERCIVR_QUERY_ROOMLIST*)pData;

	STR_CMD_LC_SERVICE_QUERY_ROOMLIST TableInfo;
	ZeroMemory(&TableInfo,sizeof(STR_CMD_LC_SERVICE_QUERY_ROOMLIST));

	memcpy(&TableInfo, pRecord, sizeof(STR_CMD_LC_SERVICE_QUERY_ROOMLIST));

	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_SERVICE_QUERY_ROOM_LIST, &TableInfo, sizeof(STR_CMD_LC_SERVICE_QUERY_ROOMLIST));

	return true;
}

//获取富豪榜
bool CAttemperEngineSink::On_SUB_CL_Service_GetRichList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验
	ASSERT( wDataSize == sizeof(STR_SUB_CL_SERVICE_GET_RICHLIST));
	if( wDataSize != sizeof(STR_SUB_CL_SERVICE_GET_RICHLIST) ) 		
	{
		return false;
	}
	
	return m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_GET_RICH_LIST, dwSocketID, pData, 0);
}

//获取富豪榜返回
bool CAttemperEngineSink::On_CMD_LC_Service_GetRichList( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_GET_RICHLIST));
	if(wDataSize!=sizeof(STR_DBO_CL_SERCIVR_GET_RICHLIST))
		return false;

	STR_DBO_CL_SERCIVR_GET_RICHLIST *pLotteryResult = (STR_DBO_CL_SERCIVR_GET_RICHLIST*)pData;

	STR_CMD_LC_SERVICE_GET_RICHLIST LotteryResult;
	ZeroMemory(&LotteryResult,sizeof(STR_CMD_LC_SERVICE_GET_RICHLIST));

	memcpy(&LotteryResult,pLotteryResult,sizeof(STR_DBO_CL_SERCIVR_GET_RICHLIST));		//修改bug，原来的sizeof里面不是这个结构体

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_GET_RICH_LIST, &LotteryResult, sizeof(STR_CMD_LC_SERVICE_GET_RICHLIST));

	return true;
}

//获取用户录像列表
bool CAttemperEngineSink::On_SUB_CL_Service_GetUserRecordList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT( wDataSize == sizeof(STR_SUB_CL_SERVICE_GET_DJ_RECORDLIST));
	if(wDataSize != sizeof(STR_SUB_CL_SERVICE_GET_DJ_RECORDLIST)) return false;

	STR_SUB_CL_SERVICE_GET_DJ_RECORDLIST* pGetRecordList = (STR_SUB_CL_SERVICE_GET_DJ_RECORDLIST*)pData;

	//录像列表请求
	STR_DBR_CL_SERCIVR_GET_RECORDLIST GetRecordList;
	GetRecordList.dwUserID = pGetRecordList->dwUserID;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_GET_USER_RECORD_LIST, dwSocketID, &GetRecordList, sizeof(STR_DBR_CL_SERCIVR_GET_RECORDLIST));

	return true;
}

//获取用户录像列表返回
bool CAttemperEngineSink::On_CMD_LC_Service_GetUserRecordList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_GET_RECORDLIST));
	STR_DBO_CL_SERCIVR_GET_RECORDLIST *pRecord = (STR_DBO_CL_SERCIVR_GET_RECORDLIST*)pData;

	STR_CMD_LC_SERVICE_GET_DJ_RECORDLIST Record;
	ZeroMemory(&Record,sizeof(STR_CMD_LC_SERVICE_GET_DJ_RECORDLIST));

	Record.dwTableID = pRecord->dwTableID;
	memcpy( Record.dwUserID, pRecord->dwUserID,sizeof(DWORD)*5);

	for(int i=0;i < 5;i++)
	{
		memcpy(Record.szUserName[i],pRecord->szUserName[i],sizeof(TCHAR)*LEN_NICKNAME);	
	}

	Record.byGameMode = pRecord->byGameMode;
	Record.byZhuangType = pRecord->byZhuangType;
	Record.wCurrentJuShu = pRecord->wCurrentJuShu;
	Record.wTotalJuShu = pRecord->wTotalJuShu;

	memcpy(Record.szGameTime, pRecord->szGameTime, sizeof(TCHAR)*30);
	memcpy(Record.szTotalScore, pRecord->szTotalScore, sizeof(TCHAR)*50);

	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_SERVICE_GET_DJ_RECORD_LIST, &Record, sizeof(STR_CMD_LC_SERVICE_GET_DJ_RECORDLIST));

	//_bstr_t b(pRecord->szData);
	//char* output = b;

	//FILE *fp = fopen(output,"rb");
	//if(NULL != fp)
	//{
	//	if(pRecord->dwDataSize <= 3072)
	//	{
	//		fread(Record.szData,pRecord->dwDataSize,1,fp);

	//		memcpy(Record.szGameTime,pRecord->szGameTime,sizeof(TCHAR)*30);
	//		memcpy(Record.szScore,pRecord->szScore,sizeof(TCHAR)*50);
	//		memcpy(Record.szTotalScore,pRecord->szTotalScore,sizeof(TCHAR)*50);

	//		m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_SERVICE_GET_DJ_RECORD_LIST, &Record, sizeof(STR_CMD_LC_SERVICE_GET_DJ_RECORDLIST));

	//	}
	//	fclose(fp);
	//}

	return true;
}

//获取指定ID录像
bool CAttemperEngineSink::On_SUB_CL_Service_GetSpecificRecord(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT( wDataSize == sizeof(STR_SUB_CL_SERVICE_GET_XJ_RECORDLIST));
	if(wDataSize != sizeof(STR_SUB_CL_SERVICE_GET_XJ_RECORDLIST)) return false;

	STR_SUB_CL_SERVICE_GET_XJ_RECORDLIST* pGetRecordList = (STR_SUB_CL_SERVICE_GET_XJ_RECORDLIST*)pData;

	//DBR数据
	STR_DBR_CL_SERCIVR_GET_SPECIFIC_RECORD GetRecordList;
	GetRecordList.dwTableID = pGetRecordList->dwTableID;

	//校验桌子ID
	if ( 0 != GetRecordList.dwTableID )
	{
		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_GET_SPECIFIC_RECORD,dwSocketID,&GetRecordList,sizeof(STR_DBR_CL_SERCIVR_GET_SPECIFIC_RECORD));
	}

	return true;
}

//获取指定ID录像返回
bool CAttemperEngineSink::On_CMD_LC_Service_GetSpecificRecord(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_GET_SPECIFIC_RECORD));
	STR_DBO_CL_SERCIVR_GET_SPECIFIC_RECORD *pRecord = (STR_DBO_CL_SERCIVR_GET_SPECIFIC_RECORD*)pData;

	STR_CMD_LC_SERVICE_GET_XJ_RECORDLIST Record;
	memcpy(&Record, pRecord, sizeof(STR_CMD_LC_SERVICE_GET_XJ_RECORDLIST));

	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_SERVICE_GET_XJ_RECORD_LIST, &Record, sizeof(STR_CMD_LC_SERVICE_GET_XJ_RECORDLIST));

	return true;
}

//获取在线奖励
bool CAttemperEngineSink::On_SUB_CL_Service_OnlineReward(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT(wDataSize == sizeof(STR_SUB_CL_SERVICE_ONLINE_REWARD));
	if(wDataSize != sizeof(STR_SUB_CL_SERVICE_ONLINE_REWARD))
		return false;

	STR_SUB_CL_SERVICE_ONLINE_REWARD * pGetLogonReward = (STR_SUB_CL_SERVICE_ONLINE_REWARD*)pData;

	STR_DBR_CL_SERCIVR_ONLINE_REWARD GetLogonReward;
	ZeroMemory(&GetLogonReward,sizeof(STR_DBR_CL_SERCIVR_ONLINE_REWARD));

	GetLogonReward.dwUserID = pGetLogonReward->dwUserID;

	return m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_ONLINE_REWARD,dwSocketID,&GetLogonReward,sizeof(GetLogonReward));
}

//获取在线奖励返回
bool CAttemperEngineSink::On_CMD_LC_Service_OnlineReward( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_ONLINE_REWARD));
	if(wDataSize!=sizeof(STR_DBO_CL_SERCIVR_ONLINE_REWARD))
		return false;

	STR_DBO_CL_SERCIVR_ONLINE_REWARD *pLotteryResult = (STR_DBO_CL_SERCIVR_ONLINE_REWARD*)pData;

	STR_CMD_LC_SERVICE_ONLINE_REWARD LotteryResult;
	ZeroMemory(&LotteryResult,sizeof(STR_CMD_LC_SERVICE_ONLINE_REWARD));

	LotteryResult.byType = pLotteryResult->byType;
	LotteryResult.dwCount = pLotteryResult->dwCount;
	
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_ONLINE_REWARD, &LotteryResult, sizeof(STR_CMD_LC_SERVICE_ONLINE_REWARD));

	return true;
}

//获取任务列表
bool CAttemperEngineSink::On_SUB_CL_Service_GetTaskList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT(wDataSize >= sizeof(STR_SUB_CL_SERVICE_GET_TASKLIST));
	if (wDataSize<sizeof(STR_SUB_CL_SERVICE_GET_TASKLIST)) return false;

	//处理消息
	STR_SUB_CL_SERVICE_GET_TASKLIST * pGetTaskList = (STR_SUB_CL_SERVICE_GET_TASKLIST *)pData;

	//变量定义
	STR_DBR_CL_SERCIVR_GET_TASKLIST GetTaskList;
	ZeroMemory(&GetTaskList,sizeof(GetTaskList));

	//构造数据
	memcpy(&GetTaskList,pGetTaskList,sizeof(GetTaskList));

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_GET_TASK_LIST,dwSocketID,&GetTaskList,sizeof(GetTaskList));

	return true;
}

//获取任务列表返回
bool CAttemperEngineSink::On_CMD_LC_Service_GetTaskList( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	if( 0 != (wDataSize % sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST)) )
		return false;

	//获得任务个数
	int count = wDataSize / sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST);

	//数据库任务列表
	STR_DBO_CL_SERCIVR_GET_TASKLIST *TaskList = (STR_DBO_CL_SERCIVR_GET_TASKLIST *)pData;

	//返回的任务列表
	STR_CMD_LC_SERVICE_GET_TASKLIST *pCMDTaskList = new STR_CMD_LC_SERVICE_GET_TASKLIST[count];

	//网络数据
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//枚举数据
	for (int i=0; i<count; i++)
	{
		//将DBO数据赋给返回的数据,完成标志单独赋值
		memcpy_s(&pCMDTaskList[i], sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST), &TaskList[i], sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST));

		//最后一次发送,设置完成标志为1
		if ( i == (count-1) )		
		{
			pCMDTaskList[i].cbListFinishMask = 1;
		}
		else
		{
			pCMDTaskList[i].cbListFinishMask = 0;
		}
		
		//发送数据(防止数据太多，一批批发送)
		if ( (wSendSize+sizeof(STR_CMD_LC_SERVICE_GET_TASKLIST)) > sizeof(cbDataBuffer) )
		{
			//发送数据
			m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_SERVICE_GET_TASK_LIST, cbDataBuffer, wSendSize);			
			wSendSize=0;
		}

		//拷贝数据
		CopyMemory(cbDataBuffer+wSendSize, &pCMDTaskList[i], sizeof(STR_CMD_LC_SERVICE_GET_TASKLIST));
		wSendSize += sizeof(STR_CMD_LC_SERVICE_GET_TASKLIST);
	}

	//发送剩余
	if (wSendSize>0) 	
	{
		m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_SERVICE_GET_TASK_LIST, cbDataBuffer, wSendSize);
	}

	//释放内存		TODO 曾经报错过
	delete [] pData;		//前面数据时new出来的，所以需要delete，进程之前是内存传输的
	delete [] pCMDTaskList;

	return true;
}

//领取任务奖励
bool CAttemperEngineSink::On_SUB_CL_Service_GetTaskReward(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT(wDataSize >= sizeof(STR_SUB_CL_SERVICE_GET_TASK_REWARD));
	if (wDataSize<sizeof(STR_SUB_CL_SERVICE_GET_TASK_REWARD)) return false;

	//处理消息
	STR_SUB_CL_SERVICE_GET_TASK_REWARD * pSetTaskDone =(STR_SUB_CL_SERVICE_GET_TASK_REWARD *)pData;
	//变量定义
	STR_DBR_CL_SERCIVR_GET_TASK_REWARD SetTaskDone;
	ZeroMemory(&SetTaskDone,sizeof(SetTaskDone));

	//构造数据
	memcpy(&SetTaskDone,pSetTaskDone,sizeof(SetTaskDone));

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_GET_TASK_REWARD,dwSocketID,&SetTaskDone,sizeof(SetTaskDone));

	return true;
}

//领取任务奖励返回
bool CAttemperEngineSink::On_CMD_LC_Service_GetTaskReward( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_GET_TASK_REWARD));
	if(wDataSize!=sizeof(STR_DBO_CL_SERCIVR_GET_TASK_REWARD))
		return false;

	STR_DBO_CL_SERCIVR_GET_TASK_REWARD *pTaskDone = (STR_DBO_CL_SERCIVR_GET_TASK_REWARD*)pData;

	STR_CMD_LC_SERVICE_GET_TASK_REWARD TaskDone;
	ZeroMemory(&TaskDone,sizeof(STR_CMD_LC_SERVICE_GET_TASK_REWARD));
	memcpy(&TaskDone,pTaskDone,sizeof(STR_CMD_LC_SERVICE_GET_TASK_REWARD));

	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_SERVICE_GET_TASK_REWARD, &TaskDone, sizeof(STR_CMD_LC_SERVICE_GET_TASK_REWARD));

	return true;
}

//获取排行榜
bool CAttemperEngineSink::On_SUB_CL_Service_GetRankList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	ASSERT(wDataSize==sizeof(STR_SUB_CL_SERVICE_GET_RANKLIST));
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_GET_RANKLIST))
		return false;

	STR_SUB_CL_SERVICE_GET_RANKLIST* pTaskList = (STR_SUB_CL_SERVICE_GET_RANKLIST*)pData;

	STR_CMD_LC_SERVICE_GET_RANKLIST rankList;
	ZeroMemory(&rankList, sizeof(STR_CMD_LC_SERVICE_GET_RANKLIST));

	//排行榜管理器获取排行榜列表
	int wCount = ((RankManager*)m_pRankManager)->GetRankCountByIndex(pTaskList->byIndex);

	for(int i=0;i < 20 && i < wCount;i++)
	{
		tagRankInfo* item = ((RankManager*)m_pRankManager)->GetRankItemByIndex(pTaskList->byIndex,i);
		if(item != NULL)
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_SERVICE, CMD_LC_SERVICE_GET_RANK_LIST, item, sizeof(STR_CMD_LC_SERVICE_GET_RANKLIST));
		}
	}

	return true;
}

//请求抽奖
bool CAttemperEngineSink::On_SUB_CL_Service_RequestLottery(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	ASSERT(wDataSize==sizeof(STR_SUB_CL_SERVICE_REQUEST_LOTTERY));
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_REQUEST_LOTTERY))
		return false;

	STR_SUB_CL_SERVICE_REQUEST_LOTTERY* pGetRankReward = (STR_SUB_CL_SERVICE_REQUEST_LOTTERY*)pData;

	STR_DBR_CL_SERCIVR_REQUEST_LOTTERY QueryLottery;
	memcpy(&QueryLottery,pGetRankReward,sizeof(STR_DBR_CL_SERCIVR_REQUEST_LOTTERY));

	return m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_REQUEST_LOTTERY,dwSocketID,&QueryLottery,sizeof(STR_DBR_CL_SERCIVR_REQUEST_LOTTERY));

}

//请求抽奖返回
bool CAttemperEngineSink::On_CMD_LC_Service_RequestLottery( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_REQUEST_LOTTERY));
	if(wDataSize!=sizeof(STR_DBO_CL_SERCIVR_REQUEST_LOTTERY))
		return false;

	STR_DBO_CL_SERCIVR_REQUEST_LOTTERY *pLotteryResult = (STR_DBO_CL_SERCIVR_REQUEST_LOTTERY*)pData;

	//构造抽奖数据
	STR_CMD_LC_SERVICE_REQUEST_LOTTERY LotteryResult;
	ZeroMemory(&LotteryResult,sizeof(STR_CMD_LC_SERVICE_REQUEST_LOTTERY));

	//赋值
	LotteryResult.byIndex = pLotteryResult->byIndex;
	LotteryResult.lResultCode = pLotteryResult->lResultCode;
	lstrcpyn(LotteryResult.szDescribeString, pLotteryResult->szDescribeString, CountArray(LotteryResult.szDescribeString));

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_REQUEST_LOTTERY, &LotteryResult, sizeof(STR_CMD_LC_SERVICE_REQUEST_LOTTERY));

	return true;
}

//pure大厅排行版 查询
bool CAttemperEngineSink::On_SUB_CL_SERVICE_PURE_STANDING_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_PURE_STANDING_LIST))
		return false;

	return m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_PURE_STANDING_LIST,dwSocketID,pData, wDataSize);
}
//pure大厅排行版 返回
bool CAttemperEngineSink::On_CMD_LC_SERVICE_PURE_STANDING_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_PURE_STANDING_LIST);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_PURE_STANDING_LIST, pData, wDataSize);

	return true;
}
//pure大厅排行版 结束
bool CAttemperEngineSink::On_CMD_LC_SERVICE_PURE_STANDING_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	if(wDataSize!=sizeof(STR_CMD_LC_SERVICE_PURE_STANDING_FINISH))
		return false;

	STR_CMD_LC_SERVICE_PURE_STANDING_FINISH *pLotteryResult = (STR_CMD_LC_SERVICE_PURE_STANDING_FINISH*)pData;
	pLotteryResult->byMask = 1;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_PURE_STANDING_FINISH, pLotteryResult, sizeof(STR_CMD_LC_SERVICE_PURE_STANDING_FINISH));

	return true;
}

//pure大局战绩 查询
bool CAttemperEngineSink::On_SUB_CL_SERVICE_PURE_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_PURE_RECORD_LIST))
		return false;

	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_PURE_RECORD_LIST,dwSocketID,pData, wDataSize);

	return true;
}
//pure大局战绩 返回
bool CAttemperEngineSink::On_CMD_LC_SERVICE_PURE_RECORD_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_PURE_RECORD_LIST, pData, wDataSize);

	return true;
}
//pure大局玩家信息 返回
bool CAttemperEngineSink::On_CMD_LC_SERVICE_PURE_RECORD_LIST_PINFO( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST_PLAYERINFO);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_PURE_RECORD_LIST_PINFO, pData, wDataSize);

	return true;
}
//pure大局战绩 结束
bool CAttemperEngineSink::On_CMD_LC_SERVICE_PURE_RECORD_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	if(wDataSize!=sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST_FINIST))
		return false;

	STR_CMD_LC_SERVICE_PURE_RECORD_LIST_FINIST *pLotteryResult = (STR_CMD_LC_SERVICE_PURE_RECORD_LIST_FINIST*)pData;
	pLotteryResult->byMask = 1;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_PURE_RECORD_FINISH, pLotteryResult, sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST_FINIST));

	return true;
}

//pure小局战绩 查询
bool CAttemperEngineSink::On_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST))
		return false;

	return m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_PURE_XJ_RECORD_LIST,dwSocketID,pData, wDataSize);
}
//pure小局战绩 返回
bool CAttemperEngineSink::On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST * pCMD = (STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST *) pData;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_PURE_XJ_RECORD_LIST, pData, wDataSize);

	return true;
}
//pure小局玩家信息 返回
bool CAttemperEngineSink::On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PLAYERINFO);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO, pData, wDataSize);

	return true;
}
//pure小局战绩 结束
bool CAttemperEngineSink::On_CMD_LC_SERVICE_PURE_XJ_RECORD_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	if(wDataSize!=sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_FINISH))
		return false;

	STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_FINISH *pLotteryResult = (STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_FINISH*)pData;
	pLotteryResult->byMask = 1;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_PURE_XJ_RECORD_FINISH, pLotteryResult, sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_FINISH));

	return true;
}

//小局录像回放
bool CAttemperEngineSink::On_SUB_CL_Service_XJRecordPlayback(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//数据大小校验
	if ( wDataSize != sizeof(STR_SUB_CL_SERVICE_XJ_RECORD_PLAYBACK) )
		return false;

	//SUB数据
	STR_SUB_CL_SERVICE_XJ_RECORD_PLAYBACK *SUB = (STR_SUB_CL_SERVICE_XJ_RECORD_PLAYBACK *)pData;
	
	//构造DBR数据
	STR_DBR_CL_SERVICE_XJ_RECORD_PLAYBACK DBR;
	ZeroMemory(&DBR, sizeof(STR_DBR_CL_SERVICE_XJ_RECORD_PLAYBACK));
	DBR.dwRecordID = SUB->dwRecordID;

	return m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_XJ_RECORD_PLAYBACK, dwSocketID, &DBR, wDataSize);
}
//小局录像回放 返回
bool CAttemperEngineSink::On_CMD_LC_Service_XJRecordPlayback( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	if(wDataSize != sizeof(STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK))
		return false;

	//DBO数据
	STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK *pDBO = (STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK*)pData;

	BYTE cbBuffer[LEN_MAX_RECORD_SIZE];	
	memcpy_s(cbBuffer, sizeof(cbBuffer), pDBO->cbRecordData, sizeof(cbBuffer));

	//分批发送
	for (int i = 0; i < 4; i++)
	{
		STR_CMD_LC_SERVICE_XJ_RECORD_PLAYBACK CMD;
		ZeroMemory(&CMD, sizeof(STR_CMD_LC_SERVICE_XJ_RECORD_PLAYBACK));
		memcpy_s(CMD.cbRecordData, sizeof(CMD.cbRecordData), cbBuffer+i*LEN_MAX_RECORD_SIZE/4, sizeof(CMD.cbRecordData));
		CMD.cbfinish = (i==3) ? 1 : 0;

		//发送数据
		m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_SERVICE_XJ_RECORD_PLAYBACK, &CMD, sizeof(CMD));

	}

	return true;
}

//客服消息
bool CAttemperEngineSink::On_SUB_CL_SERVICE_CUSTOMER_MESSEGE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//数据大小校验
	if ( wDataSize != sizeof(STR_SUB_CL_SERVICE_CUSTOMER_MESSEGE) )
		return false;

	//SUB数据
	STR_SUB_CL_SERVICE_CUSTOMER_MESSEGE *SUB = (STR_SUB_CL_SERVICE_CUSTOMER_MESSEGE *)pData;
	
	//构造DBR数据
	STR_DBR_CL_SERVICE_CUSTOMER_MESSEGE DBR;
	ZeroMemory(&DBR, sizeof(STR_DBR_CL_SERVICE_CUSTOMER_MESSEGE));
	DBR.cbMessegeFlag = SUB->cbMessegeFlag;

	return m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_CUSTOMER_MESSEGE, dwSocketID, &DBR, sizeof(DBR));
}
//客服消息 返回
bool CAttemperEngineSink::On_CMD_LC_SERVICE_CUSTOMER_MESSEGE( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	if(wDataSize != sizeof(STR_DBO_LC_SERVICE_CUSTOMER_MESSEGE))
		return false;

	//DBO数据
	STR_DBO_LC_SERVICE_CUSTOMER_MESSEGE *pDBO = (STR_DBO_LC_SERVICE_CUSTOMER_MESSEGE*)pData;

	//构造CMD数据
	STR_CMD_LC_SERVICE_CUSTOMER_MESSEGE CMD;
	ZeroMemory(&CMD, sizeof(STR_CMD_LC_SERVICE_CUSTOMER_MESSEGE));
	CMD.cbMessegeFlag = pDBO->cbMessegeFlag;
	lstrcpyn(CMD.szMessege, pDBO->szMessege, CountArray(CMD.szMessege));

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_SERVICE_CUSTOMER_MESSEGE, &CMD, sizeof(STR_CMD_LC_SERVICE_CUSTOMER_MESSEGE));

	return true;
}

//请求金币大厅信息
bool CAttemperEngineSink::On_SUB_CL_SERVICE_GOLD_INFO(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_GOLD_INFO))
		return false;

	return m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_GOLD_INFO,dwSocketID,pData, wDataSize);
}
//请求金币大厅信息 返回
bool CAttemperEngineSink::On_CMD_LC_SERVICE_GOLD_INFO( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_GOLD_INFO);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_GOLD_INFO, pData, wDataSize);

	return true;
}
//请求金币大厅信息 结束
bool CAttemperEngineSink::On_CMD_LC_SERVICE_GOLD_INFO_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	if(wDataSize!=sizeof(STR_CMD_LC_SERVICE_GOLD_INFO_FINISH))
		return false;

	STR_CMD_LC_SERVICE_GOLD_INFO_FINISH *pLotteryResult = (STR_CMD_LC_SERVICE_GOLD_INFO_FINISH*)pData;
	pLotteryResult->byMask = 1;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_GOLD_INFO_FINISH, pLotteryResult, sizeof(STR_CMD_LC_SERVICE_GOLD_INFO_FINISH));

	return true;
}

//修改个人资料
bool CAttemperEngineSink::On_SUB_CL_Service_ModifyPersonalInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验
	if (wDataSize != sizeof(STR_SUB_CL_SERVICE_MODIFY_PERSONAL_INFO)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_MODIFY_PERSONAL_INFO, dwSocketID, pData, wDataSize);

	return true;
}

//修改个人资料返回
bool CAttemperEngineSink::On_CMD_LC_Service_ModifyPersonalInfo( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	if(wDataSize!=sizeof(STR_DBO_CL_MODIFY_PERSONL_INFO))
		return false;

	STR_DBO_CL_MODIFY_PERSONL_INFO *pModifyInfo = (STR_DBO_CL_MODIFY_PERSONL_INFO*)pData;

	//构造数据
	STR_CMD_LC_SERVICE_MODIFY_PERSONL_INFO PersonalInfo;
	ZeroMemory(&PersonalInfo,sizeof(STR_CMD_LC_SERVICE_MODIFY_PERSONL_INFO));
	CopyMemory(&PersonalInfo, pModifyInfo, sizeof(PersonalInfo));

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_SERVICE_MODIFY_PERSONAL_INFO, &PersonalInfo, sizeof(STR_CMD_LC_SERVICE_MODIFY_PERSONL_INFO));

	return true;
}

//查询金币房卡返回
bool CAttemperEngineSink::On_CMD_LC_Service_QueryScoreInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO * pScoreInfo =(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO *)pData;

	//变量定义
	STR_CMD_LC_QUERY_SCORE_INFO ScoreInfo;
	ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));

	//描述信息
	ScoreInfo.lResultCode = pScoreInfo->lResultCode;
	lstrcpyn(ScoreInfo.szDescribeString, pScoreInfo->szDescribeString, CountArray(ScoreInfo.szDescribeString));

	//财富信息
	ScoreInfo.dwUserID = pScoreInfo->dwUserID;
	ScoreInfo.lGold = pScoreInfo->lGold;
	ScoreInfo.lOpenRoomCard = pScoreInfo->lOpenRoomCard;
	ScoreInfo.lDiamond = pScoreInfo->lDiamond;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_USER_QUERY_SCORE_INFO, &ScoreInfo, sizeof(STR_CMD_LC_QUERY_SCORE_INFO));

	return true;
}

//公共操作结果
bool CAttemperEngineSink::On_CMD_LC_CommonOperateResult( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//判断在线
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	STR_CMD_LC_OTHER_OPERATE_RESULT OperateResult;
	ZeroMemory(&OperateResult,sizeof(OperateResult));

	//变量定义
	DBO_GP_OperateCommand * pOperate=(DBO_GP_OperateCommand *)pData;

	//构造数据
	OperateResult.lResultCode=pOperate->lResultCode;

	//发送数据
	WORD wHeadSize=sizeof(OperateResult);
	m_pITCPNetworkEngine->SendData(dwContextID,pOperate->mCommand.MainCommand,pOperate->mCommand.SubCommand,&OperateResult,wHeadSize);
	return true;
}

//领取任务返回
bool CAttemperEngineSink::OnDBRankRewardResult( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_GET_RANK_REWARD));
	if(wDataSize!=sizeof(STR_DBO_CL_SERCIVR_GET_RANK_REWARD))
		return false;

	STR_DBO_CL_SERCIVR_GET_RANK_REWARD *pRankDone = (STR_DBO_CL_SERCIVR_GET_RANK_REWARD*)pData;

	STR_CMD_LC_SERVICE_GET_RANK_REWARD RankDone;
	ZeroMemory(&RankDone,sizeof(STR_CMD_LC_SERVICE_GET_RANK_REWARD));
	memcpy(&RankDone,pRankDone,sizeof(STR_CMD_LC_SERVICE_GET_RANK_REWARD));

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE,CMD_LC_SERVICE_GET_RANK_REWARD,&RankDone,sizeof(STR_CMD_LC_SERVICE_GET_RANK_REWARD));

	return true;
}
#pragma endregion

/***************************************** 【其他消息处理函数-主消息6】 *******************************************/
//用户充值信息
bool CAttemperEngineSink::On_SUB_CL_Other_ReChargeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	ASSERT( wDataSize == sizeof(STR_SUB_CL_OTHER_RECHARGE_INFO));
	if(wDataSize != sizeof(STR_SUB_CL_OTHER_RECHARGE_INFO)) return false;

	//处理消息
	STR_SUB_CL_OTHER_RECHARGE_INFO * pUserRequest = (STR_SUB_CL_OTHER_RECHARGE_INFO *)pData;
	//定义变量
	STR_DBR_CL_OTHER_RECHARGE_INFO UserRequest;
	ZeroMemory(&UserRequest,sizeof(UserRequest));

	UserRequest.dwUserID = pUserRequest->dwUserID;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_USER_RECHARGE_INFO,dwSocketID,&UserRequest,sizeof(UserRequest));
	return true;
}

//用户充值信息返回
bool CAttemperEngineSink::On_CMD_LC_Other_RechargeInfo( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//判断在线
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	STR_CMD_LC_OTHER_RECHARGE_INFO RechangeInfo;
	ZeroMemory(&RechangeInfo,sizeof(RechangeInfo));

	//变量定义
	STR_DBO_CL_OTHER_RECHARGE_INFO * pRechangeInfo = (STR_DBO_CL_OTHER_RECHARGE_INFO *)pData;

	//构造数据
	RechangeInfo.dwMinMoney = pRechangeInfo->dwMinMoney;
	RechangeInfo.dwChangeScale = pRechangeInfo->dwChangeScale;

	//发送数据
	WORD wHeadSize=sizeof(RechangeInfo);
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_OTHERS_RECHARGE_INFO, &RechangeInfo, wHeadSize);
	return true;
}

#pragma region 礼物道具
//赠送道具
bool CAttemperEngineSink::On_SUB_CL_GIFT_GIVE_PROPS(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	if(wDataSize!=sizeof(STR_SUB_CL_GIFT_GIVE_PROPS))
		return false;

	return m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_GIFT_GIVE_PROPS,dwSocketID, pData, wDataSize);
}

//赠送道具返回
bool CAttemperEngineSink::On_CMD_LC_GIFT_GIVE_PROPS( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	if(wDataSize!=sizeof(STR_CMD_LC_GIFT_GIVE_PROPS))
		return false;

	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GIFT, CMD_LC_GIFT_GIVE_PROPS, pData, wDataSize);

	return true;
}

//赠送道具 通知接受人
bool CAttemperEngineSink::On_CMD_LC_GIFT_GIVE_PROPS_SHOW( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//参数校验
	if(wDataSize!=sizeof(STR_CMD_LC_GIFT_GIVE_PROPS_SHOW))
		return false;

	STR_CMD_LC_GIFT_GIVE_PROPS_SHOW *pCMD = (STR_CMD_LC_GIFT_GIVE_PROPS_SHOW*) pData;

	//查找在线用户 发送数据
	for (int j = 0; j < MAX_CONTENT; j++)
	{
		tagBindParameter * pBindParameter=(m_pBindParameter+j);
		if ( pBindParameter && pBindParameter->dwUserID != 0 && 
			pBindParameter->dwSocketID != 0 && 
			pBindParameter->dwUserID == pCMD->dwTargetID)
		{
			//通知用户
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GIFT, CMD_LC_GIFT_GIVE_PROPS_SHOW, 
											pData, wDataSize);		
		}
	}

	//TODONOW 后期赠送礼物的消息频道 可以在这里发送   
	//1. 世界聊天的时候一定不要加头像, 只写昵称和聊天的消息即可
	//2. 客户端增加一个按钮, 可以控制是否接受世界频道 
	return true;
}

#pragma endregion


//兑换道具
bool CAttemperEngineSink::On_SUB_CL_Other_ExchangeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	ASSERT( wDataSize == sizeof(STR_SUB_CL_OTHER_EXCHANGE_INFO));
	if(wDataSize != sizeof(STR_SUB_CL_OTHER_EXCHANGE_INFO)) return false;

	//处理消息
	STR_SUB_CL_OTHER_EXCHANGE_INFO * pUserRequest = (STR_SUB_CL_OTHER_EXCHANGE_INFO *)pData;
	//定义变量
	STR_DBR_CL_OTHER_EXCHANGE_INFO UserRequest;
	ZeroMemory(&UserRequest,sizeof(UserRequest));

	UserRequest.dwUserID = pUserRequest->dwUserID;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_USER_EXCHANGE_INFO, dwSocketID, &UserRequest, sizeof(UserRequest));
	return true;
}

//兑换道具返回
bool CAttemperEngineSink::On_CMD_LC_Other_ExchangeInfo( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//判断在线
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	STR_DBO_CL_OTHER_EXCHANGE_INFO * pExchangeInfo=(STR_DBO_CL_OTHER_EXCHANGE_INFO *)pData;

	//构造数据
	STR_CMD_LC_OTHER_EXCHANGE_INFO ExchangeInfo;
	ZeroMemory(&ExchangeInfo,sizeof(ExchangeInfo));

	ExchangeInfo.dwMinMoney = pExchangeInfo->dwMinMoney;
	ExchangeInfo.dwChangeScale = pExchangeInfo->dwChangeScale;
	ExchangeInfo.dwBankMoney = pExchangeInfo->dwBankMoney;
	ExchangeInfo.dwWithdrawals = pExchangeInfo->dwWithdrawals;

	//发送数据
	WORD wHeadSize=sizeof(ExchangeInfo);
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_OTHERS_EXCHANGE_INFO, &ExchangeInfo, wHeadSize);

	return true;
}

#pragma region MDM_CLUB 牌友圈(版本2)
//查询牌友圈列表
bool CAttemperEngineSink::On_SUB_CL_CLUB_ALL_CLUB_INFO_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ALL_CLUB_INFO_LIST)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_ALL_CLUB_INFO_LIST, dwSocketID, pData, wDataSize);

	return true;
}
//查询牌友圈列表结果
bool CAttemperEngineSink::On_CMD_LC_CLUB_ALL_CLUB_INFO_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST);
	if( (wDataSize % Size) != 0) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_ALL_CLUB_INFO_LIST, pData, wDataSize);
	return true;
}
//查询牌友圈列表结束
bool CAttemperEngineSink::On_CMD_LC_CLUB_ALL_INFO_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_ALL_INFO_FINISH cmd;
	cmd.byMask = 1;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_ALL_INFO_FINISH, &cmd, sizeof(STR_CMD_LC_CLUB_ALL_INFO_FINISH));
	return true;
}

//查询指定牌友圈房间列表
bool CAttemperEngineSink::On_SUB_CL_CLUB_ROOM_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_LIST)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_ROOM_LIST, dwSocketID, pData, wDataSize);
	return true;
}
//查询指定牌友圈房间列表 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_ROOM_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_ROOM_LIST);
	if( (wDataSize % Size ) != 0) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_ROOM_LIST, pData, wDataSize);
	return true;
}
//查询指定牌友圈房间列表 结束
bool CAttemperEngineSink::On_CMD_LC_CLUB_ROOM_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_ROOM_LIST_FINISH cmd;
	cmd.byMask = 1;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_ROOM_LIST_FINISH, &cmd, sizeof(STR_CMD_LC_CLUB_ROOM_LIST_FINISH));
	return true;
}

//查询未满员, 随机牌友圈(最大9个)
bool CAttemperEngineSink::On_SUB_CL_CLUB_RANDOM_CLUB_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_RANDOM_CLUB_LIST)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_RANDOM_CLUB_LIST, dwSocketID, pData, wDataSize);
	return true;
}
//查询未满员, 随机牌友圈(最大9个) 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_RANDOM_CLUB_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_RANDOM_CLUB_LIST);
	if( (wDataSize % Size) != 0) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_RANDOM_CLUB_LIST, pData, wDataSize);
	return true;
}
//查询未满员, 随机牌友圈(最大9个) 结束
bool CAttemperEngineSink::On_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH cmd;
	cmd.byMask = 1;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH, &cmd, sizeof(STR_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH));
	return true;
}

//申请加入牌友圈
bool CAttemperEngineSink::On_SUB_CL_CLUB_JOIN_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_JOIN_CLUB)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_JOIN_CLUB, dwSocketID, pData, wDataSize);
	return true;
}
//申请加入牌友圈返回 
bool CAttemperEngineSink::On_CMD_LC_CLUB_JOIN_CLUB( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_JOIN_CLUB);
	if( wDataSize != Size) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_JOIN_CLUB, pData, wDataSize);
	return true;
}
//申请加入牌友圈广播
bool CAttemperEngineSink::On_CMD_LC_CLUB_JOIN_CLUB_BDCAST( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_SUB_CL_CLUB_JOIN_CLUB_BDCAST);
	if( wDataSize != Size) return false;

	//TODONOW 暂时是每个人都发送，后面改为 1.特定俱乐部的  2.会长和管理员发送
	m_pITCPNetworkEngine->SendDataBatch(MDM_CLUB, CMD_LC_CLUB_JOIN_CLUB_BDCAST, pData, wDataSize);
	return true;
}
//申请加入牌友圈 通知客户端实时刷新
bool CAttemperEngineSink::On_CMD_LC_CLUB_JOIN_CLUB_RE( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_LIST_RE);
	if( wDataSize != Size) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_LIST_RE, pData, wDataSize);
	return true;
}


//解散牌友圈
bool CAttemperEngineSink::On_SUB_CL_CLUB_DISS_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_DISS_CLUB)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_DISS_CLUB, dwSocketID, pData, wDataSize);
	return true;
}
//解散牌友圈 返回 
bool CAttemperEngineSink::On_CMD_LC_CLUB_DISS_CLUB( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_DISS_CLUB);
	if( wDataSize != Size) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_DISS_CLUB, pData, wDataSize);
	return true;
}

//创建牌友圈
bool CAttemperEngineSink::On_SUB_CL_CLUB_CREATE_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_CREATE_CLUB)) return false;

	//处理消息
	STR_SUB_CL_CLUB_CREATE_CLUB * pSub = (STR_SUB_CL_CLUB_CREATE_CLUB *)pData;
	//定义变量
	STR_DBR_CL_CLUB_CREATE_CLUB Dbr;
	ZeroMemory(&Dbr,sizeof(Dbr));

	Dbr.dwUserID = pSub->dwUserID;
	lstrcpyn(Dbr.szClubName, pSub->szClubName, CountArray(Dbr.szClubName));
	lstrcpyn(Dbr.szNotice, pSub->szNotice, CountArray(Dbr.szNotice));
	lstrcpyn(Dbr.szMessag, pSub->szMessag, CountArray(Dbr.szMessag));
	Dbr.dwMajorKindID = pSub->dwMajorKindID;

	//构造信息
	for (WORD i=0; i<10; i++)
	{
		INT nLength=lstrlen(Dbr.szMinorKindID);
		_sntprintf(&Dbr.szMinorKindID[nLength],CountArray(Dbr.szMinorKindID)-nLength,
			TEXT("%ld,"),
			pSub->dwMinorKindID[i]);
	}

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_CREATE_CLUB, dwSocketID, &Dbr, sizeof(Dbr));
	return true;
}
//创建牌友圈 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_CREATE_CLUB( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_CREATE_CLUB);
	if( wDataSize != Size ) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_CREATE_CLUB, pData, wDataSize);
	return true;
}

//房间设置
bool CAttemperEngineSink::On_SUB_CL_CLUB_ROOM_SETTING(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_SETTING)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_ROOM_SETTING, dwSocketID, pData, wDataSize);
	return true;
}
//房间设置 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_ROOM_SETTING( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_ROOM_SETTING);
	if( wDataSize != Size) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_ROOM_SETTING, pData, wDataSize);
	return true;
}

//请求房间设置
bool CAttemperEngineSink::On_SUB_CL_CLUB_ROOM_QUERY_SETTING(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_QUERY_SETTING)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_ROOM_QUERY_SETTING, dwSocketID, pData, wDataSize);
	return true;
}
//请求房间设置 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_ROOM_QUERY_SETTING( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_ROOM_QUERY_SETTING);
	if( wDataSize != Size) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_ROOM_QUERY_SETTING, pData, wDataSize);
	return true;
}


//玩家离开俱乐部房间
bool CAttemperEngineSink::On_SUB_CL_CLUB_ROOM_USER_LEAVE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_USER_LEAVE)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_ROOM_USER_LEAVE, dwSocketID, pData, wDataSize);
	return true;
}

//解散房间请求
bool CAttemperEngineSink::On_SUB_CL_CLUB_ROOM_DISSOLVE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_DISSOLVE)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_ROOM_DISSOLVE, dwSocketID, pData, wDataSize);
	return true;
}
//解散房间请求 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_ROOM_DISSOLVE( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_ROOM_DISSOLVE);
	if( wDataSize != Size) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_ROOM_DISSOLVE, pData, wDataSize);
	return true;
}

//解散房间请求
bool CAttemperEngineSink::On_SUB_CL_CLUB_TABLE_DISSOLVE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_TABLE_DISSOLVE)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_TABLE_DISSOLVE, dwSocketID, pData, wDataSize);
	return true;
}
//解散房间请求 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_TABLE_DISSOLVE( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_TABLE_DISSOLVE);
	if( wDataSize != Size) return false;

	STR_CMD_LC_CLUB_TABLE_DISSOLVE* pCmd = (STR_CMD_LC_CLUB_TABLE_DISSOLVE *) pData;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_TABLE_DISSOLVE, pData, wDataSize);

	if( (DB_SUCCESS ==  pCmd->lResultCode) && (0 == pCmd->byMask) && (0 != pCmd->dwGameID))
	{
		//通知协调服, 协调服通知游戏服
		STR_CPR_LP_CLUB_TABLE_DISSOLVE CPR;
		CPR.dwGameID = pCmd->dwGameID;
		CPR.dwTableID = pCmd->dwTableID;

		m_pITCPSocketEngine->SendData(MDM_TRANSFER, CPR_LP_CLUB_TABLE_DISSOLVE, &CPR, sizeof(STR_CPR_LP_CLUB_TABLE_DISSOLVE));
	}
	return true;
}


//牌友圈公告
bool CAttemperEngineSink::On_SUB_CL_CLUB_NOTICE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_NOTICE)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_NOTICE, dwSocketID, pData, wDataSize);
	return true;
}
//牌友圈公告 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_NOTICE( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_NOTICE);
	if( wDataSize != Size) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_NOTICE, pData, wDataSize);
	return true;
}
//牌友圈公告 广播
bool CAttemperEngineSink::On_CMD_LC_CLUB_NOTICE_BDCAST( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//TODONOW 需要实现
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_SHOP_QUERY_RESULT);
	if( wDataSize != Size) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_ROOM_SETTING, pData, wDataSize);
	return true;
}

//聊天
bool CAttemperEngineSink::On_SUB_CL_CLUB_CHAT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_CHAT)) return false;
	STR_SUB_CL_CLUB_CHAT *pSub = (STR_SUB_CL_CLUB_CHAT*)pData;

	if(0 == pSub->byChatMode) //俱乐部聊天
	{
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_CHAT, dwSocketID, pData, wDataSize);
	}
	else if( 1 == pSub->byChatMode) //世界聊天
	{
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_WORD_CHAT, dwSocketID, pData, wDataSize);
	}
	else if( 2 == pSub->byChatMode) //系统聊天
	{
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SYSTEM_CHAT, dwSocketID, pData, wDataSize);
	}
	else if( 3 == pSub->byChatMode) //私聊
	{
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SECRET_CHAT, dwSocketID, pData, wDataSize);
	}

	return true;
}
//聊天 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_CHAT( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_CHAT);
	if( wDataSize != Size) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_CHAT, pData, wDataSize);
	return true;
}
//牌友圈聊天 广播
bool CAttemperEngineSink::On_CMD_LC_CLUB_CHAT_BDCAST( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	WORD Size = sizeof(STR_CMD_LC_CLUB_CHAT_BDCAST);
	if( (wDataSize < Size) || (( wDataSize % Size) != 0 )) return false;
	STR_CMD_LC_CLUB_CHAT_BDCAST *pCmd = (STR_CMD_LC_CLUB_CHAT_BDCAST*) pData;

	switch(pCmd->byChatMode)
	{
	case 0: //俱乐部聊天
		{
			int DataNum = wDataSize / Size;

			//查询用户
			for(int i = 0; i < DataNum; i ++)
			{
				//查找在线用户 发送数据
				for (int j = 0; j < MAX_CONTENT; j++)
				{
					tagBindParameter * pBindParameter=(m_pBindParameter+j);
					if ( pBindParameter && pBindParameter->dwUserID != 0 && 
						pBindParameter->dwSocketID != 0 && 
						(pBindParameter->dwUserID == (pCmd + i)->dwTagID))
					{
						//通知用户
						m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_CLUB, CMD_LC_CLUB_CHAT_BDCAST, 
							pCmd+i, sizeof(STR_CMD_LC_CLUB_CHAT_BDCAST));
					}
				}
			}
			break;
		}
	case 1: //世界聊天
		{
			m_pITCPNetworkEngine->SendDataBatch(MDM_CLUB, CMD_LC_CLUB_CHAT_BDCAST, pData, wDataSize);
			break;
		}
	case 2: //系统聊天
		{
			break;
		}
	case 3: //私密聊天
		{
			//TODONOW added by WangChengQing 需要修改下面的逻辑
			//根据ID找到用户类
			//根据用户类 找到对应的socket
			//判断socket是否在线

			//查找目标用户 发送数据
			for (int j = 0; j < MAX_CONTENT; j++)
			{
				tagBindParameter * pBindParameter=(m_pBindParameter+j);
				if ( pBindParameter && pBindParameter->dwUserID != 0 && 
					pBindParameter->dwSocketID != 0 && 
					(pBindParameter->dwUserID == (pCmd->dwTagID)))
				{
					//通知用户
					m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_CLUB, CMD_LC_CLUB_CHAT_BDCAST, 
						pData, wDataSize);
				}
			}
			break;
		}
	}
	
	return true;
}

//牌友圈置顶
bool CAttemperEngineSink::On_SUB_CL_CLUBSTICKY_POST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_STICKY_POST)) return false;
	STR_SUB_CL_CLUB_STICKY_POST *pSub = (STR_SUB_CL_CLUB_STICKY_POST*)pData;

	//构造DBR数据
	STR_DBR_CL_CLUB_STICKY_POST DBR;
	ZeroMemory(&DBR, sizeof(STR_DBR_CL_CLUB_STICKY_POST));
	DBR.dwClubID = pSub->dwClubID;
	DBR.cbTopFlag = pSub->cbTopFlag;
	DBR.dwUserID = pSub->dwUserID;

	//发送请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_STICKY_POST, dwSocketID, pData, wDataSize);

	return true;
}
//牌友圈置顶 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_STICKY_POST( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//校验参数
	if( wDataSize != sizeof(STR_CMD_LC_CLUB_STICKY_POST)) return false;

	//构造CMD数据
	STR_DBO_LC_CLUB_STICKY_POST *DBO = (STR_DBO_LC_CLUB_STICKY_POST *)pData;

	STR_CMD_LC_CLUB_STICKY_POST CMD;
	ZeroMemory(&CMD, sizeof(STR_CMD_LC_CLUB_STICKY_POST));
	CMD.lResultCode = DBO->lResultCode;

	//发送返回
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_STICKY_POST, &CMD, sizeof(CMD));

	return true;
}

//牌友圈简介
bool CAttemperEngineSink::On_SUB_CL_CLUB_MESSAGE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_MESSAGE)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_MESSAGE, dwSocketID, pData, wDataSize);
	return true;
}
//牌友圈简介 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_MESSAGE( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_MESSAGE);
	if( wDataSize != Size) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_MESSAGE, pData, wDataSize);
	return true;
}

//贡献房卡
bool CAttemperEngineSink::On_SUB_CL_CLUB_CONTRIBUTE_FK(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_CONTRIBUTE_FK)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_CONTRIBUTE_FK, dwSocketID, pData, wDataSize);
	return true;
}
//贡献房卡 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_CONTRIBUTE_FK( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_CONTRIBUTE_FK);
	if( wDataSize != Size) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_CONTRIBUTE_FK, pData, wDataSize);
	return true;
}

//牌友圈设置
bool CAttemperEngineSink::On_SUB_CL_CLUB_AUTO_AGREE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_AUTO_AGREE)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_AUTO_AGREE, dwSocketID, pData, wDataSize);
	return true;
}
//牌友圈设置 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_AUTO_AGREE( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_AUTO_AGREE);
	if( wDataSize != Size) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_AUTO_AGREE, pData, wDataSize);
	return true;
}

//申请加入房间
bool CAttemperEngineSink::On_SUB_CL_CLUB_JOIN_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_JOIN_ROOM)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_JOIN_ROOM, dwSocketID, pData, wDataSize);
	return true;
}
//申请加入房间 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_JOIN_ROOM( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_JOIN_ROOM);
	if( wDataSize != Size) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_JOIN_ROOM, pData, wDataSize);
	return true;
}
//桌子列表
bool CAttemperEngineSink::On_CMD_LC_CLUB_TABLE_LIST_TABLE( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_TABLE_LIST);
	if( (wDataSize % Size) != 0) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_TABLE_LIST_TABLE, pData, wDataSize);
	return true;
}
//桌子玩家列表
bool CAttemperEngineSink::On_CMD_LC_CLUB_TABLE_LIST_USER( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST);
	if( (wDataSize % Size) != 0) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_TABLE_LIST_USER, pData, wDataSize);
	return true;
}



//群主|管理对申请消息的答复(同意|拒绝)
bool CAttemperEngineSink::On_SUB_CL_CLUB_APPLICANT_RESULT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_APPLICANT_RESULT)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_APPLICANT_RESULT, dwSocketID, pData, wDataSize);
	return true;
}
//群主|管理对申请消息的答复(同意|拒绝) 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_APPLICANT_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_APPLICANT_RESULT);
	if( wDataSize != Size) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_APPLICANT_RESULT, pData, wDataSize);
	return true;
}

//请求成员数据
bool CAttemperEngineSink::On_SUB_CL_CLUB_MEMBER_MANAGER(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_MEMBER_MANAGER)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_MEMBER_MANAGER, dwSocketID, pData, wDataSize);
	return true;
}
//请求成员数据 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_MEMBER_MANAGER( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_MEMBER_MANAGER);
	if( (wDataSize % Size) != 0) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_MEMBER_MANAGER, pData, wDataSize);
	return true;
}
//请求成员数据 结束
bool CAttemperEngineSink::On_CMD_LC_CLUB_MEMBER_MANAGER_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_MEMBER_MANAGER_FINISH cmd;
	cmd.byMask = 1;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_MEMBER_MANAGER_FINISH, &cmd, sizeof(STR_CMD_LC_SHOP_QUERY_FINISH));
	return true;
}
//工会基本信息
bool CAttemperEngineSink::On_CMD_LC_CLUB_DATA( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_DATA);
	if( wDataSize != Size) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_DATA, pData, wDataSize);
	return true;
}

//邀请他入加入牌友圈
bool CAttemperEngineSink::On_SUB_CL_CLUB_INVITE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_INVITE)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_INVITE, dwSocketID, pData, wDataSize);

	return true;
}
//邀请他入加入牌友圈 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_INVITE( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_INVITE);
	if( wDataSize != Size) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_INVITE, pData, wDataSize);

	return true;
}

//被邀请人的提醒 
bool CAttemperEngineSink::On_CMD_LC_CLUB_INVITE_REMIND( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_INVITE_REMIND);
	if( wDataSize != Size) return false;
	STR_CMD_LC_CLUB_INVITE_REMIND * pCMD = (STR_CMD_LC_CLUB_INVITE_REMIND*) pData;

	//查找在线用户 发送数据
	for (int j = 0; j < MAX_CONTENT; j++)
	{
		tagBindParameter * pBindParameter=(m_pBindParameter+j);
		if ( pBindParameter && pBindParameter->dwUserID != 0 && 
			pBindParameter->dwSocketID != 0 && 
			pBindParameter->dwUserID == pCMD->dwTagID)
		{
			//通知用户
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_CLUB, CMD_LC_CLUB_INVITE_REMIND, 
											pData, wDataSize);		
		}
	}

	return true;
}

//被邀请人的回复
bool CAttemperEngineSink::On_SUB_CL_CLUB_INVITE_RESULT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_INVITE_RESULT)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_INVITE_RESULT, dwSocketID, pData, wDataSize);

	return true;
}
//被邀请人的回复 返回 
bool CAttemperEngineSink::On_CMD_LC_CLUB_INVITE_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_INVITE_RESULT);
	if( wDataSize != Size) return false;

	STR_CMD_LC_CLUB_INVITE_RESULT *pCMD = (STR_CMD_LC_CLUB_INVITE_RESULT*) pData;

	//如果返回成功, 则通知客户端刷新工会列表
	if(pCMD->lResultCode == DB_SUCCESS)
	{
		STR_CMD_LC_CLUB_LIST_RE CMD;
		CMD.byMask = 1;

		m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_LIST_RE, &CMD, sizeof(CMD));
	}

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_INVITE_RESULT, pData, wDataSize);

	return true;
}

//踢出退出请求
bool CAttemperEngineSink::On_SUB_CL_CLUB_QUIT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_QUIT)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_QUIT, dwSocketID, pData, wDataSize);
	return true;
}
//用户退出请求 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_QUIT( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_QUIT);
	if( wDataSize != Size) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_QUIT, pData, wDataSize);
	return true;
}

//职务任免
bool CAttemperEngineSink::On_SUB_CL_CLUB_APPOINTMENT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_APPOINTMENT)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_APPOINTMENT, dwSocketID, pData, wDataSize);
	return true;
}
//职务任免 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_APPOINTMENT( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_APPOINTMENT);
	if( wDataSize != Size) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_APPOINTMENT, pData, wDataSize);
	return true;
}
//职务任免 提醒
bool CAttemperEngineSink::On_CMD_LC_CLUB_APPOINTMENT_NOTE( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
		//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_APPOINTMENT_NOTE);
	if( wDataSize != Size) return false;
	STR_CMD_LC_CLUB_APPOINTMENT_NOTE * pCMD = (STR_CMD_LC_CLUB_APPOINTMENT_NOTE*) pData;

	//查找在线用户 发送数据
	for (int j = 0; j < MAX_CONTENT; j++)
	{
		tagBindParameter * pBindParameter=(m_pBindParameter+j);
		if ( pBindParameter && pBindParameter->dwUserID != 0 && 
			pBindParameter->dwSocketID != 0 && 
			pBindParameter->dwUserID == pCMD->dwUserID)
		{
			//通知用户
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_CLUB, CMD_LC_CLUB_APPOINTMENT_NOTE, 
											pData, wDataSize);		
		}
	}

	return true;
}


//申请人列表
bool CAttemperEngineSink::On_SUB_CL_CLUB_APPLICANT_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_APPLICANT_LIST)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_APPLICANT_LIST, dwSocketID, pData, wDataSize);
	return true;
}
//申请人列表 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_APPLICANT_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_APPLICANT_LIST);
	if( (wDataSize % Size) != 0) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_APPLICANT_LIST, pData, wDataSize);
	return true;
}
//申请人列表 结束
bool CAttemperEngineSink::On_CMD_LC_CLUB_APPLICANT_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_APPLICANT_LIST_FINISH cmd;
	cmd.byMask = 1;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_APPLICANT_LIST_FINISH, &cmd, sizeof(cmd));
	return true;
}

//被邀请人查看自己的邀请列表
bool CAttemperEngineSink::On_SUB_CL_CLUB_INQUERY_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_INQUERY_LIST)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_INQUERY_LIST, dwSocketID, pData, wDataSize);
	return true;
}
//被邀请人查看自己的邀请列表 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_INQUERY_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_INQUERY_LIST);
	if( (wDataSize % Size) != 0) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_INQUERY_LIST, pData, wDataSize);
	return true;
}
//被邀请人查看自己的邀请列表 结束
bool CAttemperEngineSink::On_CMD_LC_CLUB_INQUERY_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_INQUERY_LIST_FINISH cmd;
	cmd.byMask = 1;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_INQUERY_LIST_FINISH, &cmd, sizeof(cmd));
	return true;
}


//工会战绩统计
bool CAttemperEngineSink::On_SUB_CL_CLUB_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_RECORD_LIST)) return false;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_RECORD_LIST, dwSocketID, pData, wDataSize);
	return true;
}
//工会战绩统计 返回
bool CAttemperEngineSink::On_CMD_LC_CLUB_RECORD_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_RECORD_LIST);
	if( (wDataSize < Size) || 
		( (wDataSize % Size) != 0))
		return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_RECORD_LIST, pData, wDataSize);
	return true;
}
//工会战绩统计 结束
bool CAttemperEngineSink::On_CMD_LC_CLUB_RECORD_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_RECORD_FINISH cmd;
	cmd.byMask = 1;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_RECORD_FINISH, &cmd, sizeof(cmd));
	return true;
}

#pragma endregion

#pragma region MDM_SHOP 商城道具
//查询商城
bool CAttemperEngineSink::On_SUB_CL_SHOP_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_SHOP_QUERY)) return false;

	//处理消息
	STR_SUB_CL_SHOP_QUERY * pSub = (STR_SUB_CL_SHOP_QUERY *)pData;
	//定义变量
	STR_DBR_CL_SHOP_QUERY Dbr;
	ZeroMemory(&Dbr,sizeof(Dbr));

	Dbr.byGoodsType = pSub->byGoodsType;
	Dbr.dwUserID = pSub->dwUserID;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SHOP_QUERY, dwSocketID, &Dbr, sizeof(Dbr));
	return true;
}
//查询商城CMD
bool CAttemperEngineSink::On_CMD_LC_SHOP_QUERY_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_SHOP_QUERY_RESULT);
	if( (wDataSize%Size) != 0) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SHOP, CMD_LC_SHOP_QUERY_RESULT, pData, wDataSize);
	return true;
}
//查询商城结束CMD
bool CAttemperEngineSink::On_CMD_LC_SHOP_QUERY_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_SHOP_QUERY_FINISH cmd;
	cmd.byMask = 1;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SHOP, CMD_LC_SHOP_QUERY_FINISH, &cmd, sizeof(STR_CMD_LC_SHOP_QUERY_FINISH));
	return true;
}

//钻石购买道具
bool CAttemperEngineSink::On_SUB_CL_SHOP_DIAMOND(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_SHOP_DIAMOND)) return false;

	//处理消息
	STR_SUB_CL_SHOP_DIAMOND * pSub = (STR_SUB_CL_SHOP_DIAMOND *)pData;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SHOP_DIAMOND, dwSocketID, pData, wDataSize);
	return true;
}

//钻石购买道具CMD
bool CAttemperEngineSink::On_CMD_LC_SHOP_DIAMOND_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	ASSERT( wDataSize == sizeof(STR_DBO_CL_SHOP_DIAMOND));
	if(wDataSize != sizeof(STR_DBO_CL_SHOP_DIAMOND)) return false;

	STR_DBO_CL_SHOP_DIAMOND * pDBO = (STR_DBO_CL_SHOP_DIAMOND*)pData;
	STR_CMD_LC_SHOP_DIAMOND_RESULT CMD;
	CMD.lResultCode = pDBO->lResultCode;
	lstrcpyn(CMD.szDescribe,pDBO->szDescribeString,CountArray(CMD.szDescribe));
	
	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SHOP, CMD_LC_SHOP_DIAMOND_RESULT, &CMD, sizeof(CMD));
	return true;
}

//背包物品查询
bool CAttemperEngineSink::On_SUB_CL_BAG_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	ASSERT( wDataSize == sizeof(STR_SUB_CL_BAG_QUERY));
	if(wDataSize != sizeof(STR_SUB_CL_BAG_QUERY)) return false;

	//处理消息
	STR_SUB_CL_BAG_QUERY * pSub = (STR_SUB_CL_BAG_QUERY *)pData;
	//定义变量
	STR_DBR_CL_BAG_QUERY Dbr;
	ZeroMemory(&Dbr,sizeof(Dbr));

	Dbr.dwUserID = pSub->dwUserID;	

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_BAG_QUERY, dwSocketID, &Dbr, sizeof(Dbr));
	return true;
}

//背包物品查询CMD
bool CAttemperEngineSink::On_CMD_LC_BAG_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_DBO_CL_BAG_QUERY);
	if( (wDataSize%Size) != 0) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SHOP, CMD_LC_BAG_RESULT, pData, wDataSize);
	return true;
}

//背包物品查询结束CMD
bool CAttemperEngineSink::On_CMD_LC_BAG_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_BAG_FINISH);

	if( wDataSize != Size) return false;

	//处理消息
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SHOP, CMD_LC_BAG_FINISH, pData, wDataSize);
	return true;
}
#pragma endregion







