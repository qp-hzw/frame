#include "Stdafx.h"
#include "TableFrame.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"
#include <iostream>
#include <vector>
#include "GameCtrl.h"

//////////////////////////////////////////////////////////////////////////////////
//校验GPS距离
#define CHECK_USER_GPS_DISTANCE		200									//玩家GPS在多少距离内提示玩家

#define IDI_GAME_CHECK				(TIME_TABLE_SINK_RANGE+2)			//超时标识
#define TIME_GAME_CHECK				3600000								//超时时间

//解散等待
#define IDI_ROOM_AUTO_DISMISS		(TIME_TABLE_SINK_RANGE+3)			//房间自动解散定时器
#define TIME_ROOM_AUTO_DISMISS		300000L								//房间自动解散定时器时间

//续费等待
#define IDI_WAIT_CONTINUE			(TIME_TABLE_SINK_RANGE+4)			//等待续费标识
#define TIME_WAIT_CONTINUE			20000L								//等待续费时间

//红包事件
#define IDI_WAIT_REDPACKET			(TIME_TABLE_SINK_RANGE+5)			//等待发红包标识
#define TIME_WAIT_REDPACKET			60000L								//每分钟检测一次

//表决解散房间
#define IDI_VOTE_DISMISS_ROOM		(TIME_TABLE_SINK_RANGE+6)			//表决解散房间
#define TIME_VOTE_DISMISS_ROOM		300000L								//事件 --5分钟 

//门票不足以继续游戏时候. 解散房间
#define IDI_CHECK_DISMISS_ROOM		(TIME_TABLE_SINK_RANGE+7)			//表决解散房间
#define TIME_CHECK_DISMISS_ROOM		4*1000L								//事件 --4秒钟 

/////////////////////////////////////////////////////////////////////////////////


//游戏记录
CGameScoreRecordArray				CTableFrame::m_GameScoreRecordBuffer;
//////////////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrame::CTableFrame()
{

	m_pITableFrameSink= g_GameCtrl->GetITableFrameSink();

	//固有属性
	m_wTableID=0;
	m_wChairCount=0;
	m_cbStartMode=START_MODE_ALL_READY;
	m_wUserCount=0;
	m_cbTableMode = 0;

	//标志变量
	m_bGameStarted=false;
	m_bDrawStarted=false;
	m_bTableStarted=false;
	ZeroMemory(m_bAllowLookon,sizeof(m_bAllowLookon));
	ZeroMemory(m_lFrozenedScore,sizeof(m_lFrozenedScore));

	//游戏变量
	m_cbGameStatus=GAME_STATUS_FREE;
	m_wDrawCount=0;

	m_dwGroupID = 0;
	m_dwCreateTableUser = 0;

	//时间变量
	m_dwDrawStartTime=0L;
	ZeroMemory(&m_SystemTimeStart,sizeof(m_SystemTimeStart));

	//动态属性
	m_dwTableOwner=0L;
	ZeroMemory(m_szEnterPassword,sizeof(m_szEnterPassword));

	//配置信息
	m_pGameParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//用户数组
	ZeroMemory(m_TableUserItemArray,sizeof(m_TableUserItemArray));

	//解散信息
	m_bUnderDissState = false;
	m_dissmisserChaiID = 0xFF;

	//房卡信息
	m_dwPassword = 0;
	return;
}

//析构函数
CTableFrame::~CTableFrame()
{
	//释放对象
	if ( NULL != m_pITableFrameSink )
	{
		SafeRelease(m_pITableFrameSink);
	}

	if (m_pIGameMatchSink!=NULL)
	{
		SafeDelete(m_pIGameMatchSink);
	}

	return;
}

//设置桌主，第一个坐下的玩家
void CTableFrame::SetTableOwner(DWORD dwUserID)
{
	m_dwTableOwner = dwUserID;
}

//获得桌主，第一个坐下的人
DWORD CTableFrame::GetTableOwner()
{
	return m_dwTableOwner;
}

//设置房间自动解散时间
void CTableFrame::SetTableAutoDismiss(DWORD dwMinutes) 
{ 
	SetGameTimer(IDI_ROOM_AUTO_DISMISS, dwMinutes*TIME_ROOM_AUTO_DISMISS, 1, NULL); 
}

//开始游戏
bool CTableFrame::StartGame()
{
	//游戏状态
	if (m_bDrawStarted==true) return false;

	//删除自动解散定时器
	KillGameTimer(IDI_ROOM_AUTO_DISMISS);

	//保存变量
	bool bGameStarted=m_bGameStarted;
	bool bTableStarted=m_bTableStarted;

	//设置状态
	m_bGameStarted=true;
	m_bDrawStarted=true;
	m_bTableStarted=true;

	//解散信息
	ZeroMemory(m_bResponseDismiss,sizeof(m_bResponseDismiss));
	ZeroMemory(m_bAgree,sizeof(m_bAgree));
	m_bUnderDissState = false;
	m_dissmisserChaiID = 0xFF;

	//开始时间
	GetLocalTime(&m_SystemTimeStart);
	m_dwDrawStartTime=(DWORD)time(NULL);

	//开始设置
	if (bGameStarted==false)
	{
		//设置用户
		for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
		{
			//获取用户
			CPlayer * pIServerUserItem=GetTableUserItem(i);

			//设置用户
			if (pIServerUserItem!=NULL)
			{
				//设置状态
				BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
				if ((cbUserStatus!=US_OFFLINE)&&(cbUserStatus!=US_PLAYING)) pIServerUserItem->SetUserStatus(US_PLAYING,m_wTableID,i);
			}
		}

		//发送状态
		if (bTableStarted!=m_bTableStarted) SendTableStatus();
	}

	//设置游戏状态为开始
	SetGameStatus(GAME_STATUS_PLAY);

	/* 通知数据库, 桌子开始 */
	//TODONOW added by WangChengQing 此处应该增加校验
	tagTableRule *pTableCfg = (tagTableRule*)GetCustomRule();
	BYTE byClubOrHalGold = 0; //1表示俱乐部房间(房卡或者房卡金币模式);  2表示大厅的金币模式;  其他字段无须处理
	BYTE byClubCreate = pTableCfg->byClubCreate;
	BYTE GameMode = pTableCfg->GameMode;

	if(( 1 == byClubCreate) || (2 == byClubCreate))
	{
		byClubOrHalGold = 1;
	}
	else if((0 == byClubCreate) && (2 == GameMode))
	{
		byClubOrHalGold = 2;
	}
	g_AttemperEngineSink->ClubTableStart(GetPassword(), byClubOrHalGold);

	//通知事件
	if (m_pITableFrameSink!=NULL) 
	{
		m_pITableFrameSink->OnEventGameStart();
	}

	return true;
}

