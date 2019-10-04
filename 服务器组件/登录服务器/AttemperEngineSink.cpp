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
# pragma region ��ʱ��
//ʱ���ʶ
#define  IDI_LOAD_GAME_LIST			1									//�����б�
#define  IDI_CONNECT_CORRESPOND		2									//������ʶ
#define  IDI_COLLECT_ONLINE_INFO	3									//ͳ���������� -- д�뵽���ݿ�
#define  IDI_UPDATA_ONLINE_COUNT	5									//������������ -- ���͸��ͻ���
#define  IDI_UPDATA_MARQUEE			6									//�����������Ϣ
#define	 IDI_PLAY_MARQUEE			7									//֪ͨclient�����������Ϣ

#define  TIME_COLLECT_ONLINE_INFO   1800L								//ͳ���������� -- д�뵽���ݿ�
#define  TIME_UPDATA_ONLINE_COUNT   30L									//������������ -- ���͸��ͻ���
#define  TIME_UPDATA_MARQUEE	    6L									//�����������Ϣ
#define	 TIME_CONNECT_CORRESPOND	30*1000L							//����ʱ��
#define  TIME_RELOAD_LIST			6									//�����б�
#define  TIME_LOAD_LIST				120									//�����б�
#define  TIME_CONNECT				30									//����ʱ��

#pragma endregion

#pragma region �¼��ӿ�
//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//״̬����
	m_bShowServerStatus=false;

	//״̬����
	m_pBindParameter=NULL;

	//�������
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketEngine=NULL;

	m_pRankManager = NULL;

	if(!g_AttemperEngineSink)
		g_AttemperEngineSink = this;

	return;
}

//��������
CAttemperEngineSink::~CAttemperEngineSink()
{
}

//�ӿڲ�ѯ
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//�󶨲���
	m_pBindParameter=new tagBindParameter[MAX_CONTENT];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*MAX_CONTENT);

	//������������ -- ֪ͨdatabase
	//m_pITimerEngine->SetTimer(IDI_COLLECT_ONLINE_INFO, TIME_COLLECT_ONLINE_INFO*1000L,TIMES_INFINITY,0);
	//������������ -- ֪ͨclient
	//m_pITimerEngine->SetTimer(IDI_UPDATA_ONLINE_COUNT,TIME_UPDATA_ONLINE_COUNT*1000L,TIMES_INFINITY, 0);
	//�����������Ϣ
	//m_pITimerEngine->SetTimer(IDI_UPDATA_MARQUEE,TIME_UPDATA_MARQUEE*1000L,1, 0);

	//��ȡĿ¼
	TCHAR szPath[MAX_PATH]=TEXT("");
	CString szFileName;
	GetModuleFileName(AfxGetInstanceHandle(),szPath,sizeof(szPath));
	szFileName=szPath;
	int nIndex = szFileName.ReverseFind(TEXT('\\'));
	szFileName = szFileName.Left(nIndex);
	szFileName += TEXT("\\PlazaOptionConfig.ini");

	//��ȡ����
	m_bShowServerStatus = (GetPrivateProfileInt(TEXT("ServerStatus"),TEXT("ShowServerStatus"),0,szFileName) != 0);

	//��ȡĿ¼
	TCHAR szServerAddr[LEN_SERVER]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);

	return true;
}

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{
	//�������
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketEngine=NULL;

	//ɾ������
	SafeDeleteArray(m_pBindParameter);

	//�б����
	m_ServerListManager.ResetServerList();

	if(m_pRankManager != NULL)
	{
		delete m_pRankManager;
		m_pRankManager = NULL;
	}

	return true;
}

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{
	}

	return false;
}

