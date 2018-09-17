#ifndef STOCK_MANAGER_HEAD_FILE
#define STOCK_MANAGER_HEAD_FILE

#include "Stdafx.h"
#define   LEVELS_COUNT				4   //�ȼ���Ŀ


struct  DifficultyConfig
{
	int								MinRate;					//��С����
	WORD							MaxRate;					//������
	DWORD							Factor[LEVELS_COUNT];		//�Ѷ�ϵ��
};

typedef CWHArray<DifficultyConfig>     DCArray;

//������
class CStockManager
{
	//��������
protected:
	SCORE							m_lStockScore;						//�����Ŀ
	WORD							m_dDecreaseRate;					//���ٱ���(���Էֱ���)
	//SCORE							m_MinStockScore;					//��С���
	SCORE							m_MaxStockScore;					//�����
	int								m_MaxToMinFactor[LEVELS_COUNT];		//�����ת��С���ϵ��
	SCORE							m_ChangeStock;						//�ܳԷ�
	DCArray							m_DifficultyConfigs;				//�����Ѷ�ϵ��ӳ��	
			

	//��������
public:
	//���캯��
	CStockManager();
	//��������
	virtual ~CStockManager();

	VOID Reset();

	//��ѯ����
public:
	//��ȡ���
	SCORE	GetStockScore() const { return m_lStockScore; }
	//���ٱ���
	WORD	GetDecreaseRate() const { return m_dDecreaseRate; }
	//��ȡ��С���
	SCORE	GetMinStockScore(WORD level) const;
	// ��ȡ��С������
	DWORD	GetMinStockRate() const;
	// ��ȡ��������
	DWORD	GetMaxStockRate(WORD level) const;
	//��ȡ�����
	SCORE	GetMaxStockScore() const { return m_MaxStockScore; }
	// ��Ӯ����
	DWORD	GetVariantOfWinOrLose(SCORE expectStock ) const;
	// ��ȡ�Ѷ�ϵ��
	DWORD	GetDegreeOfDifficultyByStockRate( int rate, WORD factor) const;
	// ��ȡ˥��ֵ
	SCORE	GetChangeStockScore( ) const;
	

	//���ú���
public:
	//���ÿ��
	VOID SetStockScore(SCORE lStockScore);
	//���ñ���
	VOID SetDecreaseRate(WORD dDecreaseRate);

	//���ܺ���
public:
	//�޸Ŀ��
	BOOL ModifyStockScore(SCORE lVariationScore, WORD level);

	VOID InitConfig( int mappingArray[], SCORE maxStockScore);

	//��������
	VOID AddDifficultyConfig(WORD minRate,WORD maxRate, DWORD dArray[]);

};

//////////////////////////////////////////////////////////////////////////////////

#endif