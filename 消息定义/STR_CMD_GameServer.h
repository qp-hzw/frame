#ifndef STR_CMD_GAME_SERVER_HEAD_FILE
#define STR_CMD_GAME_SERVER_HEAD_FILE
#include "../���������/��Ϸ������/TableFrameBase.h" //��Ϊ�Ľṹ��tagTableRule
#pragma pack(1)

#pragma region ������

#define LEN_PRIVATE_TABLE_RULE		256									//�Խ����������ݳ���
#define NUM_MAX_GOURP_CREATE_ROOM	10									//����Ȧ��������󷿼���

//������Ϣ
#define DTP_GR_TABLE_PASSWORD		1									//��������

//�û�����
#define DTP_GR_NICK_NAME			10									//�û��ǳ�
#define DTP_GR_GROUP_NAME			11									//��������
#define DTP_GR_UNDER_WRITE			12									//����ǩ��

//������Ϣ
#define DTP_GR_USER_NOTE			20									//�û���ע
#define DTP_GR_CUSTOM_FACE			21									//�Զ�ͷ��

//////////////////////////////////////////////////////////////////////////////////
//����ģʽ
#define TABLE_MODE_FK				0									//����ģʽ
#define TABLE_MODE_MATCH			1									//����ģʽ
#define TABLE_MODE_GOLD				2									//���ģʽ
#define TABLE_MODE_FK_GOLD			3									//�������ģʽ
#define TABLE_MODE_CLUB				4									//����Ȧģʽ

//�Ƹ�����
#define TREASURE_FK					1									//����
#define TREASURE_GOLD				2									//���
#define TREASURE_DIAMOND			3									//��ʯ
#define TREASURE_JF					4									//����


//�������
#define REQUEST_FAILURE_NORMAL		0									//����ԭ��
#define REQUEST_FAILURE_PASSWORD	3									//�������

//////////////////////////////////////////////////////////////////////////////////
#pragma endregion

#pragma region �����ṹ��
//�����������
struct rule_item
{
	TCHAR	                       szHeadName[15];						//����ͷ
	TCHAR	                       szItemValue[4][10];				    //����ѡ��ֵ
};
//�������
struct rule_arry
{
	rule_item	                   ItemArry[20];						//�������
};

#pragma endregion

#pragma region MDM_LOGON ��¼ģ��
//ID ��¼
struct STR_SUB_CG_LOGON_USERID
{
	DWORD							dwUserID;									//�û� I D
	TCHAR							szPassword[LEN_MD5];						//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];				//��������
	DWORD							dwSubGameVersion;							//����Ϸ�汾
};
//ID ��¼����
struct STR_CMD_GC_LOGON_USERID
{
	LONG							lResultCode;								//�������
	TCHAR							szDescribeString[LEN_MESSAGE_DESCRIBE];		//��Ϣ����
	DWORD							dwKindID;									//��ϷKindID
};
#pragma endregion

#pragma region MDM_USER �û�����

//���봴������
struct STR_SUB_CG_USER_CREATE_ROOM
{
	BYTE							byGameMode;				//��Ϸģʽ 0����Լ��; 1����ģʽ; 2���ģʽ; 3���Լ��; 4����Ȧ
};

//���÷������
struct STR_SUB_CG_USER_SET_ROOM_RULE
{
	BYTE							byGameMode;				//��Ϸģʽ 0����Լ��; 1����ģʽ; 2���ģʽ; 3���Լ��; 4����Ȧ
	BYTE							byChoose[20];			//����ѡ��
};


//club����Ȧ��������
struct STR_SUB_CG_USER_CREATE_TABLE
{
	DWORD							dwUserID;				//�û�ID
	DWORD							dwClubRoomID;			//���ֲ�����
	BYTE							byTableMode;			//1.��ͨ����  2.˽������
};
//club��������ɹ�����
struct STR_CMD_GC_USER_CREATE_TABLE
{
	DWORD							dwPassword;				//��������
	WORD							wChairID;				//�û����Ӻ�
	BYTE							byAllGameCount;			//�ܾ���
	BYTE							byGameMode;				//��Ϸģʽ 0-���� 1-���ӱ�
	BYTE							byZhuangType;			//��ׯģʽ 0-��ׯ 1-��ׯ 2-�̶�ׯ
};

//�������� -- ��Ҫ����
struct STR_SUB_CG_USER_JOIN_FK_ROOM
{
	DWORD							dwPassword;		//��������
};

