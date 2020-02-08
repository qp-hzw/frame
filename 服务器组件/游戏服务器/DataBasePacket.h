#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

#include "../../ȫ�ֶ���/Define.h"
#pragma pack(1)

//�û�Socket�ر��¼�
#define DBR_GP_GAME_USER_STATE					1									//����˳�
//����˳�
struct DBR_GP_UserQuitInfo
{
	DWORD							dwUserID;							//�û�ID
	BYTE							byOnlineMask;						//���߱�־ 1�������� ��2������Ϸ��3��Ϸ���ߣ�4����
};
//////////////////////////////////////////////////////////////////////////////////
//�������ݰ�
//USERID��¼
#define	DBR_CG_LOGON_USERID			100									//ID��¼
#define DBO_CG_LOGON_USERID			100									//ID��¼����

//��������Ȧ
#define DBR_CG_USER_CREATE_GROUP_ROOM	104								//��������Ȧ����
#define DBO_CG_USER_CREATE_GROUP_ROOM	104								//��������Ȧ���䷵��

//��������Ȧ
#define DBR_CG_USER_JOIN_GROUP_ROOM		106								//��������Ȧ����
#define DBO_CG_USER_JOIN_GROUP_ROOM		106								//��������Ȧ���䷵��

#define DBR_GR_MODIFY_USER_TREASURE		405								//�����û��Ƹ���Ϣ
#define DBO_SG_MODIFY_USER_TREASURE		406                            //�޸��û��Ƹ���Ϣ ����


#define DBR_GR_QUERY_LOTTERY		103									//�齱����

//ϵͳ�¼�
#define DBR_GR_WRITE_GAME_SCORE		200									//��Ϸ����
#define DBR_GR_LEAVE_GAME_SERVER	201									//�뿪��Ϸ��
#define DBR_GR_GAME_SCORE_RECORD	202									//���ּ�¼
#define DBR_GR_MANAGE_USER_RIGHT	203									//Ȩ�޹���

//�����¼�
#define DBR_GR_LOAD_ANDROID_USER	302									//���ػ���


#define DBR_GR_ADD_TABLE_INFO		413									//����������Ϣ
#define DBR_GR_START_TABLE			414									//��ʼ����
#define DBR_GR_END_TABLE			415									//��������
#define DBR_GR_ADD_TABLEUSER		416									//��ӷ����û�
#define DBR_GR_DELETE_TABLEUSER		417									//ɾ�������û�

//Club����Ȧ������Ϣ
#define DBR_CLUB_ROOM_INFO			420									//Club����Ȧ������Ϣ
#define DBR_CLUB_TABLE_INFO			421									//Club����Ȧ������Ϣ
#define DBR_CLUB_PLAYER_INFO		422									//Club����Ȧ�����Ϣ

//��Ҵ��� ������Ϣ
#define DBR_HALL_GOLD_TABLE_INFO	423									//��Ҵ��� ������Ϣ
#define DBR_HALL_GOLD_PLAYER_INFO	424									//��Ҵ��� �����Ϣ

//��Ϸ�¼�
#define DBR_GR_GAME_FRAME_REQUEST	502									//��Ϸ����

//��������
#define DBR_GR_MATCH_CONFIG			605									//���ñ�����

//��Ϸ����
#define DBR_GR_WRITE_CURRENT_STOCK		1006							//д��ǰ���ֵ
//////////////////////////////////////////////////////////////////////////////////
//�����Ϣ

//�߼��¼�
#define DBO_GR_LOTTERY_RESULT		105


//��Ϸ�¼�
#define DBO_GR_PROPERTY_FAILURE		401									//����ʧ��
#define DBO_GR_GAME_FRAME_RESULT	402									//��Ϸ���

#define DBO_GR_UPDATE_TABLE_INFO	413									//���¿�����Ϣ
#define DBO_GR_DELETE_TABLE_INFO	414									//ɾ��������Ϣ


//�����¼�
#define DBO_GR_MATCH_EVENT_START	500									//�¼���Χ
#define DBO_GR_MATCH_EVENT_END		599									//�¼���Χ
#define DBO_GR_MATCH_FEE_RESULT		500									//��������
#define DBO_GR_MATCH_RANK			501									//��������
#define DBO_GR_MATCH_QUIT_RESULT	502									//�������

#define DBO_GR_USERCONTROLVALMODIFY		1002							//���ɱ�ſ���
#
//���Ӽ�¼��Ϣ added by lizhihu  700-800
#define DBR_SC_TABLE_UPDATE_TASK_STATUS			700						//������Ϸ����״̬

