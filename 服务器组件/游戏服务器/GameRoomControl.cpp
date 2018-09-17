#include "StdAfx.h"
#include "GameRoomControl.h"
#include <string>
#include "document.h"
#include "stringbuffer.h"
#include "prettywriter.h"

CGameRoomControl::CGameRoomControl()
{
	Reset();
}

CGameRoomControl::~CGameRoomControl()
{
}

BOOL CGameRoomControl::Init( LPCTSTR pszString ,SCORE currentStock )
{
	//输出执行
	CTraceService::TraceString(TEXT("==================Init=============="),TraceLevel_Debug);

	rapidjson::Document doc;
	doc.Parse<0, rapidjson::UTF16<>>(pszString);
	if(doc.HasParseError())
	{
		return FALSE;
	}

	if(!doc.HasMember("RateMapping") ||
		!doc.HasMember("MinMaxMapping") ||
		!doc.HasMember("Damping")||
		!doc.HasMember("MaxStock")||
		!doc.HasMember("DecreaseRate"))
	{
		return FALSE;
	}
	
	m_StockManager.Reset();
	m_StockManager.SetStockScore(currentStock);

	const rapidjson::Value& valueArray = doc["RateMapping"];
	if (valueArray.IsArray())
	{
		for (rapidjson::SizeType i = 0; i< valueArray.Size();i++ )
		{
			const rapidjson::Value & object = valueArray[i];
			int minRate = object["MinRate"].GetInt();
			int maxRate = object["MaxRate"].GetInt();
			const rapidjson::Value& array = object["Difficulty"];
			DWORD Degreee[4];
			for (int j = 0; j < 4; j++)
			{
				Degreee[j] = array[j].GetInt();
			}

			m_StockManager.AddDifficultyConfig(minRate, maxRate, Degreee);
		}
	}
	
	int tempMapArray[4];

	const rapidjson::Value&  MappingArray = doc["MinMaxMapping"];
	if (MappingArray.IsArray())
	{
		for (rapidjson::SizeType i = 0; i < MappingArray.Size(); i++)
		{
			tempMapArray[i] = MappingArray[i].GetInt();
		}
	}

	const rapidjson::Value&  DampingArray = doc["Damping"];
	if (DampingArray.IsArray())
	{
		for (rapidjson::SizeType i = 0; i < DampingArray.Size(); i++)
		{
			m_DampingFactor[i] =  DampingArray[i].GetInt();
		}
	} 
	
	SCORE tempMaxStock = doc["MaxStock"].GetInt64();
	m_StockManager.InitConfig(tempMapArray, tempMaxStock);

	const  rapidjson::Value&  ControlArray = doc["ControlRate"];
	if (ControlArray.IsArray())
	{
		for (rapidjson::SizeType i = 0; i < ControlArray.Size(); i++)
		{
			m_ControlRateArray[i] = ControlArray[i].GetInt();
		}
	}

	DWORD tempDecreaseRate = doc["DecreaseRate"].GetInt();
	m_StockManager.SetDecreaseRate(tempDecreaseRate);

	isReadConfig = true;

	return TRUE;
}

BOOL CGameRoomControl::SetControlLevel( DWORD level )
{
	m_GameControlLevel = level;
	return true;
}

//设置库存值
BOOL CGameRoomControl::SetStockScore( SCORE currentStock )
{
	WORD factor = m_DampingFactor[m_GameControlLevel];
	
	//输出执行
	CTraceService::TraceStringEx(TraceLevel_Debug,TEXT("SetStockScore change Stock ： %I64d"),currentStock);	
	m_StockManager.ModifyStockScore(currentStock, factor);
	return TRUE;
}

//获得库存值
SCORE CGameRoomControl::GetStockScore() const
{
	return m_StockManager.GetStockScore();
}

//获取当前库存变化值
SCORE CGameRoomControl::GetChangeStockScore() const
{
	return m_StockManager.GetChangeStockScore();
}