//解散游戏
bool CTableFrame::DismissGame()
{
	//状态判断
	ASSERT(m_bTableStarted==true);
	if (m_bTableStarted==false) return false;

	KillGameTimer(IDI_GAME_CHECK);

	//g_AttemperEngineSink->ConcludeTable(m_wTableID);

	//结束游戏
	if ((m_bGameStarted==true)&&(m_pITableFrameSink->OnEventGameConclude(INVALID_CHAIR,NULL,GER_DISMISS)==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//设置状态
	if ((m_bGameStarted==false)&&(m_bTableStarted==true))
	{
		//设置变量
		m_bTableStarted=false;

		//发送状态
		SendTableStatus();
	}

	return false;
}

//申请解散房间
bool CTableFrame::OnEventApplyDismissRoom(WORD wChairID, bool bAgree)
{
	//用户校验
	CPlayer *pApplyUserItem = GetTableUserItem(wChairID);
	if (NULL == pApplyUserItem)
		return false;

	//变量赋值
	m_bAgree[wChairID] = bAgree;
	//设置用户已经响应
	m_bResponseDismiss[wChairID] = true;

	//房间处于非空闲状态
	if(GAME_STATUS_FREE != m_cbGameStatus)
	{
		//判断房间是否可以解散
		tagTableRule* pCfg = (tagTableRule*)GetCustomRule();
		//俱乐部模式 && 房间设置不可解散 时候才生效 
		if((0 != pCfg->byClubCreate) && (1 == pCfg -> bDissolve)) 
 		{
			STR_CMD_GR_FRMAE_ASK_DISMISS_RESULT cmdResult;
			ZeroMemory(&cmdResult, sizeof(cmdResult));
			cmdResult.lResultCode = 1;
			lstrcpyn(cmdResult.szDescribeString,
				TEXT("请联系会长解散房间!"),
				CountArray(cmdResult.szDescribeString));

			SendTableData(wChairID,CMD_GR_FRMAE_ASK_DISMISS_RESULT,&cmdResult, sizeof(cmdResult), MDM_G_FRAME);
			return true;
		}


		//设置自动响应定时器
		SetGameTimer(IDI_VOTE_DISMISS_ROOM, TIME_VOTE_DISMISS_ROOM, 1, NULL);

		//玩家点击解散房间，给其他玩家发送【同意/拒绝】弹框消息
		STR_CMD_GR_FRMAE_VOTE_DISMISS VoteDismiss;
		ZeroMemory(&VoteDismiss, sizeof(STR_CMD_GR_FRMAE_VOTE_DISMISS));

		//赋值
		VoteDismiss.dwVoteUser[0] = pApplyUserItem->GetUserID();
		int nVoteNum = 1;
		for(int i=0; i<MAX_CHAIR && nVoteNum < 5; i++)
		{
			CPlayer *pTableUserItem = GetTableUserItem(i);
			if(pTableUserItem == NULL || pTableUserItem == pApplyUserItem) continue;
			VoteDismiss.dwVoteUser[nVoteNum] = pTableUserItem->GetUserID();
			nVoteNum++;
		}

		lstrcpyn(VoteDismiss.szApplyUserNick, pApplyUserItem->GetUserInfo()->szNickName, CountArray(VoteDismiss.szApplyUserNick));

		//给所有人发送 房间申请解散消息
		SendTableData(INVALID_CHAIR, CMD_GR_FRMAE_VOTE_DISMISS, &VoteDismiss, sizeof(STR_CMD_GR_FRMAE_VOTE_DISMISS), MDM_G_FRAME);

		//设置房间处于解散状态
		m_bUnderDissState = true;
		m_dissmisserChaiID = wChairID;

		return true;
	}
	else		//房间处于空闲状态
	{
		//清空数据
		ZeroMemory(m_bResponseDismiss, sizeof(m_bResponseDismiss));
		ZeroMemory(m_bAgree, sizeof(m_bAgree));

		//发送解散成功消息
		STR_CMD_GR_FRAME_DISMISS_RESULT DismissResult;
		ZeroMemory(&DismissResult, sizeof(DismissResult));
		//赋值
		DismissResult.cbDismiss = 1;
		DismissResult.cbAgree = 1;
		DismissResult.dwVoteUserID = pApplyUserItem->GetUserID();
		lstrcpyn(DismissResult.szVoteUserNick, pApplyUserItem->GetUserInfo()->szNickName, CountArray(DismissResult.szVoteUserNick));

		//判断玩家是不是桌主
		if(pApplyUserItem->GetUserID() == m_dwTableOwner)
		{
			//发送消息
			SendTableData(INVALID_CHAIR, CMD_GR_FRAME_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_G_FRAME);

			//结束游戏
			ConcludeGame(GAME_CONCLUDE_NORMAL);
			return true;
		}
		else		//不是房主解散，玩家直接起立
		{
			//发送消息,代替站起，客户端直接站起，并不是解散
			SendTableData(wChairID, CMD_GR_FRAME_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_G_FRAME);

			//用户站起			
			if ( PerformStandUpAction(pApplyUserItem) ) 
			{
				return false;
			}			
		}
	}

	return false;
}

//表决是否解散房间
bool CTableFrame::OnEventVoteDismissRoom(WORD wChairID, bool bAgree)
{
	//用户校验
	CPlayer *pVoteUserItem = GetTableUserItem(wChairID);
	if (NULL == pVoteUserItem)			//TODO 为了解决用户掉线被游戏服删除，定时器到无法响应的bug，不进行用户校验
		return false;

	//变量赋值
	m_bAgree[wChairID] = bAgree;
	//设置用户已经响应
	m_bResponseDismiss[wChairID] = true;

	//房间处于非空闲状态
	if(GAME_STATUS_FREE != m_cbGameStatus)
	{
		//变量定义
		int nAgree = 0;						//同意解散玩家数
		int nResponse = 0;					//响应玩家数
		int nPlayer = GetSitUserCount();	//桌子玩家数

		//获得同意玩家数和响应玩家数
		for (int i=0; i<m_wChairCount && i<MAX_CHAIR; i++)
		{
			CPlayer *pTableUserItem = GetTableUserItem(i);
			if(pTableUserItem == NULL) continue;
			if(m_bAgree[i]) nAgree++;
			if(m_bResponseDismiss[i]) nResponse++;
		}

		//构造解散结果数据
		STR_CMD_GR_FRAME_DISMISS_RESULT DismissResult;
		ZeroMemory(&DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT));

		//赋值
		DismissResult.cbAgree = bAgree;
		if (NULL != pVoteUserItem)
		{
			DismissResult.dwVoteUserID = pVoteUserItem->GetUserID();
			lstrcpyn(DismissResult.szVoteUserNick, pVoteUserItem->GetUserInfo()->szNickName, CountArray(DismissResult.szVoteUserNick));
		}

		//所有玩家同意解散	（过半同意则解散房间）
		if(static_cast<DOUBLE>(nAgree) > static_cast<DOUBLE>(nPlayer/2))
		{
			//清空数据
			ZeroMemory(m_bResponseDismiss, sizeof(m_bResponseDismiss));
			ZeroMemory(m_bAgree, sizeof(m_bAgree));
			KillGameTimer(IDI_VOTE_DISMISS_ROOM);

			//赋值
			DismissResult.cbDismiss = 1;

			//广播房间解散成功消息			
			SendTableData(INVALID_CHAIR, CMD_GR_FRAME_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_G_FRAME);

			//设置房间不处于解散状态
			m_bUnderDissState = false;
			m_dissmisserChaiID = 0xFF;

			//游戏中，结束子游戏（大局结束）
			if (m_pITableFrameSink!=NULL) 
				m_pITableFrameSink->OnEventGameConclude(INVALID_CHAIR, NULL, GER_DISMISS);

			return true;
		}		
		else if(nResponse == nPlayer)		//所有玩家响应，但有玩家未同意
		{
			//清空数据
			ZeroMemory(m_bResponseDismiss, sizeof(m_bResponseDismiss));
			ZeroMemory(m_bAgree, sizeof(m_bAgree));
			KillGameTimer(IDI_VOTE_DISMISS_ROOM);

			//赋值
			DismissResult.cbDismiss = 2;

			//广播房间解散失败消息			
			SendTableData(INVALID_CHAIR, CMD_GR_FRAME_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_G_FRAME);

			//设置房间不处于解散状态
			m_bUnderDissState = false;
			m_dissmisserChaiID = 0xFF;
		}
		else					//有玩家还未表决，中间状态
		{
			//赋值
			DismissResult.cbDismiss = 0;

			//广播玩家表决消息			
			SendTableData(INVALID_CHAIR, CMD_GR_FRAME_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_G_FRAME);
		}
	}

	return false;
}

//结束游戏
bool CTableFrame::ConcludeGame(BYTE cbGameStatus)
{
	//设置房间不处于解散状态
	m_bUnderDissState = false;
	m_dissmisserChaiID = 0xFF;

	//结束等待续费，玩家需要从准备状态退出为坐下
	if (cbGameStatus == GAME_CONCLUDE_CONTINUE)
	{
		m_bGameStarted=false;

		m_bDrawStarted=false;

		m_cbGameStatus=GAME_STATUS_FREE;

		m_bTableStarted=false;

		ZeroMemory(m_bAgree,sizeof(m_bAgree));

		if (m_pITableFrameSink!=NULL) m_pITableFrameSink->RepositionSink();

		//状态切换为坐下，可以自由离开
		for (WORD wChairID = 0;wChairID<m_wChairCount;wChairID++)
		{
			//获取用户
			CPlayer * pIServerUserItem=GetTableUserItem(wChairID);
			//用户处理
			if (pIServerUserItem!=NULL)
			{
				//设置状态
				//pIServerUserItem->SetUserStatus(US_NULL,m_wTableID,wChairID);	
				//设置状态
				pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,wChairID,false);
			}
		}

		//设置续费定时器
		SetGameTimer(IDI_WAIT_CONTINUE,TIME_WAIT_CONTINUE,1,NULL);

		return true;
	}

	//直接解散玩家全部站起
	if(cbGameStatus == GAME_CONCLUDE_NORMAL)
	{
		for (WORD wChairID = 0;wChairID<m_wChairCount;wChairID++)
		{
			//获取用户
			CPlayer * pIServerUserItem=GetTableUserItem(wChairID);

			//用户处理
			if (pIServerUserItem!=NULL)
			{				
				//1. 若桌子用户属于断线状态，未重连
				if ( US_OFFLINE == pIServerUserItem->GetUserStatus() )
				{
					//游戏服处理  TODONOW
					pIServerUserItem->SetOfflineGameID(0);

					//发送给协调服务器, 再转发给登录服
					tagOfflineUser data;
					data.dwUserID = pIServerUserItem->GetUserID();
					data.byMask = 2; //表示删除断线用户

					//发送给协调服务器
					m_pITCPSocketEngine->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_OFFLINE,&data,sizeof(tagOfflineUser));
				}

				//2. 用户站起
				PerformStandUpAction(pIServerUserItem);			
			}
		}

		//设置桌子属性
		m_bGameStarted=false;
		m_bDrawStarted=false;
		m_cbGameStatus=GAME_STATUS_FREE;
		m_bTableStarted=false;
		m_dwGroupID = 0;
		m_dwCreateTableUser = 0;
		m_cbTableMode = 0;
		SetPassword(0);
		ZeroMemory(m_bAgree,sizeof(m_bAgree));
		if (m_pITableFrameSink!=NULL) m_pITableFrameSink->RepositionSink();

		return true;
	}

	//效验状态
	if (m_bGameStarted==false) return false;

	//保存变量
	bool bDrawStarted=m_bDrawStarted;

	//设置状态
	m_bDrawStarted=false;
	m_cbGameStatus=cbGameStatus;
	m_bGameStarted=(cbGameStatus>=GAME_STATUS_PLAY)?true:false;
	m_wDrawCount++;

	KillGameTimer(IDI_GAME_CHECK);

	//游戏记录
	RecordGameScore(bDrawStarted);

	//结束设置
	if (m_bGameStarted==false)
	{
		//变量定义
		bool bOffLineWait=false;

		//设置用户
		for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
		{
			//获取用户
			CPlayer * pIServerUserItem=GetTableUserItem(i);

			//用户处理
			if (pIServerUserItem!=NULL)
			{
				//设置状态
				if (pIServerUserItem->GetUserStatus()==US_OFFLINE)
				{
					//断线处理
					bOffLineWait=true;
					PerformStandUpAction(pIServerUserItem);
				}
				else
				{
					//设置状态
					//pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,i);

					PerformStandUpAction(pIServerUserItem);

				}
			}
		}
	}

	//重置桌子
	if (m_pITableFrameSink!=NULL) m_pITableFrameSink->RepositionSink();

	//踢出检测
	if (m_bGameStarted==false)
	{
		for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
		{
			//获取用户
			if (m_TableUserItemArray[i]==NULL) continue;
			CPlayer * pIServerUserItem=m_TableUserItemArray[i];

			//关闭判断
			if ((pIServerUserItem->GetMasterOrder()==0))
			{
				//发送消息
				LPCTSTR pszMessage=TEXT("由于系统维护，当前游戏桌子禁止用户继续游戏！");
				SendGameMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);

				//用户起立
				PerformStandUpAction(pIServerUserItem);

				continue;
			}
		}
	}

	//结束桌子
	ConcludeTable();

	//发送状态
	SendTableStatus();

	return true;
}

//结束桌子
bool CTableFrame::ConcludeTable()
{
	//结束桌子
	if ( !m_bGameStarted && m_bTableStarted )
	{
		//人数判断
		WORD wTableUserCount = GetSitUserCount();
		if (wTableUserCount==0) m_bTableStarted=false;
		if (m_pGameServiceAttrib->wChairCount==MAX_CHAIR) m_bTableStarted=false;

		//模式判断
		if (m_cbStartMode==START_MODE_FULL_READY) m_bTableStarted=false;
		if (m_cbStartMode==START_MODE_PAIR_READY) m_bTableStarted=false;
		if (m_cbStartMode==START_MODE_ALL_READY) m_bTableStarted=false;
	}

	return true;
}

//小局结束处理函数
bool CTableFrame::HandleXJGameEnd(BYTE byRound, BYTE byTableMode_NO_USER, SCORE *lUserTreasure)
{
	//TODONOW 2018/5/27 byTableMode该字段无效，不使用该字段, 下次迭代时候可以删除该字段 

	//tableMode  与  lUserTreasure 对照表
	//tableMode(桌子模式)      byRound(当前局数  )     lUserTreasure(用户财富)
	//0 房卡模式			   0					   房卡
	//0 房卡模式			   非0					   积分
	//1 竞技场模式									   待定
	//2 金币模式			   所有					   金币
	//3 房卡金币模式           0                       房卡
	//3 房卡金币模式           非0					   金币

	//房间规则
	tagTableRule* pCfg = (tagTableRule*)GetCustomRule();
	BYTE byTableMode = pCfg->GameMode;

	//更新用户财富 对应数据库 round =1 
	XJModifyUserTreasure(byTableMode, byRound, lUserTreasure, pCfg);

	//更新用户任务状态
	XJUpdateGameTaskStatus(byTableMode, byRound); //TODONOW 暂时只为房卡的

	//扣除用户门票 -- 对应数据库 round = 0
	XJGameTickets(byTableMode, byRound);

	//事件通知
	g_AttemperEngineSink->ClubTableXJ(GetPassword());

	return true;
}