//
#define	DBR_CG_CLUB_CREATE_TABLE				1100					//club����Ȧ��������
#define	DBO_GC_CLUB_CREATE_TABLE				2100					//club����Ȧ��������

#define	DBR_CG_USER_JOIN_TABLE_NO_PASS			1101					//club����Ȧ�������� -- ����Ҫ����
#define	DBO_GC_USER_JOIN_TABLE_NO_PASS			2101					//club����Ȧ�������� -- ����Ҫ����

#define	DBO_GC_CLUB_UPDATE_TABLE_INFO			1102					//club����Ȧ����������Ϣ
#define	DBO_GC_CLUB_UPDATE_PLAYER_INFO			2102					//club����Ȧ���������Ϣ

//////////////////////////////////////////////////////////////////////////////////

//ID ��¼
struct STR_DBR_CG_LOGON_USERID
{		
	//��¼��Ϣ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};
//ID ��¼����
struct STR_DBO_CG_LOGON_USERID
{		
	LONG							lResultCode;								//�������
	TCHAR							szDescribeString[LEN_MESSAGE_DESCRIBE];		//��Ϣ����

	tagUserInfo                     useInfo;                                    //�����Ϣ
};

//��������Ȧ����
struct STR_DBR_CG_USER_CREATE_GROUP_ROOM
{		
	DWORD		dwUserID;						//���������û�ID
	DWORD		dwGroupID;						//����ȦID
	BYTE		rule[LEN_PRIVATE_TABLE_RULE];	//�����������
};
//��������Ȧ���䷵��
struct STR_DBO_CG_USER_CREATE_GROUP_ROOM
{		
	DWORD       dwResultCode;							 //�����ʶ
	TCHAR		szDescribeString[LEN_MESSAGE_DESCRIBE];	 //������Ϣ
	DWORD		dwUserID;								 //���������û�ID
	DWORD		dwGroupID;								 //����ȦID
	BYTE		rule[LEN_PRIVATE_TABLE_RULE];			 //�����������
};

//��������Ȧ����
struct STR_DBR_CG_USER_JOIN_GROUP_ROOM
{		
	DWORD		dwGroupID;			//����ȦID
	DWORD		dwUserID;			//�����û�ID
};
//��������Ȧ���䷵��
struct STR_DBO_CG_USER_JOIN_GROUP_ROOM
{		
	DWORD       dwResultCode;							 //�����ʶ
	TCHAR		szDescribeString[LEN_MESSAGE_DESCRIBE];	 //������Ϣ
	DWORD		dwGroupID;								//����ȦID
};

//������Ϸ����״̬
struct STR_DBR_CG_TABLE_UPDATE_TASK_STATUS
{		
	DWORD		dwUserID;			//���ID
	BYTE		cbTableMode;		//����ģʽ����������ҡ���ʯ��
	BYTE		cbCurGameCount;		//��ǰ��Ϸ����
};

// �����û��Ƹ���Ϣ
struct STR_DBR_GR_MODIFY_USER_TREASURE
{
	DWORD		dwUserId;			//���ID
	DWORD		dwKind;				//��Ϸ���� ���ݿ�ͳ����Ҫ
	DWORD		dwTableID;			//����ID
	BYTE		byTableMode;		//����ģʽ
	BYTE		byRound;			//��ǰ��Ϸ����  0��ʾ��������  FF(255)��ʾ���ӽ���(��ֽ���)
	SCORE		lUserTreasure;		//��ҲƸ����  ������ʾ�۳�;  ������ʾ����
	BYTE		byWin;				//С������: 0ƽ 1Ӯ 2��    �������: 1��ʾ��Ӯ��

};

// �����û���������ҡ���ʯ����
struct STR_DBO_GR_MODIFY_USER_TREASURE
{
	DWORD			dwResultCode;							 //�����ʶ
	TCHAR			szDescribeString[LEN_MESSAGE_DESCRIBE];	 //������Ϣ
	DWORD			dwUserID;								 //�û�ID
	SCORE           lUserDiamond;							 //�û���ʯ
	SCORE			lUserGold;								 //�û����
	SCORE			lOpenRoomCard;							 //�û�����
};

//��Ϸ����
struct DBR_GR_WriteGameScore
{
	//�����Ϣ
struct tagVariationInfo
{
	//������Ϣ
	SCORE							lScore;								//�û�����
	SCORE							lGrade;								//�û��ɼ�
	SCORE							lInsure;							//�û�����
	SCORE							lRevenue;							//��Ϸ˰��	
	SCORE							lChoushui;							//��ˮֵ
	DWORD							dwChoushuiType;						//��ˮ����

	//��Ϸ��Ϣ
	DWORD							dwWinCount;							//ʤ������
	DWORD							dwLostCount;						//ʧ������
	DWORD							dwDrawCount;						//�;�����
	DWORD							dwFleeCount;						//��������
	DWORD							dwPlayTimeCount;					//��Ϸʱ��

	//ȫ����Ϣ
	DWORD							dwUserMedal;						//�û�����
	DWORD							dwExperience;						//�û�����
	LONG							lLoveLiness;						//�û�����
};

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ

	//�û���Ϣ
	DWORD							dwDBQuestID;						//�����ʶ
	DWORD							dwInoutIndex;						//��������

	//�ɼ�����
	tagVariationInfo				VariationInfo;						//��ȡ��Ϣ

	//����ֵ
	SCORE							lControlScore;						//����ֵ
};


//д��Ϸʤ����¼
struct DBR_GR_WriteUserWinCount
{
	DWORD dwUserID;
	DWORD dwCount;
};

//����齱
struct DBR_GR_QueryLottery
{
	DWORD	dwUserID;				//�û�ID
	BYTE	byType;					//�齱��ʽ 0-��� 1-ҡ��
};

//�齱����
struct DBO_GR_LotteryResult
{
	BYTE	byType;					//�������  0-û�г鵽 1-�һ��� 2-���  3-���� 4-��ʯ 5-���� ����-����Ԥ��
	BYTE	byIndex;				//ҡ����ţ�-1��ʾΪ����� 0-11Ϊҡ��
	DWORD	dwRewardCount;			//������������
	TCHAR	szPacketID[128];				//�һ���
	TCHAR	szDescribe[128];		//����
};

//Club����Ȧ��������
struct STR_DBO_GC_CLUB_CREATE_TABLE
{
	STR_SUB_CG_USER_CREATE_ROOM strCreateRoom;				//�������
	BYTE							byTableMode;			//1.��ͨ����  2.˽������
	DWORD							dwClubRoomID;			//���ֲ�����ID
	long							lResultCode;			//��0��ʾ ���᷿������
};

//�������� ����Ҫ����
struct STR_DBO_CG_USER_JOIN_TABLE_NO_PASS
{
	DWORD							dwPassword;		//��������
	double							dLongitude;		//����
	double							dLatitude;		//γ��
	long							lResultCode;	//��0��ʾ û���ҵ����Լ���ķ���
	long							lResultCode2;	//��0��ʾ ��������

	DWORD							dwClubRoomID;	//���ֲ�������
	STR_SUB_CG_USER_CREATE_ROOM		strCreateRoom;	//�������
};

//�������� ��Ҵ�������
struct STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD
{
	DWORD							dwKindID;		//��ϷID
	BYTE							byGameType;		//������, �м���, �߼���, ������
	//DWORD							dwPassword;		//��������
	long							lResultCode;	//��0��ʾ û���ҵ����Լ���ķ���
	DWORD							dwMinGold;		//���뷿���������С���

	STR_SUB_CG_USER_CREATE_ROOM		strCreateRoom;	//�������
};

//�뿪����
struct DBR_GR_LeaveGameServer
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwInoutIndex;						//��¼����
	DWORD							dwLeaveReason;						//�뿪ԭ��
	DWORD							dwOnLineTimeCount;					//����ʱ��

	//ϵͳ��Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//��Ϸ��¼
struct DBR_GR_GameScoreRecord
{
	//������Ϣ
	WORD							wTableID;							//���Ӻ���
	WORD							wUserCount;							//�û���Ŀ
	WORD							wAndroidCount;						//������Ŀ

	//���˰��
	SCORE							lWasteCount;						//�����Ŀ
	SCORE							lRevenueCount;						//˰����Ŀ
	//SCORE							wCurrentStockScore;					//��ǰ���
	SCORE							wChangeStockScore;					//��ǰ���ı�ֵ

	//ͳ����Ϣ
	DWORD							dwUserMemal;						//������Ŀ
	DWORD							dwPlayTimeCount;					//��Ϸʱ�� TODOLATER ɾ��

	//ʱ����Ϣ
	SYSTEMTIME						SystemTimeStart;					//��ʼʱ��
	SYSTEMTIME						SystemTimeConclude;					//����ʱ��

	//���ּ�¼
	WORD							wRecordCount;						//��¼��Ŀ
	tagGameScoreRecord				GameScoreRecord[MAX_CHAIR];			//��Ϸ��¼
};

//////////////////////////////////////////////////////////////////////////////////

//������Ϣ
struct DBO_GR_GameParameter
{
	//������Ϣ
	WORD							wMedalRate;							//���ƻ���
	WORD							wRevenueRate;						//����˰��

	//�汾��Ϣ
	DWORD							dwClientVersion;					//�ͻ��汾
	DWORD							dwServerVersion;					//����汾
};

//������Ϣ
struct DBO_GR_GameAndroidInfo
{
	//��������
	struct tagAndroidParameter
	{
		//��������
		DWORD							dwUserID;							//�û���ʶ
		TCHAR							szNickName[LEN_NICKNAME];			//�ʺ��ǳ�
		TCHAR							szHeadUrl[256];						//ͷ���ַ

		//�û�����
		BYTE							cbGender;							//�û��Ա�
		SCORE							lOpenRoomCard;						//������
		SCORE							lDiamond;							//��ʯ
		SCORE							lGold;								//���
	};

	LONG							lResultCode;						//�������
	WORD							wAndroidCount;						//�û���Ŀ
	tagAndroidParameter				AndroidParameter[MAX_ANDROID];		//������Ϣ
};


//��������
struct DBO_GR_MatchRank
{
	BYTE							cbRank;								//��������
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	LONG							lMatchScore;						//�û��÷�
};

//Ȩ�޹���
struct DBR_GR_ManageUserRight
{
	DWORD							dwUserID;							//Ŀ���û�
	DWORD							dwAddRight;							//���Ȩ��
	DWORD							dwRemoveRight;						//ɾ��Ȩ��
	bool							bGameRight;							//��ϷȨ��
};

//��������
struct DBR_GR_MatchFee
{
	//��������
	DWORD							dwUserID;							//�û�ID
	DWORD							dwMatchFee;							//��������

	//ϵͳ��Ϣ
	DWORD							dwInoutIndex;						//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	DWORD							dwMatchID;							//����ID
	DWORD							dwMatchNO;							//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//������ʼ
struct DBR_GR_MatchStart
{
	DWORD							dwMatchID;						//������ʶ
	DWORD							dwMatchNo;						//��������
	WORD							wMatchCount;					//�ܳ���
	LONG							lInitScore;						//��ʼ����
};

//��������
struct DBR_GR_MatchOver
{
	DWORD							dwMatchID;						//������ʶ
	DWORD							dwMatchNo;						//��������
	WORD							wMatchCount;					//�ܳ���
};

//��������
struct DBR_GR_MatchReward
{
	DWORD								dwUserID;							//�û� I D
	DWORD								dwMatchID;							//���� I D 
	DWORD								dwMatchNO;							//��������
	WORD								wRank;								//��������
	SCORE								lMatchScore;						//�����÷�
	DWORD								dwExperience;						//�û�����
	DWORD								dwGold;								//��Ϸ��
	DWORD								dwMedal;							//�û�����
	DWORD								dwClientAddr;						//���ӵ�ַ
};

////��������������
//struct MATCH_CONFIG
//{
//	//�׶���Ϣ
//	struct STAGE_INFO
//	{
//		TCHAR	szName[MAX_MATCH_TITLE];	//�����������������..
//		WORD	wXJCount;					//С����
//		WORD	wPlayerCount;				//����
//		WORD	wJinJi_Rule;				//��������   ͬ���Ƚ�/ȫ���Ƚ�
//		WORD	wJinJi_Count;				//��������
//	};
//
//	//��������Ϣ
//	WORD		wMatchID;							//������ID��ʶ
//	TCHAR		szTitle[MAX_MATCH_TITLE];			//����
//	TCHAR		szDescribe[MAX_MATCH_TITLE];		//��������
//	STAGE_INFO	stage[MAX_MATCH_STAGE];				//�׶���Ϣ
//	WORD		wStageSize;							//�׶θ���
//	SCORE		llBaseScore;						//�׷�
//
//	//����������
//	DWORD		dwStartTime;						//��ʼʱ�� ��-1��ʾ����������
//	WORD		wStartType;							//��ʼ����  (0-������ʼ  1-��ʱ��ʼ)
//	DWORD		dwLowestPlayer;						//���ٲ�������
//	SCORE		llFee;								//������
//	SCORE		llReword;							//����
//
//	//��������Ϣ
//	DWORD		dwRobotCount;						//����������
//	DWORD		dwRobotJoinMin;						//��������С��������
//	DWORD		dwRobotJoinMax;						//����������������
//};

//��������
struct STR_DBR_GR_MATCH_CONFIG
{
	MATCH_CONFIG	match_list[MAX_MATCH];
	WORD			match_size; 
};

//ϵͳ��Ϣ
struct DBR_GR_SystemMessage
{
	BYTE                            cbMessageID;                        //��ϢID
	BYTE							cbMessageType;						//��Ϣ����
	BYTE                            cbAllRoom;                          //ȫ�巿��
	DWORD							dwTimeRate;						    //ʱ��Ƶ��
	TCHAR							szSystemMessage[LEN_USER_CHAT];		//ϵͳ��Ϣ
};

//�����Ѷȵ���
struct DBR_GR_RoomLevelModify
{
	WORD							wSeverID;							//������ID
	WORD							wGameLevel;							//��Ϸ�Ѷ�
};

//�������ֵ����
struct DBR_GR_RoomControlValModify
{
	DWORD							wSeverID;							//������ID
	SCORE							wControlVal;						//����ֵ
};

//���������Ϣ
struct DBR_GR_RoomControlInfo
{
	WORD							wSeverID;							//������ID
	SCORE							wControlVal;						//����ֵ
	DWORD							wGameLevel;							//�������׶�
};

//��ѯ�������ֵ
struct DBR_GR_ReadRoomControlVal
 {
	DWORD							wSeverID;							//������ID
};
//�û�����ֵ����
struct DBR_GR_UserControlValModify
{
	DWORD							dwUserID;							//�û� I D
	SCORE							wControlVal;						//����ֵ
};

//����������
struct DBR_GR_GetGameRoomConfig
{
	WORD							wSeverID;							//������ID
};

//����������
struct DBR_GR_GameRoomConfig
{
	WORD							wSeverID;							//������ID
	TCHAR							szConfigName[50];					//��������
	TCHAR							szConfigValue[1024*10];				//��������	
	SCORE							wCurrentStock;						//��ǰ���
};

//д��ǰ����ֵ
struct DBR_GR_WriteCurrentStock
{
	SCORE							wCurrentStock;						//��ǰ���
};
//////////////////////////////////////////////////////////////////////////////////

//���¿�����Ϣ
struct DBR_GR_UpdateTableInfo
{
	DWORD dwTableID;			//����ID
	DWORD dwPassword;			//��������
	DWORD dwCreaterID;			//�������ID
	WORD  wJuShu;				//����
	BYTE  byMode;				//ģʽ
	BYTE  byZhuangType;			//ׯ����
	BYTE  byPlayerCount;		//�����
	BYTE  byMaxPlayerCount;		//��������
	BYTE  byIsStart;			//�Ƿ�ʼ
	BYTE  byCost;				//�ۿ���
	TCHAR szTime[24];			//����ʱ��
};


//����û�������
struct DBR_GP_AddTableUser
{
	DWORD dwUserID;
	DWORD dwTableID;
};

//ɾ�������û�
struct DBR_GP_DeleteTableUser
{
	DWORD dwUserID;
	DWORD dwTableID;	
};

struct DBR_GP_EndTable
{
	DWORD dwTableID;			//����ID
	DWORD dwUserID[5];			//�û�ID
	DWORD dwScore[5];			//�û�����
};

//���ؿ�����Ϣ
struct DBO_GP_TableInfo
{
	DWORD dwTableID;			//����ID
	DWORD dwPassword;			//��������
	DWORD dwCreaterID;			//�������ID
	WORD  wJuShu;				//����
	BYTE  byMode;				//ģʽ
	BYTE  byZhuangType;			//ׯ����
	BYTE  byPlayerCount;		//�����
	BYTE  byMaxPlayerCount;		//��������
	BYTE  byIsStart;			//�Ƿ�ʼ
	BYTE  byCost;				//�ۿ���
	TCHAR szTime[24];			//����ʱ��

	DWORD dwUserID[5];			//���ID
	TCHAR szNickName[5][LEN_NICKNAME];		//����ǳ�
};


//���ؿ�����Ϣ
struct DBO_GP_EndTableInfo
{
	DWORD dwTableID;			//����ID
	DWORD dwPassword;			//��������
	DWORD dwCreaterID;			//�������ID
	WORD  wJuShu;				//����
	BYTE  byMode;				//ģʽ
	BYTE  byZhuangType;			//ׯ����
	BYTE  byPlayerCount;		//�����
	BYTE  byMaxPlayerCount;		//��������
	BYTE  byIsStart;			//�Ƿ�ʼ
	BYTE  byCost;				//�ۿ���
	TCHAR szTime[24];			//����ʱ��

	DWORD dwUserID[5];			//���ID
	TCHAR szNickName[5][LEN_NICKNAME];		//����ǳ�
	DWORD szScore[5];
};



//pure����Ȧ������Ϣ
struct STR_DBR_CLUB_ROOM_INFO
{
	DWORD	dwUserID;						//���ID
	DWORD	dwClubID;						//���ֲ���ʶ
	DWORD	dwGameID;						//��ϷID
	BYTE	byModeID;						//1���� 2���
	TCHAR	szKindName[32];					//��Ϸ����

	//��ҳ�����
	DWORD	dwServiceGold;					//����ѣ����̶���ˮ
	DWORD	dwRevenue;						//��ʤ�ߵĹ̶���ˮ

	//��ע && ������
	BYTE	byMask;							//1 AA֧��;  2��Ӯ��֧��
	DWORD	dwDizhu;						//��ע
	DWORD	dwGold;							//������

	BYTE	byPlayerNum;					//��ǰ�������Ϸ����
	BYTE	DissolveMinute;					//Ⱥ�����Ʒ����ɢʱ�䣬0Ϊ������
	BYTE	byAllRound;						//��Ϸ�ܾ���
	BYTE	byChairNum;						//������Ŀ

	//��ҳ���������Ҫ�� ���ݿ�����ĵ����ֶ�Ϊ׼, ����ֱ�ӽ������ֶ� (�����, ��ˮ, ֧����ʽ, ��ע, ������)
	BYTE	szRealRoomRule[256];			//16���ƹ����ַ���
};


//pure����Ȧ������Ϣ
struct STR_DBR_CLUB_TABLE_INFO
{
	DWORD		dwUserID;		//���ID
	DWORD		dwClubRoomID;	//���ֲ�����ID
	DWORD		dwTableID;		//��������
	DWORD		dwTableState;	//0.���ӿ��� 1.�����Ѿ�����  2.��������
	DWORD		dwLockState;	//0.������Ҫ���� 1.���Ӳ���Ҫ����
	BYTE		byCurrentRound;	//��Ϸ��ǰ����
	BYTE		byMask;			//1���� 2���� 3��ɢ

	//added by WangChengQing ��������������ʱ��, ��֪ͨ���ݿ��ÿ�����, ������׼�������ֶ�
	BYTE		byCompanyID;	//�����ֶ�
	DWORD		dwGameID;		//gameID
};

//pure����Ȧ�û���Ϣ
struct STR_DBR_CLUB_PLAYER_INFO
{
	DWORD		dwUserID;		//���ID
	DWORD		dwTableID;		//��������
	BYTE		byChairID;		//���Ӻ�
	BYTE		byMask;			//1���� 2�뿪
};


//���ݿ��ж��Ƿ���Խ�������   1.��������ǹ��������, ����Ҳ��ڹ���, ���ܽ�������
struct STR_DBR_JOIN_ROOM
{
	DWORD							  dwTableID;	//����ID
	DWORD							  dwUserID;		//���ID
	long							  lResultCode;	//������
};

//��Ҵ��� ������Ϣ
struct STR_DBR_HALL_GOLD_TABLE_INFO
{
	DWORD		dwUserID;		//���ID
	DWORD		dwKindID;		//��ϷID
	BYTE		byGameType;		//��Ϸ������  1.������  2.�м���  3.�߼���  4.������
	DWORD		dwTableID;		//��������
	DWORD		dwTableState;	//0.���ӿ��� 1.�����Ѿ�����  2.��������
	BYTE		byMask;			//1���� 2���� 3��ɢ

	//added by WangChengQing ��������������ʱ��, ��֪ͨ���ݿ��ÿ�����, ������׼�������ֶ�
	DWORD		dwGameID;		//gameID
};

//��Ҵ��� �û���Ϣ
struct STR_DBR_HALL_GOLD_PLAYER_INFO
{
	DWORD		dwUserID;		//���ID
	DWORD		dwTableID;		//��������
	BYTE		byChairID;		//���Ӻ�
	BYTE		byMask;			//1���� 2�뿪
};

//���ض�������
struct STR_DBR_GR_LOAD_OFFLINE
{
	BYTE		byMystery;		//�����ֶ�
	DWORD		dwGameID;		//��ϷID
};

#pragma pack()

#endif
