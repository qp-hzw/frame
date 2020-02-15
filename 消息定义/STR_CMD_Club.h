#ifndef STR_CMD_CLUB_HEAD_FILE
#define STR_CMD_CLUB_HEAD_FILE

#pragma pack(1)

//MFC �ļ�
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxDisp.h>
#include <AfxDtctl.h>
#include "../ȫ�ֶ���/Define.h"

//ʵʱˢ�¹����б�
struct STR_CMD_LC_CLUB_LIST_RE
{
	BYTE	byMask;					//��ʵ������
};

//ʵʱˢ�¹��᷿���б�
struct STR_CMD_LC_CLUB_ROOM_RE
{
	DWORD	dwClubID;				//���ֲ�ID
};

#pragma region ���� ����|����|�б�
//��������Ȧ
struct STR_SUB_CL_CLUB_CREATE_CLUB
{
	TCHAR	szClubName[LEN_NICKNAME];					//����Ȧ����
	TCHAR	szMessag[128];								//����Ȧ���
};

//��������Ȧ ����
struct STR_CMD_LC_CLUB_CREATE_CLUB
{
	LONG	lResultCode;								//0�ɹ�  ����ʧ��
};

//�����������Ȧ
struct STR_SUB_CL_CLUB_JOIN_CLUB
{
	DWORD	dwClubID;									//����ȦID
};

//�����������Ȧ ����
struct STR_CMD_LC_CLUB_JOIN_CLUB
{
	LONG	lResultCode;								//0-�ɹ�  ����ʧ��
};
//�����������Ȧ �㲥
struct STR_SUB_CL_CLUB_JOIN_CLUB_BDCAST
{
	DWORD	dwClubID;									//����ȦID
	DWORD	dwApplicantNum;								//��������
};

//��ɢ����Ȧ
struct STR_SUB_CL_CLUB_DISS_CLUB
{
	DWORD	dwClubID;									//����ȦID
};

//��ɢ����Ȧ ����
struct STR_CMD_LC_CLUB_DISS_CLUB
{
	LONG	lResultCode;								//0-�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};

//������������Ȧ�б� ����
struct STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST
{
	DWORD	dwClubID;									//����ȦID
	TCHAR	szClubName[LEN_NICKNAME];					//����Ȧ����
	BYTE	byClubRole;									//���ְ�� 0Ⱥ�� 1���� 2һ���Ա
};

//����δ��Ա�������������Ȧ(���9��) ����
struct STR_CMD_LC_CLUB_RANDOM_CLUB_LIST
{
	DWORD	dwClubID;									//����ȦID
	TCHAR	szClubName[LEN_NICKNAME];					//����Ȧ����
	BYTE	byClubLevel;								//����Ȧ�ȼ�
	WORD	wMemberNum;									//��ǰ����
	TCHAR	szNotice[128];								//����Ȧ ����
	TCHAR	szMessage[128];								//����Ȧ ���
	DWORD	dwClubOwner;								//Ⱥ��ID
	TCHAR	szClubOwnerName[LEN_NICKNAME];				//Ⱥ������
};

//����δ��Ա�������������Ȧ(���9��) ����
struct STR_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH
{
	BYTE	byMask;										//������־ 1
};
#pragma endregion

#pragma region ���᷿�� ����|����|�޸�|�б�
//��ѯ�ض�����Ȧ�����б�
struct STR_SUB_CL_CLUB_ROOM_LIST
{
	DWORD	dwClubID;									//����ȦID
};

//��ѯ�ض�����Ȧ�����б� ����
struct STR_CMD_LC_CLUB_ROOM_LIST
{
	DWORD	dwRoomID;									//����Ȧ����ID
	TCHAR	szRoomName[16];								//club��������
	DWORD	dwKindID;									//��Ϸ���� -- ��Ϸ����ַ
	BYTE	byGoldOrFK;									//�������� 1������  2��ҳ�
	TCHAR	szKindName[LEN_KIND];						//��Ϸ����
	WORD	wPlayerNum;									//�÷����ڵ�ǰ�����
	TCHAR	szRoomRule[128];							//�������˵��
	BYTE	byAllRound;									//��Ϸ�ܾ���
	BYTE	byChairNum;									//������Ŀ