//每局游戏结束后，检测并扣除用户门票  
bool CTableFrame::XJGameTickets(BYTE byTableMode, BYTE byRound)
{
	//备注1:扣除门票对照表
	//替他人开房  在创建桌子的时候就扣除了 -- 有必要这么处理???
	//自己开房 扣除对照表
	//TableMode			是否需要扣除	第几局扣除       扣除的财富类型
	//0房卡模式			√				1				 房卡
	//1金币模式			待定				
	//2房卡金币模式		√				1				 房卡

	//替他人开房，在创建桌子时已经扣除房卡，不需要再扣除
	if (0 != m_dwCreateTableUser)
		return true;

	//房间规则
	tagTableRule* pCfg = (tagTableRule*)GetCustomRule();
	
	//获取房主,房主一定在线，创建桌子用户不在桌子上，这里会出错，只能通过房主，但是扣除的时候通过USERID删除数据库数据
	CPlayer *pOwnerUserItem = SearchUserItem(m_dwTableOwner);
	if(pOwnerUserItem == NULL)
		return false;


	//俱乐部 房卡场| 房卡金币场  第一小局时候
	if((byTableMode == TABLE_MODE_FK || byTableMode == TABLE_MODE_FK_GOLD)
		&& (1 == byRound)
		&&(pCfg->byClubCreate != 0))
	{
		BYTE byPlayerNum = pCfg->PlayerCount;
		BYTE byGameCountType = pCfg->GameCountType;
		SCORE cost = byPlayerNum * byGameCountType;

		//扣除房卡
		pOwnerUserItem->ModifyUserTreasure(GetPassword(), byTableMode, 0, -cost, 0);//创建房间时候的 byRount = 0
		return true;
	}

	//非俱乐部 房卡模式、房卡金币模式
	if( (byTableMode == TABLE_MODE_FK || byTableMode == TABLE_MODE_FK_GOLD)
		&& (1 == byRound))
	{
		if( pCfg->cbPayType == 0 )							//房主支付
		{
			BYTE byPlayerNum = pCfg->PlayerCount;
			BYTE byGameCountType = pCfg->GameCountType;
			SCORE cost = byPlayerNum * byGameCountType;

			//扣除房卡
			pOwnerUserItem->ModifyUserTreasure(GetPassword(), byTableMode, 0, -cost, 0);//创建房间时候的 byRount = 0
		}
		else if(pCfg->cbPayType == 1)						//AA支付
		{
			//遍历桌子上的每个人，扣除房卡
			for (WORD i=0; i<m_wChairCount && i<MAX_CHAIR; i++)
			{
				//获取用户
				CPlayer *pIServerUserItem = GetTableUserItem(i);
				if (pIServerUserItem == NULL) continue;

				BYTE byGameCountType = pCfg->GameCountType;
				SCORE cost =  byGameCountType;

				//扣除房卡 
				pIServerUserItem->ModifyUserTreasure(GetPassword(), byTableMode, 0, -cost, 0);//创建房间时候的 byRount = 0
			} 
		}	
	}


	//非俱乐部 金币模式
	if( (byTableMode == TABLE_MODE_GOLD)
		&& (1 == byRound))
	{
		//遍历桌子上的每个人，扣除金币
		for (WORD i=0; i<m_wChairCount && i<MAX_CHAIR; i++)
		{
			//获取用户
			CPlayer *pIServerUserItem = GetTableUserItem(i);
			if (pIServerUserItem == NULL) continue;

			SCORE cost =  pCfg->lSinglePayCost;

			//扣除房卡 
			pIServerUserItem->ModifyUserTreasure(GetPassword(), byTableMode, 0, -cost, 0);//创建房间时候的 byRount = 0
		}
	}

	return true;
}

//每局游戏结束后，更新用户财富信息
bool CTableFrame::XJModifyUserTreasure(BYTE byTableMode, BYTE byRound, SCORE *lGameScore, tagTableRule *pCfg)
{
	bool bIsMoneyEmpty = false; //是否有人满足不继续游戏的条件

	//遍历桌子所有用户
	for (int i = 0; i < m_wChairCount; i++)
	{
		//获取用户
		CPlayer * pIServerUserItem = GetTableUserItem(i);

		//校验用户
		if(pIServerUserItem == NULL || pIServerUserItem->IsVirtualUser())
			continue;

		BYTE byWin = 1;	//1代表是大赢家, 0代表不是
		for(int j = 0; j < m_wChairCount; j++)
		{
			if((i != j) && (lGameScore[j] > lGameScore[i]))
			{
				byWin = 0;
				break;
			}
		}


		//更改信息
		pIServerUserItem->ModifyUserTreasure(GetPassword(), byTableMode, byRound, lGameScore[i], byWin);

		//added by WangChengQing 数据库刷新有延迟, 因此此处暂时先更新一下数据
		if(TABLE_MODE_FK_GOLD == byTableMode)
		{
			pIServerUserItem->SetUserGold(pIServerUserItem->GetUserGold() + lGameScore[i]);
		}

		//离场校验
		if (CheckUserLeave(byTableMode,  pIServerUserItem, pCfg))
		{
			bIsMoneyEmpty = true;

			/*
			//通知用户财富不足，无法继续游戏		
			STR_CMD_GR_FRAME_TREASURE_NO CMD;
			CMD.wChairID = i;
			lstrcpyn(CMD.szDescribe,TEXT("货币不足，无法继续游戏"),CountArray(CMD.szDescribe));		
			SendTableData(i, CMD_GR_FRAME_TREASURE_NO, &CMD, sizeof(CMD),MDM_G_FRAME);
			*/

			//用户起立		TODO 起立测试
			//PerformStandUpAction(pIServerUserItem);
		}
	}

	//如果有人不满足. 则发送消息
	if(bIsMoneyEmpty)
	{
		for (int i = 0; i < m_wChairCount; i++)
		{
			//获取用户
			CPlayer * pIServerUserItem = GetTableUserItem(i);

			//校验用户
			if(pIServerUserItem == NULL || pIServerUserItem->IsVirtualUser())
				continue;

			//通知在座的人 暂时注释掉
			//SendRequestFailure(pIServerUserItem, szString, REQUEST_FAILURE_NORMAL);

			//设置自动响应定时器
			SetGameTimer(IDI_CHECK_DISMISS_ROOM, TIME_CHECK_DISMISS_ROOM, 1, NULL);
		}
	}
	return true;
}
//每局游戏结束后, 检查用户财富是否可以继续游戏
bool CTableFrame::CheckUserLeave(BYTE byTableMode,CPlayer *pIServerUserItem, tagTableRule *pCfg)
{
	//备注1: true表示不能继续游戏.   false表示可以继续游戏
	//备注2: 内部使用, 不校验空指针
	//备注3: 校验对照表
	//TableMode		Check？			Treasure
	//0房卡场			×
	//1竞技场			待定
	//2金币场			√				金币
	//3房卡金币场		√				金币
	switch(byTableMode)
	{
	case TABLE_MODE_GOLD:
	case TABLE_MODE_FK_GOLD:
		{
			//这里修改为门票的 10%
			if(pIServerUserItem->GetUserGold() < (pCfg->dwLevelGold * 0.1))
			{
				return true;
			}
		}
	}
	return false;
}

//小局游戏结束后，更新游戏任务状态
bool CTableFrame::XJUpdateGameTaskStatus(const BYTE &cbTableMode, const BYTE &cbCurGameCount)
{
	//房间规则
	tagTableRule *pCfg = (tagTableRule *)GetCustomRule();	

	//校验桌子规则
	if ( pCfg->GameMode != cbTableMode )
		return false;

	//遍历桌子所有用户
	for (int i = 0; i < m_wChairCount; i++)
	{
		//获取用户
		CPlayer *pIServerUserItem = GetTableUserItem(i);

		//校验用户
		if(pIServerUserItem == NULL || pIServerUserItem->IsVirtualUser())
			continue;

		//构造DBR数据
		STR_DBR_CG_TABLE_UPDATE_TASK_STATUS TaskStatus;
		ZeroMemory(&TaskStatus, sizeof(STR_DBR_CG_TABLE_UPDATE_TASK_STATUS));

		//赋值
		TaskStatus.dwUserID = pIServerUserItem->GetUserID();
		TaskStatus.cbTableMode = cbTableMode;
		TaskStatus.cbCurGameCount = cbCurGameCount;

		//发送数据库请求,无法获得用户的socketID，发0
		g_GameCtrl->PostDataBaseRequest(DBR_SC_TABLE_UPDATE_TASK_STATUS, 0, &TaskStatus, sizeof(STR_DBR_CG_TABLE_UPDATE_TASK_STATUS));

	}

	return true;
}

//写入录像记录 参数 小局数,数据和长度
bool CTableFrame::WriteRecordInfo(WORD wXJCount,TCHAR strScore[], VOID* pData, DWORD dwDataSize)
{
	//添加保存信息:房间号 游戏模式 局数
	DBR_GR_GameRecordInfo GameRecordInfo;
	ZeroMemory(&GameRecordInfo,sizeof(DBR_GR_GameRecordInfo));

	//赋值
	GameRecordInfo.dwTableID = GetPassword();	
	GameRecordInfo.wCurrentCount = wXJCount;

	if(dwDataSize>LEN_MAX_RECORD_SIZE)
	{
		dwDataSize = LEN_MAX_RECORD_SIZE;
	}

	memcpy_s(GameRecordInfo.szData, dwDataSize*sizeof(BYTE), pData, dwDataSize*sizeof(BYTE));

	//写入数据库
	g_GameCtrl->PostDataBaseRequest(DBR_GR_SAVE_RECORDINFO, 0, &GameRecordInfo, sizeof(DBR_GR_GameRecordInfo));

	return true;
}

//消费限额
SCORE CTableFrame::QueryConsumeQuota(CPlayer * pIServerUserItem)
{
	//用户效验
	ASSERT(pIServerUserItem->GetTableID()==m_wTableID);
	if (pIServerUserItem->GetTableID()!=m_wTableID) return 0L;

	//查询额度
	SCORE lTrusteeScore=pIServerUserItem->GetTrusteeScore();
	SCORE lUserConsumeQuota=m_pITableFrameSink->QueryConsumeQuota(pIServerUserItem);

	//效验额度
	ASSERT((lUserConsumeQuota>=0L)&&(lUserConsumeQuota<=pIServerUserItem->GetUserScore()-lMinEnterScore));
	if ((lUserConsumeQuota<0L)) return 0L;

	return lUserConsumeQuota+lTrusteeScore;
}

//寻找用户
CPlayer * CTableFrame::SearchUserItem(DWORD dwUserID)
{
	//变量定义
	WORD wEnumIndex=0;
	CPlayer * pIServerUserItem=NULL;

	//桌子用户
	for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
	{
		pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserID()==dwUserID)) return pIServerUserItem;
	}

	//旁观用户
	do
	{
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserID()==dwUserID)) return pIServerUserItem;
	} while (pIServerUserItem!=NULL);

	return NULL;
}

//游戏用户
CPlayer * CTableFrame::GetTableUserItem(WORD wChairID)
{
	//效验参数
	ASSERT(wChairID<m_wChairCount);
	if (wChairID>=m_wChairCount) return NULL;

	//获取用户
	return m_TableUserItemArray[wChairID];
}

//旁观用户
CPlayer * CTableFrame::EnumLookonUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_LookonUserItemArray.GetCount()) return NULL;
	return m_LookonUserItemArray[wEnumIndex];
}

//设置时间
bool CTableFrame::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	//效验参数
	ASSERT((dwTimerID>0)&&(dwTimerID<TIME_TABLE_MODULE_RANGE));
	if ((dwTimerID<=0)||(dwTimerID>=TIME_TABLE_MODULE_RANGE)) return false;

	//设置时间
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START+m_wTableID*TIME_TABLE_MODULE_RANGE;
	if (m_pITimerEngine!=NULL) m_pITimerEngine->SetTimer(dwEngineTimerID+dwTimerID,dwElapse,dwRepeat,dwBindParameter);

	return true;
}

//删除时间
bool CTableFrame::KillGameTimer(DWORD dwTimerID)
{
	//效验参数
	ASSERT((dwTimerID>0)&&(dwTimerID<=TIME_TABLE_MODULE_RANGE));
	if ((dwTimerID<=0)||(dwTimerID>TIME_TABLE_MODULE_RANGE)) return false;

	//删除时间
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START+m_wTableID*TIME_TABLE_MODULE_RANGE;
	if (m_pITimerEngine!=NULL) m_pITimerEngine->KillTimer(dwEngineTimerID+dwTimerID);

	return true;
}

//获取定时器剩余毫秒数
DWORD CTableFrame::GetTimerLeftTickCount(DWORD dwTimerID)
{
	//效验参数
	ASSERT((dwTimerID>0)&&(dwTimerID<=TIME_TABLE_MODULE_RANGE));
	if ((dwTimerID<=0)||(dwTimerID>TIME_TABLE_MODULE_RANGE)) return false;

	//删除时间
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START+m_wTableID*TIME_TABLE_MODULE_RANGE;
	if (m_pITimerEngine!=NULL) return m_pITimerEngine->GetTimerLeftTickCount(dwEngineTimerID+dwTimerID);

	return 0;

}

