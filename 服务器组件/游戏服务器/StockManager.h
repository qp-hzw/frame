#ifndef STOCK_MANAGER_HEAD_FILE
#define STOCK_MANAGER_HEAD_FILE

#include "Stdafx.h"
#define   LEVELS_COUNT				4   //等级数目


struct  DifficultyConfig
{
	int								MinRate;					//最小比率
	WORD							MaxRate;					//最大比率
	DWORD							Factor[LEVELS_COUNT];		//难度系数
};

typedef CWHArray<DifficultyConfig>     DCArray;

//库存管理
class CStockManager
{
	//变量定义
protected:
	SCORE							m_lStockScore;						//库存数目
	WORD							m_dDecreaseRate;					//减少比率(库存吃分比率)
	//SCORE							m_MinStockScore;					//最小库存
	SCORE							m_MaxStockScore;					//最大库存
	int								m_MaxToMinFactor[LEVELS_COUNT];		//最大库存转最小库存系数
	SCORE							m_ChangeStock;						//总吃分
	DCArray							m_DifficultyConfigs;				//房间难度系数映射	
			

	//函数定义
public:
	//构造函数
	CStockManager();
	//析构函数
	virtual ~CStockManager();

	VOID Reset();

	//查询函数
public:
	//获取库存
	SCORE	GetStockScore() const { return m_lStockScore; }
	//减少比率
	WORD	GetDecreaseRate() const { return m_dDecreaseRate; }
	//获取最小库存
	SCORE	GetMinStockScore(WORD level) const;
	// 获取最小库存比率
	DWORD	GetMinStockRate() const;
	// 获取最大库存比率
	DWORD	GetMaxStockRate(WORD level) const;
	//获取最大库存
	SCORE	GetMaxStockScore() const { return m_MaxStockScore; }
	// 输赢变量
	DWORD	GetVariantOfWinOrLose(SCORE expectStock ) const;
	// 获取难度系数
	DWORD	GetDegreeOfDifficultyByStockRate( int rate, WORD factor) const;
	// 获取衰减值
	SCORE	GetChangeStockScore( ) const;
	

	//设置函数
public:
	//设置库存
	VOID SetStockScore(SCORE lStockScore);
	//设置比率
	VOID SetDecreaseRate(WORD dDecreaseRate);

	//功能函数
public:
	//修改库存
	BOOL ModifyStockScore(SCORE lVariationScore, WORD level);

	VOID InitConfig( int mappingArray[], SCORE maxStockScore);

	//插入配置
	VOID AddDifficultyConfig(WORD minRate,WORD maxRate, DWORD dArray[]);

};

//////////////////////////////////////////////////////////////////////////////////

#endif