//�������� -- ����Ҫ����
struct STR_SUB_CG_USER_JOIN_TABLE_NO_PASS
{
	DWORD							dwUserID;		//���ID
	DWORD							dwClubRoomID;	//���ֲ�����
	double							dLongitude;		//����
	double							dLatitude;		//γ��
};

//�������� -- ���������ҳ�����
struct STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM
{
	BYTE							byType;			//1.������   2.�м���   3.�߼���   4.������
};

//��������
struct STR_SUB_CG_USER_RECONNECT_ROOM
{
	//BYTE	cbMask;			//0-������ 1-����  
};
//������������
struct STR_CMD_GC_USER_RECONNECT_ROOM
{
	BYTE retCode;						//����ֵ��0�ɹ�  1ʧ��

	tagTableRule					strTableRule;			//�����ķ������

	DWORD dwPassWord;					//��������
	WORD  wChairID;						//���Ӻ�
	
	BYTE cbGameCount;            //��Ϸ����
    BYTE cbPlayerCount;          //�������
    BYTE cbPayType;              //֧����ʽ
    DWORD lSinglePayCost;         //����֧������
    BYTE gameMod;                //��Ϸģʽ
};

//��������Ȧ����
struct STR_SUB_CG_USER_CREATE_GROUP_ROOM
{
	DWORD		dwUserID;								//���������û�ID
	DWORD		dwGroupID;								//����ȦID
	BYTE		rule[LEN_PRIVATE_TABLE_RULE];			//�����������
};
//��������Ȧ����
struct STR_CMD_GC_USER_CREATE_GROUP_ROOM
{
	DWORD       dwResultCode;							 //�����ʶ
	TCHAR		szDescribeString[LEN_MESSAGE_DESCRIBE];	 //������Ϣ
};

//�û�����
struct STR_SUB_CG_USER_STANDUP
{
	WORD							wTableID;							//����λ��		TODO �ͻ������֪������ID��Ӧ��ֻ֪����������Ŷ�
	WORD							wChairID;							//����λ��
	BYTE							cbForceLeave;						//ǿ���뿪
};

//��������Ȧ����
struct STR_SUB_CG_USER_JOIN_GROUP_ROOM
{
	DWORD		dwGroupID;			//����ȦID
	DWORD		dwUserID;			//�û�ID
};

//�޸��û��Ƹ���Ϣ����
struct STR_CMD_GC_USER_MODIFY_TREASURE
{
	SCORE           lUserDiamond;							 //�û���ʯ
	SCORE			lUserGold;								 //�û����
	SCORE			lOpenRoomCard;							 //�û�����
};

//ϵͳ��Ϣ֪ͨ
struct STR_CMD_GR_FRAME_TREASURE_NO
{
	WORD  wChairID;			 //�¼��û����Ӻ�
	TCHAR szDescribe[128];   //��Ϣ����
};

//������淿��
struct CMD_GR_JoinDwRoom
{
	BYTE byDwID;			//����ID ����ʱֻ������ ֵ=1
	BYTE byMode;			//ģʽ��Ԥ��
	BYTE byRule[64];		//Ԥ��
};

//������淿�ɹ�
struct CMD_GR_Join_DwTable_Success
{
	BYTE byDwID;			//����ID ����ʱֻ������ ֵ=1
	BYTE byMode;			//ģʽ��Ԥ��
	WORD wChairID;			//���Ӻ�
	BYTE byRule[64];		//Ԥ��
};

//������ҷ���
struct CMD_GR_Create_Gold_Room
{
	int dwCellScore;		//��ҳ��׷�
};

//����ɹ�
struct CMD_GR_Join_GoldTable_Success
{
	WORD							wChairID;
};

//���󷿼��б�
struct CMD_GR_GetTableList
{
	BYTE							GameMode;	//��Ϸģʽ
	BYTE							GoldRank;	//��ҵȼ�
	DWORD							dwUserID;	//�û�ID
};

//������Ϣ TOODNOW ��Щ�����϶���Ҫ��д
struct TableInfo
{
	//�������
	DWORD	dwTablePassword;
	DWORD	wCellScore;
	BYTE	byMaxPlayerCount;
	BYTE	byCurrPlayerCount;
	DWORD	dwJoinScore;
	DWORD	dwLeaveScore;
	//��Ϸ����
	BYTE	ZhangMao;					//��ë
	BYTE	XuanFengGang;				//�����
	BYTE	QiXiaoDui;					//��С��
	BYTE	QingYiSe;					//��һɫ
	BYTE	KaiHuaGang;					//�ܿ�
};

