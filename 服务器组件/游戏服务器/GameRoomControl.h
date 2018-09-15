#ifndef  GAME_ROOM_CONTROL_HEAD_FILE
#define	 GAME_ROOM_CONTROL_HEAD_FILE

#include "StockManager.h"


class CGameRoomControl
{
public:
	CGameRoomControl();
	~CGameRoomControl();

private:
	CStockManager					m_StockManager;						//库存管理
	WORD							m_GameControlLevel;					//房间等级
	WORD							m_ControlRateArray[9];				//初始控制概率
	SCORE							m_ExpectStock;						//预期控制值
	bool							isReadConfig;						//是否读取配置	
	int								m_DampingFactor[LEVELS_COUNT];		//衰减系数

private:
	// 预设控制值
	DWORD	GetPresetControlValue( SCORE expectStock ) const;
	//难度系数
	DWORD	GetDegreeOfDifficulty() const;
	// 获取控制概率
	DWORD	GetControlOdds( SCORE expectStock ) const;

public:
	void	Reset();
	//初始化配置
	BOOL	Init( LPCTSTR pszString, SCORE currentStock );
	//设置房间难度等级
	BOOL	SetControlLevel(DWORD level);
	//设置库存值
	BOOL	SetStockScore(SCORE currentStock);
	//获得库存值
	SCORE	GetStockScore() const;
	//获取当前库存变化值
	SCORE	GetChangeStockScore() const;
	// 获取随机数判定是否小于控制几率
	bool	IsControl(SCORE expectStock) const;


};

#endif