//发送数据
bool CTableFrame::SendUserItemData(CPlayer * pIServerUserItem, WORD wSubCmdID)
{
	//状态效验
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//发送数据
	g_GameCtrl->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

	return true;
}

//发送数据
bool CTableFrame::SendUserItemData(CPlayer * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//状态效验
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//发送数据
	g_GameCtrl->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);

	return true;
}

//发送数据
bool CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID)
{
	//用户群发
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
		{
			//获取用户
			CPlayer * pIServerUserItem=GetTableUserItem(i);
			if (pIServerUserItem==NULL) continue;

			//效验状态
			//ASSERT(pIServerUserItem->IsClientReady()==true);
			if (pIServerUserItem->IsClientReady()==false) continue;

			//发送数据
			g_GameCtrl->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);
		}

		return true;
	}
	else
	{
		//获取用户
		CPlayer * pIServerUserItem=GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) return false;

		//效验状态
		ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//发送数据
		g_GameCtrl->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

		return true;
	}

	return false;
}

//发送数据
bool CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID)
{
	//用户群发
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
		{
			//获取用户
			CPlayer * pIServerUserItem=GetTableUserItem(i);
			if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

			//防止不在同一桌子里面  但是仍能收到消息号
			if(pIServerUserItem->GetTableID() != m_wTableID) continue;

			//发送数据
			g_GameCtrl->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
		}

		return true;
	}
	else
	{
		//获取用户
		CPlayer * pIServerUserItem=GetTableUserItem(wChairID);

		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) return false;
		//发送数据
		g_GameCtrl->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);

		return true;
	}

	return false;
}

//发送数据
bool CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID)
{
	//变量定义
	WORD wEnumIndex=0;
	CPlayer * pIServerUserItem=NULL;

	//枚举用户
	do
	{
		//获取用户
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if(pIServerUserItem == NULL)
		{
			break;
		}

		if(wChairID == INVALID_CHAIR && pIServerUserItem->IsClientReady()==false)
		{
			continue;
		}
		else
		{
			//效验状态
			ASSERT(pIServerUserItem->IsClientReady()==true);
			if (pIServerUserItem->IsClientReady()==false) return false;
		}

		//发送数据
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			g_GameCtrl->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);
		}

	} while (true);

	return true;
}

//发送数据
bool CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//变量定义
	WORD wEnumIndex=0;
	CPlayer * pIServerUserItem=NULL;

	//枚举用户
	do
	{
		//获取用户
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if(pIServerUserItem == NULL)
		{
			break;
		}

		if(wChairID == INVALID_CHAIR && pIServerUserItem->IsClientReady()==false)
		{
			continue;
		}
		else
		{
			//效验状态
			ASSERT(pIServerUserItem->IsClientReady()==true);
			if (pIServerUserItem->IsClientReady()==false) return false;
		}

		//发送数据
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			g_GameCtrl->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);
		}

	} while (true);

	return true;
}

//发送消息
bool CTableFrame::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
	WORD wEnumIndex=0;

	//发送消息
	for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
	{
		//获取用户
		CPlayer * pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

		//发送消息
		g_GameCtrl->SendGameMessage(pIServerUserItem,lpszMessage,wType);
	}

	//枚举用户
	do
	{
		//获取用户
		CPlayer * pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//效验状态
		ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//发送消息
		g_GameCtrl->SendGameMessage(pIServerUserItem,lpszMessage,wType);

	} while (true);

	return true;
}

//房间消息
bool CTableFrame::SendRoomMessage(CPlayer * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;
	if(pIServerUserItem->IsVirtualUser())
		return true;
	//发送消息
	g_GameCtrl->SendRoomMessage(pIServerUserItem,lpszMessage,wType);

	return true;
}

//游戏消息
bool CTableFrame::SendGameMessage(CPlayer * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;
	if(pIServerUserItem->IsVirtualUser())
		return true;
	//发送消息
	return g_GameCtrl->SendGameMessage(pIServerUserItem,lpszMessage,wType);
}


//发送场景
bool CTableFrame::SendGameScene(CPlayer * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	//用户效验
	if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) return false;
	if(pIServerUserItem->IsVirtualUser())
	{
		return true;
	}

	g_GameCtrl->SendData(pIServerUserItem,MDM_G_FRAME,CMD_GR_FRAME_GAME_OPTION,pData,wDataSize);

	return true;
}

//断线事件
bool CTableFrame::OnEventUserOffLine(CPlayer * pIServerUserItem)
{
	//参数效验
	if (pIServerUserItem==NULL) return false;

	//用户变量
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	CPlayer * pITableUserItem=m_TableUserItemArray[pUserInfo->wChairID];

	//用户属性
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

	//断线处理		不为空 并且 不为null  就进行断线重连		
	if ( (cbUserStatus == US_PLAYING)  || (cbUserStatus == US_SIT) || (cbUserStatus == US_READY))
	{
		//校验用户
		if (pIServerUserItem!=GetTableUserItem(wChairID)) return false;

		//用户设置
		pIServerUserItem->SetClientReady(false);
		pIServerUserItem->SetOldGameStatus(cbUserStatus);
		pIServerUserItem->SetUserStatus(US_OFFLINE,m_wTableID,wChairID);

		if (m_pITableFrameSink!=NULL)
		{
			m_pITableFrameSink->OnActionUserNetCut(wChairID,pIServerUserItem,false);
		}
		else
		{
			m_pITableFrameSink->OnActionUserNetCutT(wChairID,pIServerUserItem,false);
		}

		return true;

	}
	else	//非断线处理 
	{
		//用户起立  为了避免：若用户进入桌子，但游戏未开始就掉线，会导致无法重新加入房间，并且创建房间之后定时器到被踢出，暂时直接踢出
		PerformStandUpAction(pIServerUserItem);

		return false;
	}

	return false;
}

//积分事件
bool CTableFrame::OnUserScroeNotify(WORD wChairID, CPlayer * pIServerUserItem, BYTE cbReason)
{
	//通知游戏
	return m_pITableFrameSink->OnUserScroeNotify(wChairID,pIServerUserItem,cbReason);
}

//时间事件
bool CTableFrame::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//回调事件
	if ((dwTimerID>=0)&&(dwTimerID<TIME_TABLE_SINK_RANGE))
	{
		ASSERT(m_pITableFrameSink!=NULL);
		return m_pITableFrameSink->OnTimerMessage(dwTimerID,dwBindParameter);
	}

	//事件处理
	switch (dwTimerID)
	{
	case IDI_GAME_CHECK:
		{
			//效验状态
			ASSERT(m_bGameStarted==true);
			if (m_bGameStarted==false) return false;

			DismissGame();

			return true;
		}
	case IDI_ROOM_AUTO_DISMISS:			//房间自动解散定时器
		{
			KillGameTimer(IDI_ROOM_AUTO_DISMISS);
			//获得房主
			CPlayer *pIServerUserItem = SearchUserItem(m_dwTableOwner);
			if ( NULL != pIServerUserItem)
			{
				//解散房间
				OnEventApplyDismissRoom(pIServerUserItem->GetChairID(), true);
			}

			return true;
		}
	case IDI_VOTE_DISMISS_ROOM:		//表决解散房间定时器
		{
			KillGameTimer(IDI_VOTE_DISMISS_ROOM);
			//默认同意解散
			for (int i=0; i<m_wChairCount && i<MAX_CHAIR; i++)
			{
				CPlayer *pTableUserItem = GetTableUserItem(i);
				if(pTableUserItem == NULL) continue;
				if(m_bResponseDismiss[i]) continue;
				OnEventVoteDismissRoom(i,true);
			}
			return true;
		}
	case IDI_CHECK_DISMISS_ROOM:	//门票不足以继续游戏时候， 解散桌子
		{
			KillGameTimer(IDI_CHECK_DISMISS_ROOM);

			#pragma region 解散房间
			//设置房间处于解散状态
			SetDismissState(true);

			//通知客户端弹出大局结算面板
			STR_CMD_GR_FRAME_DISMISS_RESULT DismissResult;
			ZeroMemory(&DismissResult, sizeof(DismissResult));
			DismissResult.cbDismiss = 1;
			DismissResult.cbAgree = 1;					
			if (GetGameStatus() != GAME_STATUS_FREE)
			{
				DismissResult.cbClubQuit = 1;
			}

			SendTableData(INVALID_CHAIR, CMD_GR_FRAME_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_G_FRAME);

			//通知子游戏解散房间
			//空闲状态 直接解散
			if(GAME_STATUS_FREE == GetGameStatus())
			{
				ConcludeGame(GAME_CONCLUDE_NORMAL);
			}
			//游戏状态则 通知子游戏
			else
			{
				OnEventClubDismissRoom();
			}
			return true;
#pragma endregion
		}
	case IDI_WAIT_CONTINUE:
		{
			KillGameTimer(IDI_WAIT_CONTINUE);

			ConcludeGame(0xFE);

			return true;
		}
	case IDI_WAIT_REDPACKET:
		{
			SYSTEMTIME sysTime; 
			GetSystemTime(&sysTime);

			if((sysTime.wHour == 8 || sysTime.wHour == 14 || sysTime.wHour == 20)
				&& sysTime.wMinute == 0)
			{
				//向所有桌子上用户发送抽红包通知
				SendTableData(INVALID_CHAIR,SUB_GR_LOTTERY_NOTIFY,NULL,0,MDM_G_FRAME);
			}
			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//游戏事件
bool CTableFrame::OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, CPlayer * pIServerUserItem)
{
	//效验参数
	bool ret = m_pITableFrameSink->OnGameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem);
	if(!ret)
	{
	}

	//消息处理
	return ret;
}

