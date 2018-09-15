#include "StdAfx.h"
#include "StockManager.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CStockManager::CStockManager()
{
	Reset();
}

//析构函数
CStockManager::~CStockManager()
{
}

VOID CStockManager::Reset()
{
	m_lStockScore=0L;
	m_dDecreaseRate=1;

	m_MaxStockScore = 0;				//最大库存
	m_ChangeStock = 0;
		
	memset(m_MaxToMinFactor, 0, sizeof(m_MaxToMinFactor));

	m_DifficultyConfigs.RemoveAll();
}

//设置库存
VOID CStockManager::SetStockScore(SCORE lStockScore)
{
	//设置变量
	m_lStockScore=lStockScore;
}

//设置吃分比率
VOID CStockManager::SetDecreaseRate(WORD dDecreaseRate)
{
	//设置变量
	m_dDecreaseRate=dDecreaseRate;
}

//修改库存
BOOL CStockManager::ModifyStockScore(SCORE lVariationScore, WORD factor)
{
	//效验参数
	m_lStockScore +=lVariationScore;

	//设置减少
	if (m_lStockScore > 0 && lVariationScore != 0)
	{
		if (m_lStockScore >= m_MaxStockScore && m_dDecreaseRate > 0)
		{
			m_ChangeStock = (SCORE)((m_lStockScore * m_dDecreaseRate) /10000);
			m_lStockScore -= m_ChangeStock;
		}
		else
		{
			m_ChangeStock = (SCORE)( m_lStockScore * factor)/10000;
			m_lStockScore -= m_ChangeStock;
		}	
	}
	return TRUE;
}

// 获取衰减值
SCORE CStockManager::GetChangeStockScore() const
{
	return m_ChangeStock;
}

//获取最小库存
SCORE CStockManager::GetMinStockScore(WORD level) const
{
	int factor = m_MaxToMinFactor[level];
	ASSERT(factor != 0);
	if ( factor == 0 )
	{
		return 0;
	}

	SCORE	minStockScore = m_MaxStockScore / factor;
	return  minStockScore;
}

// 获取最小库存比率
DWORD CStockManager::GetMinStockRate() const
{
	ASSERT(m_MaxStockScore != 0);
	if ( m_MaxStockScore == 0 )
	{
		return 0;
	}
	DWORD tempRate =static_cast<DWORD>(( m_lStockScore * 100 )/ m_MaxStockScore) ;
	return tempRate;
}

// 获取最大库存比率
DWORD CStockManager::GetMaxStockRate(WORD level) const
{
	SCORE minScore = GetMinStockScore(level);
	ASSERT(minScore != 0);
	if (minScore == 0)
	{
		return 0;
	}
	DWORD tempRate =static_cast<DWORD> (( m_lStockScore  * 100 ) / minScore);
	return tempRate;
}

// 输赢变量
DWORD CStockManager::GetVariantOfWinOrLose( SCORE expectStock ) const
{
	ASSERT(m_MaxStockScore != 0);
	if ( m_MaxStockScore == 0 )
	{
		return 0;
	}
	DWORD WinOrLoseRate = static_cast<DWORD>(abs(expectStock)*100 / m_MaxStockScore);
	return WinOrLoseRate ;
}

//初始化变量
VOID CStockManager::InitConfig( int mappingArray[], SCORE maxStockScore)
{
	CopyMemory(m_MaxToMinFactor, mappingArray, sizeof(m_MaxToMinFactor));
	m_MaxStockScore = maxStockScore;
}

// 获取难度系数
DWORD CStockManager::GetDegreeOfDifficultyByStockRate(int rate, WORD level) const
{
	
	for (int i = 0; i < m_DifficultyConfigs.GetCount(); i++)
	{
		if (rate <= 0) //容错处理
		{
			return  m_DifficultyConfigs[0].Factor[level];
		}
		else  if (rate >= m_DifficultyConfigs[i].MinRate && rate < m_DifficultyConfigs[i].MaxRate)
		{
			return m_DifficultyConfigs[i].Factor[level];
		}

	}
	return 0;
}

//插入配置
VOID CStockManager::AddDifficultyConfig(WORD minRate,WORD maxRate, DWORD dArray[] )
{
	DifficultyConfig  config;
	config.MinRate = minRate;
	config.MaxRate = maxRate;
	CopyMemory(config.Factor, dArray, sizeof(config.Factor));
	m_DifficultyConfigs.Add(config);
}


//////////////////////////////////////////////////////////////////////////////////
