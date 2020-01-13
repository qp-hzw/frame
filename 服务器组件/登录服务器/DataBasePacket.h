#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

#include "../../��Ϣ����/CMD_LogonServer.h"
#pragma pack(1)

//���͸��ͻ��˵�����������
#define MAX_SEND_CLIENT_TASK_NUM			5

#pragma region MDM_LIST �б�����
/* *********************************************************************************
**   �������������½��ʱ������Ԫ���͵���Ϣ��				(201-300)

#define DBR_GP_ONLINE_COUNT_INFO	2									//ͳ������ -- �������ݿ�

#define DBR_UPDATA_MARQUEE			3									//�����������Ϣ ��ѯ
#define DBO_UPDATA_MARQUEE			1003									//�����������Ϣ ����
#define DBO_UPDATA_MARQUEE_FINISH	2003									//�����������Ϣ ����

//������Ϣ
struct DBR_GP_OnLineCountInfo
{
	DWORD							dwGameCount;						//������Ŀ
	DWORD							dwOnLineCountSum;					//��������
	tagOnLineInfoGame				OnLineCountGame[MAX_KIND];			//��������
};

//���ؽ��
struct DBO_GP_GameListResult
{
	BYTE							cbSuccess;							//�ɹ���־
};

//�����������Ϣ ����
struct STR_DBO_UPDATA_MARQUEE
{
	BYTE							byMask;								//1��������;  2����ɾ��  3���Դ���Ϣ  4���ʹ���Ϣ
	DWORD							dwMarqueeID;						//���������
	DWORD							dwMaruqeeTime;						//����� �������
	SYSTEMTIME						timeStartTime;						//����� ��ʼʱ��
	SYSTEMTIME						timeEndTime;						//����� ����ʱ��
	TCHAR							szMarqueeMsg[1024];					//�������Ϣ
};
#pragma endregion

#pragma region MDM_LOGON ��¼����
/* *********************************************************************************
**      MAIN:1   MDM_LOGON    ��¼����
** *********************************************************************************/
//��¼����
#define	DBR_CL_LOGON_ACCOUNTS		101									//�ʺŵ�¼
#define DBO_CL_LOGON_ACCOUNTS       1101                                 //�˺ŵ�¼

#define DBR_CL_LOGON_REGISTER		102									//�ʺ�ע��

#define DBR_CL_LOGON_PLATFORM       103                                 //ƽ̨��¼
#define DBO_CL_LOGON_PLATFORM       1103                                 //ƽ̨��¼

//�ʺŵ�¼
struct STR_DBR_CL_LOGON_ACCOUNTS
{
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	DWORD							dwProxyID;							//����ID
};

//�ʺ�ע��
struct STR_DBR_CL_LOGON_REGISTER
{
	//SocketУ��
	//LPVOID							pBindParameter;						//�󶨲���
	
	//�˺���Ϣ
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE							cbGender;							//�û��Ա�
	TCHAR                           strMobilePhone[LEN_MOBILE_PHONE];   //�ֻ����� ����Ϊnull

	//������ʶ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	DWORD							dwProxyID;							//����ID
};
//�˺�ע�᷵��
struct STR_DBO_CL_LOGON_REGISTER
{
	//�������
	DWORD                           dwResultCode;                       //�����ʶ
	//������Ϣ
	TCHAR							szDescribeString[128];				//������Ϣ

	//�û���ʶ
	DWORD							dwUserID;							//�û���ʶ
	//��������
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE							cbGender;							//�û��Ա�
	WORD							wFaceID;							//ͷ������
	TCHAR							szMySignature[LEN_MY_SIGNATURE];	//����ǩ��
	//��������
	SCORE                           dwUserDiamond;                      //�û���ʯ|Ԫ��
	DWORD							dwUserMedal;						//�û�����
	DWORD							dwExperience;						//������ֵ
	DWORD							dwLoveLiness;						//�û�����
	//�û�Ȩ��
	BYTE                            cbMasterOrder;                      //����Ա�ȼ� 0��ʾ�ǹ���Ա
	BYTE							cbMemberOrder;						//��Ա�ȼ�   0��ʾ�ǹ���Ա
	SYSTEMTIME						MemberOverDate;						//��Ա����ʱ��
	//��ϵ��ʽ
	TCHAR                           szIdentityName[LEN_IDENTITY_NAME];  //��ʵ����
	TCHAR                           szIdentityNum[LEN_IDENTITY_NUM];    //���֤��
	TCHAR                           szMobilePhone[LEN_MOBILE_PHONE];    //�ֻ�����
	//�˺���Ϣ
	TCHAR							szLasLogonIp[LEN_IP_ADDR];			//����¼��ַ
	SYSTEMTIME						LasLogonDate;						//�������ʱ��
	//������Ϣ
	SCORE							lUserScore;							//�û�����
	SCORE							lUserGold;							//�û����
	SCORE							lOpenRoomCard;						//�û�����
};