//框架事件
bool CTableFrame::OnEventSocketFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, CPlayer * pIServerUserItem)
{
	//游戏处理 此处处理的应该是只有200的主消息号 TODONOW 细看
	if (m_pITableFrameSink->OnFrameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem)==true) return true;

	//默认处理
	switch (wSubCmdID)
	{
	case SUB_RG_FRAME_OPTION:	//游戏配置
		{
			//效验参数
			if (wDataSize!=sizeof(CMD_GF_GameOption)) return false;

			//变量定义
			CMD_GF_GameOption * pGameOption=(CMD_GF_GameOption *)pData;

			//获取属性
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();


			//设置状态
			pIServerUserItem->SetClientReady(true);
			if (cbUserStatus!=US_LOOKON) m_bAllowLookon[wChairID]=pGameOption->cbAllowLookon?true:false;

			//发送状态
			CMD_GF_GameStatus GameStatus;
			GameStatus.cbGameStatus=m_cbGameStatus;
			GameStatus.cbAllowLookon=m_bAllowLookon[wChairID]?TRUE:FALSE;

			g_GameCtrl->SendData(pIServerUserItem,MDM_G_FRAME,CMD_GR_FRAME_GAME_STATUS,&GameStatus,sizeof(GameStatus));

			//发送场景
			bool bSendSecret=((cbUserStatus!=US_LOOKON)||(m_bAllowLookon[wChairID]==true));
			m_pITableFrameSink->OnEventSendGameScene(wChairID,pIServerUserItem,m_cbGameStatus,bSendSecret);

			//发送解散面板状态 -- 只有处于解散状态 才会发送
			if(m_bUnderDissState)
			{
				STR_CMD_GR_FRAME_GAME_DISSMISS cmd_dismiss;
				ZeroMemory(&cmd_dismiss, sizeof(cmd_dismiss));

				BYTE num = 0;

				//发起者 放到第一位
				if (m_dissmisserChaiID != 0xFF)
				{
					CPlayer *pTableUserItem = GetTableUserItem(m_dissmisserChaiID);
					if(pTableUserItem != NULL)
					{
						cmd_dismiss.dwUserID[0] = pTableUserItem->GetUserID();

						if(!m_bResponseDismiss[m_dissmisserChaiID]) //未表决
						{
							cmd_dismiss.cbAgree[0] = 2;
						}
						else if(m_bAgree[m_dissmisserChaiID]) //同意
						{
							cmd_dismiss.cbAgree[0] = 1;
						}
						else
						{
							cmd_dismiss.cbAgree[0] = 0;
						}

						num ++;	
					}
				}

				for(int i=0; i<MAX_CHAIR && num <=(MAX_CHAIR-1); i++)
				{
					if(i == m_dissmisserChaiID) continue;

					CPlayer *pTableUserItem = GetTableUserItem(i);
					if(pTableUserItem == NULL) continue;

					cmd_dismiss.dwUserID[num] = pTableUserItem->GetUserID();
		
					if(!m_bResponseDismiss[i]) //未表决
					{
						cmd_dismiss.cbAgree[num] = 2;
					}
					else if(m_bAgree[i]) //同意
					{
						cmd_dismiss.cbAgree[num] = 1;
					}
					else
					{
						cmd_dismiss.cbAgree[num] = 0;
					}

					num++;
				}

				g_GameCtrl->SendData(pIServerUserItem,MDM_G_FRAME,CMD_GR_FRAME_GAME_DISSMISS,&cmd_dismiss,sizeof(cmd_dismiss));
			}
			return true;
		}
	case SUB_GF_USER_READY:		//用户准备
		{
			//获取属性
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//效验状态
			if (GetTableUserItem(wChairID)!=pIServerUserItem) return false;

			//效验状态
			if (cbUserStatus!=US_SIT) 
			{
				return true;
			}

			//事件通知
			if (m_pITableFrameSink!=NULL)
			{
				m_pITableFrameSink->OnActionUserOnReady(wChairID,pIServerUserItem,pData,wDataSize);
			}

			pIServerUserItem->SetUserStatus(US_READY,m_wTableID,wChairID);

			return true;
		}
	case SUB_RG_FRAME_CHAT:		//用户聊天
		{
			//效验参数
			ASSERT(wDataSize==sizeof(STR_SUB_RG_FRAME_CHAT));
			if (wDataSize != sizeof(STR_SUB_RG_FRAME_CHAT)) 
				return false;

			//变量定义
			STR_SUB_RG_FRAME_CHAT *pTableChat = (STR_SUB_RG_FRAME_CHAT *)pData;

			//获取用户
			CPlayer *pIServerUserItem = SearchUserItem(pTableChat->dwUserID);
			if ( NULL == pIServerUserItem)
				return true;

			//构造返回数据
			STR_CMD_GR_FRAME_CHAT CMDChat;
			ZeroMemory(&CMDChat, sizeof(STR_CMD_GR_FRAME_CHAT));

			//赋值
			CMDChat.dwChairID = pIServerUserItem->GetChairID();
			CMDChat.dwMsgID = pTableChat->dwMsgID;
			CMDChat.dwUserID = pTableChat->dwUserID;

			//发送数据
			SendTableData(INVALID_CHAIR, CMD_GR_FRAME_CHAT, &CMDChat, sizeof(STR_CMD_GR_FRAME_CHAT), MDM_G_FRAME);

			return true;
		}	
	case SUB_GF_LOOKON_CONFIG:		//旁观配置
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_GF_LookonConfig));
			if (wDataSize<sizeof(CMD_GF_LookonConfig)) return false;

			//变量定义
			CMD_GF_LookonConfig * pLookonConfig=(CMD_GF_LookonConfig *)pData;

			//目标用户
			if ((pLookonConfig->dwUserID!=0)&&(SearchUserItem(pLookonConfig->dwUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()!=US_LOOKON);
			if (pIServerUserItem->GetUserStatus()==US_LOOKON) return false;

			//旁观处理
			if (pLookonConfig->dwUserID!=0L)
			{
				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//获取用户
					CPlayer * pILookonUserItem=m_LookonUserItemArray[i];
					if (pILookonUserItem->GetUserID()!=pLookonConfig->dwUserID) continue;
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//构造消息
					CMD_GF_LookonStatus LookonStatus;
					LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

					//发送消息
					g_GameCtrl->SendData(pILookonUserItem,MDM_G_FRAME,CMD_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));

					break;
				}
			}
			else
			{
				//设置判断
				bool bAllowLookon=(pLookonConfig->cbAllowLookon==TRUE)?true:false;
				if (bAllowLookon==m_bAllowLookon[pIServerUserItem->GetChairID()]) return true;

				//设置变量
				m_bAllowLookon[pIServerUserItem->GetChairID()]=bAllowLookon;

				//构造消息
				CMD_GF_LookonStatus LookonStatus;
				LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

				//发送消息
				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//获取用户
					CPlayer * pILookonUserItem=m_LookonUserItemArray[i];
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//发送消息
					g_GameCtrl->SendData(pILookonUserItem,MDM_G_FRAME,CMD_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));
				}
			}

			return true;
		}
	case SUB_RG_FRAME_ASK_DISMISS:		//申请解散房间
		{
			//0. 数据包校验
			if ( wDataSize != sizeof(STR_SUB_RG_FRAME_ASK_DISMISS) )
				return false;

			STR_SUB_RG_FRAME_ASK_DISMISS *pApply = (STR_SUB_RG_FRAME_ASK_DISMISS*)pData;

			//1. 解散校验
			if (0 == pApply->cbAgree)
			{
				return true;
			}

			//2. 校验房间是否已经处于解散状态
			if (m_bUnderDissState || NULL == pIServerUserItem)
			{
				return true;
			}

			//重新点击解散按钮，需要删除定时器，防止一个人不停点击解散，定时器到自动退出房间
			KillGameTimer(IDI_VOTE_DISMISS_ROOM);

			OnEventApplyDismissRoom(pIServerUserItem->GetChairID(), (pApply->cbAgree != 0)); //0为false  其他为true

			return true;
		}
	case SUB_RG_FRAME_VOTE_DISMISS:		//表决解散房间
		{
			//数据包校验
			if ( wDataSize != sizeof(STR_SUB_RG_FRAME_VOTE_DISMISS) )
				return false;

			//变量定义
			STR_SUB_RG_FRAME_VOTE_DISMISS *pVote = (STR_SUB_RG_FRAME_VOTE_DISMISS*)pData;

			//解散成功
			OnEventVoteDismissRoom(pIServerUserItem->GetChairID(), (pVote->cbAgree != 0 ));
			return true;

		}
	case SUB_CG_COM_CHECK_USER_GPS:		//请求校验GPS
		{
			//数据包校验
			if ( wDataSize != sizeof(STR_SUB_CG_COM_CHECK_USER_GPS) )
				return false;

			//变量定义
			STR_SUB_CG_COM_CHECK_USER_GPS *pCheck = (STR_SUB_CG_COM_CHECK_USER_GPS*)pData;

			//校验
			if (1 != pCheck->cbRequestFlag)
				return true;

			//处理
			CheckUserDistance();
			return true;
		}
	case SUB_CG_COM_CHECK_USER_IP:		//请求校验IP
		{
			//数据包校验
			if ( wDataSize != sizeof(STR_SUB_CG_COM_CHECK_USER_IP) )
				return false;

			//变量定义
			STR_SUB_CG_COM_CHECK_USER_IP *pCheck = (STR_SUB_CG_COM_CHECK_USER_IP*)pData;

			//校验
			if (1 != pCheck->cbRequestFlag)
				return true;

			//处理
			CheckUserIpAddress();
			return true;
		}
	}

	return false;
}

//获取空位
WORD CTableFrame::GetNullChairID()
{
	DWORD ChairCount = MAX_CHAIR;
	if(m_wChairCount > 0)
	{
		ChairCount = m_wChairCount;
	}

	//椅子搜索
	for (WORD i=0;i<ChairCount;i++)
	{
		if (m_TableUserItemArray[i]==NULL)
		{
			return i;
		}
	}

	return INVALID_CHAIR;
}

//随机空位
WORD CTableFrame::GetRandNullChairID()
{
	//椅子搜索
	WORD wIndex = rand()%m_wChairCount;
	for (WORD i=wIndex;i<m_wChairCount && i<MAX_CHAIR+wIndex;i++)
	{
		if (m_TableUserItemArray[i%m_wChairCount]==NULL)
		{
			return i%m_wChairCount;
		}
	}

	return INVALID_CHAIR;
}

//用户数目
WORD CTableFrame::GetSitUserCount()
{
	//变量定义
	WORD wUserCount=0;

	//数目统计
	for (WORD i=0; i<m_wChairCount && i<MAX_CHAIR; i++)
	{
		if (GetTableUserItem(i)!=NULL)
		{
			wUserCount++;
		}
	}

	return wUserCount;
}

//旁观数目
WORD CTableFrame::GetLookonUserCount()
{
	//获取数目
	INT_PTR nLookonCount=m_LookonUserItemArray.GetCount();

	return (WORD)(nLookonCount);
}

//桌子状况
WORD CTableFrame::GetTableUserInfo(tagTableUserInfo & TableUserInfo)
{
	//设置变量
	ZeroMemory(&TableUserInfo,sizeof(TableUserInfo));

	//用户分析
	for (WORD i=0;i<m_pGameServiceAttrib->wChairCount;i++)
	{
		//获取用户
		CPlayer * pIServerUserItem=GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//用户类型
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			TableUserInfo.wTableUserCount++;
		}
		else
		{
			TableUserInfo.wTableAndroidCount++;
		}

		//准备判断
		if (pIServerUserItem->GetUserStatus()==US_READY)
		{
			TableUserInfo.wTableReadyCount++;
		}
	}

	//最少数目
	switch (m_cbStartMode)
	{
	case START_MODE_ALL_READY:		//所有准备
		{
			TableUserInfo.wMinUserCount=1;
			break;
		}
	case START_MODE_PAIR_READY:		//配对开始
		{
			TableUserInfo.wMinUserCount=2;
			break;
		}
	case START_MODE_TIME_CONTROL:	//时间控制
		{
			TableUserInfo.wMinUserCount=1;
			break;
		}
	default:						//默认模式
		{
			TableUserInfo.wMinUserCount=m_pGameServiceAttrib->wChairCount;
			break;
		}
	}

	return TableUserInfo.wTableAndroidCount+TableUserInfo.wTableUserCount;
}