//ʱ���¼�
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	switch (dwTimerID)
	{
	case IDI_LOAD_GAME_LIST:		//�����б�
		{
			m_pITimerEngine->KillTimer(IDI_LOAD_GAME_LIST);
			//�����б�
			m_ServerListManager.DisuseKernelItem();
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_GAME_LIST,0,NULL,0);

			return true;
		}
	case IDI_CONNECT_CORRESPOND:	//����Э��
		{
			m_pITimerEngine->KillTimer(IDI_CONNECT_CORRESPOND);
			//��������
			m_pITCPSocketEngine->Connect(_CPD_SERVER_ADDR, PORT_CENTER);

			return true;
		}
		/*
	case IDI_COLLECT_ONLINE_INFO:		//ͳ������ -- �������ݿ�
		{
			//��������
			DBR_GP_OnLineCountInfo OnLineCountInfo;
			ZeroMemory(&OnLineCountInfo,sizeof(OnLineCountInfo));

			//��ȡ����
			OnLineCountInfo.dwOnLineCountSum=m_ServerListManager.CollectOnlineInfo();

			//��ȡ����
			POSITION ServerPosition=NULL;
			do
			{
				CGameServerItem * pGameServerItem=m_ServerListManager.EmunGameServerItem(ServerPosition);
				if (pGameServerItem != NULL)
				{
					DWORD dwGameID = (pGameServerItem->m_GameServer.wServerID) & 0xFFFFFFFF;
					bool bIsExit = false;

					//�ж��Ƿ���list���Ѵ���gameID
					for(DWORD i = 0; i <= OnLineCountInfo.dwGameCount; i ++)
					{
						//�������
						if( (dwGameID == OnLineCountInfo.OnLineCountGame[i].dwGameID))
						{
							bIsExit = true;
							break;		
						}
					}

					//���������
					if(!bIsExit)
					{
						//���������Ŀ
						OnLineCountInfo.dwGameCount++;
					}
					if( OnLineCountInfo.dwGameCount > 0)
					{
						//���ͱ�ʶ && ��������
						OnLineCountInfo.OnLineCountGame[OnLineCountInfo.dwGameCount-1].dwGameID=dwGameID;			
						OnLineCountInfo.OnLineCountGame[OnLineCountInfo.dwGameCount-1].dwOnLineCount += pGameServerItem->m_GameServer.dwOnlineCount;	
					}
				}

				//����ж�
				if (OnLineCountInfo.dwGameCount > CountArray(OnLineCountInfo.OnLineCountGame))
				{
					//��ʾ��Ϣ
					CLog::Log(log_warn, "ͳ���������� ��� IDI_COLLECT_ONLINE_INFO");
					break;
				}

			} while (ServerPosition!=NULL);

			//��������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_ONLINE_COUNT_INFO,0,&OnLineCountInfo,sizeof(OnLineCountInfo));

			return true;
		}
	case  IDI_UPDATA_ONLINE_COUNT: //���͸��ͻ���
		{
			//��������
			WORD wSendSize=0;
			BYTE cbDataBuffer[SOCKET_TCP_PACKET];

			//ö������
			POSITION Position=NULL;
			CGameServerItem * pGameServerItem=NULL;

			STR_CMD_LC_LIST_ROOM_ONLINE pServerOnLine;
			ZeroMemory( &pServerOnLine, sizeof(pServerOnLine));

			//ö������
			for (DWORD i=0;i<m_ServerListManager.GetGameServerCount();i++)
			{
				//��ȡ����
				pGameServerItem=m_ServerListManager.EmunGameServerItem(Position);
				if (pGameServerItem==NULL) break;

				pServerOnLine.dwServerID = pGameServerItem->m_GameServer.wServerID;
				pServerOnLine.dwOnlineCount = pGameServerItem->m_GameServer.dwOnlineCount;

				//��������
				CopyMemory( cbDataBuffer+wSendSize, &pServerOnLine,sizeof(STR_CMD_LC_LIST_ROOM_ONLINE));
				wSendSize += sizeof(STR_CMD_LC_LIST_ROOM_ONLINE);
			}

			//�����û�����,������Ϣ
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
	case IDI_UPDATA_MARQUEE:	//�����������Ϣ
		{
			m_pITimerEngine->KillTimer(IDI_UPDATA_MARQUEE);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_UPDATA_MARQUEE,0,NULL,0);
			return true;
		}
	case IDI_PLAY_MARQUEE:		//֪ͨclient�����������Ϣ   
		{
			//TODONOW �������Ϣ��
			//added by WangChengQing 2018/6/2
			//1. Ӧ�÷���Э����
			//2. ������client��¼��ʱ��, ֻ����һ�θ�client��  û��Ҫ�����ʱ�̷��͸����пͻ���

			static DWORD s_MarqueeID;

			//1. ����ʱ�� < ��ǰʱ���  ȥ����
			for(int i = 0; i < m_MarqueeMsgListManager.GetCount() ; i++)
			{

				STR_DBO_UPDATA_MARQUEE strMarquee = m_MarqueeMsgListManager.GetAt(i);

				//�� s_MarqueeID ��ʼ�ж�,  s_MarqueeID�ǵ�����
				if(strMarquee.dwMarqueeID <= s_MarqueeID)
				{
					if(i != (m_MarqueeMsgListManager.GetCount()-1))
					{
						continue;
					}
					else //ѭ����һ����
					{
						s_MarqueeID = 0;
						//���һ���ʱ�� ��Ҫ�����ö�ʱ��
						m_pITimerEngine->SetTimer(IDI_PLAY_MARQUEE,3*1000L,1,0);
						continue;
					}
				}

				//ʱ���ж�
				SYSTEMTIME nowTime;
				 GetLocalTime(&nowTime);

				 COleDateTime dTimeStart(strMarquee.timeStartTime); 
				 COleDateTime dTimeEnd(strMarquee.timeEndTime); 
				 COleDateTime dTimeNow(nowTime);

				 COleDateTimeSpan dTimeSpan;
				 double nSecnonSpan = 0;

				 //�������ʱ�� ���� ����ʱ�� -- ɾ������Ϣ
				 dTimeSpan = dTimeNow - dTimeEnd;
				 nSecnonSpan = dTimeSpan.GetTotalSeconds();
				 if(nSecnonSpan > 0)
				 {
					 m_MarqueeMsgListManager.GetAt(i).byMask = 2; //ɾ��
					 continue;
				 }

				 //�������ʱ�� С�� ��ʼʱ�� -- ���Ը���Ϣ
				 dTimeSpan = dTimeStart - dTimeNow;
				 nSecnonSpan = dTimeSpan.GetTotalSeconds();
				 if(nSecnonSpan > 0)
				 {
					 m_MarqueeMsgListManager.GetAt(i).byMask = 3;//����
					 continue;
				 } 

				  m_MarqueeMsgListManager.GetAt(i).byMask = 4; //����

				  //��¼��ǰ���͵�MarqueeID
				  s_MarqueeID = strMarquee.dwMarqueeID;

				 //��ʱ�����ʱ��
				 DWORD wTimerTime =  static_cast<DWORD> (10 + _tcslen(strMarquee.szMarqueeMsg)*0.45);
				 wTimerTime += strMarquee.dwMaruqeeTime;
				 m_pITimerEngine->SetTimer(IDI_PLAY_MARQUEE,wTimerTime*1000L,1,0);

				 //��������
				 m_pITCPNetworkEngine->SendDataBatch(MDM_SERVICE, CMD_LC_SERVICE_MARQUEE, &strMarquee, sizeof(strMarquee));
				 break;
			}


			//ɾ����ʱ�� �������Ϣ
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

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBaseResult(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
#pragma region ��¼ģ��
	case DBO_CL_LOGON_ACCOUNTS:			//�˺ŵ�¼
		{
			return On_CMD_LC_Logon_Account(dwContextID,pData,wDataSize);
		}
	case DBO_CL_LOGON_PLATFORM:			//��������½
		{
			return On_CMD_LC_Logon_Platform(dwContextID, pData, wDataSize);
		}
#pragma endregion

	case DBO_CL_SERVICE_USER_FEEDBACK:			//��ҷ�������
		{
			return On_CMD_LC_Service_UserFeedBack(dwContextID, pData);
		}
	case DBO_CL_SERVICE_REFRESH_USER_INFO:		//ˢ���û���Ϣ����
		{
			return On_CMD_LC_Service_RefreshUserInfo(dwContextID, pData);
		}
	case DBO_CL_SERVICE_QUERY_ROOM_LIST:		//��ѯ�����б���
		{
			return On_CMD_LC_Service_QueryRoomList(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_RICH_LIST:			//��ȡ�����񷵻�
		{
			return On_CMD_LC_Service_GetRichList(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_USER_RECORD_LIST:	//��ȡ�û�¼���б��أ���֣�
		{
			return On_CMD_LC_Service_GetUserRecordList(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_SPECIFIC_RECORD:	//��ȡָ��ID¼�񷵻أ�С�֣�
		{
			return On_CMD_LC_Service_GetSpecificRecord(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_ONLINE_REWARD:			//��ȡ���߽�������
		{
			return On_CMD_LC_Service_OnlineReward(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_TASK_LIST:			//��ȡ�����б���
		{
			return On_CMD_LC_Service_GetTaskList(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_TASK_REWARD:		//��ȡ����������
		{
			return On_CMD_LC_Service_GetTaskReward(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_REQUEST_LOTTERY:		//����齱����
		{
			return On_CMD_LC_Service_RequestLottery(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_MODIFY_PERSONAL_INFO:	//�޸ĸ������Ϸ���
		{
			return On_CMD_LC_Service_ModifyPersonalInfo(dwContextID,pData,wDataSize);
		}
	case DBO_CL_USER_QUERY_SCORE_INFO:			//��ѯ(���)��ҷ���
		{
			return On_CMD_LC_Service_QueryScoreInfo(dwContextID,pData,wDataSize);
		}
#pragma region ��������
	case DBO_GP_GAME_LIST_RESULT:		//���ؽ��
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
	case DBO_UPDATA_MARQUEE:			//��������� ����
		{
			return On_DBO_UPDATA_MARQUEE(dwContextID,pData,wDataSize);
		}
	case DBO_UPDATA_MARQUEE_FINISH:		//��������� ����
		{
			return On_DBO_UPDATA_MARQUEE_FINISH(dwContextID,pData,wDataSize);
		}
#pragma endregion
	
	case DBO_CL_USER_COMMAND_RESULT:		//�����������
		{
			return On_CMD_LC_CommonOperateResult(dwContextID,pData,wDataSize);
		}
#pragma region MDM_GIFT �������
	case DBO_CL_GIFT_GIVE_PROPS:			//�������� ����
		{
			return On_CMD_LC_GIFT_GIVE_PROPS(dwContextID,pData,wDataSize);
		}
	case DBO_CL_GIFT_GIVE_PROPS_SHOW:		//�������� ֪ͨ������
		{
			return On_CMD_LC_GIFT_GIVE_PROPS_SHOW(dwContextID,pData,wDataSize);
		}
#pragma endregion
	case DBO_CL_USER_RECHARGE_INFO:			//��ֵ��Ϣ����
		{
			return On_CMD_LC_Other_RechargeInfo(dwContextID,pData,wDataSize);
		}
	case DBO_CL_USER_EXCHANGE_INFO:			//�һ����߷���
		{
			return On_CMD_LC_Other_ExchangeInfo(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_RANK_REWARD:		//������а���	
		{
			return OnDBRankRewardResult(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_STANDING_LIST:		//pure�������а� ����
		{
			return On_CMD_LC_SERVICE_PURE_STANDING_LIST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_STANDING_FINISH:	//pure�������а� ����
		{
			return On_CMD_LC_SERVICE_PURE_STANDING_FINISH(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_RECORD_LIST:		//���ս��
		{
			return On_CMD_LC_SERVICE_PURE_RECORD_LIST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_RECORD_LIST_PINFO:	//��������Ϣ
		{
			return On_CMD_LC_SERVICE_PURE_RECORD_LIST_PINFO(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_RECORD_FINISH://���ս������
		{
			return On_CMD_LC_SERVICE_PURE_RECORD_FINISH(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_XJ_RECORD_LIST:		//С��ս��
		{
			return On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO:	//С�������Ϣ
		{
			return On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_XJ_RECORD_PLAYBACK:			//С��¼��ط�
		{
			return On_CMD_LC_Service_XJRecordPlayback(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_CUSTOMER_MESSEGE:		//�ͷ���Ϣ
		{
			return On_CMD_LC_SERVICE_CUSTOMER_MESSEGE(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_XJ_RECORD_FINISH://С��ս������
		{
			return On_CMD_LC_SERVICE_PURE_XJ_RECORD_FINISH(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_GOLD_INFO:		//�����Ҵ��� ����
		{
			return On_CMD_LC_SERVICE_GOLD_INFO(dwContextID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_GOLD_INFO_FINISH:	//�����Ҵ��� ����
		{
			return On_CMD_LC_SERVICE_GOLD_INFO_FINISH(dwContextID,pData,wDataSize);
		}
			 
#pragma region MDM_CLUB ����Ȧ
	case DBO_LC_CLUB_ALL_CLUB_INFO_LIST:		//��ѯ����Ȧ�б� ����
		{
			return On_CMD_LC_CLUB_ALL_CLUB_INFO_LIST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ALL_INFO_FINISH:		//��ѯ����Ȧ�б� ����
		{
			return On_CMD_LC_CLUB_ALL_INFO_FINISH(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ROOM_LIST: //��ѯָ������Ȧ�����б� ����
		{
			return On_CMD_LC_CLUB_ROOM_LIST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ROOM_LIST_FINISH: //��ѯָ������Ȧ�����б� ����
		{
			return On_CMD_LC_CLUB_ROOM_LIST_FINISH(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_RANDOM_CLUB_LIST : //��ѯδ��Ա�������Ȧ ����
		{
			return On_CMD_LC_CLUB_RANDOM_CLUB_LIST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_RANDOM_CLUB_LIST_FINISH : //��ѯδ��Ա�������Ȧ  ����
		{
			return On_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_JOIN_CLUB: //�����������Ȧ
		{
			return On_CMD_LC_CLUB_JOIN_CLUB(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_JOIN_CLUB_BDCAST: //�����������Ȧ �㲥
		{
			return On_CMD_LC_CLUB_JOIN_CLUB_BDCAST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_JOIN_CLUB_RE:	//�����������Ȧ �ͻ���ʵʱˢ�¾��ֲ�
		{
			return On_CMD_LC_CLUB_JOIN_CLUB_RE(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_DISS_CLUB : //��ɢ����Ȧ
		{
			return On_CMD_LC_CLUB_DISS_CLUB(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ROOM_SETTING: //��������
		{
			return On_CMD_LC_CLUB_ROOM_SETTING(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ROOM_QUERY_SETTING: //���󷿼�����
		{
			return On_CMD_LC_CLUB_ROOM_QUERY_SETTING(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ROOM_DISSOLVE: //��ɢ�������� ����
		{
			return On_CMD_LC_CLUB_ROOM_DISSOLVE(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_TABLE_DISSOLVE: //��ɢ�������� ����
		{
			return On_CMD_LC_CLUB_TABLE_DISSOLVE(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_NOTICE: //����Ȧ����
		{
			return On_CMD_LC_CLUB_NOTICE(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_MESSAGE: //����Ȧ���
		{
			return On_CMD_LC_CLUB_MESSAGE(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_CONTRIBUTE_FK://���׷���
		{
			return On_CMD_LC_CLUB_CONTRIBUTE_FK(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_AUTO_AGREE://����Ȧ����
		{
			return On_CMD_LC_CLUB_AUTO_AGREE(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_INVITE://�������˼�������Ȧ
		{
			return On_CMD_LC_CLUB_INVITE(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_INVITE_REMIND://�������˵�����
		{
			return On_CMD_LC_CLUB_INVITE_REMIND(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_INVITE_RESULT: //�������� �ظ�
		{
			return On_CMD_LC_CLUB_INVITE_RESULT(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_INQUERY_LIST: //�������˲鿴�Լ��������б� ����
		{
			return On_CMD_LC_CLUB_INQUERY_LIST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_APPLICANT_LIST://�������б� ����
		{
			return On_CMD_LC_CLUB_APPLICANT_LIST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_APPLICANT_LIST_FINISH://�������б� ����
		{
			return On_CMD_LC_CLUB_APPLICANT_LIST_FINISH(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_APPOINTMENT: //ְ������
		{
			return On_CMD_LC_CLUB_APPOINTMENT(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_APPOINTMENT_NOTE: //ְ������ ����
		{
			return On_CMD_LC_CLUB_APPOINTMENT_NOTE(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CHAT_ALL://���ֲ����� ����
		{
			return On_CMD_LC_CLUB_CHAT(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CHAT_ALL_BDCAST://���ֲ����� �㲥
		{
			return On_CMD_LC_CLUB_CHAT_BDCAST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_STICKY_POST:	//����Ȧ�ö�����
		{
			return On_CMD_LC_CLUB_STICKY_POST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_INQUERY_LIST_FINISH: //�������˲鿴�Լ��������б� ����
		{
			return On_CMD_LC_CLUB_INQUERY_LIST_FINISH(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_TABLE_LIST_TABLE: //�����б�
		{
			return On_CMD_LC_CLUB_TABLE_LIST_TABLE(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_TABLE_LIST_USER:	//��������б�
		{
			return On_CMD_LC_CLUB_TABLE_LIST_USER(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_CREATE_CLUB://��������Ȧ ����
		{
			return On_CMD_LC_CLUB_CREATE_CLUB(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_JOIN_ROOM: //������뷿�� ����
		{
			return On_CMD_LC_CLUB_JOIN_ROOM(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_APPLICANT_RESULT: //Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�) ����
		{
			return On_CMD_LC_CLUB_APPLICANT_RESULT(dwContextID,pData,wDataSize);
		}
		/*
	case DBO_LC_CLUB_JOIN_CLUB_MESSAGE_FINISH: //Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�) ����
		{
			return On_CMD_LC_CLUB_APPLICANT_LIST_FINISH(dwContextID,pData,wDataSize);
		}
		*/
	case DBO_LC_CLUB_MEMBER_MANAGER://�����Ա���� ����
		{
			return On_CMD_LC_CLUB_MEMBER_MANAGER(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_MEMBER_MANAGER_FINISH://�����Ա���� ����
		{
			return On_CMD_LC_CLUB_MEMBER_MANAGER_FINISH(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_DATA ://���������Ϣ
		{
			return On_CMD_LC_CLUB_DATA(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_QUIT://�߳��˳����� ����
		{
			return On_CMD_LC_CLUB_QUIT(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_RECORD_LIST: //����ս��ͳ�� ����
		{
			return On_CMD_LC_CLUB_RECORD_LIST(dwContextID,pData,wDataSize);
		}
	case DBO_LC_CLUB_RECORD_FINISH: //����ս��ͳ�� ����
		{
			return On_CMD_LC_CLUB_RECORD_FINISH(dwContextID,pData,wDataSize);
		}
#pragma endregion
#pragma region MDM_SHOP �̳ǵ���
	case DBO_CL_SHOP_QUERY:		//��ѯ�̳�
		{
			return On_CMD_LC_SHOP_QUERY_RESULT(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SHOP_QUERY_FINISH: //��ѯ�̳ǽ���
		{
			return On_CMD_LC_SHOP_QUERY_FINISH(dwContextID,pData,wDataSize);
		}
	case DBO_CL_SHOP_DIAMOND : //��ʯ�������
		{
			return On_CMD_LC_SHOP_DIAMOND_RESULT(dwContextID,pData,wDataSize);
		}
	case DBO_CL_BAG_QUERY: //������Ʒ��ѯ
		{
			return On_CMD_LC_BAG_RESULT(dwContextID,pData,wDataSize);
		}
	case DBO_CL_BAG_FINISH://������Ʒ��ѯ���
		{
			return On_CMD_LC_BAG_FINISH(dwContextID,pData,wDataSize);
		}
#pragma endregion
	}

	return false;
}

#pragma endregion

#pragma region Socket�¼�
//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	//��ȡ����
	if (LOWORD(dwSocketID)>=MAX_CONTENT) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//���ñ���
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientAddr=dwClientAddr;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);
	return true;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	///Socket���ӹر� ���ݿ⴦��
	if(pBindParameter->dwUserID != 0)
	{
		/*
		** ������ ��¼�� socket�ϵ���ʱ��
		** �ж��Ƿ��ڶ����б���, ������ڣ�������Ϊ��Ϸ���ߣ� ��������ڣ� ������Ϊ����
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

		//1�������� ��2������Ϸ��3��Ϸ���ߣ�4����
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

	//�����Ϣ
	//WORD wBindIndex=LOWORD(dwSocketID);
	ZeroMemory((m_pBindParameter+wBindIndex),sizeof(tagBindParameter));

	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_LOGON:				//��¼����
		{
			return OnTCPNetworkMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_SERVICE:			//��������
		{
			return OnTCPNetworkMainService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CLUB:				//����Ȧ(�汾2)
		{
			return On_MDM_CLUB(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GIFT:				//�������	
		{
			return OnTCPNetworkMainOther(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_SHOP:				//�̳ǵ���
		{
			return On_MDM_SHOP(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GAME:				//��Ϸ�����
		{
			return On_MDM_GAME(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}

	}

	//��¼���Ƕ�����, ��������Ч
	return false;
}



//��ȡ����ip
int GetInternetIP( TCHAR *szInernet_ip)
{
	//���ؽű�
	TCHAR szTempPath[_MAX_PATH] = {0}, szTempFile[MAX_PATH] = {0};
	GetTempPath(MAX_PATH, szTempPath);

	UINT nResult = GetTempFileName(szTempPath, _T("ex"), 0, szTempFile);
	int ret=URLDownloadToFile(NULL,_T("http://www.net.cn/static/customercare/yourip.asp"),szTempFile,BINDF_GETNEWESTVERSION,NULL);
	if (ret == S_FALSE)
		return 1;

	//�жϽű��Ƿ����سɹ�
	FILE *fp;
	if (_wfopen_s(&fp,szTempFile,_T("r"))!=0)
		return 2;


	fseek(fp,0,SEEK_END);//�õ��ļ���С
	int ilength=ftell(fp);
	fseek(fp,0,SEEK_SET);


	//��ȡ�ű��е�ip��ַ
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

			//��ʾ��Ϣ
			CLog::Log(log_debug, "����IP: %s", szInernet_ip);
		}
	}
	else
	{
		fclose(fp);
		return 4;
	}

	return 5;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//����ʱ��
		m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND, TIME_CONNECT_CORRESPOND, 1, 0);
		return true;
	}

	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//�����ж�
		if (nErrorCode!=0)
		{
			//����ʱ��
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND, TIME_CONNECT_CORRESPOND, 1, 0);
			return false;
		}

		//��������
		STR_CPR_LP_REGISTER_LOGON CPR;
		ZeroMemory(&CPR,sizeof(CPR));

		//���ñ���
		TCHAR szInernet_ip[32] = TEXT("0.0.0.0");
		//GetInternetIP(szInernet_ip);
		//lstrcpyn(CPR.szServerAddr,szInernet_ip ,CountArray(CPR.szServerAddr));

		lstrcpyn(CPR.szServerAddr,TEXT("127.0.0.1"),CountArray(CPR.szServerAddr));

		//��������
		m_pITCPSocketEngine->SendData(MDM_REGISTER,CPR_LP_REGISTER_LOGON,&CPR,sizeof(CPR));

		return true;
	}

	return true;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_REGISTER:		//ע��ģ��
			{
				return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CPD_LIST:	//�б�����
			{
				return OnTCPSocketMainServiceInfo(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_TRANSFER:	//��ת����
			{
				return OnTCPSocketMainTransfer(Command.wSubCmdID,pData,wDataSize);	
			}
		case MDM_WEB: //��̨����
			{
				return OnTCPSocketMainWeb(Command.wSubCmdID,pData,wDataSize);
			}
		}
	}

	return true;
}

#pragma endregion

#pragma region  ��������
/*********************************************��������½��ʱ������Ԫ���͵���Ϣ��������*********************************************************/
//��Ϸ����
bool CAttemperEngineSink::OnDBPCGameTypeItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(tagGameType)==0);
	if (wDataSize%sizeof(tagGameType)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(tagGameType);
	tagGameType * pGameType=(tagGameType *)pData;

	//��������
	for (WORD i=0;i<wItemCount;i++)
	{
		//��������
		tagGameType GameType;
		ZeroMemory(&GameType,sizeof(GameType));

		//��������
		GameType.wTypeID=(pGameType+i)->wTypeID;
		lstrcpyn(GameType.szTypeName,(pGameType+i)->szTypeName,CountArray(GameType.szTypeName));

		//�����б�
		m_ServerListManager.InsertGameType(&GameType);
	}

	return true;
}

//��Ϸ����
bool CAttemperEngineSink::OnDBPCGameKindItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(tagGameKind)==0);
	if (wDataSize%sizeof(tagGameKind)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(tagGameKind);
	tagGameKind * pGameKind=(tagGameKind *)pData;

	//��������
	for (WORD i=0;i<wItemCount;i++)
	{
		//��������
		tagGameKind GameKind;
		ZeroMemory(&GameKind,sizeof(GameKind));

		//��������
		GameKind.wKindID=(pGameKind+i)->wKindID;
		GameKind.wTypeID=(pGameKind+i)->wTypeID;
		lstrcpyn(GameKind.szKindName,(pGameKind+i)->szKindName,CountArray(GameKind.szKindName));

		//�����б�
		m_ServerListManager.InsertGameKind(&GameKind);
	}

	return true;
}

//��Ϸ�ڵ�
bool CAttemperEngineSink::OnDBPCGameNodeItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(tagGameNode)==0);
	if (wDataSize%sizeof(tagGameNode)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(tagGameNode);
	tagGameNode * pGameNode=(tagGameNode *)pData;

	//��������
	for (WORD i=0;i<wItemCount;i++)
	{
		//��������
		tagGameNode GameNode;
		ZeroMemory(&GameNode,sizeof(GameNode));

		//��������
		GameNode.wKindID=(pGameNode+i)->wKindID;
		GameNode.wNodeID=(pGameNode+i)->wNodeID;
		lstrcpyn(GameNode.szNodeName,(pGameNode+i)->szNodeName,CountArray(GameNode.szNodeName));

		//�����б�
		m_ServerListManager.InsertGameNode(&GameNode);
	}

	return true;
}

//��Ϸ�б�
bool CAttemperEngineSink::OnDBPCGameListResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize!=sizeof(DBO_GP_GameListResult)) return false;

	//��������
	DBO_GP_GameListResult * pGameListResult=(DBO_GP_GameListResult *)pData;

	//��Ϣ����
	if (pGameListResult->cbSuccess==TRUE)
	{
		//�����б�
		//m_ServerListManager.CleanKernelItem();

		//�¼�֪ͨ
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess=ER_SUCCESS;

	}
	else
	{
		//��ʾ��Ϣ
		CLog::Log(log_warn, "�������б����ʧ�ܣ�%ld ������¼���", TIME_RELOAD_LIST);

		//����ʱ��
		m_pITimerEngine->SetTimer(IDI_LOAD_GAME_LIST,TIME_RELOAD_LIST*1000L,1,0);
	}

	return true;
}

//�����������Ϣ ����
bool CAttemperEngineSink::On_DBO_UPDATA_MARQUEE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	WORD size = sizeof(STR_DBO_UPDATA_MARQUEE);
	if ((wDataSize < size) || ( (wDataSize % size) != 0))  return false;

	//��������
	WORD wItemCount=wDataSize/size;
	STR_DBO_UPDATA_MARQUEE * pMarquee=(STR_DBO_UPDATA_MARQUEE *)pData;

	//��������
	for (WORD i=0;i<wItemCount;i++)
	{
		STR_DBO_UPDATA_MARQUEE strMarquee;
		strMarquee.byMask = (pMarquee+i)->byMask;
		strMarquee.dwMarqueeID =  (pMarquee+i)->dwMarqueeID;
		strMarquee.dwMaruqeeTime = (pMarquee+i)->dwMaruqeeTime;
		memcpy(&strMarquee.timeStartTime, &(pMarquee+i)->timeStartTime, sizeof(SYSTEMTIME));
		memcpy(&strMarquee.timeEndTime, &(pMarquee+i)->timeEndTime, sizeof(SYSTEMTIME));
		memcpy(strMarquee.szMarqueeMsg, (pMarquee+i)->szMarqueeMsg, sizeof(strMarquee.szMarqueeMsg));

		//�����б�
		m_MarqueeMsgListManager.Add(strMarquee);
	}

	return true;
}

//�����������Ϣ ����
bool CAttemperEngineSink::On_DBO_UPDATA_MARQUEE_FINISH(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//������ʱ��,  ֪ͨ�ͻ��� ���������
	m_pITimerEngine->SetTimer(IDI_PLAY_MARQUEE,1*1000L,1, 0);

	return true;
}


#pragma endregion

#pragma region Э���� ����Ϣ�ŷַ�����
//ע��ģ��
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case CPO_PGPL_REGISTER_SUCESS:	//ע��ɹ�
		{
			//����socket::server����
			if(g_pServiceUnits->StartNetworkService() != 0)
			{
				g_pServiceUnits->ConcludeService();
				return true;
			}

			//��ȡ���ݿ� �����б�
			m_ServerListManager.DisuseKernelItem();
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_GAME_LIST,0,NULL,0);

			//ע����ɣ��������ʼ�����а�
			if(NULL == m_pRankManager)
				m_pRankManager = new RankManager(m_pIDataBaseEngine);

			return true;
		}

	case CPO_PGPL_REGISTER_FAILURE:		//ע��ʧ��
		{
			//�¼�֪ͨ 
			g_pServiceUnits ->ConcludeService();
			return true;
		}
	}

	return true;
}