//ƽ̨��¼
struct STR_DBR_CL_LOGON_PLATFORM
{
	LPVOID							pBindParameter;						//�󶨲���
	TCHAR							szOpenID[33];						//OpenID | tokenID
	TCHAR							szUnionID[33];						//UnionID
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE							cbGender;							//�û��Ա�
	TCHAR							strHeadUrl[LEN_HEAD_URL];			//ͷ���ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	DWORD							dwProxyID;							//����ID
};

#pragma endregion

#pragma region MDM_SERVICE ��������
/* *********************************************************************************
**      MAIN:3   MDM_SERVICE    �������� 
** *********************************************************************************/
#define DBR_CL_SERVICE_USER_FEEDBACK		301									//��ҷ���
#define	DBO_CL_SERVICE_USER_FEEDBACK		1301								//��ҷ�������

#define DBR_CL_SERVICE_REFRESH_USER_INFO	302									//ˢ���û���Ϣ
#define DBO_CL_SERVICE_REFRESH_USER_INFO	1302								//ˢ���û���Ϣ����

#define DBR_CL_SERVICE_QUERY_ROOM_LIST		303									//��ѯ������Ϣ�б�
#define DBO_CL_SERVICE_QUERY_ROOM_LIST		1303								//��ѯ������Ϣ�б���

#define DBR_CL_SERVICE_GET_RICH_LIST			304								//��ȡ������
#define DBO_CL_SERVICE_GET_RICH_LIST			1304							//��ȡ�����񷵻�

#define DBR_CL_SERVICE_GET_USER_RECORD_LIST		305								//��ȡ�û�¼���б���֣�
#define DBO_CL_SERVICE_GET_USER_RECORD_LIST		1305							//��ȡ�û�¼���б���

#define DBR_CL_SERVICE_GET_SPECIFIC_RECORD		306								//��ȡָ��ID¼��С�֣�
#define DBO_CL_SERVICE_GET_SPECIFIC_RECORD		1306							//��ȡָ��ID¼�񷵻�

#define DBR_CL_SERVICE_ONLINE_REWARD			307								//��ȡ���߽���
#define DBO_CL_SERVICE_ONLINE_REWARD			1307							//��ȡ���߽�������

#define DBR_CL_SERVICE_GET_TASK_LIST			308								//��ȡ�����б�
#define DBO_CL_SERVICE_GET_TASK_LIST			1308							//��ȡ�����б���

#define DBR_CL_SERVICE_GET_TASK_REWARD			309								//��ȡ������
#define DBO_CL_SERVICE_GET_TASK_REWARD			1309							//������ɷ���

#define DBR_CL_SERVICE_REQUEST_LOTTERY			310								//����齱
#define DBO_CL_SERVICE_REQUEST_LOTTERY			1310							//����齱����

#define DBR_CL_SERVICE_MODIFY_PERSONAL_INFO		311								//�޸ĸ�������
#define DBO_CL_SERVICE_MODIFY_PERSONAL_INFO		1311							//�޸ĸ������Ϸ���

#define DBR_CL_USER_QUERY_SCORE_INFO			312								//��ѯ(���)��ҷ�����Ϣ
#define DBO_CL_USER_QUERY_SCORE_INFO			1312							//��ѯ���������ҷ�����Ϣ����

//������Щ���а����Ϣ�ţ�����������а�����õ���Ϣ TODO ��ʱ��������
#define DBR_CL_SERVICE_GET_RANK_REWARD			313								//��ȡ���а���
#define DBO_CL_SERVICE_GET_RANK_REWARD			1313							//��ȡ���а�������
#define DBR_GP_UPDATE_RANK_VALUE				2313							//�������а��û����� TODO ���̴���������
#define DBR_GP_READ_RANK_LIST					3313							//��ȡ���ݿ����а�


#define DBR_GP_MODIFY_USER_INSURE				314								//����û��Ƹ�		TODO �ڵ�¼�������õ�����ʱע����

#define DBR_GP_READ_VERSION_AND_URL				315								//��ȡ�汾��		//��ʱע�ͣ�������ܻ��õ�

#define DBR_CL_SERVICE_PURE_STANDING_LIST		316								//pure�������а�
#define DBO_LC_SERVICE_PURE_STANDING_LIST		1316							//pure�������а� ����
#define DBO_LC_SERVICE_PURE_STANDING_FINISH		2316							//pure�������а� ����

#define DBR_CL_SERVICE_PURE_RECORD_LIST			317								//pure���ս��
#define DBO_LC_SERVICE_PURE_RECORD_LIST			1317							//pure���ս�� ����
#define DBO_LC_SERVICE_PURE_RECORD_LIST_PINFO	2317							//pure��������Ϣ ����
#define DBO_LC_SERVICE_PURE_RECORD_FINISH		3317							//pure���ս�� ����

#define DBR_CL_SERVICE_PURE_XJ_RECORD_LIST		318								//pureС��ս��
#define DBO_LC_SERVICE_PURE_XJ_RECORD_LIST		1318							//pureС��ս�� ����
#define DBO_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO 2318							//pureС�������Ϣ ����
#define DBO_LC_SERVICE_PURE_XJ_RECORD_FINISH	3318							//pureС��ս�� ����

#define DBR_CL_SERVICE_GOLD_INFO				319								//�����Ҵ�����Ϣ
#define DBO_LC_SERVICE_GOLD_INFO				1319							//�����Ҵ�����Ϣ ����
#define DBO_LC_SERVICE_GOLD_INFO_FINISH			2319							//�����Ҵ�����Ϣ ����

#define DBR_CL_SERVICE_XJ_RECORD_PLAYBACK		320								//С��¼��ط�
#define DBO_LC_SERVICE_XJ_RECORD_PLAYBACK		1320							//С��¼��طŷ���

#define DBR_CL_SERVICE_CUSTOMER_MESSEGE			321								//�ͷ���Ϣ
#define DBO_LC_SERVICE_CUSTOMER_MESSEGE			1321							//�ͷ���Ϣ����

//////////////////////////////////////////////////////////////////////////////////
//���ṹ�嶨�塿

//��ҷ���
struct  STR_DBR_CL_SERVICE_FEEDBACK
{
	DWORD							dwUserID;							//�û� ID
	TCHAR							szFB_Title[LEN_USERNOTE];			//��������
	TCHAR							szFB_Content[LEN_USER_NOTE];		//��������
	TCHAR							szContact[LEN_MOBILE_PHONE];		//��ϵ��ʽ
};
//��ҷ�������
struct STR_DBO_CL_SERVICE_FEEDBACK
{
	LONG							lResultCode;						//��������
	TCHAR							szDescribeString[128];				//�ɹ���Ϣ
};

//ˢ���û���Ϣ
struct STR_DBR_CL_SERCIVR_REFRESH_INFO
{
	DWORD							dwUserID;							//�û�ID
};
//ˢ���û���Ϣ����
struct STR_DBO_CL_SERCIVR_REFRESH_INFO
{
	DWORD                           dwResultCode;                       //�����ʶ
	TCHAR							szDescribeString[128];				//������Ϣ

	/* *****************************    �û���Ϣ     ****************************/
	//�û���ʶ
	DWORD							dwUserID;							//�û���ʶ
	//��������
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE							cbGender;							//�û��Ա�
	WORD							wFaceID;							//ͷ������
	TCHAR							szMySignature[LEN_MY_SIGNATURE];	//����ǩ��
	//��������
	SCORE                           dwUserDiamond;						//�û�Ԫ��
	DWORD							dwUserMedal;						//�û�����
	DWORD							dwExperience;						//������ֵ
	DWORD							dwLoveLiness;						//�û�����
	//�û�Ȩ��
	BYTE                            cbMasterOrder;                      //����Ա�ȼ� 0��ʾ�ǹ���Ա
	BYTE							cbMemberOrder;						//��Ա�ȼ�   0��ʾ�ǹ���Ա
	SYSTEMTIME						MemberOverDate;						//��Ա����ʱ��
	//��ϵ��ʽ
	TCHAR                           szIdentityName[LEN_IDENTITY_NAME];  //��ʵ����
	TCHAR                           szIdentityNum[LEN_IDENTITY_NUM];    //���֤��
	TCHAR                           szMobilePhone[LEN_MOBILE_PHONE];    //�ֻ�����
	
	/* *****************************    �˺���Ϣ     ****************************/
	TCHAR							szLasLogonIp[LEN_IP_ADDR];			//����¼��ַ
	SYSTEMTIME						LasLogonDate;						//�������ʱ��
	
	/* *****************************    ��������     ****************************/
	//����Ȧ 
	DWORD							dwGroupID;							//���ű�ʶ
	TCHAR							szGroupName[LEN_GROUP_NAME];		//��������

	//������Ϣ
	SCORE							lUserScore;							//�û�����
	SCORE							lUserGold;							//�û����
	SCORE							lUserRoomCard;						//�û�����
};

