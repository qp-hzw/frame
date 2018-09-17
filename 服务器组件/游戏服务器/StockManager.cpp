#include "StdAfx.h"
#include "StockManager.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CStockManager::CStockManager()
{
	Reset();
}

//��������
CStockManager::~CStockManager()
{
}

VOID CStockManager::Reset()
{
	m_lStockScore=0L;
	m_dDecreaseRate=1;

	m_MaxStockScore = 0;				//�����
	m_ChangeStock = 0;
		
	memset(m_MaxToMinFactor, 0, sizeof(m_MaxToMinFactor));

	m_DifficultyConfigs.RemoveAll();
}

//���ÿ��
VOID CStockManager::SetStockScore(SCORE lStockScore)
{
	//���ñ���
	m_lStockScore=lStockScore;
}

//���óԷֱ���
VOID CStockManager::SetDecreaseRate(WORD dDecreaseRate)
{
	//���ñ���
	m_dDecreaseRate=dDecreaseRate;
}

//�޸Ŀ��
BOOL CStockManager::ModifyStockScore(SCORE lVariationScore, WORD factor)
{
	//Ч�����
	m_lStockScore +=lVariationScore;

	//���ü���
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

// ��ȡ˥��ֵ
SCORE CStockManager::GetChangeStockScore() const
{
	return m_ChangeStock;
}

//��ȡ��С���
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

// ��ȡ��С������
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

// ��ȡ��������
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

// ��Ӯ����
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

//��ʼ������
VOID CStockManager::InitConfig( int mappingArray[], SCORE maxStockScore)
{
	CopyMemory(m_MaxToMinFactor, mappingArray, sizeof(m_MaxToMinFactor));
	m_MaxStockScore = maxStockScore;
}

// ��ȡ�Ѷ�ϵ��
DWORD CStockManager::GetDegreeOfDifficultyByStockRate(int rate, WORD level) const
{
	
	for (int i = 0; i < m_DifficultyConfigs.GetCount(); i++)
	{
		if (rate <= 0) //�ݴ���
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

//��������
VOID CStockManager::AddDifficultyConfig(WORD minRate,WORD maxRate, DWORD dArray[] )
{
	DifficultyConfig  config;
	config.MinRate = minRate;
	config.MaxRate = maxRate;
	CopyMemory(config.Factor, dArray, sizeof(config.Factor));
	m_DifficultyConfigs.Add(config);
}


//////////////////////////////////////////////////////////////////////////////////