//�б�����
bool CAttemperEngineSink::OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SERVER_INFO:		//������Ϣ
		{
			//�����б�
			m_ServerListManager.DisuseServerItem();

			return true;
		}
	case CPO_PL_LIST_GAME_ONLINE:	//��������
		{
			//Ч�����
			if (wDataSize!=sizeof(STR_CPR_PL_LIST_GAME_ONLINE)) return false;

			//��������
			STR_CPR_PL_LIST_GAME_ONLINE * pServerOnLine=(STR_CPR_PL_LIST_GAME_ONLINE *)pData;

			//���ҷ���
			CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(pServerOnLine->wServerID);
			if(pGameServerItem == NULL) return true;

			pGameServerItem->m_GameServer.dwOnlineCount = pServerOnLine->dwOnLineCount;
			return true;
		}
	case CPO_PL_LIST_INSERT_GAME:	//�������
		{
			//Ч�����
			if (wDataSize != sizeof(tagGameServer)) return false;

			//��������
			tagGameServer * pGameServer=(tagGameServer *)pData;

			//���뷿��
			byte byRet = m_ServerListManager.InsertGameServer(pGameServer);

			// ������·���, ��Ҫ֪ͨ����client
			if(byRet == 2)
			{
				pGameServer->byMask = 0;
				m_pITCPNetworkEngine->SendDataBatch(MDM_LIST, CMD_LC_LIST_ROOM, pGameServer, sizeof(tagGameServer));
			}

			return true;
		}
	case CPO_PL_LIST_INSERT_GAME_LIST:	//�������(�б�)
		{
			//Ч�����
			if ((wDataSize < sizeof(tagGameServer))
				||(wDataSize%sizeof(tagGameServer)!=0)) return false;

			//��������
			WORD wItemCount=wDataSize/sizeof(tagGameServer);
			tagGameServer * pGameServer=(tagGameServer *)pData;

			//��������
			for (WORD i=0;i<wItemCount;i++)
			{
				//listʱ������֪ͨclient
				m_ServerListManager.InsertGameServer(pGameServer++);
			}

			return true;
		}
	case CPO_PL_LIST_REMOVE_GAME:	//����ɾ��
		{
			//Ч�����
			if (wDataSize!=sizeof(STR_CPO_PL_LIST_REMOVE_GAME)) return false;

			//��������
			STR_CPO_PL_LIST_REMOVE_GAME * pServerRemove=(STR_CPO_PL_LIST_REMOVE_GAME *)pData;

			//��������
			m_ServerListManager.DeleteGameServer(pServerRemove->dwServerID);

			//ͬʱɾ�� ��� ServerID��صĶ����û�
			for(int i=0; i < m_OfflineUserListManager.GetCount(); i++)
			{
				if(m_OfflineUserListManager.GetAt(i).dwServerID == pServerRemove->dwServerID)
				{
					m_OfflineUserListManager.RemoveAt(i);
				}
			}

			//֪ͨclient, �и���Ϸ��������
			tagGameServer GameServer;
			GameServer.wServerID = pServerRemove->dwServerID;
			GameServer.byMask = 1; //ɾ����־
			m_pITCPNetworkEngine->SendDataBatch(MDM_LIST, CMD_LC_LIST_ROOM, &GameServer, sizeof(tagGameServer));

			return true;
		}
	case SUB_CS_C_USER_OFFLINE_B: //�û�����
		{
			//Ч�����
			if (wDataSize!=sizeof(tagOfflineUser)) return false;

			//��������
			tagOfflineUser * pOfflineUser=(tagOfflineUser *)pData;
			tagOfflineUser OfflineUser;
			OfflineUser.dwUserID = pOfflineUser->dwUserID;
			OfflineUser.dwServerID = pOfflineUser->dwServerID;
			OfflineUser.byMask = pOfflineUser->byMask;

			if(1 == OfflineUser.byMask)
			{
				m_OfflineUserListManager.Add(OfflineUser);

				//֪ͨ���ݿ�, �����������״̬
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

//��ת����
bool CAttemperEngineSink::OnTCPSocketMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case CPO_PL_CLUB_TABLE_INFO:  //���ֲ�������Ϣ
		{
			//TODONOW ��ʱ�ȸ�Ϊ֪ͨ����client, �����Ϊ֪ͨ�ڸ÷����ڵ���
			//added by WangChengQing 2018/5/21
			//Ч������
			if (wDataSize!=sizeof(STR_CMD_LC_CLUB_TABLE_LIST)) return true;

			//����֪ͨ -- ȫ���ͻ���
			m_pITCPNetworkEngine->SendDataBatch(MDM_CLUB,CMD_LC_CLUB_TABLE_LIST_TABLE,pData,wDataSize);
			return true;
		}
	case CPO_PL_CLUB_PLAYER_INFO: //���ֲ������Ϣ
		{
			//TODONOW ��ʱ�ȸ�Ϊ֪ͨ����client, �����Ϊ֪ͨ�ڸ÷����ڵ���
			//added by WangChengQing 2018/5/21
			//Ч������
			if (wDataSize!=sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST)) return true;

			//����֪ͨ -- ȫ���ͻ���
			m_pITCPNetworkEngine->SendDataBatch(MDM_CLUB, CMD_LC_CLUB_TABLE_LIST_USER,pData,wDataSize);
			return true;
		}
	case CPO_PL_CREATE_TABLE:	//�������� ��ѯ���õ�GameID
		{
			//Ч������
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

//��̨����
bool CAttemperEngineSink::OnTCPSocketMainWeb( WORD wSubCmdID, VOID * pData, WORD wDataSize )
{
	switch (wSubCmdID)
	{
		case CPO_PL_WEB_MARQUEE: //�������Ϣ��ȡ
		{
			if (wDataSize!=sizeof(STR_CPR_WP_WEB_MARQUEE)) return true;

			//TODONOW added by WangChengQing 2018/8/14 ����������, ��鿴�˴�
			//1. �����������б�
			m_MarqueeMsgListManager.RemoveAll();

			//2. ���¶�ȡ���ݿ�
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_UPDATA_MARQUEE,0,NULL,0);
			return true;
		}
		case CPO_PL_WEB_SYSTEM_MESSAGE: //������Ϣ -- ϵͳ��Ϣ
		{
			//Ч���С
			if (wDataSize!=sizeof(STR_CPR_WP_WEB_SYSTEM_MESSAGE))
			{
				return true;
			}
			STR_CPR_WP_WEB_SYSTEM_MESSAGE *pSub = (STR_CPR_WP_WEB_SYSTEM_MESSAGE*) pData;

			//�ṹ�幹�� 
			STR_CMD_LC_CLUB_CHAT_BDCAST CMD;
			ZeroMemory(&CMD, sizeof(CMD));
			CMD.byChatMode = 2;
			memcpy(CMD.szUserNickName, TEXT("ϵͳ����Ա"), sizeof(CMD.szUserNickName));
			memcpy(CMD.szChat, pSub->szMessage, sizeof(CMD.szChat));

			m_pITCPNetworkEngine->SendDataBatch(MDM_CLUB, CMD_LC_CLUB_CHAT_BDCAST, &CMD, sizeof(CMD));
			return true;
		}
	}
	return true;
}

#pragma endregion

#pragma region �ͻ��� ����Ϣ�ŷַ�����
/*********************************************������<->�ͻ�����Ϣ��Ӧ����*********************************************************/

//��¼ģ��
bool CAttemperEngineSink::OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CL_LOGON_ACCOUNTS:		//�ʺŵ�¼
		{
			return On_SUB_CL_Logon_Accounts(pData,wDataSize,dwSocketID);
		}
	case SUB_CL_LOGON_REGISTER:		//�ʺ�ע��
		{
			return On_SUB_CL_Logon_Register(pData,wDataSize,dwSocketID);
		}
	case SUB_CL_LOGON_PLATFORM:		//ƽ̨��¼
		{
			return On_SUB_CL_Logon_Platform(pData,wDataSize,dwSocketID);
		}
	}
	return false;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkMainService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CL_SERVICE_USER_FEEDBACK:			//��ҷ���
		{
			return On_SUB_CL_Service_UserFeedBack(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_REFRESH_USER_INFO:		//ˢ���û���Ϣ
		{
			return On_SUB_CL_Service_RefreshUserInfo(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_QUERY_ROOM_LIST:		//��ѯ������Ϣ�б�
		{
			return On_SUB_CL_Service_QueryRoomList(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_MODIFY_PERSONAL_INFO:	//�޸ĸ�������
		{
			return On_SUB_CL_Service_ModifyPersonalInfo(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_RICH_LIST:			//��ȡ������
		{
			return On_SUB_CL_Service_GetRichList(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_DJ_RECORD_LIST:	//��ȡ�û�¼���б�
		{
			return On_SUB_CL_Service_GetUserRecordList(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_XJ_RECORD_LIST:	//��ȡָ��ID¼��
		{
			return On_SUB_CL_Service_GetSpecificRecord(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_ONLINE_REWARD:			//��ȡ���߽���
		{
			return On_SUB_CL_Service_OnlineReward(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_TASK_LIST:			//��ȡ�����б�
		{
			return On_SUB_CL_Service_GetTaskList(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_TASK_REWARD:		//��ȡ������
		{
			return On_SUB_CL_Service_GetTaskReward(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_RANK_LIST:			//��ȡ���а�
		{
			return On_SUB_CL_Service_GetRankList(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_REQUEST_LOTTERY:		//����齱
		{
			return On_SUB_CL_Service_RequestLottery(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_PURE_STANDING_LIST:		//pure�������а�
		{
			return On_SUB_CL_SERVICE_PURE_STANDING_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_PURE_RECORD_LIST:		//���ս��
		{
			return On_SUB_CL_SERVICE_PURE_RECORD_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_PURE_XJ_RECORD_LIST:	//С��ս��
		{
			return On_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GOLD_INFO: //��ҳ���Ϣ
		{
			return On_SUB_CL_SERVICE_GOLD_INFO(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_XJ_RECORD_PLAYBACK:	//С��¼��ط�
		{
			return On_SUB_CL_Service_XJRecordPlayback(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_CUSTOMER_MESSEGE:	//�ͷ���Ϣ
		{
			return On_SUB_CL_SERVICE_CUSTOMER_MESSEGE(pData, wDataSize, dwSocketID);
		}
	}

	return false;
}

//����Ȧ
bool CAttemperEngineSink::On_MDM_CLUB(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch(wSubCmdID)
	{
	case SUB_CL_CLUB_ALL_CLUB_INFO_LIST:	 //��ѯ����Ȧ�б�
		{
			return On_SUB_CL_CLUB_ALL_CLUB_INFO_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_ROOM_LIST:  //��ѯָ������Ȧ�����б�
		{
			return On_SUB_CL_CLUB_ROOM_LIST(pData, wDataSize, dwSocketID);	
		}
	case SUB_CL_CLUB_RANDOM_CLUB_LIST: //��ѯδ��Ա, �������Ȧ(���9��)
		{
			return On_SUB_CL_CLUB_RANDOM_CLUB_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_JOIN_CLUB: //�����������Ȧ
		{
			return On_SUB_CL_CLUB_JOIN_CLUB(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_DISS_CLUB: //��ɢ����Ȧ
		{
			return On_SUB_CL_CLUB_DISS_CLUB(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_CREATE_CLUB:	 //��������Ȧ
		{
			return On_SUB_CL_CLUB_CREATE_CLUB(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_JOIN_ROOM: //������뷿��
		{
			return On_SUB_CL_CLUB_JOIN_ROOM(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_ROOM_SETTING: //��������
		{
			return On_SUB_CL_CLUB_ROOM_SETTING(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_ROOM_QUERY_SETTING://���󷿼�����
		{
			return On_SUB_CL_CLUB_ROOM_QUERY_SETTING(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_ROOM_USER_LEAVE://����뿪����
		{
			return On_SUB_CL_CLUB_ROOM_USER_LEAVE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_ROOM_DISSOLVE:	//��ɢ����
		{
			return On_SUB_CL_CLUB_ROOM_DISSOLVE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_TABLE_DISSOLVE:	//��ɢ����
		{
			return On_SUB_CL_CLUB_TABLE_DISSOLVE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_NOTICE: //����Ȧ����
		{
			return On_SUB_CL_CLUB_NOTICE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_MESSAGE: //����Ȧ���
		{
			return On_SUB_CL_CLUB_MESSAGE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_CONTRIBUTE_FK: //���׷���
		{
			return On_SUB_CL_CLUB_CONTRIBUTE_FK(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_AUTO_AGREE: //����Ȧ����
		{
			return On_SUB_CL_CLUB_AUTO_AGREE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_CHAT://����Ȧ ����
		{
			return On_SUB_CL_CLUB_CHAT(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_STICKY_POST:	//����Ȧ�ö�
		{
			return On_SUB_CL_CLUBSTICKY_POST(pData, wDataSize, dwSocketID);
		}
		/*
	case SUB_CL_CLUB_CREATE_CLUB_ROOM_APPLY:  //��������Ȧ����
		{
			return On_SUB_CL_CLUB_CREATE_CLUB_ROOM_APPLY(pData, wDataSize, dwSocketID);	
		}
		*/
	case SUB_CL_CLUB_APPLICANT_RESULT:	 //Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�)
		{
			return On_SUB_CL_CLUB_APPLICANT_RESULT(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_MEMBER_MANAGER:  //�����Ա����
		{
			return On_SUB_CL_CLUB_MEMBER_MANAGER(pData, wDataSize, dwSocketID);	
		}
	case SUB_CL_CLUB_INVITE://�������˼�������Ȧ
		{
			return On_SUB_CL_CLUB_INVITE(pData, wDataSize, dwSocketID);	
		}
	case SUB_CL_CLUB_INVITE_RESULT:// �������� �ظ�
		{
			return On_SUB_CL_CLUB_INVITE_RESULT(pData, wDataSize, dwSocketID);	
		}
	case SUB_CL_CLUB_INQUERY_LIST://�������˲鿴�Լ��������б�
		{
			return On_SUB_CL_CLUB_INQUERY_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_APPLICANT_LIST: //�������б�
		{
			return On_SUB_CL_CLUB_APPLICANT_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_QUIT: //�߳��˳�����
		{
			return On_SUB_CL_CLUB_QUIT(pData, wDataSize, dwSocketID);
		}
	case  SUB_CL_CLUB_APPOINTMENT://ְ������
		{
			return On_SUB_CL_CLUB_APPOINTMENT(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_RECORD_LIST:	//����ս��ͳ��
		{
			return On_SUB_CL_CLUB_RECORD_LIST(pData, wDataSize, dwSocketID);
		}
	}

	return false;
}

//�������
bool CAttemperEngineSink::OnTCPNetworkMainOther(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CL_GIFT_GIVE_PROPS:				//���͵���
		{
			return On_SUB_CL_GIFT_GIVE_PROPS(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_OTHERS_RECHARGE_INFO:				//��ֵ��Ϣ
		{
			return On_SUB_CL_Other_ReChargeInfo(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_OTHERS_EXCHANGE_INFO:				//�һ�������Ϣ
		{
			return On_SUB_CL_Other_ExchangeInfo(pData, wDataSize, dwSocketID);
		}
	}

	return false;
}

//�̳ǵ���
bool CAttemperEngineSink::On_MDM_SHOP(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch(wSubCmdID)
	{
	case SUB_CL_SHOP_QUERY:	 //��ѯ�̳�
		{
			return On_SUB_CL_SHOP_QUERY(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SHOP_MONEY:  //��Ǯ�������
		{
			//return On_SUB_CL_SHOP_MONEY(pData, wDataSize, dwSocketID);	
		}
	case SUB_CL_SHOP_DIAMOND: //��ʯ�������
		{
			return On_SUB_CL_SHOP_DIAMOND(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_BAG_QUERY: //������Ʒ��ѯ
		{
			return On_SUB_CL_BAG_QUERY(pData, wDataSize, dwSocketID);
		}	
	}

	return false;
}

//��Ϸ�����
bool CAttemperEngineSink::On_MDM_GAME(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch(wSubCmdID)
	{
	case SUB_CL_GAME_QUERY_GAMEID:	 //��������֮ǰ, �Ȳ�ѯ���õ�GameID
		{
			//У������
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

#pragma region MDM_LOGON ��¼ģ��
/***************************************** ����¼������-����Ϣ1�� *******************************************/
//�ʺŵ�¼
bool CAttemperEngineSink::On_SUB_CL_Logon_Accounts(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_LOGON_ACCOUNTS))
	{
		return false;
	}

	//������Ϣ
	STR_SUB_CL_LOGON_ACCOUNTS * pSUBLogonAccounts=(STR_SUB_CL_LOGON_ACCOUNTS *)pData;

	//�汾�жϣ��汾���ԣ�ֱ���˳���
	if ( On_CMD_LC_Logon_UpdateNotify(pSUBLogonAccounts->dwVersionCheck, dwSocketID) )
	{
		return true;
	}

	//��������
	STR_DBR_CL_LOGON_ACCOUNTS DBRLogonAccounts;
	ZeroMemory(&DBRLogonAccounts,sizeof(DBRLogonAccounts));
	//SocketУ��
	DBRLogonAccounts.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));
	//���ݸ�ֵ
	lstrcpyn(DBRLogonAccounts.szAccounts,pSUBLogonAccounts->szAccounts,CountArray(DBRLogonAccounts.szAccounts));
	lstrcpyn(DBRLogonAccounts.szPassword,pSUBLogonAccounts->szPassword,CountArray(DBRLogonAccounts.szPassword));
	lstrcpyn(DBRLogonAccounts.szMachineID,pSUBLogonAccounts->szMachineID,CountArray(DBRLogonAccounts.szMachineID));
	DBRLogonAccounts.dwProxyID = pSUBLogonAccounts->dwProxyID;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_LOGON_ACCOUNTS,dwSocketID,&DBRLogonAccounts,sizeof(DBRLogonAccounts));
	return true;
}

//�˺ŵ�¼����
bool CAttemperEngineSink::On_CMD_LC_Logon_Account(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;
	if(wDataSize != sizeof(STR_CMD_LC_LOGON_PLATFORM)) return false;

	//��������
	STR_CMD_LC_LOGON_PLATFORM * pCMD=(STR_CMD_LC_LOGON_PLATFORM *)pData;

	//�����¼ʧ��, ֱ�ӷ���
	if(DB_SUCCESS != pCMD->dwResultCode)
	{
		//��������
		m_pITCPNetworkEngine->SendData(dwContextID, MDM_LOGON, CMD_LC_LOGON_ACCOUNTS, pCMD, sizeof(STR_CMD_LC_LOGON_PLATFORM));

		return true;
	}

	//�ظ���¼�߳�
	On_CMD_LC_Logon_RepeatLogon( pCMD->dwUserID, dwContextID );

	//���������Ĵ��� 1.CMD�ṹ�帳ֵ  2.list�����itme 
	bool bIsExist = false; //�Ƿ���ڶ����б���
	for(int i=0; i < m_OfflineUserListManager.GetCount(); i++)
	{
		if(m_OfflineUserListManager.GetAt(i).dwUserID == pCMD->dwUserID)
		{
			pCMD->dwOffLineGameID = m_OfflineUserListManager.GetAt(i).dwServerID;
			bIsExist = true;
			break;
		}
	}

	//��������
	(m_pBindParameter+LOWORD(dwContextID))->dwUserID = pCMD->dwUserID;	//��¼�����ӵ��û�ID

	//���͵�¼�ɹ�
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_LOGON, CMD_LC_LOGON_ACCOUNTS, pCMD, sizeof(STR_CMD_LC_LOGON_PLATFORM));


	//�����б�
	On_CMD_LC_List_Type(dwContextID);
	On_CMD_LC_List_Kind(dwContextID);
	On_CMD_LC_List_Node(dwContextID);
	On_CMD_LC_List_Room(dwContextID);

	//��¼����
	On_CMD_LC_Logon_Logon_Reward(dwContextID, pCMD->LasLogonDate);

	// �����û����߱�־
	CheckUserState_Logon(pCMD->dwUserID, bIsExist);

	return true;
}

void CAttemperEngineSink::CheckUserState_Logon(DWORD dwUserID, bool bIsExist)
{
	/*
	 ** ����һ  ��¼�� ��¼��ʱ��
     ** 1. �� ��Ϸ���϶�û������
     ** 2. �� ��¼���϶�������
     ** 3. �ж϶����б� -- �������, �򲻷���; ���������, ��Ϊ�������� 
	 */

	if(!bIsExist)
	{
		//TODONOWW ֪ͨ���ݿ�������״̬Ϊ "��������"
		DBR_GP_UserQuitInfo quitInfo;
		quitInfo.dwUserID = dwUserID;
		quitInfo.byOnlineMask =  1; //1�������� ��2������Ϸ��3��Ϸ���ߣ�4����

		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOGON_USER_STATE,0, &quitInfo,sizeof(quitInfo));
	}
}

//�ʺ�ע��
bool CAttemperEngineSink::On_SUB_CL_Logon_Register(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_LOGON_REGISTER))
	{
		return false;
	}
	
	//������Ϣ
	STR_SUB_CL_LOGON_REGISTER * pSUBLogonRegister=(STR_SUB_CL_LOGON_REGISTER *)pData;

	//Ч��汾
	if ( On_CMD_LC_Logon_UpdateNotify(pSUBLogonRegister->dwVersionCheck, dwSocketID) )
	{
		return true;
	}
	

	//��������
	//TODONOW ���Ӵ���ID  added by WangChengQing 2018/8/13
	STR_DBR_CL_LOGON_REGISTER DBRLogonRegister;
	ZeroMemory(&DBRLogonRegister,sizeof(DBRLogonRegister));
	DBRLogonRegister.pBindParameter = (m_pBindParameter+LOWORD(dwSocketID));
	//���ݸ�ֵ
	lstrcpyn(DBRLogonRegister.szAccounts,pSUBLogonRegister->szAccounts,CountArray(DBRLogonRegister.szAccounts));
	lstrcpyn(DBRLogonRegister.szPassword,pSUBLogonRegister->szPassword,CountArray(DBRLogonRegister.szPassword));
	lstrcpyn(DBRLogonRegister.szNickName,pSUBLogonRegister->szNickName,CountArray(DBRLogonRegister.szNickName));
	DBRLogonRegister.cbGender = pSUBLogonRegister->cbGender;
	lstrcpyn(DBRLogonRegister.strMobilePhone,pSUBLogonRegister->strMobilePhone,CountArray(DBRLogonRegister.strMobilePhone));
	lstrcpyn(DBRLogonRegister.szMachineID,pSUBLogonRegister->szMachineID,CountArray(DBRLogonRegister.szMachineID));
	DBRLogonRegister.dwProxyID = pSUBLogonRegister->dwProxyID;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_LOGON_REGISTER,dwSocketID,&DBRLogonRegister,sizeof(DBRLogonRegister));

	return true;
}

//ƽ̨��¼
bool CAttemperEngineSink::On_SUB_CL_Logon_Platform(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_LOGON_PLATFORM))
	{
		return false;
	}

	//������Ϣ
	STR_SUB_CL_LOGON_PLATFORM * pSUBLogonPlatform=(STR_SUB_CL_LOGON_PLATFORM *)pData;

	//Ч��汾
	if ( On_CMD_LC_Logon_UpdateNotify(pSUBLogonPlatform->dwVersionCheck, dwSocketID) )
	{
		return true;
	}

	//��������
	STR_DBR_CL_LOGON_PLATFORM DBRLogonPlatform;
	ZeroMemory(&DBRLogonPlatform,sizeof(DBRLogonPlatform));
	//SocketУ��
	DBRLogonPlatform.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));
	DBRLogonPlatform.cbGender = pSUBLogonPlatform->cbGender;
	lstrcpyn(DBRLogonPlatform.szNickName, pSUBLogonPlatform->szNickName, CountArray(DBRLogonPlatform.szNickName));
	lstrcpyn(DBRLogonPlatform.strHeadUrl, pSUBLogonPlatform->strHeadUrl, CountArray(DBRLogonPlatform.strHeadUrl));
	lstrcpyn(DBRLogonPlatform.szOpenID, pSUBLogonPlatform->szOpenID, CountArray(DBRLogonPlatform.szOpenID));
	lstrcpyn(DBRLogonPlatform.szUnionID, pSUBLogonPlatform->szUnionID, CountArray(DBRLogonPlatform.szUnionID));
	lstrcpyn(DBRLogonPlatform.szMachineID,pSUBLogonPlatform->szMachineID,CountArray(DBRLogonPlatform.szMachineID));
	DBRLogonPlatform.dwProxyID = pSUBLogonPlatform->dwProxyID;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_LOGON_PLATFORM, dwSocketID, &DBRLogonPlatform, sizeof(DBRLogonPlatform));

	return true;}

//ƽ̨��¼����
bool CAttemperEngineSink::On_CMD_LC_Logon_Platform(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	STR_CMD_LC_LOGON_PLATFORM * pDBOLogonPlatform = (STR_CMD_LC_LOGON_PLATFORM *)pData;

	//��������
	STR_CMD_LC_LOGON_PLATFORM pCMDLogonPlatform;
	ZeroMemory(&pCMDLogonPlatform, sizeof(STR_CMD_LC_LOGON_PLATFORM));
	memcpy_s(&pCMDLogonPlatform, sizeof(STR_CMD_LC_LOGON_PLATFORM), pDBOLogonPlatform, sizeof(STR_CMD_LC_LOGON_PLATFORM));

	//�����¼ʧ��, ֱ�ӷ���
	if(DB_SUCCESS != pDBOLogonPlatform->dwResultCode)
	{
		m_pITCPNetworkEngine->SendData(dwContextID, MDM_LOGON, CMD_LC_LOGON_ACCOUNTS, &pCMDLogonPlatform, sizeof(STR_CMD_LC_LOGON_PLATFORM));
		return true;
	}

	//�ظ���¼�߳�
	On_CMD_LC_Logon_RepeatLogon( pCMDLogonPlatform.dwUserID ,dwContextID);

	//���������Ĵ��� 1.CMD�ṹ�帳ֵ  2.list�����itme 
	bool bIsExist = false; //�Ƿ���ڶ����б���
	for(int i=0; i < m_OfflineUserListManager.GetCount(); i++)
	{
		if(m_OfflineUserListManager.GetAt(i).dwUserID == pCMDLogonPlatform.dwUserID)
		{
			pCMDLogonPlatform.dwOffLineGameID = m_OfflineUserListManager.GetAt(i).dwServerID;
			bIsExist = true;
			break;
		}
	}
	
	//��������
	(m_pBindParameter+LOWORD(dwContextID))->dwUserID = pCMDLogonPlatform.dwUserID;//��¼�����ӵ��û�ID

	//���͵�¼�ɹ�
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_LOGON,CMD_LC_LOGON_PLATFORM, &pCMDLogonPlatform, sizeof(STR_CMD_LC_LOGON_PLATFORM));

	//�����б�
	On_CMD_LC_List_Type(dwContextID);
	On_CMD_LC_List_Kind(dwContextID);
	On_CMD_LC_List_Node(dwContextID);
	On_CMD_LC_List_Room(dwContextID);

	//TODONOW ��������½����½����������ҽ���Ӧ�÷�������

	//�����û����߱�־
	CheckUserState_Logon(pCMDLogonPlatform.dwUserID, bIsExist);

	return true;
}

//�ظ���¼�߳�
bool CAttemperEngineSink::On_CMD_LC_Logon_RepeatLogon(DWORD UserID, DWORD dwContextID)
{
	//�ظ���¼
	bool bIsRepeatLogon = false;

	//������������
	for ( int i = 0; i <  MAX_CONTENT; i++ )
	{
		tagBindParameter* p_tempBind = (m_pBindParameter+LOWORD(i));

		//�ж��û��Ƿ��Ѿ���¼
		if (p_tempBind->dwUserID == UserID)
		{
			STR_CMD_LC_LOGON_REPEAT_LOGON pCMDLogonRepeatLogon;
			ZeroMemory(&pCMDLogonRepeatLogon, sizeof(STR_CMD_LC_LOGON_REPEAT_LOGON));

			//��ʾ��ǰ�û��˺��ѵ�¼���޷���¼
			_snwprintf(pCMDLogonRepeatLogon.szDescribe, sizeof(pCMDLogonRepeatLogon.szDescribe), TEXT("�˺��Ѿ��������ط���¼"));
			m_pITCPNetworkEngine->SendData(p_tempBind->dwSocketID, MDM_LOGON, CMD_LC_LOGON_REPEAT_LOGON, &pCMDLogonRepeatLogon, sizeof(STR_CMD_LC_LOGON_REPEAT_LOGON));


			//��ʾ�ѵ�¼���û�
			_snwprintf(pCMDLogonRepeatLogon.szDescribe, sizeof(pCMDLogonRepeatLogon.szDescribe), TEXT("�˺��������ط���¼"));
			m_pITCPNetworkEngine->SendData(dwContextID, MDM_LOGON, CMD_LC_LOGON_REPEAT_LOGON, &pCMDLogonRepeatLogon, sizeof(STR_CMD_LC_LOGON_REPEAT_LOGON));

			//�ر����� --  �ر�֮ǰ������
			//m_pITCPNetworkEngine->ShutDownSocket(p_tempBind->dwSocketID);
			m_pITCPNetworkEngine->CloseSocket(p_tempBind->dwSocketID);
			
			//�������
			WORD wBindIndex = LOWORD(i);
			ZeroMemory( p_tempBind,sizeof(tagBindParameter));

			bIsRepeatLogon = true;
		}
	}

	return bIsRepeatLogon;
}

//������ʾ���汾У�飩
bool CAttemperEngineSink::On_CMD_LC_Logon_UpdateNotify(DWORD dwVersionCheck, DWORD dwSocketID)
{
	DWORD serverFrameVersion = Get_Framework_Version(PLATFORM_VERSION);  //����� frame �汾
	DWORD clientFrameVersion = dwVersionCheck; //client  Hall �汾

	byte ret = Compate_Hall_LogonServer(clientFrameVersion, serverFrameVersion);
	CLog::Log(log_debug, "�汾У����:%d  �������汾:%ld  �ͻ��˰汾:%ld", ret, serverFrameVersion, clientFrameVersion);


	//�汾ƥ��, ��ֱ���˳�, ����Ҫ������Ϣ
	if(0 == ret )
	{
		return false;
	}

	//��������
	STR_CMD_LC_LOGON_UPDATE_NOTIFY UpdateNotify;
	ZeroMemory(&UpdateNotify,sizeof(UpdateNotify));
	UpdateNotify.cbUpdateNotify = ret;
	UpdateNotify.dwCurrentServerVersion = serverFrameVersion;
	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwSocketID, MDM_LOGON, CMD_LC_LOGON_UPDATE_NOTIFY, &UpdateNotify, sizeof(UpdateNotify));

	return true;
}

//��¼����
bool CAttemperEngineSink::On_CMD_LC_Logon_Logon_Reward(DWORD dwContextID, SYSTEMTIME LasLogonDate)
{ 
	// 1. ����Ӧ���Ƕ�ȡ���ݿ�
	// 2. ��¼���� -- 1)���ս���  2)����ҽ�����
	
	//���ϵͳʱ��
	SYSTEMTIME systime;
	GetLocalTime(&systime);

	CTime tmLeft(systime.wYear, systime.wMonth, systime.wDay, 0, 0, 0);
	CTime tmRight(LasLogonDate.wYear, LasLogonDate.wMonth, LasLogonDate.wDay, 0, 0, 0);

	//����ϴε�¼ʱ��
	CTimeSpan sp = tmLeft - tmRight;
	long MillisecondsL = (systime.wHour*3600 + systime.wMinute*60 + systime.wSecond)*1000 + systime.wMilliseconds;  
	long MillisecondsR = (LasLogonDate.wHour*3600 + LasLogonDate.wMinute*60 + LasLogonDate.wSecond)*1000 + LasLogonDate.wMilliseconds;  

	//����ʱ����
	LONGLONG seconds =  ((__int64)sp.GetDays()*86400000 + (MillisecondsL - MillisecondsR))/1000;//�˴�������

	//�����Ƿ���30��,����50����
	if(seconds > 30*24*3600)
	{
		//����û��Ƹ�
		DBR_GP_ModifyUserInsure modifyUserInsure;
		ZeroMemory(&modifyUserInsure,sizeof(DBR_GP_ModifyUserInsure));
		modifyUserInsure.lOpenRoomCard = 50;
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_USER_INSURE, dwContextID, &modifyUserInsure, sizeof(DBR_GP_ModifyUserInsure));

		//����ҽ�������
		STR_CMD_LC_LOGON_LOGON_REWARD OldBackReward;
		OldBackReward.byRewardType = 2;
		OldBackReward.dwRewardCount = 50;
		lstrcpyn( OldBackReward.szDescribe,TEXT("��ӭ���������ѻ������һع齱��50����!"),CountArray(OldBackReward.szDescribe));

		m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_LOGON_LOGON_REWARD, &OldBackReward, sizeof(STR_CMD_LC_LOGON_LOGON_REWARD));
	}
	
	return true;
}

#pragma endregion

#pragma region MDM_LIST �б�����
/***************************************** ���б�����-����Ϣ2�� *******************************************/
//������Ϸ���ͣ����ơ��齫�ȣ�
VOID CAttemperEngineSink::On_CMD_LC_List_Type(DWORD dwSocketID)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGameTypeItem * pGameTypeItem=NULL;		//���ݿ�Platform�е�GameTypeItem������/����...��

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameTypeCount();i++)
	{
		//��������(��ֹ����̫�࣬һ��������)
		if ((wSendSize+sizeof(tagGameType))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_LIST,CMD_LC_LIST_TYPE,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGameTypeItem=m_ServerListManager.EmunGameTypeItem(Position);
		if (pGameTypeItem==NULL) break;

		//��������
		CopyMemory(cbDataBuffer+wSendSize,&pGameTypeItem->m_GameType,sizeof(tagGameType));
		wSendSize+=sizeof(tagGameType);
	}

	//����ʣ��
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_LIST,CMD_LC_LIST_TYPE,cbDataBuffer,wSendSize);

	return;
}

//������Ϸ���ࣨ��������ը�𻨵ȣ�
VOID CAttemperEngineSink::On_CMD_LC_List_Kind(DWORD dwSocketID)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGameKindItem * pGameKindItem=NULL;

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameKindCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameKind))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_LIST, CMD_LC_LIST_KIND, cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGameKindItem=m_ServerListManager.EmunGameKindItem(Position);
		if (pGameKindItem==NULL) break;

		//��������
		CopyMemory(cbDataBuffer+wSendSize,&pGameKindItem->m_GameKind,sizeof(tagGameKind));
		wSendSize+=sizeof(tagGameKind);
	}

	//����ʣ��
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID, MDM_LIST, CMD_LC_LIST_KIND, cbDataBuffer, wSendSize);

	return;
}

//���ͽڵ㣨����������ͨ���ȣ�
VOID CAttemperEngineSink::On_CMD_LC_List_Node(DWORD dwSocketID)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGameNodeItem * pGameNodeItem=NULL;

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameNodeCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameNode))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_LIST,CMD_LC_LIST_NODE,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGameNodeItem=m_ServerListManager.EmunGameNodeItem(Position);
		if (pGameNodeItem==NULL) break;

		CopyMemory(cbDataBuffer+wSendSize,&pGameNodeItem->m_GameNode,sizeof(tagGameNode));
		wSendSize+=sizeof(tagGameNode);
	}

	//����ʣ��
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_LIST,CMD_LC_LIST_NODE,cbDataBuffer,wSendSize);

	return;
}

//���ͷ��䣨��Ϸ�����䣩
VOID CAttemperEngineSink::On_CMD_LC_List_Room(DWORD dwSocketID)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGameServerItem * pGameServerItem=NULL;	

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameServerCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameServer))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_LIST,CMD_LC_LIST_ROOM,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ�б�����
		pGameServerItem=m_ServerListManager.EmunGameServerItem(Position);
		if (pGameServerItem==NULL) break;

		//��������
		CopyMemory(cbDataBuffer+wSendSize,&pGameServerItem->m_GameServer,sizeof(tagGameServer));
		wSendSize+=sizeof(tagGameServer);
	}

	//����ʣ��
	if (wSendSize>0)
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_LIST, CMD_LC_LIST_ROOM, cbDataBuffer, wSendSize);

	return;
}

#pragma endregion

#pragma region MDM_SERVICE �û�����
/***************************************** ����������-����Ϣ3�� *******************************************/
//��ҷ���
bool CAttemperEngineSink::On_SUB_CL_Service_UserFeedBack(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	ASSERT( wDataSize == sizeof(STR_SUB_CL_SERVICE_FEEDBACK));
	if(wDataSize != sizeof(STR_SUB_CL_SERVICE_FEEDBACK)) return false;

	//������Ϣ
	STR_SUB_CL_SERVICE_FEEDBACK * pUserSuggestion = (STR_SUB_CL_SERVICE_FEEDBACK *)pData;

	//������ҷ�������
	STR_DBR_CL_SERVICE_FEEDBACK UserSuggestion;
	ZeroMemory(&UserSuggestion,sizeof(UserSuggestion));
	UserSuggestion.dwUserID = pUserSuggestion->dwUserID;
	lstrcpyn(UserSuggestion.szFB_Title,pUserSuggestion->szFB_Title,CountArray(UserSuggestion.szFB_Title));
	lstrcpyn(UserSuggestion.szContact,pUserSuggestion->szContact,CountArray(UserSuggestion.szContact));
	lstrcpyn(UserSuggestion.szFB_Content,pUserSuggestion->szFB_Content,CountArray(UserSuggestion.szFB_Content));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_USER_FEEDBACK, dwSocketID, &UserSuggestion, sizeof(UserSuggestion));

	return true;
}

//��ҷ�������
bool CAttemperEngineSink::On_CMD_LC_Service_UserFeedBack( DWORD dwContextID, VOID * pData)
{
	//�ж�����
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	STR_CMD_LC_SERVICE_FEEDBACK FeedBack;
	ZeroMemory(&FeedBack, sizeof(FeedBack));

	//��������
	STR_DBO_CL_SERVICE_FEEDBACK *pOperate = (STR_DBO_CL_SERVICE_FEEDBACK *)pData;

	//��������
	FeedBack.lResultCode=pOperate->lResultCode;
	lstrcpyn(FeedBack.szDescribeString, pOperate->szDescribeString, CountArray(FeedBack.szDescribeString));

	//��������
	WORD wDataSize = sizeof(FeedBack);
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_SERVICE_USER_FEEDBACK, &FeedBack, wDataSize);

	return true;
}

//ˢ���û���Ϣ
bool CAttemperEngineSink::On_SUB_CL_Service_RefreshUserInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	ASSERT( wDataSize == sizeof(STR_SUB_CL_SERVICE_REFRESH_INFO));
	if(wDataSize != sizeof(STR_SUB_CL_SERVICE_REFRESH_INFO)) return false;

	//������Ϣ
	STR_SUB_CL_SERVICE_REFRESH_INFO *pUserRequest = (STR_SUB_CL_SERVICE_REFRESH_INFO *)pData;
	//�������
	STR_DBR_CL_SERCIVR_REFRESH_INFO UserRequest;
	ZeroMemory(&UserRequest,sizeof(UserRequest));

	UserRequest.dwUserID = pUserRequest->dwUserID;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_REFRESH_USER_INFO, dwSocketID, &UserRequest, sizeof(UserRequest));
	return true;
}

//ˢ���û���Ϣ����
bool CAttemperEngineSink::On_CMD_LC_Service_RefreshUserInfo( DWORD dwContextID, VOID * pData )
{
	//�ж�����
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	STR_CMD_LC_SERVICE_REFRESH_INFO UserInfo;
	ZeroMemory(&UserInfo,sizeof(UserInfo));

	//��������
	STR_DBO_CL_SERCIVR_REFRESH_INFO * pUserInfo=(STR_DBO_CL_SERCIVR_REFRESH_INFO *)pData;

	//��������
	memcpy_s(&UserInfo, sizeof(STR_CMD_LC_SERVICE_REFRESH_INFO), pUserInfo, sizeof(STR_CMD_LC_SERVICE_REFRESH_INFO));

	//��������
	WORD wDataSize = sizeof(STR_CMD_LC_SERVICE_REFRESH_INFO);
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_CL_SERVICE_REFRESH_USER_INFO, &UserInfo, wDataSize);

	return true;
}

//��ѯ������Ϣ�б�
bool CAttemperEngineSink::On_SUB_CL_Service_QueryRoomList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT( wDataSize == sizeof(STR_SUB_CL_SERVICE_QUERY_ROOMLIST));
	if(wDataSize != sizeof(STR_SUB_CL_SERVICE_QUERY_ROOMLIST)) return false;

	STR_SUB_CL_SERVICE_QUERY_ROOMLIST* pTableInfoList = (STR_SUB_CL_SERVICE_QUERY_ROOMLIST*)pData;

	
	STR_DBR_CL_SERCIVR_QUERY_ROOMLIST GetTableInfoList;
	GetTableInfoList.dwUserID = pTableInfoList->dwUserID;

	//��ѯ�����б�
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_QUERY_ROOM_LIST,dwSocketID,&GetTableInfoList,sizeof(STR_DBR_CL_SERCIVR_QUERY_ROOMLIST));

	return true;
}

//��ѯ�����б���
bool CAttemperEngineSink::On_CMD_LC_Service_QueryRoomList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_QUERY_ROOMLIST));
	STR_DBO_CL_SERCIVR_QUERY_ROOMLIST *pRecord = (STR_DBO_CL_SERCIVR_QUERY_ROOMLIST*)pData;

	STR_CMD_LC_SERVICE_QUERY_ROOMLIST TableInfo;
	ZeroMemory(&TableInfo,sizeof(STR_CMD_LC_SERVICE_QUERY_ROOMLIST));

	memcpy(&TableInfo, pRecord, sizeof(STR_CMD_LC_SERVICE_QUERY_ROOMLIST));

	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_SERVICE_QUERY_ROOM_LIST, &TableInfo, sizeof(STR_CMD_LC_SERVICE_QUERY_ROOMLIST));

	return true;
}

//��ȡ������
bool CAttemperEngineSink::On_SUB_CL_Service_GetRichList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У��
	ASSERT( wDataSize == sizeof(STR_SUB_CL_SERVICE_GET_RICHLIST));
	if( wDataSize != sizeof(STR_SUB_CL_SERVICE_GET_RICHLIST) ) 		
	{
		return false;
	}
	
	return m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_GET_RICH_LIST, dwSocketID, pData, 0);
}

//��ȡ�����񷵻�
bool CAttemperEngineSink::On_CMD_LC_Service_GetRichList( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_GET_RICHLIST));
	if(wDataSize!=sizeof(STR_DBO_CL_SERCIVR_GET_RICHLIST))
		return false;

	STR_DBO_CL_SERCIVR_GET_RICHLIST *pLotteryResult = (STR_DBO_CL_SERCIVR_GET_RICHLIST*)pData;

	STR_CMD_LC_SERVICE_GET_RICHLIST LotteryResult;
	ZeroMemory(&LotteryResult,sizeof(STR_CMD_LC_SERVICE_GET_RICHLIST));

	memcpy(&LotteryResult,pLotteryResult,sizeof(STR_DBO_CL_SERCIVR_GET_RICHLIST));		//�޸�bug��ԭ����sizeof���治������ṹ��

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_GET_RICH_LIST, &LotteryResult, sizeof(STR_CMD_LC_SERVICE_GET_RICHLIST));

	return true;
}

//��ȡ�û�¼���б�
bool CAttemperEngineSink::On_SUB_CL_Service_GetUserRecordList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT( wDataSize == sizeof(STR_SUB_CL_SERVICE_GET_DJ_RECORDLIST));
	if(wDataSize != sizeof(STR_SUB_CL_SERVICE_GET_DJ_RECORDLIST)) return false;

	STR_SUB_CL_SERVICE_GET_DJ_RECORDLIST* pGetRecordList = (STR_SUB_CL_SERVICE_GET_DJ_RECORDLIST*)pData;

	//¼���б�����
	STR_DBR_CL_SERCIVR_GET_RECORDLIST GetRecordList;
	GetRecordList.dwUserID = pGetRecordList->dwUserID;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_GET_USER_RECORD_LIST, dwSocketID, &GetRecordList, sizeof(STR_DBR_CL_SERCIVR_GET_RECORDLIST));

	return true;
}

//��ȡ�û�¼���б���
bool CAttemperEngineSink::On_CMD_LC_Service_GetUserRecordList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
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

//��ȡָ��ID¼��
bool CAttemperEngineSink::On_SUB_CL_Service_GetSpecificRecord(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT( wDataSize == sizeof(STR_SUB_CL_SERVICE_GET_XJ_RECORDLIST));
	if(wDataSize != sizeof(STR_SUB_CL_SERVICE_GET_XJ_RECORDLIST)) return false;

	STR_SUB_CL_SERVICE_GET_XJ_RECORDLIST* pGetRecordList = (STR_SUB_CL_SERVICE_GET_XJ_RECORDLIST*)pData;

	//DBR����
	STR_DBR_CL_SERCIVR_GET_SPECIFIC_RECORD GetRecordList;
	GetRecordList.dwTableID = pGetRecordList->dwTableID;

	//У������ID
	if ( 0 != GetRecordList.dwTableID )
	{
		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_GET_SPECIFIC_RECORD,dwSocketID,&GetRecordList,sizeof(STR_DBR_CL_SERCIVR_GET_SPECIFIC_RECORD));
	}

	return true;
}

//��ȡָ��ID¼�񷵻�
bool CAttemperEngineSink::On_CMD_LC_Service_GetSpecificRecord(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_GET_SPECIFIC_RECORD));
	STR_DBO_CL_SERCIVR_GET_SPECIFIC_RECORD *pRecord = (STR_DBO_CL_SERCIVR_GET_SPECIFIC_RECORD*)pData;

	STR_CMD_LC_SERVICE_GET_XJ_RECORDLIST Record;
	memcpy(&Record, pRecord, sizeof(STR_CMD_LC_SERVICE_GET_XJ_RECORDLIST));

	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_SERVICE_GET_XJ_RECORD_LIST, &Record, sizeof(STR_CMD_LC_SERVICE_GET_XJ_RECORDLIST));

	return true;
}

//��ȡ���߽���
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

//��ȡ���߽�������
bool CAttemperEngineSink::On_CMD_LC_Service_OnlineReward( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
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

//��ȡ�����б�
bool CAttemperEngineSink::On_SUB_CL_Service_GetTaskList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT(wDataSize >= sizeof(STR_SUB_CL_SERVICE_GET_TASKLIST));
	if (wDataSize<sizeof(STR_SUB_CL_SERVICE_GET_TASKLIST)) return false;

	//������Ϣ
	STR_SUB_CL_SERVICE_GET_TASKLIST * pGetTaskList = (STR_SUB_CL_SERVICE_GET_TASKLIST *)pData;

	//��������
	STR_DBR_CL_SERCIVR_GET_TASKLIST GetTaskList;
	ZeroMemory(&GetTaskList,sizeof(GetTaskList));

	//��������
	memcpy(&GetTaskList,pGetTaskList,sizeof(GetTaskList));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_GET_TASK_LIST,dwSocketID,&GetTaskList,sizeof(GetTaskList));

	return true;
}

//��ȡ�����б���
bool CAttemperEngineSink::On_CMD_LC_Service_GetTaskList( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
	if( 0 != (wDataSize % sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST)) )
		return false;

	//����������
	int count = wDataSize / sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST);

	//���ݿ������б�
	STR_DBO_CL_SERCIVR_GET_TASKLIST *TaskList = (STR_DBO_CL_SERCIVR_GET_TASKLIST *)pData;

	//���ص������б�
	STR_CMD_LC_SERVICE_GET_TASKLIST *pCMDTaskList = new STR_CMD_LC_SERVICE_GET_TASKLIST[count];

	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	for (int i=0; i<count; i++)
	{
		//��DBO���ݸ������ص�����,��ɱ�־������ֵ
		memcpy_s(&pCMDTaskList[i], sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST), &TaskList[i], sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST));

		//���һ�η���,������ɱ�־Ϊ1
		if ( i == (count-1) )		
		{
			pCMDTaskList[i].cbListFinishMask = 1;
		}
		else
		{
			pCMDTaskList[i].cbListFinishMask = 0;
		}
		
		//��������(��ֹ����̫�࣬һ��������)
		if ( (wSendSize+sizeof(STR_CMD_LC_SERVICE_GET_TASKLIST)) > sizeof(cbDataBuffer) )
		{
			//��������
			m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_SERVICE_GET_TASK_LIST, cbDataBuffer, wSendSize);			
			wSendSize=0;
		}

		//��������
		CopyMemory(cbDataBuffer+wSendSize, &pCMDTaskList[i], sizeof(STR_CMD_LC_SERVICE_GET_TASKLIST));
		wSendSize += sizeof(STR_CMD_LC_SERVICE_GET_TASKLIST);
	}

	//����ʣ��
	if (wSendSize>0) 	
	{
		m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_SERVICE_GET_TASK_LIST, cbDataBuffer, wSendSize);
	}

	//�ͷ��ڴ�		TODO ���������
	delete [] pData;		//ǰ������ʱnew�����ģ�������Ҫdelete������֮ǰ���ڴ洫���
	delete [] pCMDTaskList;

	return true;
}

//��ȡ������
bool CAttemperEngineSink::On_SUB_CL_Service_GetTaskReward(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT(wDataSize >= sizeof(STR_SUB_CL_SERVICE_GET_TASK_REWARD));
	if (wDataSize<sizeof(STR_SUB_CL_SERVICE_GET_TASK_REWARD)) return false;

	//������Ϣ
	STR_SUB_CL_SERVICE_GET_TASK_REWARD * pSetTaskDone =(STR_SUB_CL_SERVICE_GET_TASK_REWARD *)pData;
	//��������
	STR_DBR_CL_SERCIVR_GET_TASK_REWARD SetTaskDone;
	ZeroMemory(&SetTaskDone,sizeof(SetTaskDone));

	//��������
	memcpy(&SetTaskDone,pSetTaskDone,sizeof(SetTaskDone));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_GET_TASK_REWARD,dwSocketID,&SetTaskDone,sizeof(SetTaskDone));

	return true;
}

//��ȡ����������
bool CAttemperEngineSink::On_CMD_LC_Service_GetTaskReward( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
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

//��ȡ���а�
bool CAttemperEngineSink::On_SUB_CL_Service_GetRankList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	ASSERT(wDataSize==sizeof(STR_SUB_CL_SERVICE_GET_RANKLIST));
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_GET_RANKLIST))
		return false;

	STR_SUB_CL_SERVICE_GET_RANKLIST* pTaskList = (STR_SUB_CL_SERVICE_GET_RANKLIST*)pData;

	STR_CMD_LC_SERVICE_GET_RANKLIST rankList;
	ZeroMemory(&rankList, sizeof(STR_CMD_LC_SERVICE_GET_RANKLIST));

	//���а��������ȡ���а��б�
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

//����齱
bool CAttemperEngineSink::On_SUB_CL_Service_RequestLottery(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	ASSERT(wDataSize==sizeof(STR_SUB_CL_SERVICE_REQUEST_LOTTERY));
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_REQUEST_LOTTERY))
		return false;

	STR_SUB_CL_SERVICE_REQUEST_LOTTERY* pGetRankReward = (STR_SUB_CL_SERVICE_REQUEST_LOTTERY*)pData;

	STR_DBR_CL_SERCIVR_REQUEST_LOTTERY QueryLottery;
	memcpy(&QueryLottery,pGetRankReward,sizeof(STR_DBR_CL_SERCIVR_REQUEST_LOTTERY));

	return m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_REQUEST_LOTTERY,dwSocketID,&QueryLottery,sizeof(STR_DBR_CL_SERCIVR_REQUEST_LOTTERY));

}