	//��ҷ� && ������ͨ��
	BYTE	bDissolve;									//�Ƿ������ɢ  0����  1������

	//��ҷ�����
	DWORD	dwAmount;									//��Ͷ��
	DWORD	dwOwnerPercentage;							//Ⱥ�����

	//��ע && ������
	BYTE	byMask;										//1 AA֧��;  2��Ӯ��֧��
	DWORD	dwDizhu;									//��ע
	DWORD	dwGold;										//������, ���ڴ�ֵ, ����˰��
};

//������������
struct STR_SUB_CL_CLUB_ROOM_SETTING
{
	DWORD	dwRoomID;									//����ID
	TCHAR	szRoomName[16];								//��������
	DWORD   dwClubID;									//���ֲ�ID
	DWORD   dwKindID;                                   //kindID

	BYTE	byGoldOrFK;									//1������ 2��ҷ�
	//��ҷ� && ������ ͨ������
	bool	bDissolve;									//�Ƿ������ɢ

	//��ҷ�����
	BYTE	byMask;										//1 AA֧��;  2��Ӯ��֧��

	DWORD	dwAmount;									//��Ͷ��(��Ӯ������������ �ſ�ʼ��ˮ)
	DWORD	dwOwnerPercentage;							//Ⱥ�����

	//��ע && ������
	DWORD	dwDizhu;									//��ע
	DWORD	dwGold;										//������
};

//������������ ����
struct STR_CMD_LC_CLUB_ROOM_SETTING
{
	LONG	lResultCode;								//0-�ɹ�  ����ʧ��
};

//��ɢ��������
struct STR_SUB_CL_CLUB_ROOM_DISSOLVE
{
	DWORD	dwClubRoomID;								//����ID
};

//��ɢ�������� ����
struct STR_CMD_LC_CLUB_ROOM_DISSOLVE
{
	LONG	lResultCode;								//0-�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};

//������뷿��
struct STR_SUB_CL_CLUB_JOIN_ROOM
{
	DWORD	dwRoomID;									//����ID
};

//������뷿�� ����
struct STR_CMD_LC_CLUB_JOIN_ROOM
{
	LONG	lResultCode;								//0-�ɹ�  ����ʧ��
};

//�����ϵ� userList
struct STR_CMD_LC_CLUB_TABLE_USER_LIST
{
	DWORD	dwUserID;									//���ID
	TCHAR	szUserName[LEN_NICKNAME];					//�������
	BYTE	bySex;										//�Ա�
	TCHAR	szHeadUrl[LEN_HEAD_URL];					//ͷ���ַ
	WORD	wLevel;										//�ȼ�
	
	BYTE	byClubRole;									//���ְ�� 0 Ⱥ��; 1����; 2һ���Ա
	BYTE	byClubReputation;							//����ڹ����е������ȼ�

	BYTE	byChairID;									//����ID
};

//�÷����tableList
struct STR_CMD_LC_CLUB_TABLE_LIST
{
	BYTE	byDel;										//mask 0�޲���;  1ɾ��

	DWORD	dwClubID;									//����ID
	DWORD	dwRoomID;									//���᷿��ID
	DWORD	dwTableID;									//����ID

	DWORD	TableState;									//0���ӿ��� 1�����Ѿ�����
	DWORD	LockState;									//0������Ҫ����  1���Ӳ���Ҫ����
	DWORD	CurrentRound;								//��ǰ����
	DWORD	AllRound;									//�ܾ���
	DWORD	ChairCount;									//������Ŀ

	STR_CMD_LC_CLUB_TABLE_USER_LIST player_info[6];     //�����Ϣ
};

//��������
struct STR_SUB_CL_CLUB_ROOM_CREATE
{
	//��������
	STR_SUB_CL_CLUB_ROOM_SETTING setting;