//配置桌子
bool CTableFrame::InitializationFrame(WORD wTableID, tagTableFrameParameter & TableFrameParameter)
{
	//设置变量
	m_wTableID=wTableID;
	m_wChairCount=TableFrameParameter.pGameServiceAttrib->wChairCount;

	//配置参数
	m_pGameParameter=TableFrameParameter.pGameParameter;
	m_pGameServiceAttrib=TableFrameParameter.pGameServiceAttrib;
	m_pGameServiceOption=TableFrameParameter.pGameServiceOption;


	//错误判断
	if (m_pITableFrameSink==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置桌子
	IUnknownEx * pITableFrame=static_cast<IUnknownEx*>(this);
	if (m_pITableFrameSink->Initialization(pITableFrame)==false) return false;

	return true;
}

//起立动作
bool CTableFrame::PerformStandUpAction(CPlayer *pIServerUserItem)
{
	//效验参数
	if ( (NULL == pIServerUserItem) || 
		(pIServerUserItem->GetTableID() != m_wTableID) || 
		(pIServerUserItem->GetChairID() >= m_wChairCount) )
	{
		return false;
	}

	//用户属性
	WORD wChairID = pIServerUserItem->GetChairID();
	BYTE cbUserStatus = pIServerUserItem->GetUserStatus();
	//桌子用户
	CPlayer *pITableUserItem = GetTableUserItem(wChairID);

	//用户为桌子用户
	if (pIServerUserItem == pITableUserItem)
	{
		//TODONOW added by WangChengQing 此处应该增加校验
		tagTableRule *pTableCfg = (tagTableRule*)GetCustomRule();

		/* 通知数据库, 玩家起立 */
		BYTE byClubOrHalGold = 0; //1表示俱乐部房间(房卡或者房卡金币模式);  2表示大厅的金币模式;  其他字段无须处理
		BYTE byClubCreate = pTableCfg->byClubCreate;
		BYTE GameMode = pTableCfg->GameMode;

		if(( 1 == byClubCreate) || (2 == byClubCreate))
		{
			byClubOrHalGold = 1;
		}
		else if((0 == byClubCreate) && (2 == GameMode))
		{
			byClubOrHalGold = 2;
		}
		g_AttemperEngineSink->ClubPlayerSitUp(GetPassword(), pIServerUserItem->GetUserID(),0, byClubOrHalGold);


		/* 通知数据库, 桌子为可加入 -- 若桌子是满员状态, 有人离开了, 设置桌子为可加入 */
		BYTE byPlayerNum = pTableCfg->PlayerCount;
		if(GetSitUserCount() == byPlayerNum)
		{
			g_AttemperEngineSink->ClubTableNotMax(GetPassword(), byClubOrHalGold);
		}

		//TODO 和用户坐下的对应，这里应该只需要记录替他人开房的用户离开信息
		g_AttemperEngineSink->LeaveTable(m_wTableID,pIServerUserItem->GetUserID());

		//桌子用户设为NULL
		m_TableUserItemArray[wChairID] = NULL;

		//事件通知，通知子游戏用户站起
		if (m_pITableFrameSink!=NULL)
		{
			m_pITableFrameSink->OnActionUserStandUp(wChairID, pIServerUserItem, false);
		}
		else
		{
			m_pITableFrameSink->OnActionUserStandUpT(wChairID, pIServerUserItem, false);
		}

		//设置用户状态
		pIServerUserItem->SetClientReady(false);
		pIServerUserItem->SetUserStatus(US_FREE, INVALID_TABLE, INVALID_CHAIR);
		//pIServerUserItem->SetUserStatus((cbUserStatus==US_OFFLINE) ? US_OFFLINE:US_FREE, INVALID_TABLE, INVALID_CHAIR);

		//变量定义
		bool bTableLocked = IsTableLocked();
		bool bTableStarted = IsTableStarted();
		WORD wTableUserCount = GetSitUserCount();

		//设置桌子用户人数
		m_wUserCount = wTableUserCount;

		//若最后一个用户也离开了,结束桌子
		if (wTableUserCount==0)
		{
			g_AttemperEngineSink->ClubLastPlayerSitUp(GetPassword(), pIServerUserItem->GetUserID(),0, byClubOrHalGold);

			//设置桌子属性
			m_dwTableOwner = 0L;			
			m_bGameStarted = false;
			m_bDrawStarted = false;
			m_bTableStarted = false;
			m_cbGameStatus = GAME_STATUS_FREE;		
			m_dwGroupID = 0;
			m_dwCreateTableUser = 0;
			SetPassword(0);
			ZeroMemory(m_szEnterPassword,sizeof(m_szEnterPassword));
			ZeroMemory(m_bAgree,sizeof(m_bAgree));

			//子游戏复位
			if (m_pITableFrameSink!=NULL) 
				m_pITableFrameSink->RepositionSink();	

			//踢走旁观
			for (int i=0; i<m_LookonUserItemArray.GetCount(); i++)
			{
				SendGameMessage(m_LookonUserItemArray[i], TEXT("此房间已解散！"), SMT_CLOSE_GAME|SMT_EJECT);
			}
		}

		//结束桌子,设置桌子未未开始状态
		ConcludeTable();


		//发送状态
		if ((bTableLocked != IsTableLocked()) || (bTableStarted != IsTableStarted()))
		{
			SendTableStatus();
		}

		return true;
	}
	else		//用户不等于桌子用户
	{
		//判断用户是不是旁观用户
		for (int i=0; i<m_LookonUserItemArray.GetCount(); i++)
		{
			//旁观用户
			if (pIServerUserItem == m_LookonUserItemArray[i])
			{
				//删除子项
				m_LookonUserItemArray.RemoveAt(i);

				//事件通知
				if (m_pITableFrameSink!=NULL)
				{
					m_pITableFrameSink->OnActionUserStandUp(wChairID,pIServerUserItem,true);
				}
				else
				{
					m_pITableFrameSink->OnActionUserStandUpT(wChairID,pIServerUserItem,false);
				}

				//用户状态
				pIServerUserItem->SetClientReady(false);
				pIServerUserItem->SetUserStatus(US_FREE,INVALID_TABLE,INVALID_CHAIR);

				return true;
			}
		}
	}

	return true;
}

//旁观动作
bool CTableFrame::PerformLookonAction(WORD wChairID, CPlayer * pIServerUserItem)
{
	//效验参数
	ASSERT((pIServerUserItem!=NULL)&&(wChairID<m_wChairCount));
	ASSERT((pIServerUserItem->GetTableID()==INVALID_TABLE)&&(pIServerUserItem->GetChairID()==INVALID_CHAIR));

	//变量定义
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	tagUserRule * pUserRule=pIServerUserItem->GetUserRule();
	CPlayer * pITableUserItem=GetTableUserItem(wChairID);

	//游戏状态
	if ((m_bGameStarted==false)&&(pIServerUserItem->GetMasterOrder()==0L))
	{
		SendRequestFailure(pIServerUserItem,TEXT("游戏还没有开始，不能旁观此游戏桌！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	////旁观判断
	//if (CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule)==true
	//	&& (CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule)==false))
	//{
	//	if ((pITableUserItem!=NULL)&&(pITableUserItem->IsAndroidUser()==true))
	//	{
	//		SendRequestFailure(pIServerUserItem,TEXT("抱歉，当前游戏房间禁止用户旁观！"),REQUEST_FAILURE_NORMAL);
	//		return false;
	//	}
	//}

	//状态判断
	//if ((CServerRule::IsForfendGameLookon(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0))
	//{
	//	SendRequestFailure(pIServerUserItem,TEXT("抱歉，当前游戏房间禁止用户旁观！"),REQUEST_FAILURE_NORMAL);
	//	return false;
	//}

	//椅子判断
	if ((pITableUserItem==NULL)&&(pIServerUserItem->GetMasterOrder()==0L))
	{
		SendRequestFailure(pIServerUserItem,TEXT("您所请求的位置没有游戏玩家，无法旁观此游戏桌"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//密码效验
	if ((IsTableLocked()==true)&&(pIServerUserItem->GetMasterOrder()==0L)&&(lstrcmp(pUserRule->szPassword,m_szEnterPassword)!=0))
	{
		SendRequestFailure(pIServerUserItem,TEXT("游戏桌进入密码不正确，不能旁观游戏！"),REQUEST_FAILURE_PASSWORD);
		return false;
	}

	//扩展效验
	if (m_pITableFrameSink!=NULL)
	{
		//变量定义
		tagRequestResult RequestResult;
		ZeroMemory(&RequestResult,sizeof(RequestResult));

		//坐下效验
		if (m_pITableFrameSink->OnUserRequestLookon(wChairID,pIServerUserItem,RequestResult)==false)
		{
			//发送信息
			SendRequestFailure(pIServerUserItem,RequestResult.szFailureReason,RequestResult.cbFailureCode);

			return false;
		}
	}

	//设置用户
	m_LookonUserItemArray.Add(pIServerUserItem);

	//用户状态
	pIServerUserItem->SetClientReady(false);
	pIServerUserItem->SetUserStatus(US_LOOKON,m_wTableID,wChairID);

	//事件通知
	if (m_pITableFrameSink!=NULL)
	{
		m_pITableFrameSink->OnActionUserSitDown(wChairID,pIServerUserItem,true);
	}
	else
	{
		m_pITableFrameSink->OnActionUserSitDownT(wChairID,pIServerUserItem,true);
	}

	return true;
}

//坐下动作
bool CTableFrame::PerformSitDownAction(WORD wChairID, CPlayer * pIServerUserItem, LPCTSTR lpszPassword, bool bCheckUserGPS)
{
	/* 0. 效验参数 */
	if (NULL == pIServerUserItem)
	{
		return false;
	}
	
	if(wChairID > m_wChairCount)
	{
		return false;
	}

	//变量定义 TOODNOWW added by WangChengQing 2018/7/5 此处应该增加判断
	tagUserInfo *pUserInfo = pIServerUserItem->GetUserInfo();			//用户基本信息
	tagUserRule *pUserRule = pIServerUserItem->GetUserRule();			//TODO 这个用户规则中貌似暂时只用到桌子密码
	CPlayer *pITableUserItem = GetTableUserItem(wChairID);		//桌子用户，肯定为NULL，用来校验用户是不是已经在桌子中
	tagTableRule *pTableCfg = (tagTableRule*)GetCustomRule();


	/* 判断椅子上是否有人 */
	if ( (NULL != pITableUserItem) && 
		(pIServerUserItem->GetUserID() != pITableUserItem->GetUserID()) )
	{
		//构造信息
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe), TEXT("椅子已经被 [ %s ] 捷足先登了，下次动作要快点了！"), pITableUserItem->GetNickName());

		//发送信息
		SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 密码效验:桌子是锁定状态且不是管理员权限，并且密码错误 */
	if( ((IsTableLocked()) && (pIServerUserItem->GetMasterOrder()==0L))
		&&((lpszPassword==NULL)||(lstrcmp(lpszPassword, m_szEnterPassword)!=0)))
	{
		SendRequestFailure(pIServerUserItem, TEXT("游戏桌进入密码不正确，不能加入游戏！"), REQUEST_FAILURE_PASSWORD);
		return false;
	}

	//IP效验和用户规则校验
	//if (EfficacyIPAddress(pIServerUserItem)==false) return false;
	//if (EfficacyScoreRule(pIServerUserItem)==false) return false;
	 
	/* 扩展效验 */
	if (NULL != m_pITableFrameSink)
	{
		//变量定义
		tagRequestResult RequestResult;
		ZeroMemory(&RequestResult,sizeof(RequestResult));

		//坐下效验
		if ( !m_pITableFrameSink->OnUserRequestSitDown(wChairID, pIServerUserItem, RequestResult) )
		{
			//发送信息
			SendRequestFailure(pIServerUserItem, RequestResult.szFailureReason, RequestResult.cbFailureCode);
			return false;
		}
	}

	//用户用户赋给桌子用户
	m_TableUserItemArray[wChairID] = pIServerUserItem;
	//游戏局数
	m_wDrawCount = 0;

	//TODO 这里应该需要校验房间模式，若为他人开房才需要加入桌子的处理
	//g_AttemperEngineSink->JoinTable(m_wTableID, pIServerUserItem->GetUserID());

	/* 设置游戏用户状态 */

	//设置状态
	pIServerUserItem->SetClientReady(false);
	pIServerUserItem->SetUserStatus(US_SIT, m_wTableID, wChairID);


	//设置变量
	m_wUserCount = GetSitUserCount();

	/* 桌子人数为1，则设置该用户为桌主 */
	if ( 1 == GetSitUserCount() )
	{
		//状态变量
		bool bTableLocked = IsTableLocked();

		//设置房主
		m_dwTableOwner = pIServerUserItem->GetUserID();
		lstrcpyn(m_szEnterPassword, pUserRule->szPassword, CountArray(m_szEnterPassword));

		//设置桌子状态，群发桌子状态
		if (bTableLocked != IsTableLocked()) 
			SendTableStatus();
	}

	///* 需要校验用户GPS距离，发送太早，客户端场景还未初始化*/
	//if ( bCheckUserGPS )
	//{
	//	// 校验用户GPS距离
	//	//CheckUserDistance();

	//	//校验IP地址
	//	CheckUserIpAddress();
	//}

	/* 通知子游戏, 子游戏设置用户状态 */
	if (NULL != m_pITableFrameSink)
	{
		//用户坐下，子游戏设置用户坐下状态
		m_pITableFrameSink->OnActionUserSitDown(wChairID, pIServerUserItem, false);
	}
	else
	{
		m_pITableFrameSink->OnActionUserSitDownT(wChairID, pIServerUserItem, false);
	}	

	/* 通知数据库, 玩家坐下 */
	BYTE byClubOrHalGold = 0; //1表示俱乐部房间(房卡或者房卡金币模式);  2表示大厅的金币模式;  其他字段无须处理
	BYTE byClubCreate = pTableCfg->byClubCreate;
	BYTE GameMode = pTableCfg->GameMode;

	if(( 1 == byClubCreate) || (2 == byClubCreate))
	{
		byClubOrHalGold = 1;
	}
	else if((0 == byClubCreate) && (2 == GameMode))
	{
		byClubOrHalGold = 2;
	}

	g_AttemperEngineSink->ClubPlayerSitDown(GetPassword(), pIServerUserItem->GetUserID(),static_cast<BYTE>(wChairID), byClubOrHalGold);

	/* 通知数据库, 桌子坐满了 */
	//tagTableRule *pTableCfg = (tagTableRule*)GetCustomRule();
	if(pTableCfg != NULL)
	{
		BYTE byPlayerNum = pTableCfg->PlayerCount;
		if(GetSitUserCount() == byPlayerNum)
		{
			g_AttemperEngineSink->ClubTableMax(GetPassword(), byClubOrHalGold);
		}
	}
	
	return true;
}

//桌子状态
bool CTableFrame::SendTableStatus()
{
	//变量定义
	CMD_GR_TableStatus TableStatus;
	ZeroMemory(&TableStatus, sizeof(TableStatus));

	//构造数据
	TableStatus.wTableID = m_wTableID;
	TableStatus.TableStatus.cbTableLock = IsTableLocked()?TRUE:FALSE;
	TableStatus.TableStatus.cbPlayStatus = IsTableStarted()?TRUE:FALSE;

	//电脑数据
	g_GameCtrl->SendData(BG_COMPUTER, MDM_GR_STATUS, CMD_GR_TABLE_STATUS, &TableStatus, sizeof(TableStatus));

	//手机数据

	return true;
}

//请求失败
bool CTableFrame::SendRequestFailure(CPlayer * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode)
{
	//变量定义
	CMD_GR_RequestFailure RequestFailure;
	ZeroMemory(&RequestFailure,sizeof(RequestFailure));

	//构造数据
	RequestFailure.lErrorCode=lErrorCode;
	lstrcpyn(RequestFailure.szDescribeString,pszDescribe,CountArray(RequestFailure.szDescribeString));

	//发送数据
	WORD wDataSize=CountStringBuffer(RequestFailure.szDescribeString);
	WORD wHeadSize=sizeof(RequestFailure)-sizeof(RequestFailure.szDescribeString);
	g_GameCtrl->SendData(pIServerUserItem,MDM_USER,SUB_GR_REQUEST_FAILURE,&RequestFailure,wHeadSize+wDataSize);

	return true;
}

//开始效验
bool CTableFrame::EfficacyStartGame(WORD wReadyChairID)
{
	TCHAR szString[512]=TEXT("");

	//状态判断
	if (m_bGameStarted==true) 
	{
		return false;
	}

	//模式过滤
	if (m_cbStartMode==START_MODE_TIME_CONTROL) 
	{
		return false;
	}
	
	if (m_cbStartMode==START_MODE_MASTER_CONTROL)
	{
		return false;
	}
	
	//电玩模式，游戏创建后立即开始
	if (((tagTableRule*)GetCustomRule())->GameMode == TABLE_MODE_DW)
	{
		return true;
	}  

	//准备人数
	WORD wReadyUserCount=0;
	for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
	{
		//获取用户
		CPlayer * pITableUserItem=GetTableUserItem(i);
		if (pITableUserItem==NULL) continue;

		//用户统计
		if (pITableUserItem!=NULL)
		{
			//状态判断 -- 有人掉线则不开始游戏
			if (pITableUserItem->IsClientReady()==false)
			{
				return false;
			}

			//准备状态的玩家 数量
			if (pITableUserItem->GetUserStatus()==US_READY)
			{
				wReadyUserCount++;
			}
		}
	}

	//开始处理
	switch (m_cbStartMode)
	{
	case START_MODE_ALL_READY:			//所有准备
		{
			//数目判断
			if (wReadyUserCount>=1L)
			{
				return true;
			}

			return false;
		}
	case START_MODE_FULL_READY:			//满人开始
		{
			//人数判断
			if (wReadyUserCount==m_wChairCount)
			{
				return true;
			}

			return false;
		}
	case START_MODE_PAIR_READY:			//配对开始
		{
			//数目判断
			if (wReadyUserCount==m_wChairCount) return true;
			if ((wReadyUserCount<2L)||(wReadyUserCount%2)!=0) return false;

			//位置判断
			for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR/2;i++)
			{
				//获取用户
				CPlayer * pICurrentUserItem=GetTableUserItem(i);
				CPlayer * pITowardsUserItem=GetTableUserItem(i+m_wChairCount/2);

				//位置过滤
				if ((pICurrentUserItem==NULL)&&(pITowardsUserItem!=NULL)) return false;
				if ((pICurrentUserItem!=NULL)&&(pITowardsUserItem==NULL)) return false;
			}

			return true;
		}
	default:
		{
			return false;
		}
	}

	return false;
}

//地址效验
bool CTableFrame::EfficacyIPAddress(CPlayer * pIServerUserItem)
{
	//管理员不受限制
	if(pIServerUserItem->GetMasterOrder()!=0) return true;

	//规则判断
//	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	//地址效验
	const tagUserRule * pUserRule=pIServerUserItem->GetUserRule(),*pTableUserRule=NULL;
	bool bCheckSameIP=pUserRule->bLimitSameIP;
	for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR && i <MAX_CHAIR;i++)
	{
		//获取用户
		CPlayer * pITableUserItem=GetTableUserItem(i);
		if (pITableUserItem!=NULL && (!pITableUserItem->IsAndroidUser()) && (pITableUserItem->GetMasterOrder()==0))
		{
			pTableUserRule=pITableUserItem->GetUserRule();
			if (pTableUserRule->bLimitSameIP==true) 
			{
				bCheckSameIP=true;
				break;
			}
		}
	}

	//地址效验
	if (bCheckSameIP==true)
	{
		DWORD dwUserIP=pIServerUserItem->GetClientAddr();
		for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
		{
			//获取用户
			CPlayer * pITableUserItem=GetTableUserItem(i);
			if ((pITableUserItem!=NULL)&&(pITableUserItem != pIServerUserItem)&&(!pITableUserItem->IsAndroidUser())&&(pITableUserItem->GetMasterOrder()==0)&&(pITableUserItem->GetClientAddr()==dwUserIP))
			{
				if (!pUserRule->bLimitSameIP)
				{
					//发送信息
					LPCTSTR pszDescribe=TEXT("此游戏桌玩家设置了不跟相同 IP 地址的玩家游戏，您 IP 地址与此玩家的 IP 地址相同，不能加入游戏！");
					SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
					return false;
				}
				else
				{
					//发送信息
					LPCTSTR pszDescribe=TEXT("您设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在与您 IP 地址相同的玩家，不能加入游戏！");
					SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
					return false;
				}
			}
		}
		for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR-1;i++)
		{
			//获取用户
			CPlayer * pITableUserItem=GetTableUserItem(i);
			if (pITableUserItem!=NULL && (!pITableUserItem->IsAndroidUser()) && (pITableUserItem->GetMasterOrder()==0))
			{
				for (WORD j=i+1;j<m_wChairCount;j++)
				{
					//获取用户
					CPlayer * pITableNextUserItem=GetTableUserItem(j);
					if ((pITableNextUserItem!=NULL) && (!pITableNextUserItem->IsAndroidUser()) && (pITableNextUserItem->GetMasterOrder()==0)&&(pITableUserItem->GetClientAddr()==pITableNextUserItem->GetClientAddr()))
					{
						LPCTSTR pszDescribe=TEXT("您设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在 IP 地址相同的玩家，不能加入游戏！");
						SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
						return false;
					}
				}
			}
		}
	}
	return true;
}

//校验用户GPS距离
bool CTableFrame::CheckUserDistance()
{
	//遍历桌子上的其他用户，两两比较
	std::vector<STR_CMD_CG_COM_CHECK_USER_GPS> vecCheckGPS;

	//两两遍历
	for(int i=0; i<m_wChairCount; i++)
	{
		//获得该用户的位置信息
		CPlayer *pCheckUserItem = GetTableUserItem(i);
		if(NULL == pCheckUserItem) continue;
		double dCheckUserLong = pCheckUserItem->GetUserInfo()->dLongitude;
		double dCheckUserLat  = pCheckUserItem->GetUserInfo()->dLatitude;

		//校验位置信息
		if( (dCheckUserLong<0.01 && dCheckUserLong>-0.01) || 
			(dCheckUserLat<0.01 && dCheckUserLat>-0.01))
		{
			continue;
		}

		//计算与其他用户的距离
		for (int j = i+1; j < m_wChairCount; j++)
		{
			//校验桌子用户
			CPlayer *pTableUserItem = GetTableUserItem(j);
			if(NULL == pTableUserItem) continue;

			//获得桌子用户位置
			double dTableUserLong = pTableUserItem->GetUserInfo()->dLongitude;
			double dTableUserLat = pTableUserItem->GetUserInfo()->dLatitude;

			//校验用户位置信息
			if( (dTableUserLong<0.01 && dTableUserLong>-0.01) || 
				(dTableUserLat<0.01 && dTableUserLat>-0.01))
			{
				continue;
			}

			//比较两者距离
			double Distance = GetDistance(dCheckUserLong, dCheckUserLat, dTableUserLong, dTableUserLat);

			//发送数据至客户端
			STR_CMD_CG_COM_CHECK_USER_GPS CheckGPS;
			ZeroMemory(&CheckGPS, sizeof(STR_CMD_CG_COM_CHECK_USER_GPS));

			//赋值
			CheckGPS.fDistance = Distance;
			CheckGPS.dwFirstUserID = pCheckUserItem->GetUserID();
			CheckGPS.dwNextUserID = pTableUserItem->GetUserID();
			CheckGPS.cbIsCloser = (CHECK_USER_GPS_DISTANCE > Distance) ? 1 : 0;
			vecCheckGPS.push_back(CheckGPS);
		}
	}

	//发送数据
	for (WORD i =0; i<m_wChairCount && i<MAX_CHAIR; i++)
	{
		//获取用户
		CPlayer *pIServerUserItem = GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//发送数据
		for (DWORD j = 0; j < vecCheckGPS.size(); j++)
		{
			g_GameCtrl->SendData(pIServerUserItem, MDM_G_FRAME, CMD_GC_COM_CHECK_USER_GPS, &vecCheckGPS[j], sizeof(STR_CMD_CG_COM_CHECK_USER_GPS));
		}	
	}


	return true;
}

//根据经纬度计算距离，单位m
double CTableFrame::GetDistance(double long1, double lat1, double long2, double lat2)
{
	//变量定义
	double dEarthRadius = 6378137;
	double dPi = 3.1415926;
	double distance = 0.0;

	//转化成弧度
	lat1 = lat1 * dPi / 180.0;
	lat2 = lat2 * dPi / 180.0;

	double dLatTheta = fabs(lat1 - lat2);
	double dLongTheta = fabs((long1 - long2)*dPi / 180.0);

	double sa2 = sin(dLatTheta / 2.0);
	double sb2 = sin(dLongTheta / 2.0);

	//计算距离
	distance = fabs( 2 * dEarthRadius * asin( sqrt(sa2*sa2 + cos(lat1)*cos(lat2)*sb2*sb2) ) );

	return distance;
}

//检验用户的IP地址
bool CTableFrame::CheckUserIpAddress()
{
	//遍历桌子上的其他用户，两两比较
	std::vector<STR_CMD_CG_COM_CHECK_USER_IP> vecCheckIP;

	//遍历其他玩家
	for (WORD i=0; i<m_wChairCount && i<MAX_CHAIR; i++)
	{
		//用户IP地址
		CPlayer *pCheckUserItem = GetTableUserItem(i);
		if (NULL == pCheckUserItem) continue;
		DWORD dwUserIP = pCheckUserItem->GetClientAddr();

		//与其他用户比较IP
		for (int j = i+1; j < m_wChairCount && i<MAX_CHAIR; j++)
		{
			//获取用户IP
			CPlayer * pITableUserItem = GetTableUserItem(j);
			if ( (pITableUserItem != NULL) &&
				 (pITableUserItem != pCheckUserItem) &&
				 (!pITableUserItem->IsAndroidUser()) &&
				 (pITableUserItem->GetClientAddr()==dwUserIP) )
			{
				//发送数据至客户端
				STR_CMD_CG_COM_CHECK_USER_IP CheckIP;
				ZeroMemory(&CheckIP, sizeof(STR_CMD_CG_COM_CHECK_USER_IP));

				//赋值
				CheckIP.dwFirstUserID = pCheckUserItem->GetUserID();
				CheckIP.dwNextUserID = pITableUserItem->GetUserID();
				CheckIP.cbISame = 1;
				vecCheckIP.push_back(CheckIP);
			}
		}
	}

	//发送数据
	for (WORD k =0; k<m_wChairCount && k<MAX_CHAIR; k++)
	{
		//获取用户
		CPlayer *pIServerUserItem = GetTableUserItem(k);
		if (pIServerUserItem==NULL) continue;

		//发送数据
		for (DWORD i = 0; i < vecCheckIP.size(); i++)
		{
			g_GameCtrl->SendData(pIServerUserItem, MDM_G_FRAME, CMD_GC_COM_CHECK_USER_IP, &vecCheckIP[i], sizeof(STR_CMD_CG_COM_CHECK_USER_IP));
		}	
	}

	return true;
}

//积分效验
bool CTableFrame::EfficacyScoreRule(CPlayer * pIServerUserItem)
{
	//管理员不受限制
	if(pIServerUserItem->GetMasterOrder()!=0) return true;

	//规则判断
//	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	//变量定义
	WORD wWinRate=pIServerUserItem->GetUserWinRate();
	WORD wFleeRate=pIServerUserItem->GetUserFleeRate();

	//积分范围
	for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
	{
		//获取用户
		CPlayer * pITableUserItem=GetTableUserItem(i);

		//规则效验
		if (pITableUserItem!=NULL)
		{
			//获取规则
			tagUserRule * pTableUserRule=pITableUserItem->GetUserRule();

			//逃率效验
			if ((pTableUserRule->bLimitFleeRate)&&(wFleeRate>pTableUserRule->wMaxFleeRate))
			{
				//构造信息
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("您的逃跑率太高，与 %s 设置的设置不符，不能加入游戏！"),pITableUserItem->GetNickName());

				//发送信息
				SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

				return false;
			}

			//胜率效验
			if ((pTableUserRule->bLimitWinRate)&&(wWinRate<pTableUserRule->wMinWinRate))
			{
				//构造信息
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("您的胜率太低，与 %s 设置的设置不符，不能加入游戏！"),pITableUserItem->GetNickName());

				//发送信息
				SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

				return false;
			}

			//积分效验
			if (pTableUserRule->bLimitGameScore==true)
			{
				//最高积分
				if (pIServerUserItem->GetUserScore()>pTableUserRule->lMaxGameScore)
				{
					//构造信息
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("您的积分太高，与 %s 设置的设置不符，不能加入游戏！"),pITableUserItem->GetNickName());

					//发送信息
					SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

					return false;
				}

				//最低积分
				if (pIServerUserItem->GetUserScore()<pTableUserRule->lMinGameScore)
				{
					//构造信息
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("您的积分太低，与 %s 设置的设置不符，不能加入游戏！"),pITableUserItem->GetNickName());

					//发送信息
					SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

					return false;
				}
			}
		}
	}

	return true;
}