//����齱����
bool CAttemperEngineSink::On_CMD_LC_Service_RequestLottery( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_REQUEST_LOTTERY));
	if(wDataSize!=sizeof(STR_DBO_CL_SERCIVR_REQUEST_LOTTERY))
		return false;

	STR_DBO_CL_SERCIVR_REQUEST_LOTTERY *pLotteryResult = (STR_DBO_CL_SERCIVR_REQUEST_LOTTERY*)pData;

	//����齱����
	STR_CMD_LC_SERVICE_REQUEST_LOTTERY LotteryResult;
	ZeroMemory(&LotteryResult,sizeof(STR_CMD_LC_SERVICE_REQUEST_LOTTERY));

	//��ֵ
	LotteryResult.byIndex = pLotteryResult->byIndex;
	LotteryResult.lResultCode = pLotteryResult->lResultCode;
	lstrcpyn(LotteryResult.szDescribeString, pLotteryResult->szDescribeString, CountArray(LotteryResult.szDescribeString));

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_REQUEST_LOTTERY, &LotteryResult, sizeof(STR_CMD_LC_SERVICE_REQUEST_LOTTERY));

	return true;
}

//pure�������а� ��ѯ
bool CAttemperEngineSink::On_SUB_CL_SERVICE_PURE_STANDING_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_PURE_STANDING_LIST))
		return false;

	return m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_PURE_STANDING_LIST,dwSocketID,pData, wDataSize);
}
//pure�������а� ����
bool CAttemperEngineSink::On_CMD_LC_SERVICE_PURE_STANDING_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_PURE_STANDING_LIST);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_PURE_STANDING_LIST, pData, wDataSize);

	return true;
}
//pure�������а� ����
bool CAttemperEngineSink::On_CMD_LC_SERVICE_PURE_STANDING_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
	if(wDataSize!=sizeof(STR_CMD_LC_SERVICE_PURE_STANDING_FINISH))
		return false;

	STR_CMD_LC_SERVICE_PURE_STANDING_FINISH *pLotteryResult = (STR_CMD_LC_SERVICE_PURE_STANDING_FINISH*)pData;
	pLotteryResult->byMask = 1;

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_PURE_STANDING_FINISH, pLotteryResult, sizeof(STR_CMD_LC_SERVICE_PURE_STANDING_FINISH));

	return true;
}

