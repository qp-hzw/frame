#ifndef STRUCT_HEAD_FILE
#define STRUCT_HEAD_FILE

#pragma pack(1)
#include "Define.h"

//////////////////////////////////////////////////////////////////////////////////
//�û���Ϣ


//�û�����
struct tagUserProperty
{
	//����ʹ��
	struct tagUsePropertyInfo
	{
		WORD                            wPropertyCount;                     //������Ŀ
		WORD                            dwValidNum;						    //��Ч����
		DWORD                           dwEffectTime;                       //��Чʱ��
	};

	WORD                            wPropertyUseMark;                   //���߱�ʾ
	tagUsePropertyInfo              PropertyInfo[MAX_PT_MARK];			//ʹ����Ϣ   
};


//�û���Ϣ
struct tagUserInfo
{
	//��������
	DWORD							dwUserID;							//�û� I D
	DWORD							wTableID;						//��������
	WORD							wChairID;							//��������
	BYTE							cbUserStatus;						//�û�״̬

	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	DWORD							dwGroupID;							//���� I D
	TCHAR							szGroupName[LEN_GROUP_NAME];		//��������
	TCHAR							szUnderWrite[LEN_MY_SIGNATURE];		//����ǩ��

	//ͷ����Ϣ
	DWORD							dwCustomID;							//�Զ���ʶ
	TCHAR							szHeadUrl[LEN_HEAD_URL];			//ͷ���ַ

	//�û�����
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	BYTE							cbMasterOrder;						//����ȼ�

	

	//������Ϣ
	SCORE							lScore;								//�û�����
	SCORE							lGrade;								//�û��ɼ�
	SCORE							lControlScore;						//�û�����ֵ
	SCORE							lOpenRoomCard;						//��������
	SCORE							lDiamond;							//��ʯ
	SCORE							lGold;								//���

	//��Ϸ��Ϣ
	DWORD							dwWinCount;							//ʤ������
	DWORD							dwLostCount;						//ʧ������
	DWORD							dwDrawCount;						//�;�����
	DWORD							dwFleeCount;						//��������
	DWORD							dwUserMedal;						//�û�����
	DWORD							dwExperience;						//�û�����
	LONG							lLoveLiness;						//�û�����

	//GPS added by lizhihu TODO ��ʱ�����������ͳһ
	double							dLongitude;							//����
	double							dLatitude;							//γ��

	//��¼��Ϣ
	DWORD							dwLogonTime;						//��¼ʱ��
	DWORD							dwInoutIndex;						//������ʶ

	//������Ϣ
	WORD							wBindIndex;							//������
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ

	//���ӱ���
	DWORD							dwUserRight;						//�û�Ȩ��
	DWORD							dwMasterRight;						//����Ȩ��
	SCORE							lRestrictScore;						//���ƻ���

	//��������
	bool							bAndroidUser;						//�����û�
	TCHAR							szPassword[LEN_MD5];				//��������
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