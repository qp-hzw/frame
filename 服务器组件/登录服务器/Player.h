#ifndef C_PLAYER_H
#define C_PLAYER_H
#include "Stdafx.h"

class  CPlayer
{
private:
	//Socket属性
	DWORD                           m_dwSocketID;                       //scoketID
	//DWORD							m_dwClientAddr;						//连接地址
	//DWORD							m_dwLogonTime;						//登录时间


	//属性变量
private:
	tagUserInfo						m_UserInfo;							//用户信息
//	tagUserProperty                 m_UserProperty;                     //玩家道具
//
//	//登录信息
//private:
//	DWORD							m_dwDBQuestID;						//请求标识
//	
//	DWORD							m_dwInoutIndex;						//进出索引
//
//private:
//	bool							m_bAndroidUser;						//机器用户
//
//	//辅助变量
//private:
//	bool							m_bClientReady;						//连接标志
//	bool							m_bModifyScore;						//修改标志
//	TCHAR							m_szLogonPass[LEN_PASSWORD];		//用户密码
//
//	//系统属性
//private:
//	WORD							m_wBindIndex;						//绑定索引
//	TCHAR							m_szMachineID[LEN_MACHINE_ID];		//机器标识
//
//	DWORD							m_dwOfflineGameID;					//断线重连标志 
//	BYTE							m_OldGameStatus;					//用户断线之前的状态

	//函数定义
public:
	//构造函数
	CPlayer(DWORD dwSocketID, tagUserInfo& userinfo);
	//析构函数
	~CPlayer(){}

	/**********************************  游戏静态数据 ****************************************/
public:
	//用户SocketID
	DWORD GetSocketID() { return m_dwSocketID;}

	//用户信息
	tagUserInfo * GetUserInfo() { return &m_UserInfo; }
//	//道具信息
//	tagUserProperty * GetUserProperty() { return &m_UserProperty; };
//
	//用户标识
	DWORD GetUserID() { return m_UserInfo.dwUserID; }
//	//用户昵称
//	LPCTSTR GetNickName() { return m_UserInfo.szNickName; }
//	//用户性别
//	BYTE GetGender() { return m_UserInfo.cbGender; }
//
//	//用户权限
//	 BYTE GetMemberOrder() { return m_UserInfo.cbMemberOrder; } 
//	//管理权限
//	 BYTE GetMasterOrder() { return m_UserInfo.cbMasterOrder; } 
//
//	//登录时间
//	 DWORD GetLogonTime() { return m_dwLogonTime; }
//
//	 //是否机器人
//	  bool IsAndroidUser() { return m_bAndroidUser; }
//	/**********************************  游戏动态数据 ****************************************/
//public:
//	//桌子号码
//	 WORD GetTableID() { return m_UserInfo.wTableID; }
//	//椅子号码
//	 WORD GetChairID() { return m_UserInfo.wChairID; }
//
//	//设置断线标志 GameID  TODONOW
//	 void SetOfflineGameID(DWORD dwOfflineGameID){m_dwOfflineGameID = dwOfflineGameID;}
//	//获取断线标志 GameID  TODONOW
//	 DWORD GetOfflineGameID(){return m_dwOfflineGameID; }
//	
//	//用户断线之前的状态
//	 BYTE GetOldGameStatus() { return m_OldGameStatus;}
//	 void SetOldGameStatus(BYTE gamestatus) { m_OldGameStatus = gamestatus;}
//
//
//	/**********************************  socket信息 ****************************************/
//public:
//	//绑带索引
//	 WORD GetBindIndex() { return m_wBindIndex; }
//	//用户地址
//	// DWORD GetClientAddr() { return m_dwClientAddr; }
//	//机器标识
//	 LPCTSTR GetMachineID() { return m_szMachineID; }
//
//	//登录信息
//public:
//	//请求标识
//	virtual DWORD GetDBQuestID() { return m_dwDBQuestID++; }
//
//	//积分信息
//public:
//	//用户积分
//	virtual SCORE GetUserScore() { return m_UserInfo.lScore; }
//	//用户成绩
//	virtual SCORE GetUserGrade() { return m_UserInfo.lGrade; }
//	//获得用户房卡
//	virtual SCORE GetUserRoomCard() { return m_UserInfo.lOpenRoomCard; }
//	//设置用户房卡
//	virtual void SetUserRoomCard(SCORE lRoomCard) { m_UserInfo.lOpenRoomCard = lRoomCard; }
//
//	//用户钻石
//	virtual SCORE GetUserDiamond(){ return m_UserInfo.lDiamond; };
//	//设置用户钻石
//	virtual void SetUserDiamond(SCORE lDiamod) { m_UserInfo.lDiamond = lDiamod; }
//
//	//用户金币
//	virtual SCORE GetUserGold(){ return m_UserInfo.lGold; };
//	//设置用户金币
//	virtual void SetUserGold(SCORE lGold){ m_UserInfo.lGold = lGold; }
};

#endif