//pure���ս�� ��ѯ
bool CAttemperEngineSink::On_SUB_CL_SERVICE_PURE_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_PURE_RECORD_LIST))
		return false;

	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_PURE_RECORD_LIST,dwSocketID,pData, wDataSize);

	return true;
}
//pure���ս�� ����
bool CAttemperEngineSink::On_CMD_LC_SERVICE_PURE_RECORD_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_PURE_RECORD_LIST, pData, wDataSize);

	return true;
}
//pure��������Ϣ ����
bool CAttemperEngineSink::On_CMD_LC_SERVICE_PURE_RECORD_LIST_PINFO( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST_PLAYERINFO);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_PURE_RECORD_LIST_PINFO, pData, wDataSize);

	return true;
}
//pure���ս�� ����
bool CAttemperEngineSink::On_CMD_LC_SERVICE_PURE_RECORD_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
	if(wDataSize!=sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST_FINIST))
		return false;

	STR_CMD_LC_SERVICE_PURE_RECORD_LIST_FINIST *pLotteryResult = (STR_CMD_LC_SERVICE_PURE_RECORD_LIST_FINIST*)pData;
	pLotteryResult->byMask = 1;

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_PURE_RECORD_FINISH, pLotteryResult, sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST_FINIST));

	return true;
}