    //�������
	BYTE							byChoose[20];			//����ѡ��
};

//�������� ����
struct STR_CMD_LC_CLUB_ROOM_CREATE
{
	LONG	lResultCode;								//0-�ɹ�  ����ʧ��
};
#pragma endregion

#pragma region �������� ����|����|�б�//��ɢ��������
//����÷����������Ϣ
struct STR_SUB_CG_CLUB_TABLE_LIST_TABLE
{
	DWORD	dwClubID;								//club id 
	DWORD	dwRoomID;								//room id
};

//��������
struct STR_SUB_CG_CLUB_CREATE_TABLE
{
	DWORD	dwClubID;								//club id 
	DWORD	dwRoomID;								//room id
	BYTE	byType;									//1.��ͨ���� 2.˽������
	DWORD	dwPasswd;								//˽������ ����
};

//��ɢ��������
struct STR_SUB_CL_CLUB_TABLE_DISSOLVE
{
	DWORD	dwClubRoomID;								//room id 
	DWORD	dwTableID;									//table id
};
//��ɢ�������� ����
struct STR_CMD_LC_CLUB_TABLE_DISSOLVE
{
	LONG	lResultCode;								//������
	TCHAR	szDescribe[128];							//������Ϣ
};

#pragma endregion

#pragma region ������Ϣ���
//����Ȧ���� -- �ڲ���Ա�ɿ�
struct STR_SUB_CL_CLUB_NOTICE
{
	DWORD	dwClubID;									//����ȦID
	TCHAR	szNotice[128];								//����Ȧ ����
};

//����Ȧ���� ����
struct STR_CMD_LC_CLUB_NOTICE
{
	LONG	lResultCode;								//0�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};


//����Ȧ���
struct STR_SUB_CL_CLUB_MESSAGE
{
	DWORD	dwClubID;									//����ID
	TCHAR	szMessage[128];								//����Ȧ ���
};

//����Ȧ��� ����
struct STR_CMD_LC_CLUB_MESSAGE
{
	LONG	lResultCode;								//0�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};

//���׷���
struct STR_SUB_CL_CLUB_CONTRIBUTE_FK
{
	DWORD	dwClubID;									//���ֲ�ID
	DWORD	dwFK;										//���׵ķ�������
};

//���׷��� ����
struct STR_CMD_LC_CLUB_CONTRIBUTE_FK
{
	LONG	lResultCode;								//0�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��

	DWORD	dwClubFK;									//��ǰ����Ȧ�з�������
};

//����Ȧ����
struct STR_SUB_CL_CLUB_AUTO_AGREE
{
	DWORD	dwClubID;									//���ֲ�ID
	BYTE	byAutoAgree;									//�Ƿ����Զ�ͬ�� 0ͬ��
	//����Ϊ�������� ����10�����ϵ�Ů�Բ��������ȵ� -- �������
	BYTE	bySex;										//0������; ����1��; 2Ů
	WORD	wLevel;										//����ȼ�
};

//����Ȧ���� ����
struct STR_CMD_LC_CLUB_AUTO_AGREE
{
	LONG	lResultCode;								//0�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};

//�����Ա����
struct STR_SUB_CL_CLUB_MEMBER_MANAGER
{
	DWORD	dwClubID;									//����ȦID
};

//�����Ա���� ����
struct STR_CMD_LC_CLUB_MEMBER_MANAGER
{
	DWORD	dwUserID;									//���ID
	TCHAR	szUserName[LEN_NICKNAME];					//�������
	BYTE	byClubRole;									//���ְ�� 0 Ⱥ��; 1����; 2һ���Ա
	BYTE	byUserLevel;								//��ҵȼ�
	BYTE	byClubReputation;							//����ڹ����е������ȼ�

	BYTE	byUserState;								//���״̬  0���� 1���� 2��Ϸ��
	TCHAR	szHeadUrl[LEN_HEAD_URL];							//ͷ���ַ
};