bool CTableFrame::SetMatchInterface(IUnknownEx * pIUnknownEx)
{
	return NULL;
}

//检查分配
bool CTableFrame::CheckDistribute()
{
//	//防作弊
//	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule))
//	{
//		//桌子状况
//		tagTableUserInfo TableUserInfo;
//		WORD wUserSitCount=GetTableUserInfo(TableUserInfo);
//
//		//用户起立
//		if(wUserSitCount < TableUserInfo.wMinUserCount)
//		{
//			return true;
//		}
//	}
//
	return false;
}

//游戏记录
void CTableFrame::RecordGameScore(bool bDrawStarted, DWORD dwStartGameTime)
{
	//因定时类游戏只调startgame（）一次导致该类游戏无法写游戏记录，因此注释
	//if (bDrawStarted==true)
	{
		//写入记录
		if (true )//CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule)==true)
		{
			//变量定义
			DBR_GR_GameScoreRecord GameScoreRecord;
			ZeroMemory(&GameScoreRecord,sizeof(GameScoreRecord));

			//设置变量
			GameScoreRecord.wTableID=m_wTableID;
			GameScoreRecord.dwPlayTimeCount=(bDrawStarted==true)?(DWORD)time(NULL)-m_dwDrawStartTime:0;

			//添加库存相关yang
			GameScoreRecord.wChangeStockScore = g_AttemperEngineSink->GetChangeStockScore();


			//游戏时间
			if(dwStartGameTime!=INVALID_DWORD)
			{
				CTime startTime(dwStartGameTime);
				startTime.GetAsSystemTime(GameScoreRecord.SystemTimeStart);
			}
			else
			{
				GameScoreRecord.SystemTimeStart=m_SystemTimeStart;
			}
			GetLocalTime(&GameScoreRecord.SystemTimeConclude);

			//用户积分
			for (INT_PTR i=0;i<m_GameScoreRecordActive.GetCount();i++)
			{
				//获取对象
				ASSERT(m_GameScoreRecordActive[i]!=NULL);
				tagGameScoreRecord * pGameScoreRecord=m_GameScoreRecordActive[i];

				//用户数目
				if (pGameScoreRecord->cbAndroid==FALSE)
				{
					GameScoreRecord.wUserCount++;
				}
				else
				{
					GameScoreRecord.wAndroidCount++;
				}

				//奖牌统计
				GameScoreRecord.dwUserMemal+=pGameScoreRecord->dwUserMemal;

				//统计信息
				if (pGameScoreRecord->cbAndroid==FALSE)
				{
					GameScoreRecord.lWasteCount-=(pGameScoreRecord->lScore+pGameScoreRecord->lRevenue);
					GameScoreRecord.lRevenueCount+=pGameScoreRecord->lRevenue;
				}

				//成绩信息
				if (GameScoreRecord.wRecordCount<CountArray(GameScoreRecord.GameScoreRecord))
				{
					WORD wIndex=GameScoreRecord.wRecordCount++;
					CopyMemory(&GameScoreRecord.GameScoreRecord[wIndex],pGameScoreRecord,sizeof(tagGameScoreRecord));
				}
			}

			//投递数据
			if(GameScoreRecord.wUserCount > 0)
			{
				WORD wHeadSize=sizeof(GameScoreRecord)-sizeof(GameScoreRecord.GameScoreRecord);
				WORD wDataSize=sizeof(GameScoreRecord.GameScoreRecord[0])*GameScoreRecord.wRecordCount;
				g_GameCtrl->PostDataBaseRequest(DBR_GR_GAME_SCORE_RECORD,0,&GameScoreRecord,wHeadSize+wDataSize);
			}
		}

		//清理记录
		if (m_GameScoreRecordActive.GetCount()>0L)
		{
			m_GameScoreRecordBuffer.Append(m_GameScoreRecordActive);
			m_GameScoreRecordActive.RemoveAll();
		}
	}

}