//pureС��ս�� ��ѯ
bool CAttemperEngineSink::On_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST))
		return false;

	return m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_PURE_XJ_RECORD_LIST,dwSocketID,pData, wDataSize);
}
//pureС��ս�� ����
bool CAttemperEngineSink::On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST * pCMD = (STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST *) pData;

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_PURE_XJ_RECORD_LIST, pData, wDataSize);

	return true;
}
//pureС�������Ϣ ����
bool CAttemperEngineSink::On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PLAYERINFO);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO, pData, wDataSize);

	return true;
}
//pureС��ս�� ����
bool CAttemperEngineSink::On_CMD_LC_SERVICE_PURE_XJ_RECORD_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
	if(wDataSize!=sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_FINISH))
		return false;

	STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_FINISH *pLotteryResult = (STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_FINISH*)pData;
	pLotteryResult->byMask = 1;

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_PURE_XJ_RECORD_FINISH, pLotteryResult, sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_FINISH));

	return true;
}

//С��¼��ط�
bool CAttemperEngineSink::On_SUB_CL_Service_XJRecordPlayback(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//���ݴ�СУ��
	if ( wDataSize != sizeof(STR_SUB_CL_SERVICE_XJ_RECORD_PLAYBACK) )
		return false;

	//SUB����
	STR_SUB_CL_SERVICE_XJ_RECORD_PLAYBACK *SUB = (STR_SUB_CL_SERVICE_XJ_RECORD_PLAYBACK *)pData;
	
	//����DBR����
	STR_DBR_CL_SERVICE_XJ_RECORD_PLAYBACK DBR;
	ZeroMemory(&DBR, sizeof(STR_DBR_CL_SERVICE_XJ_RECORD_PLAYBACK));
	DBR.dwRecordID = SUB->dwRecordID;

	return m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_XJ_RECORD_PLAYBACK, dwSocketID, &DBR, wDataSize);
}
//С��¼��ط� ����
bool CAttemperEngineSink::On_CMD_LC_Service_XJRecordPlayback( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
	if(wDataSize != sizeof(STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK))
		return false;

	//DBO����
	STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK *pDBO = (STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK*)pData;

	BYTE cbBuffer[LEN_MAX_RECORD_SIZE];	
	memcpy_s(cbBuffer, sizeof(cbBuffer), pDBO->cbRecordData, sizeof(cbBuffer));

	//��������
	for (int i = 0; i < 4; i++)
	{
		STR_CMD_LC_SERVICE_XJ_RECORD_PLAYBACK CMD;
		ZeroMemory(&CMD, sizeof(STR_CMD_LC_SERVICE_XJ_RECORD_PLAYBACK));
		memcpy_s(CMD.cbRecordData, sizeof(CMD.cbRecordData), cbBuffer+i*LEN_MAX_RECORD_SIZE/4, sizeof(CMD.cbRecordData));
		CMD.cbfinish = (i==3) ? 1 : 0;

		//��������
		m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_SERVICE_XJ_RECORD_PLAYBACK, &CMD, sizeof(CMD));

	}

	return true;
}

//�ͷ���Ϣ
bool CAttemperEngineSink::On_SUB_CL_SERVICE_CUSTOMER_MESSEGE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//���ݴ�СУ��
	if ( wDataSize != sizeof(STR_SUB_CL_SERVICE_CUSTOMER_MESSEGE) )
		return false;

	//SUB����
	STR_SUB_CL_SERVICE_CUSTOMER_MESSEGE *SUB = (STR_SUB_CL_SERVICE_CUSTOMER_MESSEGE *)pData;
	
	//����DBR����
	STR_DBR_CL_SERVICE_CUSTOMER_MESSEGE DBR;
	ZeroMemory(&DBR, sizeof(STR_DBR_CL_SERVICE_CUSTOMER_MESSEGE));
	DBR.cbMessegeFlag = SUB->cbMessegeFlag;

	return m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_CUSTOMER_MESSEGE, dwSocketID, &DBR, sizeof(DBR));
}
//�ͷ���Ϣ ����
bool CAttemperEngineSink::On_CMD_LC_SERVICE_CUSTOMER_MESSEGE( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
	if(wDataSize != sizeof(STR_DBO_LC_SERVICE_CUSTOMER_MESSEGE))
		return false;

	//DBO����
	STR_DBO_LC_SERVICE_CUSTOMER_MESSEGE *pDBO = (STR_DBO_LC_SERVICE_CUSTOMER_MESSEGE*)pData;

	//����CMD����
	STR_CMD_LC_SERVICE_CUSTOMER_MESSEGE CMD;
	ZeroMemory(&CMD, sizeof(STR_CMD_LC_SERVICE_CUSTOMER_MESSEGE));
	CMD.cbMessegeFlag = pDBO->cbMessegeFlag;
	lstrcpyn(CMD.szMessege, pDBO->szMessege, CountArray(CMD.szMessege));

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_SERVICE_CUSTOMER_MESSEGE, &CMD, sizeof(STR_CMD_LC_SERVICE_CUSTOMER_MESSEGE));

	return true;
}

//�����Ҵ�����Ϣ
bool CAttemperEngineSink::On_SUB_CL_SERVICE_GOLD_INFO(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_GOLD_INFO))
		return false;

	return m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_GOLD_INFO,dwSocketID,pData, wDataSize);
}
//�����Ҵ�����Ϣ ����
bool CAttemperEngineSink::On_CMD_LC_SERVICE_GOLD_INFO( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_GOLD_INFO);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_GOLD_INFO, pData, wDataSize);

	return true;
}
//�����Ҵ�����Ϣ ����
bool CAttemperEngineSink::On_CMD_LC_SERVICE_GOLD_INFO_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
	if(wDataSize!=sizeof(STR_CMD_LC_SERVICE_GOLD_INFO_FINISH))
		return false;

	STR_CMD_LC_SERVICE_GOLD_INFO_FINISH *pLotteryResult = (STR_CMD_LC_SERVICE_GOLD_INFO_FINISH*)pData;
	pLotteryResult->byMask = 1;

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_SERVICE, CMD_LC_SERVICE_GOLD_INFO_FINISH, pLotteryResult, sizeof(STR_CMD_LC_SERVICE_GOLD_INFO_FINISH));

	return true;
}

//�޸ĸ�������
bool CAttemperEngineSink::On_SUB_CL_Service_ModifyPersonalInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У��
	if (wDataSize != sizeof(STR_SUB_CL_SERVICE_MODIFY_PERSONAL_INFO)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_MODIFY_PERSONAL_INFO, dwSocketID, pData, wDataSize);

	return true;
}

//�޸ĸ������Ϸ���
bool CAttemperEngineSink::On_CMD_LC_Service_ModifyPersonalInfo( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
	if(wDataSize!=sizeof(STR_DBO_CL_MODIFY_PERSONL_INFO))
		return false;

	STR_DBO_CL_MODIFY_PERSONL_INFO *pModifyInfo = (STR_DBO_CL_MODIFY_PERSONL_INFO*)pData;

	//��������
	STR_CMD_LC_SERVICE_MODIFY_PERSONL_INFO PersonalInfo;
	ZeroMemory(&PersonalInfo,sizeof(STR_CMD_LC_SERVICE_MODIFY_PERSONL_INFO));
	CopyMemory(&PersonalInfo, pModifyInfo, sizeof(PersonalInfo));

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_SERVICE_MODIFY_PERSONAL_INFO, &PersonalInfo, sizeof(STR_CMD_LC_SERVICE_MODIFY_PERSONL_INFO));

	return true;
}

//��ѯ��ҷ�������
bool CAttemperEngineSink::On_CMD_LC_Service_QueryScoreInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO * pScoreInfo =(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO *)pData;

	//��������
	STR_CMD_LC_QUERY_SCORE_INFO ScoreInfo;
	ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));

	//������Ϣ
	ScoreInfo.lResultCode = pScoreInfo->lResultCode;
	lstrcpyn(ScoreInfo.szDescribeString, pScoreInfo->szDescribeString, CountArray(ScoreInfo.szDescribeString));

	//�Ƹ���Ϣ
	ScoreInfo.dwUserID = pScoreInfo->dwUserID;
	ScoreInfo.lGold = pScoreInfo->lGold;
	ScoreInfo.lOpenRoomCard = pScoreInfo->lOpenRoomCard;
	ScoreInfo.lDiamond = pScoreInfo->lDiamond;

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_USER_QUERY_SCORE_INFO, &ScoreInfo, sizeof(STR_CMD_LC_QUERY_SCORE_INFO));

	return true;
}

//�����������
bool CAttemperEngineSink::On_CMD_LC_CommonOperateResult( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//�ж�����
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	STR_CMD_LC_OTHER_OPERATE_RESULT OperateResult;
	ZeroMemory(&OperateResult,sizeof(OperateResult));

	//��������
	DBO_GP_OperateCommand * pOperate=(DBO_GP_OperateCommand *)pData;

	//��������
	OperateResult.lResultCode=pOperate->lResultCode;

	//��������
	WORD wHeadSize=sizeof(OperateResult);
	m_pITCPNetworkEngine->SendData(dwContextID,pOperate->mCommand.MainCommand,pOperate->mCommand.SubCommand,&OperateResult,wHeadSize);
	return true;
}