//��ѯ�����б�
struct STR_DBR_CL_SERCIVR_QUERY_ROOMLIST
{
	DWORD dwUserID;
};
//��ѯ�����б���
struct STR_DBO_CL_SERCIVR_QUERY_ROOMLIST
{
	DWORD dwTableID;					//����ID
	DWORD dwPassword;					//��������
	DWORD dwCreaterID;					//�������ID
	WORD  wJuShu;						//����
	BYTE  byMode;						//ģʽ
	BYTE  byZhuangType;					//ׯ����
	BYTE  byPlayerCount;				//�����
	BYTE  byMaxPlayerCount;				//��������
	BYTE  byIsStart;					//�Ƿ�ʼ
	BYTE  byCost;						//�ۿ���
	TCHAR szTime[24];					//����ʱ��
	DWORD dwUserID[5];					//���ID
	TCHAR szNickName[5][LEN_NICKNAME];	//����ǳ�
	DWORD dwScore[5];					//�û�����
};

//��ȡ������
struct STR_DBR_CL_SERCIVR_GET_RICHLIST
{
};
//��ȡ�����񷵻�
struct STR_DBO_CL_SERCIVR_GET_RICHLIST
{
	BYTE byCount;						//���а�����
	tagSingleRichInfo RegalInfoList[15];	//�����б�
};

//��ȡ�û�¼���б�
struct STR_DBR_CL_SERCIVR_GET_RECORDLIST
{
	DWORD							dwUserID;		//�û�ID	
};
//��ȡ�û�¼���б���
struct STR_DBO_CL_SERCIVR_GET_RECORDLIST
{
	DWORD							dwTableID;				//����ID
	DWORD							dwUserID[5];			//���ID
	TCHAR							szUserName[5][LEN_NICKNAME];//�û��ǳ�
	BYTE							byGameMode;			//��Ϸģʽ
	BYTE							byZhuangType;		//��ׯ����
	WORD							wCurrentJuShu;		//��ǰ����
	WORD							wTotalJuShu;		//�ܾ���
	TCHAR							szGameTime[30];		//��Ϸʱ��
	TCHAR							szScore[50];		//��ҵ��ַ���
	TCHAR							szTotalScore[50];	//����ܷ���
	DWORD							dwDataSize;			//���ݳ���
};

//��ȡС��¼���б�
struct STR_DBR_CL_SERCIVR_GET_SPECIFIC_RECORD
{
	DWORD dwTableID;	   //����ID
};
//��ȡС��¼���б���
struct STR_DBO_CL_SERCIVR_GET_SPECIFIC_RECORD
{
	WORD							wCurrentJuShu;					//��ǰ����
	TCHAR							szNickName[5][LEN_NICKNAME];	//�û��ǳ�
	TCHAR							szScore[50];					//���С�ַ���
};

//С��¼��ط�
struct STR_DBR_CL_SERVICE_XJ_RECORD_PLAYBACK
{
	DWORD		dwRecordID;				//¼��ID
};
//С��¼��طŷ���
struct STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK
{
	WORD							wCurrentCount;						//��ǰ����
	DWORD							dwDataSize;							//���ݳ���
	BYTE							cbRecordData[LEN_MAX_RECORD_SIZE];	//¼������
};

//�ͷ���Ϣ
struct STR_DBR_CL_SERVICE_CUSTOMER_MESSEGE
{
	BYTE					cbMessegeFlag;						//��Ϣ��ʶ	0-��ʾ��Ϣ��ʾ	>=1��ʾ������Ϣ����
};
//�ͷ���ʾ��Ϣ ����
struct STR_DBO_LC_SERVICE_CUSTOMER_MESSEGE
{
	BYTE					cbMessegeFlag;						//��Ϣ��ʶ
	TCHAR					szMessege[LEN_MAX_SERVICE_MEG_SIZE];//��Ϣ����
};

//��ȡ���߽���
struct STR_DBR_CL_SERCIVR_ONLINE_REWARD
{
	DWORD dwUserID;
};
//��ȡ���߽�������
struct STR_DBO_CL_SERCIVR_ONLINE_REWARD
{
	BYTE  byType;		//��������
	DWORD dwCount;		//��������
	TCHAR szDescribe[128]; //����
};

//��ȡ�����б�
struct STR_DBR_CL_SERCIVR_GET_TASKLIST
{
	DWORD	dwUserID;									//�û�ID
};
//��ȡ�����б���
struct STR_DBO_CL_SERCIVR_GET_TASKLIST
{
	WORD	wTaskID;							//����ID
	BYTE	cbTaskType;							//�������� 1�ճ�; 2�ܳ�; 3�³�; 4�ɾ�
	BYTE	cbActionType;						//���������� ��1����Ϣ; 2΢�ŷ���; 3��ֵ; 4������Ϸ; 5�����Ϸ	
	DWORD	dwNeedCount;						//����������������, �������5�ַ�����Ϸ, �������5	
	TCHAR	szDescribe[128];					//��������
	BYTE	byRewardType;						//��������1���; 2����; 3��ʯ
	DWORD	dwRewardCount;						//��������
	BYTE	cbTaskStatus;						//����״̬ ��0-������  1-��ɵ�δ��ȡ 2-�������ȡ
	DWORD	dwFinishedCount;					//����ɵ�������������				
};

