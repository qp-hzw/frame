#ifndef STR_CMD_GAME_SERVER_HEAD_FILE
#define STR_CMD_GAME_SERVER_HEAD_FILE

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
#define TABLE_MODE_JJC				1									//������ģʽ
#define TABLE_MODE_GOLD				2									//���ģʽ
#define TABLE_MODE_FK_GOLD			3									//�������ģʽ
#define TABLE_MODE_DW				4									//����ģʽ

//�������
#define REQUEST_FAILURE_NORMAL		0									//����ԭ��
#define REQUEST_FAILURE_NOGOLD		1									//��Ҳ���
#define REQUEST_FAILURE_NOSCORE		2									//���ֲ���
#define REQUEST_FAILURE_PASSWORD	3									//�������
#define REQUEST_FAILURE_END			4									//��Ϸ����
#define REQUEST_FAILURE_NOROOMCARD	5									//��ʯ����

//////////////////////////////////////////////////////////////////////////////////
#pragma endregion

#pragma region MDM_LOGON ��¼ģ��
//ID ��¼
struct STR_SUB_CG_LOGON_USERID
{
	DWORD							dwUserID;									//�û� I D
	TCHAR							szPassword[LEN_MD5];						//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];				//��������
	DWORD							dwSubGameVersion;							//����Ϸ�汾

	double							dLongitude;									//��¼��Ϸ��ʱGPS����
	double							dLatitude;									//��¼��Ϸ��ʱGPSγ��
};
//ID ��¼����
struct STR_CMD_GC_LOGON_USERID
{
	LONG							lResultCode;								//�������
	TCHAR							szDescribeString[LEN_MESSAGE_DESCRIBE];		//��Ϣ����
	DWORD							dwKindID;									//��ϷKindID
	DWORD							dwOffLineGameID;							//������������ϷID, ��0��ʾ�Ƕ�������
};
#pragma endregion

#pragma region MDM_USER �û�����

//��������
struct STR_SUB_CG_USER_CREATE_ROOM
{
	BYTE CommonRule[LEN_PRIVATE_TABLE_RULE/2];						//ͨ�÷������
	BYTE SubGameRule[LEN_PRIVATE_TABLE_RULE/2];						//����Ϸ�������
};

//ͨ�÷������
struct tagTableRule
{
	/********************************* ����ʹ�� ************************************/
	BYTE	GameMode;				//��Ϸģʽ 0-����ģʽ; 1-����ģʽ;  2-���ģʽ;  3-�������;
	BYTE	RobBankType;			//��ׯģʽ 0-��ׯ����ע������ׯ�� 1-��ׯ��������ׯ��2-�̶����̶�������ׯ��  3-Ӯׯ��Ӯ������ׯ�� 4-ҡ���Ӷ�ׯ�����ׯ�ң�
	BYTE	GameCount;				//��Ϸ���� 0-���޾�
	BYTE	GameCountType;			//������� �ǵ�һ���, �ڶ����, ���ǵ������
	BYTE	PlayerCount;			//������� 0-���������ɿ�
	BYTE	cbPayType;				//֧����ʽ��0����֧����1AA��
	DWORD	lSinglePayCost;			//��ҳ� �볡��Ʊ  TODONOW ���ֶν��ڿ۳���Ʊ��ʱ�����ã������ĵ��þ�����ǰ��, ����ɾ��
	WORD	CellScore;				//*�׷�	  Ĭ��Ϊ1 -- ����Ϸʹ�õ��Ǹ��ֶ�
	DWORD	MaxFan;					//*�ⶥ���� 0-���ⶥ 
	DWORD	LeaveScore;				//*�볡���� 0-������         -- ������
	DWORD	JoinScore;				//*�볡�趨 0-������         -- ������
	BYTE	bRefuseSameIP;			//����ͬIP    0-������ 1-����
	BYTE	bDistanceIn300;			//����300��	  0-������ 1-����
	double	dLongitude;				//����	
	double	dLatitude;				//γ��	
	BYTE	bCreateToOther;			//�Ƿ�Ϊ���˿��� 0-�� 1-��
	DWORD	FangZhu;				//*����
	BYTE	bAllowStranger;			//����İ���˼���

	/********************************** ����Ȧ��� ************************************/
	BYTE	byClubCreate;			//0�������� 1 ����Ȧ��ͨ����  2����Ȧ˽�ܴ���
	DWORD	dwUserID;				//Ⱥ��ID
    DWORD	dwClubID;				//����Ⱥ/���ֲ����
	DWORD	dwKindID;			    //��ϷID
	BYTE	byGoldOrFK;				//(2.��� 1.����)

	BYTE	bDissolve;				//�Ƿ������ɢ 0���� 1������
	DWORD	dwDissolveTime;			//��ɢʱ�� (����)

	//��ҷ�����
	DWORD	dwAmount;				//��Ͷ��
	DWORD	dwOwnerPercentage;		//Ⱥ�����
    
	BYTE	byMask;					//1 AA֧��;  2��Ӯ��֧��
	DWORD	dwDizhu;				//��ע
	//TODONOW ������ھ��ֲ��Ľ�ҳ�,������Ƿ������õ�; 
	//������ڴ����ķ�����ҳ�,�������ϵͳ���õ�; 
	//������ڴ����Ľ�ҳ�, �������ϵͳ���õ�
    DWORD	dwLevelGold;			//��������С��� 

	/************************************ �����ֶ� ************************************/
	BYTE byRetain1;
    BYTE byRetain2;
    BYTE byRetain3;
	BYTE byRetain4;

	DWORD szRetain1;
    DWORD szRetain2;
	DWORD szRetain3;
    DWORD szRetain4;
	DWORD szRetain5;
    DWORD szRetain6;
	DWORD szRetain7;
    DWORD szRetain8;
    DWORD szRetain9;
    DWORD szRetain10;
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
	double							dLongitude;		//����
	double							dLatitude;		//γ��
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
	BYTE							byGameMod;		//��Ϸģʽ ��Ҵ���2
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
};

struct STR_CMD_ROOM_RULE
{
	tagTableRule common;			 //�������� frameͨ�÷������
	DWORD TableID;                   //�����
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
#pragma endregion

#pragma region MDM_GR_MANAGE ��������
//���;���
struct CMD_GR_SendWarning
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwTargetUserID;						//Ŀ���û�
	TCHAR							szWarningMessage[LEN_USER_CHAT];	//������Ϣ
};

//ϵͳ��Ϣ
struct CMD_GR_SendMessage
{
	BYTE							cbGame;								//��Ϸ��Ϣ
	BYTE							cbRoom;								//��Ϸ��Ϣ
	BYTE							cbAllRoom;							//��Ϸ��Ϣ
	WORD							wChatLength;						//��Ϣ����
	TCHAR							szSystemMessage[LEN_USER_CHAT];		//ϵͳ��Ϣ
};

//�鿴��ַ
struct CMD_GR_LookUserIP
{
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//�߳��û�
struct CMD_GR_KickUser
{
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//�����ʻ�
struct CMD_GR_LimitAccounts
{
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//Ȩ������
struct CMD_GR_SetUserRight
{
	//Ŀ���û�
	DWORD							dwTargetUserID;						//Ŀ���û�

	//�󶨱���
	BYTE							cbGameRight;						//�ʺ�Ȩ��
	BYTE							cbAccountsRight;					//�ʺ�Ȩ��

	//Ȩ�ޱ仯
	BYTE							cbLimitRoomChat;					//��������
	BYTE							cbLimitGameChat;					//��Ϸ����
	BYTE							cbLimitPlayGame;					//��ϷȨ��
	BYTE							cbLimitSendWisper;					//������Ϣ
	BYTE							cbLimitLookonGame;					//�Թ�Ȩ��
};

//�߳������û�
struct CMD_GR_KickAllUser
{
	TCHAR							szKickMessage[LEN_USER_CHAT];		//�߳���ʾ
};

//��ɢ��Ϸ
struct CMD_GR_DismissGame
{
	WORD							wDismissTableNum;		            //��ɢ����
};
//////////////////////////////////////////////////////////////////////////////////

//��������
struct CMD_GR_OptionServer
{
	BYTE							cbOptionFlags;						//���ñ�־
	BYTE							cbOptionValue;						//���ñ�־
};

//��������
struct CMD_GR_LimitUserChat
{
	DWORD							dwTargetUserID;						//Ŀ���û�
	BYTE							cbLimitFlags;						//���Ʊ�־
	BYTE							cbLimitValue;						//�������
};
#pragma endregion

#pragma region  MDM_GR_MATCH ��������
//��������
struct CMD_GR_Match_Num
{
	DWORD							dwWaitting;							//�ȴ�����
	DWORD							dwTotal;							//��������
	DWORD							dwMatchTotal;						//������
};

//������Ϣ
struct CMD_GR_Match_Info
{
	TCHAR							szTitle[4][64];						//��Ϣ����
    WORD							wGameCount;							//��Ϸ����
};

//��ʾ��Ϣ
struct CMD_GR_Match_Wait_Tip
{
	SCORE							lScore;								//��ǰ����
	WORD							wRank;								//��ǰ����
	WORD							wCurTableRank;						//��������
	WORD							wUserCount;							//��ǰ����
	WORD							wPlayingTable;						//��Ϸ����
	TCHAR							szMatchName[LEN_SERVER];			//��������
};

//�������
struct CMD_GR_MatchResult
{
	TCHAR							szDescribe[256];					//�ý�����
	DWORD							dwGold;								//��ҽ���
	DWORD							dwMedal;							//���ƽ���
	DWORD							dwExperience;						//���齱��
};

#define MAX_MATCH_DESC				4									//�������
//��������
struct CMD_GR_MatchDesc
{
	TCHAR							szTitle[MAX_MATCH_DESC][16];		//��Ϣ����
	TCHAR							szDescribe[MAX_MATCH_DESC][64];		//��������
	COLORREF						crTitleColor;						//������ɫ
	COLORREF						crDescribeColor;					//������ɫ
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
	tagUserInfo				UserInfo;								//�����Ϣ
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
	DWORD					dwApplyUserID;					//�����ɢ�û�ID
	//added by WangChengQing ����˲���У����ֶ�, �յ�����Ϣ�ż��ɴ���
	BYTE					cbAgree;						//�Ƿ�ͬ���ɢ		0-��ͬ��	1-ͬ��
};
//�㲥�����ɢ����
struct STR_CMD_GR_FRMAE_VOTE_DISMISS
{
	DWORD					dwVoteUser[5];					//��ҪͶƱ��ɢ�����	//�ͻ���Ŀǰ�����ʾ5����
	TCHAR					szApplyUserNick[LEN_NICKNAME];	//�����ɢ�û��ǳ�
};

//�����ɢ���� ���
struct STR_CMD_GR_FRMAE_ASK_DISMISS_RESULT
{
	LONG							lResultCode;								//������� ��0��ʾ���ܽ�ɢ����
	TCHAR							szDescribeString[LEN_MESSAGE_DESCRIBE];		//��Ϣ����
};

//�����ɢ����
struct STR_SUB_RG_FRAME_VOTE_DISMISS
{
	DWORD					dwVoteUserID;					//�����ɢ�����û�ID
	BYTE					cbAgree;						//�Ƿ�ͬ���ɢ		0-��ͬ��	1-ͬ��
};
//�㲥��ɢ������
struct STR_CMD_GR_FRAME_DISMISS_RESULT
{
	BYTE					cbDismiss;						//0-δ��ɢ  1-��ɢ�ɹ����û�������	2-��ɢʧ��
	DWORD					dwVoteUserID;					//�����ɢ�����û�ID
	TCHAR					szVoteUserNick[LEN_NICKNAME];	//�����ɢ�û��ǳ�
	BYTE					cbAgree;						//�Ƿ�ͬ���ɢ
	BYTE					cbClubQuit;						//����Ȧ��ɢ 1-�ǿ���״̬��Ҫ���������壨ֻ������Ȧ���õ�,��	
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

#pragma endregion

#pragma region MDM_GF_GAME ����Ϸ����

#pragma endregion

#pragma pack()

#endif