//��ȡ���񷵻�
bool CAttemperEngineSink::OnDBRankRewardResult( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
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

/***************************************** ��������Ϣ������-����Ϣ6�� *******************************************/
//�û���ֵ��Ϣ
bool CAttemperEngineSink::On_SUB_CL_Other_ReChargeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	ASSERT( wDataSize == sizeof(STR_SUB_CL_OTHER_RECHARGE_INFO));
	if(wDataSize != sizeof(STR_SUB_CL_OTHER_RECHARGE_INFO)) return false;

	//������Ϣ
	STR_SUB_CL_OTHER_RECHARGE_INFO * pUserRequest = (STR_SUB_CL_OTHER_RECHARGE_INFO *)pData;
	//�������
	STR_DBR_CL_OTHER_RECHARGE_INFO UserRequest;
	ZeroMemory(&UserRequest,sizeof(UserRequest));

	UserRequest.dwUserID = pUserRequest->dwUserID;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_USER_RECHARGE_INFO,dwSocketID,&UserRequest,sizeof(UserRequest));
	return true;
}

//�û���ֵ��Ϣ����
bool CAttemperEngineSink::On_CMD_LC_Other_RechargeInfo( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//�ж�����
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	STR_CMD_LC_OTHER_RECHARGE_INFO RechangeInfo;
	ZeroMemory(&RechangeInfo,sizeof(RechangeInfo));

	//��������
	STR_DBO_CL_OTHER_RECHARGE_INFO * pRechangeInfo = (STR_DBO_CL_OTHER_RECHARGE_INFO *)pData;

	//��������
	RechangeInfo.dwMinMoney = pRechangeInfo->dwMinMoney;
	RechangeInfo.dwChangeScale = pRechangeInfo->dwChangeScale;

	//��������
	WORD wHeadSize=sizeof(RechangeInfo);
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_OTHERS_RECHARGE_INFO, &RechangeInfo, wHeadSize);
	return true;
}

#pragma region �������
//���͵���
bool CAttemperEngineSink::On_SUB_CL_GIFT_GIVE_PROPS(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	if(wDataSize!=sizeof(STR_SUB_CL_GIFT_GIVE_PROPS))
		return false;

	return m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_GIFT_GIVE_PROPS,dwSocketID, pData, wDataSize);
}

//���͵��߷���
bool CAttemperEngineSink::On_CMD_LC_GIFT_GIVE_PROPS( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
	if(wDataSize!=sizeof(STR_CMD_LC_GIFT_GIVE_PROPS))
		return false;

	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GIFT, CMD_LC_GIFT_GIVE_PROPS, pData, wDataSize);

	return true;
}

//���͵��� ֪ͨ������
bool CAttemperEngineSink::On_CMD_LC_GIFT_GIVE_PROPS_SHOW( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
	if(wDataSize!=sizeof(STR_CMD_LC_GIFT_GIVE_PROPS_SHOW))
		return false;

	STR_CMD_LC_GIFT_GIVE_PROPS_SHOW *pCMD = (STR_CMD_LC_GIFT_GIVE_PROPS_SHOW*) pData;

	//���������û� ��������
	for (int j = 0; j < MAX_CONTENT; j++)
	{
		tagBindParameter * pBindParameter=(m_pBindParameter+j);
		if ( pBindParameter && pBindParameter->dwUserID != 0 && 
			pBindParameter->dwSocketID != 0 && 
			pBindParameter->dwUserID == pCMD->dwTargetID)
		{
			//֪ͨ�û�
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GIFT, CMD_LC_GIFT_GIVE_PROPS_SHOW, 
											pData, wDataSize);		
		}
	}

	//TODONOW ���������������ϢƵ�� ���������﷢��   
	//1. ���������ʱ��һ����Ҫ��ͷ��, ֻд�ǳƺ��������Ϣ����
	//2. �ͻ�������һ����ť, ���Կ����Ƿ��������Ƶ�� 
	return true;
}

#pragma endregion


//�һ�����
bool CAttemperEngineSink::On_SUB_CL_Other_ExchangeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	ASSERT( wDataSize == sizeof(STR_SUB_CL_OTHER_EXCHANGE_INFO));
	if(wDataSize != sizeof(STR_SUB_CL_OTHER_EXCHANGE_INFO)) return false;

	//������Ϣ
	STR_SUB_CL_OTHER_EXCHANGE_INFO * pUserRequest = (STR_SUB_CL_OTHER_EXCHANGE_INFO *)pData;
	//�������
	STR_DBR_CL_OTHER_EXCHANGE_INFO UserRequest;
	ZeroMemory(&UserRequest,sizeof(UserRequest));

	UserRequest.dwUserID = pUserRequest->dwUserID;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_USER_EXCHANGE_INFO, dwSocketID, &UserRequest, sizeof(UserRequest));
	return true;
}

//�һ����߷���
bool CAttemperEngineSink::On_CMD_LC_Other_ExchangeInfo( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//�ж�����
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	STR_DBO_CL_OTHER_EXCHANGE_INFO * pExchangeInfo=(STR_DBO_CL_OTHER_EXCHANGE_INFO *)pData;

	//��������
	STR_CMD_LC_OTHER_EXCHANGE_INFO ExchangeInfo;
	ZeroMemory(&ExchangeInfo,sizeof(ExchangeInfo));

	ExchangeInfo.dwMinMoney = pExchangeInfo->dwMinMoney;
	ExchangeInfo.dwChangeScale = pExchangeInfo->dwChangeScale;
	ExchangeInfo.dwBankMoney = pExchangeInfo->dwBankMoney;
	ExchangeInfo.dwWithdrawals = pExchangeInfo->dwWithdrawals;

	//��������
	WORD wHeadSize=sizeof(ExchangeInfo);
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SERVICE, CMD_LC_OTHERS_EXCHANGE_INFO, &ExchangeInfo, wHeadSize);

	return true;
}

#pragma region MDM_CLUB ����Ȧ(�汾2)
//��ѯ����Ȧ�б�
bool CAttemperEngineSink::On_SUB_CL_CLUB_ALL_CLUB_INFO_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ALL_CLUB_INFO_LIST)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_ALL_CLUB_INFO_LIST, dwSocketID, pData, wDataSize);

	return true;
}
//��ѯ����Ȧ�б���
bool CAttemperEngineSink::On_CMD_LC_CLUB_ALL_CLUB_INFO_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST);
	if( (wDataSize % Size) != 0) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_ALL_CLUB_INFO_LIST, pData, wDataSize);
	return true;
}
//��ѯ����Ȧ�б����
bool CAttemperEngineSink::On_CMD_LC_CLUB_ALL_INFO_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_ALL_INFO_FINISH cmd;
	cmd.byMask = 1;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_ALL_INFO_FINISH, &cmd, sizeof(STR_CMD_LC_CLUB_ALL_INFO_FINISH));
	return true;
}

//��ѯָ������Ȧ�����б�
bool CAttemperEngineSink::On_SUB_CL_CLUB_ROOM_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_LIST)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_ROOM_LIST, dwSocketID, pData, wDataSize);
	return true;
}
//��ѯָ������Ȧ�����б� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_ROOM_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_ROOM_LIST);
	if( (wDataSize % Size ) != 0) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_ROOM_LIST, pData, wDataSize);
	return true;
}
//��ѯָ������Ȧ�����б� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_ROOM_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_ROOM_LIST_FINISH cmd;
	cmd.byMask = 1;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_ROOM_LIST_FINISH, &cmd, sizeof(STR_CMD_LC_CLUB_ROOM_LIST_FINISH));
	return true;
}

//��ѯδ��Ա, �������Ȧ(���9��)
bool CAttemperEngineSink::On_SUB_CL_CLUB_RANDOM_CLUB_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_RANDOM_CLUB_LIST)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_RANDOM_CLUB_LIST, dwSocketID, pData, wDataSize);
	return true;
}
//��ѯδ��Ա, �������Ȧ(���9��) ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_RANDOM_CLUB_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_RANDOM_CLUB_LIST);
	if( (wDataSize % Size) != 0) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_RANDOM_CLUB_LIST, pData, wDataSize);
	return true;
}
//��ѯδ��Ա, �������Ȧ(���9��) ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH cmd;
	cmd.byMask = 1;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH, &cmd, sizeof(STR_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH));
	return true;
}

//�����������Ȧ
bool CAttemperEngineSink::On_SUB_CL_CLUB_JOIN_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_JOIN_CLUB)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_JOIN_CLUB, dwSocketID, pData, wDataSize);
	return true;
}
//�����������Ȧ���� 
bool CAttemperEngineSink::On_CMD_LC_CLUB_JOIN_CLUB( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_JOIN_CLUB);
	if( wDataSize != Size) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_JOIN_CLUB, pData, wDataSize);
	return true;
}
//�����������Ȧ�㲥
bool CAttemperEngineSink::On_CMD_LC_CLUB_JOIN_CLUB_BDCAST( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_SUB_CL_CLUB_JOIN_CLUB_BDCAST);
	if( wDataSize != Size) return false;

	//TODONOW ��ʱ��ÿ���˶����ͣ������Ϊ 1.�ض����ֲ���  2.�᳤�͹���Ա����
	m_pITCPNetworkEngine->SendDataBatch(MDM_CLUB, CMD_LC_CLUB_JOIN_CLUB_BDCAST, pData, wDataSize);
	return true;
}
//�����������Ȧ ֪ͨ�ͻ���ʵʱˢ��
bool CAttemperEngineSink::On_CMD_LC_CLUB_JOIN_CLUB_RE( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_LIST_RE);
	if( wDataSize != Size) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_LIST_RE, pData, wDataSize);
	return true;
}


//��ɢ����Ȧ
bool CAttemperEngineSink::On_SUB_CL_CLUB_DISS_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_DISS_CLUB)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_DISS_CLUB, dwSocketID, pData, wDataSize);
	return true;
}
//��ɢ����Ȧ ���� 
bool CAttemperEngineSink::On_CMD_LC_CLUB_DISS_CLUB( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_DISS_CLUB);
	if( wDataSize != Size) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_DISS_CLUB, pData, wDataSize);
	return true;
}

//��������Ȧ
bool CAttemperEngineSink::On_SUB_CL_CLUB_CREATE_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_CREATE_CLUB)) return false;

	//������Ϣ
	STR_SUB_CL_CLUB_CREATE_CLUB * pSub = (STR_SUB_CL_CLUB_CREATE_CLUB *)pData;
	//�������
	STR_DBR_CL_CLUB_CREATE_CLUB Dbr;
	ZeroMemory(&Dbr,sizeof(Dbr));

	Dbr.dwUserID = pSub->dwUserID;
	lstrcpyn(Dbr.szClubName, pSub->szClubName, CountArray(Dbr.szClubName));
	lstrcpyn(Dbr.szNotice, pSub->szNotice, CountArray(Dbr.szNotice));
	lstrcpyn(Dbr.szMessag, pSub->szMessag, CountArray(Dbr.szMessag));
	Dbr.dwMajorKindID = pSub->dwMajorKindID;

	//������Ϣ
	for (WORD i=0; i<10; i++)
	{
		INT nLength=lstrlen(Dbr.szMinorKindID);
		_sntprintf(&Dbr.szMinorKindID[nLength],CountArray(Dbr.szMinorKindID)-nLength,
			TEXT("%ld,"),
			pSub->dwMinorKindID[i]);
	}

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_CREATE_CLUB, dwSocketID, &Dbr, sizeof(Dbr));
	return true;
}
//��������Ȧ ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_CREATE_CLUB( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_CREATE_CLUB);
	if( wDataSize != Size ) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_CREATE_CLUB, pData, wDataSize);
	return true;
}

//��������
bool CAttemperEngineSink::On_SUB_CL_CLUB_ROOM_SETTING(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_SETTING)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_ROOM_SETTING, dwSocketID, pData, wDataSize);
	return true;
}
//�������� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_ROOM_SETTING( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_ROOM_SETTING);
	if( wDataSize != Size) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_ROOM_SETTING, pData, wDataSize);
	return true;
}

//���󷿼�����
bool CAttemperEngineSink::On_SUB_CL_CLUB_ROOM_QUERY_SETTING(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_QUERY_SETTING)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_ROOM_QUERY_SETTING, dwSocketID, pData, wDataSize);
	return true;
}
//���󷿼����� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_ROOM_QUERY_SETTING( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_ROOM_QUERY_SETTING);
	if( wDataSize != Size) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_ROOM_QUERY_SETTING, pData, wDataSize);
	return true;
}


//����뿪���ֲ�����
bool CAttemperEngineSink::On_SUB_CL_CLUB_ROOM_USER_LEAVE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_USER_LEAVE)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_ROOM_USER_LEAVE, dwSocketID, pData, wDataSize);
	return true;
}

//��ɢ��������
bool CAttemperEngineSink::On_SUB_CL_CLUB_ROOM_DISSOLVE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_DISSOLVE)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_ROOM_DISSOLVE, dwSocketID, pData, wDataSize);
	return true;
}
//��ɢ�������� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_ROOM_DISSOLVE( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_ROOM_DISSOLVE);
	if( wDataSize != Size) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_ROOM_DISSOLVE, pData, wDataSize);
	return true;
}

//��ɢ��������
bool CAttemperEngineSink::On_SUB_CL_CLUB_TABLE_DISSOLVE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_TABLE_DISSOLVE)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_TABLE_DISSOLVE, dwSocketID, pData, wDataSize);
	return true;
}
//��ɢ�������� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_TABLE_DISSOLVE( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_TABLE_DISSOLVE);
	if( wDataSize != Size) return false;

	STR_CMD_LC_CLUB_TABLE_DISSOLVE* pCmd = (STR_CMD_LC_CLUB_TABLE_DISSOLVE *) pData;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_TABLE_DISSOLVE, pData, wDataSize);

	if( (DB_SUCCESS ==  pCmd->lResultCode) && (0 == pCmd->byMask) && (0 != pCmd->dwGameID))
	{
		//֪ͨЭ����, Э����֪ͨ��Ϸ��
		STR_CPR_LP_CLUB_TABLE_DISSOLVE CPR;
		CPR.dwGameID = pCmd->dwGameID;
		CPR.dwTableID = pCmd->dwTableID;

		m_pITCPSocketEngine->SendData(MDM_TRANSFER, CPR_LP_CLUB_TABLE_DISSOLVE, &CPR, sizeof(STR_CPR_LP_CLUB_TABLE_DISSOLVE));
	}
	return true;
}


//����Ȧ����
bool CAttemperEngineSink::On_SUB_CL_CLUB_NOTICE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_NOTICE)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_NOTICE, dwSocketID, pData, wDataSize);
	return true;
}
//����Ȧ���� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_NOTICE( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_NOTICE);
	if( wDataSize != Size) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_NOTICE, pData, wDataSize);
	return true;
}
//����Ȧ���� �㲥
bool CAttemperEngineSink::On_CMD_LC_CLUB_NOTICE_BDCAST( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//TODONOW ��Ҫʵ��
	//У�����
	WORD Size = sizeof(STR_CMD_LC_SHOP_QUERY_RESULT);
	if( wDataSize != Size) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_ROOM_SETTING, pData, wDataSize);
	return true;
}