//��ȡ������
struct STR_DBR_CL_SERCIVR_GET_TASK_REWARD
{
	DWORD	dwUserID;
	WORD	wTaskID;	
};
//��ȡ����������
struct STR_DBO_CL_SERCIVR_GET_TASK_REWARD
{
	LONG							lResultCode;						//������ 0-�ɹ� ����ʧ��
	TCHAR							szDescribe[128];					//����
};

//����齱
struct STR_DBR_CL_SERCIVR_REQUEST_LOTTERY
{
	DWORD	dwUserID;				//�û�ID
};
//����齱����
struct STR_DBO_CL_SERCIVR_REQUEST_LOTTERY
{
	LONG							lResultCode;						//��������
	TCHAR							szDescribeString[128];				//������Ϣ
	BYTE							byIndex;							//ҡ����ţ�0-11Ϊҡ��
};

//�޸ĸ������Ϸ���
struct STR_DBO_CL_MODIFY_PERSONL_INFO
{
	LONG							lResultCode;						//��������
	TCHAR							szDescribeString[128];				//�ɹ���Ϣ
};

//��ѯ�û���ҷ���
struct STR_DBR_CL_SERCIVR_QUERY_SCORE_INFO
{
	DWORD							dwUserID;							//�û�ID
};
//��ѯ�û���ҷ�������
struct STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO
{
	LONG						lResultCode;						//��������
	TCHAR						szDescribeString[128];				//�ɹ���Ϣ

	DWORD						dwUserID;							//�û�ID
	SCORE						lOpenRoomCard;						//������
	SCORE						lGold;								//���	
	SCORE						lDiamond;							//��ʯ
	SCORE						lRewardCard;						//����
	SCORE						lScore;								//����
};

//��ȡ���а���
struct STR_DBR_CL_SERCIVR_GET_RANK_REWARD
{
	DWORD	dwUserID;
	BYTE	byIndex;
};
//��ȡ���а�������   
struct STR_DBO_CL_SERCIVR_GET_RANK_REWARD				
{
	LONG	lResultCode;												//0-��ȡ�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];											//������������� ��ȡ�ɹ�,���10000���
};

//����û��Ƹ�
struct DBR_GP_ModifyUserInsure
{
	DWORD	dwUserID;	
	SCORE	lGold;				//���
	SCORE	lOpenRoomCard;		//����
	SCORE	lDiamond;			//��ʯ
	SCORE	lRewardCard;		//����
	SCORE	lScore;				//����
};

//������Ϣ-ʧ�ܷ���
struct STR_DBO_CL_SERCIVR_FAIL
{
	LONG							lResultCode;						//��������
	TCHAR							szDescribeString[128];				//������Ϣ
};
#pragma endregion

#pragma region MDM_CLUB  ����Ȧ
//////////////////////////////////////////////////////////////////////////////////
/* *******************************************************************************
**          MAIN:6      MDM_CLUB  ����Ȧ(�汾2)
** ******************************************************************************/
#define DBR_CL_CLUB_ALL_CLUB_INFO_LIST			601							//��ѯ����Ȧ�б�
#define DBO_LC_CLUB_ALL_CLUB_INFO_LIST			1601						//��ѯ����Ȧ�б� ����
#define DBO_LC_CLUB_ALL_INFO_FINISH				2601						//��ѯ����Ȧ�б� ����

#define DBR_CL_CLUB_ROOM_LIST					602							//��ѯָ������Ȧ�����б�
#define DBO_LC_CLUB_ROOM_LIST					1602						//��ѯָ������Ȧ�����б� 
#define DBO_LC_CLUB_ROOM_LIST_FINISH			2602						//��ѯָ������Ȧ�����б� ����

#define DBR_CL_CLUB_RANDOM_CLUB_LIST			603							//��ѯδ��Ա, �������Ȧ(���9��)
#define DBO_LC_CLUB_RANDOM_CLUB_LIST			1603						//��ѯδ��Ա�������Ȧ 
#define DBO_LC_CLUB_RANDOM_CLUB_LIST_FINISH		2603						//��ѯδ��Ա�������Ȧ ����

#define DBR_CL_CLUB_JOIN_CLUB					604							//�����������Ȧ
#define DBO_LC_CLUB_JOIN_CLUB					1604						//�����������Ȧ ����
#define DBO_LC_CLUB_JOIN_CLUB_BDCAST			2604						//�����������Ȧ �㲥
#define DBO_LC_CLUB_JOIN_CLUB_RE				3604						//�����������Ȧ �Զ�����, ֪ͨ�ͻ���ʵʱˢ��

#define DBR_CL_CLUB_CREATE_CLUB					605							//��������Ȧ
#define DBO_LC_CLUB_CREATE_CLUB					1605						//��������Ȧ����

#define DBR_CL_CLUB_JOIN_ROOM					607							//������뷿��
#define DBO_LC_CLUB_JOIN_ROOM					1607						//������뷿�� ����

#define DBR_CL_CLUB_APPLICANT_RESULT			609							//Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�)
#define DBO_LC_CLUB_APPLICANT_RESULT			1609						//Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�) ����
//#define DBO_LC_CLUB_JOIN_CLUB_MESSAGE_FINISH	2609						//Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�) ����

#define DBR_CL_CLUB_MEMBER_MANAGER				610							//�����Ա����
#define DBO_LC_CLUB_MEMBER_MANAGER				1610						//�����Ա���� ����
#define DBO_LC_CLUB_MEMBER_MANAGER_FINISH		2610						//�����Ա���� ����
#define DBO_LC_CLUB_DATA						3610						//���������Ϣ

#define DBR_CL_CLUB_QUIT						611							//�߳��˳�����
#define DBO_LC_CLUB_QUIT						1611						//�߳��˳����� ����

#define DBR_CL_CLUB_DISS_CLUB					612							//��ɢ����Ȧ
#define DBO_LC_CLUB_DISS_CLUB					1612						//��ɢ����Ȧ ����

#define DBO_LC_CLUB_TABLE_LIST_TABLE			1613						//�����б� ����
#define DBO_LC_CLUB_TABLE_LIST_USER				2613						//��������б� ����
//#define DBO_LC_CLUB_TABLE_LIST_FINISH					1613					//�����б� ���� client����������Ϣ����ʾ

#define DBR_CL_CLUB_ROOM_SETTING				614							//��������
#define DBO_LC_CLUB_ROOM_SETTING				1614						//�������� ����

#define DBR_CL_CLUB_NOTICE						615							//����Ȧ����
#define DBO_LC_CLUB_NOTICE						1615						//����Ȧ���� ����

#define DBR_CL_CLUB_MESSAGE						616							//����Ȧ��� -- ���ⲿչʾ����Ϣ
#define DBO_LC_CLUB_MESSAGE						1616						//����Ȧ��� ����  ע:����Ȧ��� ����㲥

#define DBR_CL_CLUB_CONTRIBUTE_FK				617							//���׷��� -- ���ⲿչʾ����Ϣ
#define DBO_LC_CLUB_CONTRIBUTE_FK				1617						//���׷��� ����

#define DBR_CL_CLUB_AUTO_AGREE					618							//����Ȧ���� -- ��������Զ�ͬ��
#define DBO_LC_CLUB_AUTO_AGREE					1618						//����Ȧ���� ����

#define DBR_CL_CLUB_INVITE						619							//���������������Ȧ 
#define DBO_LC_CLUB_INVITE						1619						//�������˼�������Ȧ ����
#define DBO_LC_CLUB_INVITE_REMIND				2619						//�����˵�����

#define DBR_CL_CLUB_INVITE_RESULT				620							//�������˵Ļظ�
#define DBO_LC_CLUB_INVITE_RESULT				1620						//�������˵Ļظ� ����

#define DBR_CL_CLUB_INQUERY_LIST				621							//�������˲鿴�Լ��������б�
#define DBO_LC_CLUB_INQUERY_LIST				1621						//�������˲鿴�Լ��������б� ����
#define DBO_LC_CLUB_INQUERY_LIST_FINISH			2621						//�������˲鿴�Լ��������б� ����

#define DBR_CL_CLUB_APPLICANT_LIST				622							//�������б�
#define DBO_LC_CLUB_APPLICANT_LIST				1622						//�������б� ����
#define DBO_LC_CLUB_APPLICANT_LIST_FINISH		2622						//�������б� ����

#define DBR_CL_CLUB_APPOINTMENT					623							//ְ������ 0��ͨ 1����Ա 
#define DBO_LC_CLUB_APPOINTMENT					1623						//ְ������ ����
#define DBO_LC_CLUB_APPOINTMENT_NOTE			2623						//ְ������ ����

#define DBR_CL_CLUB_CHAT						624							//����Ȧ����
#define DBR_CL_WORD_CHAT						1624						//��������
#define DBR_CL_SYSTEM_CHAT						2624						//ϵͳ����
#define DBR_CL_SECRET_CHAT						3624						//˽������
#define DBO_LC_CHAT_ALL							4624						//���� ����
#define DBO_LC_CHAT_ALL_BDCAST					5624						//���� �㲥


#define DBR_CL_CLUB_ROOM_USER_LEAVE				625							//����뿪���ֲ�����

#define DBR_CL_CLUB_RECORD_LIST					626							//����ս��ͳ��
#define DBO_LC_CLUB_RECORD_LIST					1626						//����ս��ͳ�� ����
#define DBO_LC_CLUB_RECORD_FINISH				2626						//����ս��ͳ�� ����

#define DBR_CL_CLUB_ROOM_DISSOLVE				627							//��ɢ��������
#define DBO_LC_CLUB_ROOM_DISSOLVE				1627						//��ɢ�������� ����

#define DBR_CL_CLUB_TABLE_DISSOLVE				628							//��ɢ��������
#define DBO_LC_CLUB_TABLE_DISSOLVE				1628						//��ɢ�������� ����

#define DBR_CL_CLUB_STICKY_POST					629							//����Ȧ�ö�����
#define DBO_LC_CLUB_STICKY_POST					1629						//����Ȧ�ö����� ����

#define DBR_CL_CLUB_ROOM_QUERY_SETTING			630							//���󷿼�����
#define DBO_LC_CLUB_ROOM_QUERY_SETTING			1630						//���󷿼����� ����
#pragma region �ṹ��	

//��������Ȧ�б� ����
struct STR_DBO_LC_CLUB_ALL_CLUB_INFO_LIST
{
	DWORD	dwClubID;									//����ȦID
	TCHAR	szClubName[LEN_NICKNAME];					//����Ȧ����
	DWORD	dwMajorKindID;								//������ϷID
	TCHAR	szMinorKindID[128];							//��Ҫ��Ϸ ���10��
	BYTE	byClubLevel;								//����Ȧ�ȼ�
	WORD	wMemberNum;									//��ǰ����
	TCHAR	szNotice[128];								//����Ȧ ����
	TCHAR	szMessage[128];								//����Ȧ ���
	DWORD	dwClubOwner;								//Ⱥ��ID
	BYTE	byClubRole;								//���ְ�� 0Ⱥ�� 1���� 2һ���Ա
};

//��ѯ�ض�����Ȧ�����б�
struct STR_DBR_CL_CLUB_ROOM_LIST
{
	DWORD	dwClubID;									//����ȦID
};

//��������Ȧ
struct STR_DBR_CL_CLUB_CREATE_CLUB
{
	DWORD	dwUserID;									//���ID
	TCHAR	szClubName[LEN_NICKNAME];					//����Ȧ����
	DWORD	dwMajorKindID;								//������ϷID
	TCHAR	szMinorKindID[128];							//��Ҫ��Ϸ ���10��

	TCHAR	szNotice[128];								//����Ȧ����
	TCHAR	szMessag[128];								//����Ȧ���
};

//��������Ȧ���� ����
struct STR_DBO_CL_CLUB_CREATE_CLUB_ROOM_APPLY
{
	LONG	lResultCode;								//0-����ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};

//�����������Ȧ ����
struct STR_DBO_LC_CLUB_JOIN_CLUB
{
	LONG	lResultCode;								//0-����ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};

//������뷿��
struct STR_DBR_CL_CLUB_JOIN_ROOM
{
	DWORD	dwUserID;									//���ID
	DWORD	dwRoomID;									//����ID
};

//�����������
struct STR_DBR_CL_CLUB_JOIN_TABLE
{
	DWORD	dwUserID;									//���ID
	int		JoinType;									//��������
	DWORD	dwTableID;									//����ID
	DWORD	dwChairID;									//����ID
	DWORD	dwCreatePassword;							//�������뷿����
	DWORD	dwJoinPassword;								//�������뷿����
};

//�����Ա����
struct STR_DBR_CL_CLUB_MEMBER_MANAGER
{
	DWORD	dwUserID;									//���ID
	DWORD	dwClubID;									//����ȦID
};

//Ⱥ��|���� ��ÿһ��������Ϣ�Ĵ�
struct STR_DBR_CL_CLUB_JOIN_CLUB_MESSAGE
{
	DWORD	dwResult;									//0ͬ�� �����ܾ�
	DWORD	dwUserID;									//���ID
};

//Ⱥ��|���� �������������Ⱥ����Ϣ
struct STR_DBO_LC_CLUB_JOIN_CLUB_MESSAGE
{
	DWORD	dwUserID;									//�������ID
	TCHAR	szUserName[LEN_NICKNAME];					//��������ǳ�

	DWORD	dwCustomID;									//�Զ����ʶ
	TCHAR	szHeadUrl[LEN_HEAD_URL];							//ͷ���ַ
};

//�߳��˳� ����
struct STR_DBR_CL_CLUB_QUIT
{
	DWORD	dwClubID;									//����ȦID
	DWORD	dwUserID;									//���ID
	DWORD	dwQuitPlayerID;								//���߳����ID
};

//����Ȧ�ö�
struct STR_DBR_CL_CLUB_STICKY_POST
{
	DWORD	dwUserID;									//���ID
	DWORD	dwClubID;									//���ֲ�ID
	BYTE	cbTopFlag;									//�Ƿ��ö�	1-�ö�	0-ȡ���ö�
};

//����Ȧ�ö�����
struct STR_DBO_LC_CLUB_STICKY_POST
{
	LONG	lResultCode;								//0�ɹ�  ����ʧ��
};

#pragma endregion
#pragma endregion

#pragma region MDM_SHOP �̳ǵ���
#define DBR_CL_SHOP_QUERY						701							//�̳ǲ�ѯ -- ��ѯ��Ʒ�������
#define DBO_CL_SHOP_QUERY						702							//�̳ǲ�ѯ -- ��ѯ��Ʒ�������
#define DBO_CL_SHOP_QUERY_FINISH				703							//�̳ǲ�ѯ -- ��ѯ��Ʒ�������


#define DBR_CL_SHOP_MONEY						704							//(��δ�õ�)��Ǯ�������
#define DBO_CL_SHOP_MONEY						705							//(��δ�õ�)��Ǯ�������

#define DBR_CL_SHOP_DIAMOND						706							//��ʯ�������
#define DBO_CL_SHOP_DIAMOND						707							//��ʯ�������

#define DBR_CL_BAG_QUERY						708							//������Ʒ��ѯ
#define DBO_CL_BAG_QUERY						709							//������Ʒ��ѯ
#define DBO_CL_BAG_FINISH						710							//������Ʒ��ѯ����


//��ѯ�̳�DBR
struct STR_DBR_CL_SHOP_QUERY
{
	DWORD dwUserID;			//���ID
	BYTE byGoodsType;		//��Ʒ���� 1rmb���򷿿�  2rmb������ 3��ҹ������� 4���ֹ�������
};

//��ʯ�������DBO
struct STR_DBO_CL_SHOP_DIAMOND
{
	LONG							lResultCode;						//��������
	TCHAR							szDescribeString[128];				//�ɹ���Ϣ
};

//������Ʒ��ѯ
struct STR_DBR_CL_BAG_QUERY
{
	DWORD dwUserID;			//���ID
};

//������Ʒ��ѯ DBO
struct STR_DBO_CL_BAG_QUERY
{
	DWORD dwGoodsID;		//��ƷID
	DWORD dwGoodsNum;		//��Ʒ����
	BYTE  byGoodsType;		//��Ʒ����
};
#pragma endregion

#pragma region ������

/* *******************************************************************************
**        MAIN:6       MDM_GIFT   �������
** ******************************************************************************/
#define DBR_CL_GIFT_GIVE_PROPS				801							//���͵���
#define DBO_CL_GIFT_GIVE_PROPS				1801						//���͵��߷���
#define DBO_CL_GIFT_GIVE_PROPS_SHOW			2801						//���͵��� ֪ͨ������


#define DBR_CL_USER_RECHARGE_INFO			802							//��ֵ��Ϣ
#define DBO_CL_USER_RECHARGE_INFO			1802						//��ֵ��Ϣ

#define DBR_CL_USER_EXCHANGE_INFO			803							//�һ���Ϣ
#define DBO_CL_USER_EXCHANGE_INFO			1803						//�һ���Ϣ

#define	DBO_CL_USER_COMMAND_RESULT			804							//���ò������


//��������
struct DBR_CommandSource
{
	WORD							MainCommand;						//������ID
	WORD							SubCommand;							//������ID
};

//��ֵ��Ϣ
struct STR_DBR_CL_OTHER_RECHARGE_INFO
{
	DWORD							dwUserID;							//�û�ID
};
//��ֵ��Ϣ����
struct STR_DBO_CL_OTHER_RECHARGE_INFO
{
	SCORE								dwMinMoney;///���ٳ�ֵǮ  
	UINT32								dwChangeScale;  ///�һ�����
};

//�һ�������Ϣ
struct STR_DBR_CL_OTHER_EXCHANGE_INFO
{
	DWORD							dwUserID;							//�û�ID
};
//�һ�������Ϣ����
struct STR_DBO_CL_OTHER_EXCHANGE_INFO
{
	SCORE								dwMinMoney;		///���ٳ�ֵǮ  
	UINT32								dwChangeScale;  ///�һ�����
	SCORE								dwWithdrawals;  ///������RMB
	SCORE								dwBankMoney;	///���н��
};


//������������	
struct DBO_GP_OperateCommand
{
	DBR_CommandSource				mCommand;							//Э����Ϣ
	LONG							lResultCode;						//��������
	TCHAR							szDescribeString[128];				//�ɹ���Ϣ
};

/* *******************************************************************************
**       �����������������������ݿ���Ϣ
** ******************************************************************************/
struct DBR_GP_UserRankItem
{
	DWORD dwUserID;		
	DWORD dwCount;		//���ѻ�ʤ������
	BYTE byRankNum;		//����
	BYTE byReceived;	//����ȡ
	BYTE byIndex;		//���а����
};

#pragma endregion

#endif