//�����б�
struct CMD_GR_TableListResult
{
	BYTE		byCount;				//��������
	BYTE		GoldRank;				//0������ 50-100 1�߼��� 100-500 2������ 500-5000
	TableInfo	tableInfoList[20];
};

//�齱֪ͨ
struct CMD_GR_LotteryNotify
{
	BYTE	byType;					//����
	DWORD	dwTimeLeft;				//��ʱ���룩
};

//����ҡ��
struct CMD_GR_QueryLottery
{
	DWORD	dwUserID;				//�û�ID
	BYTE	byType;					//�齱��ʽ 0-��� 1-ҡ��
};

//�齱���
struct CMD_GR_LotteryResult
{
	BYTE	byType;					//�������  0-û�г鵽 1-�һ��� 2-���  3-���� 4-��ʯ 5-���� ����-����Ԥ��
	BYTE    byIndex;				//0-11
	DWORD	dwRewardCount;			//������������
	TCHAR	szPacketID[128];				//�һ���
	TCHAR	szDescribe[128];		//����
};

//����
struct CMD_GF_XUFEI
{
	WORD							wChairID;
	BYTE							bFangzhu;
};

//�Թ�����
struct CMD_GR_UserLookon
{
	WORD							wTableID;							//����λ��
	WORD							wChairID;							//����λ��
};

//��������
struct CMD_GR_UserSitDown
{
	WORD							wTableID;							//����λ��
	WORD							wChairID;							//����λ��
	TCHAR							szPassword[LEN_PASSWORD];			//��������
};

//�����û� 
struct CMD_GR_UserInvite
{
	WORD							wTableID;							//���Ӻ���
	DWORD							dwUserID;							//�û� I D
};

//�����û����� 
struct CMD_GR_UserInviteReq
{
	WORD							wTableID;							//���Ӻ���
	DWORD							dwUserID;							//�û� I D
};

//����ʧ��
struct CMD_GR_RequestFailure
{
	DWORD							lErrorCode;							//�������
	TCHAR							szDescribeString[256];				//������Ϣ
};

//�����û�
struct CMD_GR_C_InviteUser
{
	WORD							wTableID;							//���Ӻ���
	DWORD							dwSendUserID;						//�����û�
};

//�����û�
struct CMD_GR_S_InviteUser
{
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//�û��ܾ�����������
struct CMD_GR_UserRepulseSit
{
	WORD							wTableID;							//���Ӻ���
	WORD							wChairID;							//����λ��
	DWORD							dwUserID;							//�û� I D
	DWORD							dwRepulseUserID;					//�û� I D
};

//֪ͨ�û�
struct CMD_GF_OnlinePlayers
{
	WORD wOnlinePlayers;			//��������
};

//�û�׼��
struct STR_SUB_CG_USER_READY
{
	BYTE byType;					//0-Ready  1-cancel
};

//׼������
struct STR_CMD_GC_USER_READY
{
	BYTE cbRet;						//0-�ɹ� 1-ʧ��
	BYTE cbType;					//0-׼������ 1-ȡ��׼������
};

struct STR_CMD_ROOM_RULE
{
	tagTableRule common;			 //�������� frameͨ�÷������
	DWORD TableID;                   //�����
	BYTE bStart;                     //��Ϸ�Ƿ��ѿ�ʼ 0δ��ʼ, 1��ʼ
};

//��ҳ�������Ϣ ����
struct STR_CMD_GC_USER_GOLD_INFO
{
	BYTE bGoldMod;// ����ȼ� 1.���� 2.�м� 3.�߼� 4.����

	DWORD dwScore;  //�׷�
	DWORD dwGold;  //�볡���
};

//////////////////////////////////////////////////////////////////////////////////

//�����־
#define UR_LIMIT_SAME_IP			0x01								//���Ƶ�ַ
#define UR_LIMIT_WIN_RATE			0x02								//����ʤ��
#define UR_LIMIT_FLEE_RATE			0x04								//��������
#define UR_LIMIT_GAME_SCORE			0x08								//���ƻ���

//�û�����
struct CMD_GR_UserRule
{
	BYTE							cbRuleMask;							//��������
	WORD							wMinWinRate;						//���ʤ��
	WORD							wMaxFleeRate;						//�������
	LONG							lMaxGameScore;						//��߷��� 
	LONG							lMinGameScore;						//��ͷ���
};

//�����û���Ϣ
struct CMD_GR_UserInfoReq
{
	DWORD                           dwUserIDReq;                        //�����û�
	WORD							wTablePos;							//����λ��
};

//�����û���Ϣ
struct CMD_GR_ChairUserInfoReq
{
	WORD							wTableID;							//���Ӻ���
	WORD							wChairID;							//����λ��
};

//�߳��û�
struct CMD_GR_KickUser
{
	DWORD							dwTargetUserID;						//Ŀ���û�
};
#pragma endregion

#pragma region  MDM_GR_MATCH ��������

//����������
struct STR_SUB_CG_MATCH_APPLY
{
	WORD			wMatchID;		//������ID
};

//��������
struct STR_CMD_GC_MATCH_APPLY
{
	BYTE			byResult;			//0-�ɹ�  1-ʧ��
	WORD			wMatchID;
};

//����ȡ��
struct STR_CMD_GC_MATCH_CANCEL
{
	TCHAR			szDescribe[128];	//Ϊɶȡ��
};

//ȡ����������
struct STR_CMD_GC_MATCH_UNAPPLY
{
	BYTE			byResult;			//0-�ɹ�  1-ʧ��
	WORD			wMatchID;
};

//���������󷵻�
struct STR_CMD_GC_MATCH_QUERY_PLAYER
{
	WORD			wMatchID;		//ID
	DWORD			dwApplyCount;	//��������
	DWORD			dwTimer;		//ʣ��ʱ��
};

//�����׶ν�� ����
struct STR_CMD_GC_MATCH_RESULT_JINJI
{
	WORD			wRanking;			//����
	TCHAR			szStageName[16];		//�׶���
};

//�����׶ν�� ��̭
struct STR_CMD_GC_MATCH_RESULT_TAOTAI
{
	WORD			wRanking;			//����
	TCHAR			szStageName[16];		//�׶���
};

//�����Լ�����
struct STR_CMD_CG_MATCH_RANKING_MY
{
	TCHAR			szStageName[16];	//�׶���
	DWORD			dwRanking;			//�������
	DWORD			dwAllUserCount;		//�����
	DWORD			dwCurCount;			//С����
	DWORD			dwAllCount;			//�ܾ���
};

//��������
struct STR_CMD_GC_MATCH_RANKING
{
	TCHAR			szName[LEN_NICKNAME];		//�ǳ�
	WORD			wRanking;					//����
	SCORE			llScore;					//�ܷ�
};

//����
struct STR_CMD_GC_MATCH_JUESAI_RECODE
{
	TCHAR			szResult[16];				//���� ���Ǽ�
	WORD			wRanking;					//����
	DWORD			dwRewordType;				//��������  0-��� 1-��ʯ 2-���� 3-����
	DWORD			dwReword;					//����
};

//�ȴ�����
struct STR_CMD_GC_MATCH_WAIT_COUNT
{
	WORD			wait_count;
};

//������������
struct STR_CMD_GC_MATCH_APPLY_OFFLINE
{
	DWORD			dwStartType;
	DWORD			dwApplyCount;
	DWORD			dwLowPlayer;
	DWORD			dwLeaveTime;
};

#pragma endregion

#pragma region MDM_G_FRAME �������
//��Ϸ����
struct CMD_GF_GameOption
{
};

//�Թ�����
struct CMD_GF_LookonConfig
{
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbAllowLookon;						//�����Թ�
};

//�Թ�״̬
struct CMD_GF_LookonStatus
{
	BYTE							cbAllowLookon;						//�����Թ�
};

//��Ϸ����
struct CMD_GF_GameStatus
{
	BYTE							cbUserAction;							//��Ϸ״̬
	tagUserInfo						UserInfo;								//�����Ϣ
};

//�û�����
struct STR_SUB_RG_FRAME_CHAT
{
	DWORD							dwUserID;							//�û�ID
	DWORD							dwMsgID;							//������ʶ
};

//�û�����
struct STR_CMD_GR_FRAME_CHAT
{
	DWORD							dwUserID;							//��Ϣ��ɫ
	DWORD							dwChairID;							//�����û�
	DWORD							dwMsgID;							//Ŀ���û�
};

//�û�����
struct CMD_GF_C_UserExpression
{
	WORD							wItemIndex;							//��������
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//�û�����
struct CMD_GF_S_UserExpression
{
	WORD							wItemIndex;							//��������
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//�����ɢ����
struct STR_SUB_RG_FRAME_ASK_DISMISS
{
	
};
//�㲥�����ɢ����
struct STR_CMD_GR_FRMAE_VOTE_DISMISS
{
	BYTE							cbChairID;		//id
	BYTE							cbAgree;		//0-ͬ��  1-��ͬ��
	BYTE							cbApplyChair;	//�����ɢ�ߵ�chair
};

//�����ɢ���� ���
struct STR_CMD_GR_FRMAE_ASK_DISMISS_RESULT
{
	LONG							lResultCode;								//������� 0-ͶƱ��ɢ����
	TCHAR							szDescribeString[LEN_MESSAGE_DESCRIBE];		//��Ϣ����
};

//�����ɢ����
struct STR_SUB_RG_FRAME_VOTE_DISMISS
{
	BYTE					cbAgree;						//�Ƿ�ͬ���ɢ		0-��ͬ��	1-ͬ��
};
//�㲥��ɢ������
struct STR_CMD_GR_FRAME_DISMISS_RESULT
{
	BYTE					cbDismiss;						//0-δ��ɢ  1-��ɢ�ɹ����û�������	2-��ɢʧ��
};

//����У���û�GPSλ����Ϣ
struct STR_SUB_CG_COM_CHECK_USER_GPS
{
	BYTE					cbRequestFlag;					//�����ʶ	1-����	0-δ����
};

//У���û�GPSλ����Ϣ����
struct STR_CMD_CG_COM_CHECK_USER_GPS
{
	DWORD					dwFirstUserID;					//��һ�����
	DWORD					dwNextUserID;					//�ڶ������
	double					fDistance;						//������ҵľ���
	BYTE					cbIsCloser;						//����С�ڣ�200m������	1-����	0-������
};

//����У���û�IP
struct STR_SUB_CG_COM_CHECK_USER_IP
{
	BYTE					cbRequestFlag;					//�����ʶ	1-����	0-δ����
};

//У���û�IP����
struct STR_CMD_CG_COM_CHECK_USER_IP
{
	DWORD					dwFirstUserID;					//��һ�����
	DWORD					dwNextUserID;					//�ڶ������
	BYTE					cbISame;						//IP��ͬ	1-��ͬ	0-��ͬ
};

//��Ϸ������
struct STR_SUB_CG_FRAME_USER_CHAT
{
	DWORD dwUserID;				//�û�ID
	DWORD dwMsgID;				//0-15:������ʶ
};

//��Ϸ�����췵��
struct STR_CMD_GC_FRAME_USER_CHAT
{
	DWORD dwUserID;				//�û�ID
	DWORD dwChairID;			//���Ӻ�
	DWORD dwMsgID;				//0-15:������ʶ
};


//��������  ��ɢ���״̬
struct STR_CMD_GR_FRAME_GAME_DISSMISS
{
	DWORD					dwUserID[MAX_CHAIR];					//�û�ID
	BYTE					cbAgree[MAX_CHAIR];						    //�Ƿ�ͬ���ɢ		0-��ͬ��	1-ͬ��   2-δ���
};

//��ֽ���
struct STR_CMD_GR_TABLE_DJ_END
{
	TCHAR					szName[LEN_NICKNAME];
	TCHAR					szHeadUrl[LEN_HEAD_URL];
	SCORE					llScore;
	BYTE					wBigWinner;					//��Ӯ�� 0-�� 1-��
};

//��Ϸ��ʼ, С�ֽ���, ��ֽ���
struct STR_CMD_GR_TABLE_GAME_ACT
{
	BYTE					act;					//0��Ϸ��ʼ, 1С�ֽ���, 2��ֽ���
};

//�йܷ���
struct STR_CMD_GR_TUOGUAN
{
	DWORD					dwUserID;
};

//ȡ���йܷ���
struct STR_CMD_GR_CANCEL_TUOGUAN
{
	DWORD					dwUserID;
};

//���ĵ���
struct STR_SUB_CG_EFFECT
{
	BYTE					target_chair;						    //Ŀ����� ���Ӻ�
	DWORD					dwGoodsID;								//��ƷID
};


//���ĵ��߹㲥
struct STR_CMD_GC_EFFECT_BRODCAST
{
	BYTE					from_chair;								//from ���Ӻ�
	BYTE					target_chair;						    //to ���Ӻ�
	DWORD					dwGoodsID;								//��ƷID
};

#pragma endregion

#endif