bool CTableFrame::SendAndroidUserData( WORD wChairID, WORD wSubCmdID )
{
	//用户群发
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
		{
			//获取用户
			CPlayer * pIServerUserItem=GetTableUserItem(i);
			if (pIServerUserItem==NULL) continue;
			if (pIServerUserItem->IsAndroidUser())
			{
				//效验状态
				ASSERT(pIServerUserItem->IsClientReady()==true);
				if (pIServerUserItem->IsClientReady()==false) continue;

				//发送数据
				g_GameCtrl->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

			}
		}

		return true;

	}
	else
	{
		//获取用户
		CPlayer * pIServerUserItem=GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) return false;

		if (pIServerUserItem->IsAndroidUser())
		{
			//效验状态
			ASSERT(pIServerUserItem->IsClientReady()==true);
			if (pIServerUserItem->IsClientReady()==false) return false;

			//发送数据
			g_GameCtrl->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);
			return true;
		}

	}
	return false;
}

bool CTableFrame::SendAndroidUserData( WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID/*=MDM_GF_GAME*/ )
{
	//用户群发
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
		{
			//获取用户
			CPlayer * pIServerUserItem=GetTableUserItem(i);
			if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

			if (pIServerUserItem->IsAndroidUser())
			{
				//发送数据
				g_GameCtrl->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
			}
		}
		return true;
	}
	else
	{
		//获取用户
		CPlayer * pIServerUserItem=GetTableUserItem(wChairID);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) return false;

		if (pIServerUserItem->IsAndroidUser())
		{
			//发送数据
			g_GameCtrl->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);

			return true;
		}

	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////
bool CTableFrame::AddVirtualUser(CPlayer* pIServerUserItem)
{
	//变量定义
	WORD wChairID= GetNullChairID();

	//分配用户
	if (wChairID!=INVALID_CHAIR)
	{
		pIServerUserItem->SetClientReady(true);
		pIServerUserItem->SetUserStatus(US_READY,m_wTableID,wChairID);

		if(PerformSitDownAction(wChairID,pIServerUserItem)==false)
		{
			SendRequestFailure(pIServerUserItem,TEXT("房间号不对!"),REQUEST_FAILURE_PASSWORD);
			return true;
		}	
	}
	else
	{
		SendRequestFailure(pIServerUserItem,TEXT("房间已满,请更换其他房间!"),REQUEST_FAILURE_NORMAL);
		return true;
	}

	return true;
}

//牌友圈群主解散房间
void CTableFrame::OnEventClubDismissRoom()
{
	m_pITableFrameSink->OnEventGameConclude(INVALID_CHAIR, NULL, GER_DISMISS);
}

//房间是否处于解散状态
bool CTableFrame::GetDismissState()
{
	return m_bUnderDissState;
}

//设置房间处于解散状态
void CTableFrame::SetDismissState(bool bState)
{
	m_bUnderDissState = bState;
}