 //ͬʱ���صĹ��������Ϣ
struct STR_CMD_LC_CLUB_DATA
{
	TCHAR	szClubName[LEN_NICKNAME];					//Ⱥ����
	TCHAR	szClubOwnerName[LEN_NICKNAME];				//Ⱥ������
	DWORD	dwClubID;									//ȺID
	DWORD	dwClubOwner;								//Ⱥ��ID
	byte	byClubLevel;								//����Ȧ�ȼ�
	WORD	wMemberNum;									//��ǰ����
	TCHAR	szNotice[128];								//����Ȧ����
	TCHAR	szMessage[128];								//����Ȧ���
	DWORD	dwFK;										//����Ȧ�з�������
	BYTE	byApplyNum;									//�µ���Ⱥ������
	BYTE	byAutoAgree;								//�Ƿ����Զ�ͬ��
	BYTE	bySex;										//0������; ����1��; 2Ů
	WORD	wLevel;										//����ȼ�
};

//�������˼�������Ȧ
struct STR_SUB_CL_CLUB_INVITE
{
	DWORD	dwClubID;									//����ȦID
	DWORD	dwTagID;									//Ŀ�����ID
};

//�������˼�������Ȧ ����
struct STR_CMD_LC_CLUB_INVITE
{
	LONG	lResultCode;								//0-�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};

//�������˵�����
struct STR_CMD_LC_CLUB_INVITE_REMIND
{
	DWORD	dwTagID;									//Ŀ��ID
};

//�������˲鿴�Լ��������б� ����
struct STR_CMD_LC_CLUB_INQUERY_LIST
{
	DWORD	dwClubID;									//����ȦID
	TCHAR	szClubName[LEN_NICKNAME];					//����Ⱥ����
	BYTE	byClubLevel;								//����Ȧ�ȼ�
	WORD	wMemberNum;									//��ǰ����
	TCHAR	szNotice[128];								//����Ȧ ����
	TCHAR	szMessage[128];								//����Ȧ ���
	DWORD	dwClubOwner;								//Ⱥ��ID
	TCHAR	szClubOwnerName[LEN_NICKNAME];				//Ⱥ���ǳ�
};

//�������˲鿴�Լ��������б� ����
struct STR_CMD_LC_CLUB_INQUERY_LIST_FINISH
{
	BYTE	byMask;										//������־ 1
};

//�������˵Ļظ�
struct STR_SUB_CL_CLUB_INVITE_RESULT
{
	DWORD	dwClubID;									//����ȦID
	byte	byAgree;									//0ͬ��; 1��ͬ��
};

//�������˵Ļظ� ����
struct STR_CMD_LC_CLUB_INVITE_RESULT
{
	LONG	lResultCode;								//0-�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};

//�������б�
struct STR_SUB_CL_CLUB_APPLICANT_LIST
{
	DWORD	dwClubID;									//����ȦID
};

// �������б� ����
struct STR_CMD_LC_CLUB_APPLICANT_LIST
{
	DWORD	dwUserID;									//�������ID
	TCHAR	szUserName[LEN_NICKNAME];					//��������ǳ�
	BYTE	byUserSex;									//����Ա�
	BYTE	byUserLevel;									//��ҵȼ�
	TCHAR	szNote[128];								//����˵��

	TCHAR	szHeadUrl[LEN_HEAD_URL];							//ͷ���ַ
};

//�������б� ����
struct STR_CMD_LC_CLUB_APPLICANT_LIST_FINISH
{
	BYTE	byMask;										//������־ 1
};

//Ⱥ��|���� 
struct STR_SUB_CL_CLUB_APPLICANT_RESULT
{
	DWORD	dwUserID;									//���ID
	DWORD	dwCludID;									//���ֲ�ID
	DWORD	dwResult;									//0ͬ�� �����ܾ�
	DWORD	dwTagID;									//������ID
};

//Ⱥ��|����  ����
struct STR_CMD_LC_CLUB_APPLICANT_RESULT
{
	LONG	lResultCode;								//0-�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��

	DWORD dwTargetID;									//Ŀ�����ID
};

//�˳�����Ȧ 
struct STR_SUB_CL_CLUB_QUIT
{
	DWORD	dwClubID;									//����ȦID
};

//�˳�����Ȧ ����
struct STR_CMD_LC_CLUB_QUIT
{
	LONG	lResultCode;								//0-����ɹ�  ����ʧ��
};

//ְ������ ����
struct STR_SUB_CL_CLUB_APPOINTMENT
{
	DWORD	dwClubID;									//����ȦID
	DWORD	dwTagID;									//Ŀ�����ID
	BYTE	byAction;									//0Ϊ�߳��û�; 1��Ϊƽ��; 2��Ϊ����Ա
};

//ְ������ ����
struct STR_CMD_LC_CLUB_APPOINTMENT
{
	LONG	lResultCode;								//������
	TCHAR	szDescribe[128];							//������Ϣ
};

//ְ������ ����
struct STR_CMD_LC_CLUB_APPOINTMENT_NOTE
{
	DWORD	dwUserID;									//Ŀ�����ID
	BYTE	byAction;									//0�߳�  1��Ϊ��ͨ��Ա 2����Ա
	TCHAR	szDescribe[128];							//������Ϣ
};

//����Ȧ����
struct STR_SUB_CL_CLUB_CHAT
{
	DWORD	dwUserID;									//���ID
	DWORD	dwTagUserID;								//˽��ʱ���͵���ҵ�ID
	BYTE	byChatMode;									//0.���ֲ�����  1.��������   2.ϵͳ����    3.˽��
	DWORD	dwClubID;									//���ֲ�ID
	TCHAR	szChat[128];								//��������
};
//����Ȧ���� ����
struct STR_CMD_LC_CLUB_CHAT
{
	BYTE	byChatMode;									//0.���ֲ�����  1.��������   2.ϵͳ����    3.˽��
	LONG	lResultCode;								//0-����ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};
//����Ȧ���� �㲥
struct STR_CMD_LC_CLUB_CHAT_BDCAST
{
	BYTE	byChatMode;									//0.���ֲ�����  1.��������   2.ϵͳ����    3.˽��

	DWORD	dwClubID;									//���ֲ�ID
	DWORD	dwUserID;									//�������ID
	DWORD	dwTagID;									//Ŀ�����ID

	TCHAR	szUserNickName[32];							//��������ǳ�
	TCHAR	szHeadUrl[256];								//�������ͷ���ַ
	TCHAR	szChat[128];								//���������������
};

#pragma endregion

#pragma region 5. ս���鿴
//����ս��ͳ�� ��ѯ
struct STR_SUB_CL_CLUB_RECORD_LIST
{
	DWORD						dwUserID;					//���ID
	DWORD						dwClubID;					//����Ȧ(�汾2)����, Ϊ0��ʾ�Ǵ���
	SYSTEMTIME					tmQueryStartData;			//��Ҫ��ѯ�Ŀ�ʼ����(��С��λΪ��)
	SYSTEMTIME					tmQueryEndData;				//��Ҫ��ѯ�Ľ�ֹ����(��С��λΪ��)
};
//����ս��ͳ�� ����
struct STR_CMD_LC_CLUB_RECORD_LIST
{
	BYTE						byMask;						//1������  2��ҳ�

	DWORD						dwUserID;					//���ID
	TCHAR						szNickName[32];				//�ǳ�
	TCHAR						szHeadUrl[LEN_HEAD_URL];	//ͷ��

	DWORD						dwAllCount;					//�ܳ���
	SCORE						lAllScore;					//����ܳ�����

	DWORD						dwWinCount;					//��Ӯ�ҳ���
	SCORE						lWinScore;					//��Ҵ�Ӯ�ҷ���
};

//����ս��ͳ�� ����
struct STR_CMD_LC_CLUB_RECORD_FINISH
{
	BYTE						byMask;						//������־
};

#pragma endregion

#pragma region ��ɾ��
//��������Ȧ����
struct STR_SUB_CL_CLUB_CREATE_CLUB_ROOM_APPLY
{
	DWORD	dwUserID;									//���ID
	DWORD	dwClubID;
	DWORD	dwKindID;									//��Ϸ��������
};

//��������Ȧ���� ����
struct STR_CMD_LC_CLUB_CREATE_CLUB_ROOM_APPLY
{
	LONG	lResultCode;								//0-����ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};

//����Ȧ�������� ����
struct STR_CMD_LC_CLUB_CREATE_ROOM_INFO
{
	LONG	lResultCode;								//0-����ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
	DWORD	dwRoomID;									//����ID
};

//�����������
struct STR_SUB_CL_CLUB_JOIN_TABLE
{
	DWORD	dwUserID;									//���ID
	int		JoinType;									//��������
	DWORD	dwTableID;									//����ID
	DWORD	dwChairID;									//����ID
	DWORD	dwCreatePassword;							//�������뷿����
	DWORD	dwJoinPassword;								//�������뷿����
};

//����������� ����
struct STR_CMD_LC_CLUB_JOIN_TABLE
{
	LONG	lResultCode;								//0-����ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
	DWORD	dwTableID;									//����ID
	DWORD	dwChairID;									//����ID
};

#pragma endregion

#endif