//����
bool CAttemperEngineSink::On_SUB_CL_CLUB_CHAT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_CHAT)) return false;
	STR_SUB_CL_CLUB_CHAT *pSub = (STR_SUB_CL_CLUB_CHAT*)pData;

	if(0 == pSub->byChatMode) //���ֲ�����
	{
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_CHAT, dwSocketID, pData, wDataSize);
	}
	else if( 1 == pSub->byChatMode) //��������
	{
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_WORD_CHAT, dwSocketID, pData, wDataSize);
	}
	else if( 2 == pSub->byChatMode) //ϵͳ����
	{
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SYSTEM_CHAT, dwSocketID, pData, wDataSize);
	}
	else if( 3 == pSub->byChatMode) //˽��
	{
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SECRET_CHAT, dwSocketID, pData, wDataSize);
	}

	return true;
}
//���� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_CHAT( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_CHAT);
	if( wDataSize != Size) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_CHAT, pData, wDataSize);
	return true;
}
//����Ȧ���� �㲥
bool CAttemperEngineSink::On_CMD_LC_CLUB_CHAT_BDCAST( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	WORD Size = sizeof(STR_CMD_LC_CLUB_CHAT_BDCAST);
	if( (wDataSize < Size) || (( wDataSize % Size) != 0 )) return false;
	STR_CMD_LC_CLUB_CHAT_BDCAST *pCmd = (STR_CMD_LC_CLUB_CHAT_BDCAST*) pData;

	switch(pCmd->byChatMode)
	{
	case 0: //���ֲ�����
		{
			int DataNum = wDataSize / Size;

			//��ѯ�û�
			for(int i = 0; i < DataNum; i ++)
			{
				//���������û� ��������
				for (int j = 0; j < MAX_CONTENT; j++)
				{
					tagBindParameter * pBindParameter=(m_pBindParameter+j);
					if ( pBindParameter && pBindParameter->dwUserID != 0 && 
						pBindParameter->dwSocketID != 0 && 
						(pBindParameter->dwUserID == (pCmd + i)->dwTagID))
					{
						//֪ͨ�û�
						m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_CLUB, CMD_LC_CLUB_CHAT_BDCAST, 
							pCmd+i, sizeof(STR_CMD_LC_CLUB_CHAT_BDCAST));
					}
				}
			}
			break;
		}
	case 1: //��������
		{
			m_pITCPNetworkEngine->SendDataBatch(MDM_CLUB, CMD_LC_CLUB_CHAT_BDCAST, pData, wDataSize);
			break;
		}
	case 2: //ϵͳ����
		{
			break;
		}
	case 3: //˽������
		{
			//TODONOW added by WangChengQing ��Ҫ�޸�������߼�
			//����ID�ҵ��û���
			//�����û��� �ҵ���Ӧ��socket
			//�ж�socket�Ƿ�����

			//����Ŀ���û� ��������
			for (int j = 0; j < MAX_CONTENT; j++)
			{
				tagBindParameter * pBindParameter=(m_pBindParameter+j);
				if ( pBindParameter && pBindParameter->dwUserID != 0 && 
					pBindParameter->dwSocketID != 0 && 
					(pBindParameter->dwUserID == (pCmd->dwTagID)))
				{
					//֪ͨ�û�
					m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_CLUB, CMD_LC_CLUB_CHAT_BDCAST, 
						pData, wDataSize);
				}
			}
			break;
		}
	}
	
	return true;
}

//����Ȧ�ö�
bool CAttemperEngineSink::On_SUB_CL_CLUBSTICKY_POST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_STICKY_POST)) return false;
	STR_SUB_CL_CLUB_STICKY_POST *pSub = (STR_SUB_CL_CLUB_STICKY_POST*)pData;

	//����DBR����
	STR_DBR_CL_CLUB_STICKY_POST DBR;
	ZeroMemory(&DBR, sizeof(STR_DBR_CL_CLUB_STICKY_POST));
	DBR.dwClubID = pSub->dwClubID;
	DBR.cbTopFlag = pSub->cbTopFlag;
	DBR.dwUserID = pSub->dwUserID;

	//��������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_STICKY_POST, dwSocketID, pData, wDataSize);

	return true;
}
//����Ȧ�ö� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_STICKY_POST( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//У�����
	if( wDataSize != sizeof(STR_CMD_LC_CLUB_STICKY_POST)) return false;

	//����CMD����
	STR_DBO_LC_CLUB_STICKY_POST *DBO = (STR_DBO_LC_CLUB_STICKY_POST *)pData;

	STR_CMD_LC_CLUB_STICKY_POST CMD;
	ZeroMemory(&CMD, sizeof(STR_CMD_LC_CLUB_STICKY_POST));
	CMD.lResultCode = DBO->lResultCode;

	//���ͷ���
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_STICKY_POST, &CMD, sizeof(CMD));

	return true;
}

//����Ȧ���
bool CAttemperEngineSink::On_SUB_CL_CLUB_MESSAGE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_MESSAGE)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_MESSAGE, dwSocketID, pData, wDataSize);
	return true;
}
//����Ȧ��� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_MESSAGE( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_MESSAGE);
	if( wDataSize != Size) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_MESSAGE, pData, wDataSize);
	return true;
}

//���׷���
bool CAttemperEngineSink::On_SUB_CL_CLUB_CONTRIBUTE_FK(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_CONTRIBUTE_FK)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_CONTRIBUTE_FK, dwSocketID, pData, wDataSize);
	return true;
}
//���׷��� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_CONTRIBUTE_FK( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_CONTRIBUTE_FK);
	if( wDataSize != Size) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_CONTRIBUTE_FK, pData, wDataSize);
	return true;
}

//����Ȧ����
bool CAttemperEngineSink::On_SUB_CL_CLUB_AUTO_AGREE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_AUTO_AGREE)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_AUTO_AGREE, dwSocketID, pData, wDataSize);
	return true;
}
//����Ȧ���� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_AUTO_AGREE( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_AUTO_AGREE);
	if( wDataSize != Size) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_AUTO_AGREE, pData, wDataSize);
	return true;
}

//������뷿��
bool CAttemperEngineSink::On_SUB_CL_CLUB_JOIN_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_JOIN_ROOM)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_JOIN_ROOM, dwSocketID, pData, wDataSize);
	return true;
}
//������뷿�� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_JOIN_ROOM( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_JOIN_ROOM);
	if( wDataSize != Size) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_JOIN_ROOM, pData, wDataSize);
	return true;
}
//�����б�
bool CAttemperEngineSink::On_CMD_LC_CLUB_TABLE_LIST_TABLE( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_TABLE_LIST);
	if( (wDataSize % Size) != 0) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_TABLE_LIST_TABLE, pData, wDataSize);
	return true;
}
//��������б�
bool CAttemperEngineSink::On_CMD_LC_CLUB_TABLE_LIST_USER( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST);
	if( (wDataSize % Size) != 0) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_TABLE_LIST_USER, pData, wDataSize);
	return true;
}



//Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�)
bool CAttemperEngineSink::On_SUB_CL_CLUB_APPLICANT_RESULT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_APPLICANT_RESULT)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_APPLICANT_RESULT, dwSocketID, pData, wDataSize);
	return true;
}
//Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�) ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_APPLICANT_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_APPLICANT_RESULT);
	if( wDataSize != Size) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_APPLICANT_RESULT, pData, wDataSize);
	return true;
}

//�����Ա����
bool CAttemperEngineSink::On_SUB_CL_CLUB_MEMBER_MANAGER(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_MEMBER_MANAGER)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_MEMBER_MANAGER, dwSocketID, pData, wDataSize);
	return true;
}
//�����Ա���� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_MEMBER_MANAGER( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_MEMBER_MANAGER);
	if( (wDataSize % Size) != 0) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_MEMBER_MANAGER, pData, wDataSize);
	return true;
}
//�����Ա���� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_MEMBER_MANAGER_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_MEMBER_MANAGER_FINISH cmd;
	cmd.byMask = 1;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_MEMBER_MANAGER_FINISH, &cmd, sizeof(STR_CMD_LC_SHOP_QUERY_FINISH));
	return true;
}
//���������Ϣ
bool CAttemperEngineSink::On_CMD_LC_CLUB_DATA( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_DATA);
	if( wDataSize != Size) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_DATA, pData, wDataSize);
	return true;
}

//���������������Ȧ
bool CAttemperEngineSink::On_SUB_CL_CLUB_INVITE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_INVITE)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_INVITE, dwSocketID, pData, wDataSize);

	return true;
}
//���������������Ȧ ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_INVITE( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_INVITE);
	if( wDataSize != Size) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_INVITE, pData, wDataSize);

	return true;
}

//�������˵����� 
bool CAttemperEngineSink::On_CMD_LC_CLUB_INVITE_REMIND( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_INVITE_REMIND);
	if( wDataSize != Size) return false;
	STR_CMD_LC_CLUB_INVITE_REMIND * pCMD = (STR_CMD_LC_CLUB_INVITE_REMIND*) pData;

	//���������û� ��������
	for (int j = 0; j < MAX_CONTENT; j++)
	{
		tagBindParameter * pBindParameter=(m_pBindParameter+j);
		if ( pBindParameter && pBindParameter->dwUserID != 0 && 
			pBindParameter->dwSocketID != 0 && 
			pBindParameter->dwUserID == pCMD->dwTagID)
		{
			//֪ͨ�û�
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_CLUB, CMD_LC_CLUB_INVITE_REMIND, 
											pData, wDataSize);		
		}
	}

	return true;
}

//�������˵Ļظ�
bool CAttemperEngineSink::On_SUB_CL_CLUB_INVITE_RESULT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_INVITE_RESULT)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_INVITE_RESULT, dwSocketID, pData, wDataSize);

	return true;
}
//�������˵Ļظ� ���� 
bool CAttemperEngineSink::On_CMD_LC_CLUB_INVITE_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_INVITE_RESULT);
	if( wDataSize != Size) return false;

	STR_CMD_LC_CLUB_INVITE_RESULT *pCMD = (STR_CMD_LC_CLUB_INVITE_RESULT*) pData;

	//������سɹ�, ��֪ͨ�ͻ���ˢ�¹����б�
	if(pCMD->lResultCode == DB_SUCCESS)
	{
		STR_CMD_LC_CLUB_LIST_RE CMD;
		CMD.byMask = 1;

		m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_LIST_RE, &CMD, sizeof(CMD));
	}

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_INVITE_RESULT, pData, wDataSize);

	return true;
}

//�߳��˳�����
bool CAttemperEngineSink::On_SUB_CL_CLUB_QUIT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_QUIT)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_QUIT, dwSocketID, pData, wDataSize);
	return true;
}
//�û��˳����� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_QUIT( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_QUIT);
	if( wDataSize != Size) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_QUIT, pData, wDataSize);
	return true;
}

//ְ������
bool CAttemperEngineSink::On_SUB_CL_CLUB_APPOINTMENT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_APPOINTMENT)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_APPOINTMENT, dwSocketID, pData, wDataSize);
	return true;
}
//ְ������ ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_APPOINTMENT( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_APPOINTMENT);
	if( wDataSize != Size) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_APPOINTMENT, pData, wDataSize);
	return true;
}
//ְ������ ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_APPOINTMENT_NOTE( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
		//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_APPOINTMENT_NOTE);
	if( wDataSize != Size) return false;
	STR_CMD_LC_CLUB_APPOINTMENT_NOTE * pCMD = (STR_CMD_LC_CLUB_APPOINTMENT_NOTE*) pData;

	//���������û� ��������
	for (int j = 0; j < MAX_CONTENT; j++)
	{
		tagBindParameter * pBindParameter=(m_pBindParameter+j);
		if ( pBindParameter && pBindParameter->dwUserID != 0 && 
			pBindParameter->dwSocketID != 0 && 
			pBindParameter->dwUserID == pCMD->dwUserID)
		{
			//֪ͨ�û�
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_CLUB, CMD_LC_CLUB_APPOINTMENT_NOTE, 
											pData, wDataSize);		
		}
	}

	return true;
}


//�������б�
bool CAttemperEngineSink::On_SUB_CL_CLUB_APPLICANT_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_APPLICANT_LIST)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_APPLICANT_LIST, dwSocketID, pData, wDataSize);
	return true;
}
//�������б� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_APPLICANT_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_APPLICANT_LIST);
	if( (wDataSize % Size) != 0) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_APPLICANT_LIST, pData, wDataSize);
	return true;
}
//�������б� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_APPLICANT_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_APPLICANT_LIST_FINISH cmd;
	cmd.byMask = 1;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_APPLICANT_LIST_FINISH, &cmd, sizeof(cmd));
	return true;
}

//�������˲鿴�Լ��������б�
bool CAttemperEngineSink::On_SUB_CL_CLUB_INQUERY_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_INQUERY_LIST)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_INQUERY_LIST, dwSocketID, pData, wDataSize);
	return true;
}
//�������˲鿴�Լ��������б� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_INQUERY_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_INQUERY_LIST);
	if( (wDataSize % Size) != 0) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_INQUERY_LIST, pData, wDataSize);
	return true;
}
//�������˲鿴�Լ��������б� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_INQUERY_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_INQUERY_LIST_FINISH cmd;
	cmd.byMask = 1;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_INQUERY_LIST_FINISH, &cmd, sizeof(cmd));
	return true;
}


//����ս��ͳ��
bool CAttemperEngineSink::On_SUB_CL_CLUB_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_RECORD_LIST)) return false;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_CLUB_RECORD_LIST, dwSocketID, pData, wDataSize);
	return true;
}
//����ս��ͳ�� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_RECORD_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_RECORD_LIST);
	if( (wDataSize < Size) || 
		( (wDataSize % Size) != 0))
		return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_RECORD_LIST, pData, wDataSize);
	return true;
}
//����ս��ͳ�� ����
bool CAttemperEngineSink::On_CMD_LC_CLUB_RECORD_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_RECORD_FINISH cmd;
	cmd.byMask = 1;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CLUB, CMD_LC_CLUB_RECORD_FINISH, &cmd, sizeof(cmd));
	return true;
}

#pragma endregion

#pragma region MDM_SHOP �̳ǵ���
//��ѯ�̳�
bool CAttemperEngineSink::On_SUB_CL_SHOP_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_SHOP_QUERY)) return false;

	//������Ϣ
	STR_SUB_CL_SHOP_QUERY * pSub = (STR_SUB_CL_SHOP_QUERY *)pData;
	//�������
	STR_DBR_CL_SHOP_QUERY Dbr;
	ZeroMemory(&Dbr,sizeof(Dbr));

	Dbr.byGoodsType = pSub->byGoodsType;
	Dbr.dwUserID = pSub->dwUserID;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SHOP_QUERY, dwSocketID, &Dbr, sizeof(Dbr));
	return true;
}
//��ѯ�̳�CMD
bool CAttemperEngineSink::On_CMD_LC_SHOP_QUERY_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_SHOP_QUERY_RESULT);
	if( (wDataSize%Size) != 0) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SHOP, CMD_LC_SHOP_QUERY_RESULT, pData, wDataSize);
	return true;
}
//��ѯ�̳ǽ���CMD
bool CAttemperEngineSink::On_CMD_LC_SHOP_QUERY_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_SHOP_QUERY_FINISH cmd;
	cmd.byMask = 1;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SHOP, CMD_LC_SHOP_QUERY_FINISH, &cmd, sizeof(STR_CMD_LC_SHOP_QUERY_FINISH));
	return true;
}

//��ʯ�������
bool CAttemperEngineSink::On_SUB_CL_SHOP_DIAMOND(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_SHOP_DIAMOND)) return false;

	//������Ϣ
	STR_SUB_CL_SHOP_DIAMOND * pSub = (STR_SUB_CL_SHOP_DIAMOND *)pData;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SHOP_DIAMOND, dwSocketID, pData, wDataSize);
	return true;
}

//��ʯ�������CMD
bool CAttemperEngineSink::On_CMD_LC_SHOP_DIAMOND_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	ASSERT( wDataSize == sizeof(STR_DBO_CL_SHOP_DIAMOND));
	if(wDataSize != sizeof(STR_DBO_CL_SHOP_DIAMOND)) return false;

	STR_DBO_CL_SHOP_DIAMOND * pDBO = (STR_DBO_CL_SHOP_DIAMOND*)pData;
	STR_CMD_LC_SHOP_DIAMOND_RESULT CMD;
	CMD.lResultCode = pDBO->lResultCode;
	lstrcpyn(CMD.szDescribe,pDBO->szDescribeString,CountArray(CMD.szDescribe));
	
	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SHOP, CMD_LC_SHOP_DIAMOND_RESULT, &CMD, sizeof(CMD));
	return true;
}

//������Ʒ��ѯ
bool CAttemperEngineSink::On_SUB_CL_BAG_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	ASSERT( wDataSize == sizeof(STR_SUB_CL_BAG_QUERY));
	if(wDataSize != sizeof(STR_SUB_CL_BAG_QUERY)) return false;

	//������Ϣ
	STR_SUB_CL_BAG_QUERY * pSub = (STR_SUB_CL_BAG_QUERY *)pData;
	//�������
	STR_DBR_CL_BAG_QUERY Dbr;
	ZeroMemory(&Dbr,sizeof(Dbr));

	Dbr.dwUserID = pSub->dwUserID;	

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_BAG_QUERY, dwSocketID, &Dbr, sizeof(Dbr));
	return true;
}

//������Ʒ��ѯCMD
bool CAttemperEngineSink::On_CMD_LC_BAG_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_DBO_CL_BAG_QUERY);
	if( (wDataSize%Size) != 0) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SHOP, CMD_LC_BAG_RESULT, pData, wDataSize);
	return true;
}

//������Ʒ��ѯ����CMD
bool CAttemperEngineSink::On_CMD_LC_BAG_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_BAG_FINISH);

	if( wDataSize != Size) return false;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_SHOP, CMD_LC_BAG_FINISH, pData, wDataSize);
	return true;
}
#pragma endregion







