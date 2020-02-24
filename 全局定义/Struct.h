#ifndef STRUCT_HEAD_FILE
#define STRUCT_HEAD_FILE
#pragma pack(1)
#include "Define.h"

//��������������
struct MATCH_CONFIG
{
	//�׶���Ϣ
	struct STAGE_INFO
	{
		TCHAR	szName[16];	//�����������������..
		DWORD	wXJCount;					//С����
		DWORD	wPlayerCount;				//����
		DWORD	wJinJi_Rule;				//��������   ͬ���Ƚ�/ȫ���Ƚ� 1,2,
		DWORD	wJinJi_Count;				//��������
	};

	//��������Ϣ
	DWORD		wMatchID;							//������ID��ʶ
	TCHAR		szTitle[32];			//����
	TCHAR		szDescribe[128];		//��������
	STAGE_INFO	stage[5];				//�׶���Ϣ
	DWORD		wStageSize;							//�׶θ���
	DWORD		llBaseScore;						//�׷�

	//����������
	DWORD		dwStartTime;						//��ʼʱ�� ��-1��ʾ����������
	DWORD		dwTimePeriod;						//��������
	DWORD		wStartType;							//��ʼ����  (0-������ʼ  1-��ʱ��ʼ)
	DWORD		dwLowestPlayer;						//���ٲ�������
	DWORD		FeeType;							//���������� 0-��� 1-שʯ 2-��Ʊ
	DWORD		llFee;								//������
	DWORD		llReword;							//����

	//��������Ϣ
	DWORD		dwRobotCount;						//����������
};

//�û����� -- ��client����һ��
struct tagUserProperty
{
	DWORD       dwPropID;		  //item id
	DWORD       dwPropCount;      //item count;
};

//�û���Ϣ -- ��client����һ��
struct tagUserInfo
{
	//��������
	DWORD							dwUserID;							//�û�ID
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szHeadUrl[LEN_HEAD_URL];			//ͷ���ַ
	TCHAR							szUnderWrite[LEN_MY_SIGNATURE];		//����ǩ��

	//������Ϣ
	DWORD							dwGroupID;							//���� ID
	TCHAR							szGroupName[LEN_GROUP_NAME];		//��������

	//�û��ȼ�
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	BYTE							dwLevel;                            //����ȼ�   
	DWORD							dwExperience;						//������ֵ

	//�û��Ƹ�
	SCORE							lOpenRoomCard;						//��������
	SCORE							lGold;								//���
	SCORE							lDiamond;							//��ʯ
	
	//��Ϸ��Ϣ
	DWORD							dwWinCount;							//ʤ������
	DWORD							dwLostCount;						//ʧ������
	DWORD							dwDrawCount;						//�;�����
	DWORD							dwFleeCount;						//��������

	/* �������� ��Ϸ��̬���� */
	DWORD							wTableID;							//��������
	WORD							wChairID;							//��������
	BYTE							cbUserStatus;						//�û�״̬
	SCORE							lScore;								//����
};

//��Ϸ��¼
struct tagGameScoreRecord
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	WORD							wChairID;							//���Ӻ���
	BYTE							cbAndroid;							//������־

	//�ɼ���Ϣ
	SCORE							lScore;								//�û�����
	SCORE							lGrade;								//�û��ɼ�
	SCORE							lRevenue;							//��Ϸ˰��
	SCORE							lControlScore;						//�û�����ֵ
	SCORE							lChoushui;							//��ˮ
	DWORD							dwChoushuiType;						//��ˮ����

	//�û���Ϣ
	DWORD							dwDBQuestID;						//�����ʶ
	DWORD							dwInoutIndex;						//��¼����

	//������Ϣ
	DWORD							dwUserMemal;						//������Ŀ
	DWORD							dwPlayTimeCount;					//��Ϸʱ��
};

#endif