// 预设控制值
DWORD CGameRoomControl::GetPresetControlValue( SCORE expectStock ) const
{
	SCORE	currentStock = m_StockManager.GetStockScore();
	if ( currentStock > 0 )
	{
		DWORD	winOrLoseRate = m_StockManager.GetVariantOfWinOrLose(expectStock);
		DWORD	minStockRate = m_StockManager.GetMinStockRate() ;
		//输出执行

		CTraceService::TraceStringEx(TraceLevel_Debug, 
			TEXT("GetPresetControlValue winOrLoseRate： %d, minStockRate: %d,expectStock: %I64d"), 
			winOrLoseRate, minStockRate, expectStock);

		if ( winOrLoseRate <= 2 )
		{
			return m_ControlRateArray[0];
		}
		else if (winOrLoseRate >= 2 && winOrLoseRate <= 15 && minStockRate >= 50 && minStockRate <= 80)
		{
			return m_ControlRateArray[1];
		}
		else if( winOrLoseRate >= 2 && winOrLoseRate <= 20 && minStockRate > 80 && minStockRate <= 90)
		{
			return m_ControlRateArray[1];
		}
		else if( winOrLoseRate >= 2 && winOrLoseRate <= 30 && minStockRate > 90 )
		{
			return m_ControlRateArray[1];
		}
		else if ( winOrLoseRate > 2)
		{
			return m_ControlRateArray[2];
		}
	}
	else if (currentStock < 0)
	{
		DWORD maxStockRate = m_StockManager.GetMaxStockRate(m_GameControlLevel) ;		
		if (maxStockRate <= 20)
		{
			return m_ControlRateArray[3];
		}
		if (maxStockRate > 20 && maxStockRate <= 50)
		{
			return m_ControlRateArray[4];
		}
		if (maxStockRate > 50 && maxStockRate <= 60)
		{
			return m_ControlRateArray[5];
		}
		if (maxStockRate > 60 && maxStockRate <= 70)
		{
			return m_ControlRateArray[6];
		}
		if (maxStockRate > 70 && maxStockRate <= 80)
		{
			return m_ControlRateArray[7];
		}
		if (maxStockRate > 80)
		{
			return m_ControlRateArray[8];
		}
	}
	return 0;
}

//难度系数
DWORD CGameRoomControl::GetDegreeOfDifficulty() const
{
	SCORE	currentStock = m_StockManager.GetStockScore();
	SCORE	maxStockScore =  m_StockManager.GetMaxStockScore();
	ASSERT(maxStockScore != 0);
	if (maxStockScore == 0)
	{
		return 0;
	}
	int		tempRate =(currentStock *100 )/ maxStockScore;
	DWORD	stockFactor= m_StockManager.GetDegreeOfDifficultyByStockRate( tempRate ,m_GameControlLevel);
	return  stockFactor;
}

// 获取控制概率
DWORD CGameRoomControl::GetControlOdds(  SCORE expectStock  ) const
{
	DWORD presetControlRate = GetPresetControlValue(expectStock);
	DWORD maxStock = m_StockManager.GetMaxStockScore();
	ASSERT(maxStock != 0);
	if (presetControlRate == 0 || maxStock == 0 )
	{
		return 0;
	}
	DWORD factor =  GetDegreeOfDifficulty();
	DWORD odds = presetControlRate + ( abs(expectStock) * factor * 100) / maxStock;

	//输出执行
	CTraceService::TraceStringEx(TraceLevel_Debug, 
		TEXT("GetControlOdds presetControlRate： %d, factor: %d, odds: %d, expectStock:%I64d"), 
		presetControlRate, factor, odds, expectStock);

	return odds;
}

// 获取随机数判定是否小于控制几率
bool CGameRoomControl::IsControl( SCORE expectStock ) const
{
	//输出执行
	CTraceService::TraceStringEx(TraceLevel_Debug,TEXT(" expectStock: %ld"), expectStock);
	if (!isReadConfig || expectStock == 0)
	{
		//输出执行
		CTraceService::TraceString(TEXT("The paramter is error!"),TraceLevel_Debug);
		return false;
	}
	DWORD odds = GetControlOdds(expectStock);
	bool isContol = false;
	DWORD randNum = rand()%100;

	//输出执行
	CTraceService::TraceStringEx(TraceLevel_Debug, 
		TEXT("IsControl odds： %d, randNum: %d, m_lStockScore: %I64d, expectStock: %I64d"), 
		odds, randNum, m_StockManager.GetStockScore(), expectStock);
	if (randNum <= odds  && odds > 0)
	{
		isContol = true;
	}
	return isContol;
}

void CGameRoomControl::Reset()
{
	m_StockManager.Reset();
	m_GameControlLevel = 0;	
	ZeroMemory(m_ControlRateArray,sizeof(m_ControlRateArray));
	memset(m_DampingFactor, 0, sizeof(m_DampingFactor));
	m_ExpectStock = 0;	
	isReadConfig = false;
	
}

