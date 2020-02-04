#ifndef STRUCT_HEAD_FILE
#define STRUCT_HEAD_FILE

#pragma pack(1)
#include "Define.h"

//////////////////////////////////////////////////////////////////////////////////
//�û���Ϣ


//�û�����
struct tagUserProperty
{
	DWORD       dwPropID;		  //item id
	DWORD       dwPropCount;      //item count;
};


//�û���Ϣ
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

//////////////////////////////////////////////////////////////////////////////////

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