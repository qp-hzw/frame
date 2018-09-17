#ifndef  GAME_ROOM_CONTROL_HEAD_FILE
#define	 GAME_ROOM_CONTROL_HEAD_FILE

#include "StockManager.h"


class CGameRoomControl
{
public:
	CGameRoomControl();
	~CGameRoomControl();

private:
	CStockManager					m_StockManager;						//������
	WORD							m_GameControlLevel;					//����ȼ�
	WORD							m_ControlRateArray[9];				//��ʼ���Ƹ���
	SCORE							m_ExpectStock;						//Ԥ�ڿ���ֵ
	bool							isReadConfig;						//�Ƿ��ȡ����	
	int								m_DampingFactor[LEVELS_COUNT];		//˥��ϵ��

private:
	// Ԥ�����ֵ
	DWORD	GetPresetControlValue( SCORE expectStock ) const;
	//�Ѷ�ϵ��
	DWORD	GetDegreeOfDifficulty() const;
	// ��ȡ���Ƹ���
	DWORD	GetControlOdds( SCORE expectStock ) const;

public:
	void	Reset();
	//��ʼ������
	BOOL	Init( LPCTSTR pszString, SCORE currentStock );
	//���÷����Ѷȵȼ�
	BOOL	SetControlLevel(DWORD level);
	//���ÿ��ֵ
	BOOL	SetStockScore(SCORE currentStock);
	//��ÿ��ֵ
	SCORE	GetStockScore() const;
	//��ȡ��ǰ���仯ֵ
	SCORE	GetChangeStockScore() const;
	// ��ȡ������ж��Ƿ�С�ڿ��Ƽ���
	bool	IsControl(SCORE expectStock) const;


};

#endif