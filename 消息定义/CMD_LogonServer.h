#ifndef CMD_LONGON_HEAD_FILE
#define CMD_LONGON_HEAD_FILE

#include "STR_CMD_LogonServer.h"

/*
** ��Ҫ����:  ��¼�� �� �ͻ��˵� ��Ϣ
** ��ϸ����:  1. ��Ϣ��(�� + ��)     2. �ṹ��
** ��ע˵��:  ����Ϣ�ŵĺ���: 
**            1. ��һ���ֶ�: ������־,        
**				 SUB(client->server����Ϣ)�� CMD(server->client����Ϣ), 
**				 DBR(�����ݿ�Request)��DBO(���ݿⷵ�ص���Ϣ).
**            2. �ڶ����ֶ�: ��Ϣ����         
**				 C:client;
**            3. �������ֶ�: ����Ϣ�ŵı�־   ����LOGON��Ϊ��¼ģ�����Ϣ
**            4. ֮����ֶ�: ����Ϣ�ŵĺ���   ����Accounts��ʾ�˺ŵ�¼
**           
**            �����淶
**            1. �ṹ�������:  ����Ϣ�ŵ�ǰ���STR, ����SUB_CL_LOGON_ACCOUNTS ��Ϊ STR_SUB_CL_LOGON_ACCOUNTS
**
**            2. DBO, DBR��Ϣ�ŵ�����Ϊ:  ��SUB�滻ΪDBR��DBO����
**
**            3. �����������淶: 1) ָ���p  2)������־SUB  3)����Ϣ�ű�־  4)����Ϣ�ű�־
**
**            4. ����������:  On_������־_����Ϣ������Ϣ��
**
**
*/

#pragma region MDM_LOGON ��¼ģ��
/* *********************************************************************************
**      MAIN:1   MDM_LOGON    ��¼ ģ��
** *********************************************************************************/
#define MDM_LOGON					1									//����Ϣ

#define SUB_CL_LOGON_ACCOUNTS		1									//�ʺŵ�¼
#define CMD_LC_LOGON_ACCOUNTS		101									//�˺ŵ�¼

#define SUB_CL_LOGON_REGISTER	    2									//�ʺ�ע�ᣨ���ص����˺ŵ�¼��

#define SUB_CL_LOGON_PLATFORM		3									//ƽ̨��½����������¼��
#define CMD_LC_LOGON_PLATFORM       103                                 //ƽ̨��½����

#define CMD_LC_LOGON_REPEAT_LOGON	104									//�ظ���¼

#define CMD_LC_LOGON_LOGON_REWARD   105                                 //��¼����������һع� + ���ս�����

#define CMD_LC_LOGON_UPDATE_NOTIFY	106									//������ʾ����¼ + ע�� + ƽ̨��¼�����õ���


#pragma endregion

#pragma region MDM_LIST �б�����
/* *********************************************************************************
**         MAIN:2    MDM_LIST    �б������¼�ɹ����������͸��ͻ��ˣ��ͻ�����������)
** *********************************************************************************/
#define MDM_LIST					2									//�б���Ϣ

//�б���Ϣ����
#define CMD_LC_LIST_TYPE			101									//�����б�TypeItem
#define CMD_LC_LIST_KIND			102									//�����б�KindItem
#define CMD_LC_LIST_NODE			103									//�ڵ��б�NodeItem
#define CMD_LC_LIST_ROOM			104									//�����б�RoomItem

#define CMD_LC_LIST_ROOM_ONLINE		105									//�������� TODONOW client��Ҫ����

#pragma endregion

#pragma region MDM_SERVICE �û�����
/* *********************************************************************************
**      MAIN:3    MDM_SERVICE    ����		//�����������пͻ��˴��������ť��Ϣ
**		˵����1��C(�ͻ���)->L(��½��)����Ϣ�ŷ�Χ��	1-100
**			  2��L(�ͻ���)->C(��½��)����Ϣ�ŷ�Χ��	101-200
**			  3��������Ϣ�ŷ�Χ��	201-300���������������ͣ�
** *********************************************************************************/
//������Ϣ�š�
#define MDM_SERVICE								3							//����

//������Ϣ�š�
#define SUB_CL_SERVICE_USER_FEEDBACK			1							//��ҷ���
#define CMD_LC_SERVICE_USER_FEEDBACK			101							//��ҷ�������

#define SUB_CL_SERVICE_REFRESH_USER_INFO		2							//ˢ���û���Ϣ
#define CMD_CL_SERVICE_REFRESH_USER_INFO		102							//ˢ���û���Ϣ����

#define SUB_CL_SERVICE_QUERY_ROOM_LIST			3							//��ѯ������Ϣ�б�
#define CMD_LC_SERVICE_QUERY_ROOM_LIST			103							//������Ϣ����

#define SUB_CL_SERVICE_MODIFY_PERSONAL_INFO		4							//�޸ĸ������ϣ������û����޸ĵ����϶������ﴦ��
#define CMD_LC_SERVICE_MODIFY_PERSONAL_INFO		104							//�޸ĸ������Ϸ���

/* START �ϴ��������а�, ֮�����ɾ�� */
#define SUB_CL_SERVICE_GET_RICH_LIST			5							//��ȡ������
#define CMD_LC_SERVICE_GET_RICH_LIST			105							//�����񷵻�
/* END */

/* START �ϴ�����ս��, ֮�����ɾ���� */
#define SUB_CL_SERVICE_GET_DJ_RECORD_LIST		6							//��ȡ���¼���б�
#define CMD_LC_SERVICE_GET_DJ_RECORD_LIST		106							//��ȡ���¼���б���

#define SUB_CL_SERVICE_GET_XJ_RECORD_LIST		7							//��ȡС��¼���б�
#define CMD_LC_SERVICE_GET_XJ_RECORD_LIST		107							//��ȡС��¼���б���
/* END */

#define SUB_CL_SERVICE_ONLINE_REWARD			8							//��ȡ���߽��������ս���������ȡ������
#define CMD_LC_SERVICE_ONLINE_REWARD			108							//��ȡ���߽�������

#define SUB_CL_SERVICE_GET_TASK_LIST			9							//��ȡ�����б�			//TODO ����״̬�ĸ����߼���û�޸�
#define CMD_LC_SERVICE_GET_TASK_LIST			109							//��ȡ�����б���

#define SUB_CL_SERVICE_GET_TASK_REWARD			10							//��ȡ������
#define CMD_LC_SERVICE_GET_TASK_REWARD			110							//��ȡ����������

#define SUB_CL_SERVICE_REQUEST_LOTTERY			11							//����齱
#define CMD_LC_SERVICE_REQUEST_LOTTERY			111							//����齱����

#define SUB_CL_SERVICE_GET_RANK_LIST			12							//��ȡָ�����а�			TOOD �����������Ǵ�����Ϸ��ȡ�ģ�С�ֽ��㣬��ֻ�н��յ����ݣ�û�и��µ�����֮���
#define CMD_LC_SERVICE_GET_RANK_LIST			112							//��ȡָ�����а񷵻�

#define SUB_CL_SERVICE_GET_RANK_REWARD			13							//��ȡ���а���
#define CMD_LC_SERVICE_GET_RANK_REWARD			113							//��ȡ���а�������

#define CMD_LC_USER_QUERY_SCORE_INFO			114							//��ѯ���޸ģ��û���ҷ�����ʯ��Ϣ����(�漰����ҷ����ı�Ķ����õ������Ϣ)

#define SUB_CL_SERVICE_PURE_STANDING_LIST		15							//pure�������а� ��ѯ
#define CMD_LC_SERVICE_PURE_STANDING_LIST		115							//pure�������а� ����
#define CMD_LC_SERVICE_PURE_STANDING_FINISH		215							//pure�������а� ����

#define SUB_CL_SERVICE_PURE_RECORD_LIST			16							//���ս�� ��ѯ
#define CMD_LC_SERVICE_PURE_RECORD_LIST			116							//���ս�� ����
#define CMD_LC_SERVICE_PURE_RECORD_LIST_PINFO	216							//���ս�������Ϣ ����
#define CMD_LC_SERVICE_PURE_RECORD_FINISH		316							//���ս�� ����

#define SUB_CL_SERVICE_PURE_XJ_RECORD_LIST		17							//С��ս�� ��ѯ
#define CMD_LC_SERVICE_PURE_XJ_RECORD_LIST		117							//С��ս�� ����
#define CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO 217						//С��ս�������Ϣ ����
#define CMD_LC_SERVICE_PURE_XJ_RECORD_FINISH	317							//С��ս�� ����

#define CMD_LC_SERVICE_MARQUEE					18							//�������Ϣ

#define SUB_CL_SERVICE_GOLD_INFO				19							//�����Ҵ�����Ϣ
#define CMD_LC_SERVICE_GOLD_INFO				119							//�����Ҵ�����Ϣ ����
#define CMD_LC_SERVICE_GOLD_INFO_FINISH			219							//�����Ҵ�����Ϣ ����

#define SUB_CL_SERVICE_XJ_RECORD_PLAYBACK		20							//С��¼��ط�
#define CMD_LC_SERVICE_XJ_RECORD_PLAYBACK		120							//С��¼��ط� ����

#define SUB_CL_SERVICE_CUSTOMER_MESSEGE			21							//�ͷ���ʾ��Ϣ
#define CMD_LC_SERVICE_CUSTOMER_MESSEGE			121							//�ͷ���ʾ��Ϣ ����

#pragma endregion

#pragma region MDM_CLUB  ����Ȧ
/* *******************************************************************************
**          MAIN:6      MDM_CLUB  ����Ȧ(�汾2)
** ******************************************************************************/
//������Ϣ�š�
#define MDM_CLUB								6							//����Ȧ(�汾2)

//������Ϣ�š�
/*************** ����Ȧ��� ***********/
#define SUB_CL_CLUB_CREATE_CLUB					1							//��������Ȧ
#define CMD_LC_CLUB_CREATE_CLUB					101							//��������Ȧ����

#define SUB_CL_CLUB_JOIN_CLUB					2							//�����������Ȧ
#define CMD_LC_CLUB_JOIN_CLUB					102							//�����������Ȧ����
#define CMD_LC_CLUB_JOIN_CLUB_BDCAST			202							//�����������Ȧ�㲥

#define SUB_CL_CLUB_DISS_CLUB					3							//��ɢ����Ȧ
#define CMD_LC_CLUB_DISS_CLUB					103							//��ɢ����Ȧ����

#define SUB_CL_CLUB_ALL_CLUB_INFO_LIST			4							//��ѯ��������Ȧ�б�
#define CMD_LC_CLUB_ALL_CLUB_INFO_LIST			104							//��ѯ��������Ȧ�б���
#define CMD_LC_CLUB_ALL_INFO_FINISH				204							//��ѯ��������Ȧ�б����

#define SUB_CL_CLUB_RANDOM_CLUB_LIST			5							//��ѯδ��Ա, �������������Ȧ(���9��)
#define CMD_LC_CLUB_RANDOM_CLUB_LIST			105							//��ѯδ��Ա�������������Ȧ ����
#define CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH		205							//��ѯδ��Ա�������������Ȧ ����

/*************** ����Ȧ ������� -- ���˲�ѯ, ����������Ϸ������ ***********/
#define SUB_CL_CLUB_ROOM_LIST					11							//��ѯָ������Ȧ�����б�
#define CMD_LC_CLUB_ROOM_LIST					111							//��ѯָ������Ȧ�����б� ����
#define CMD_LC_CLUB_ROOM_LIST_FINISH			211							//��ѯָ������Ȧ�����б� ����

#define SUB_CL_CLUB_JOIN_ROOM					12							//������뷿��
#define CMD_LC_CLUB_JOIN_ROOM					112							//������뷿�� ����
#define CMD_LC_CLUB_TABLE_LIST_TABLE			212							//�÷����������Ϣ
#define CMD_LC_CLUB_TABLE_LIST_USER				312							//�÷���������Ϣ

#define SUB_CL_CLUB_ROOM_SETTING				13							//������������ 
#define CMD_LC_CLUB_ROOM_SETTING				113							//������������ ����

#define SUB_CL_CLUB_ROOM_USER_LEAVE				14							//����뿪���� 

#define SUB_CL_CLUB_ROOM_DISSOLVE				15							//��ɢ�������� 
#define CMD_LC_CLUB_ROOM_DISSOLVE				115							//��ɢ�������� ����

#define CMD_LC_CLUB_LIST_RE						116							//ʵʱˢ�¹����б�

//TODONOWW ɾ�������ʱ��Ҳ��Ҫ ʵʱˢ�·����б�
#define CMD_LC_CLUB_ROOM_RE						117							//ʵʱˢ�¹��᷿���б�

#define SUB_CL_CLUB_TABLE_DISSOLVE				18							//��ɢ�������� 
#define CMD_LC_CLUB_TABLE_DISSOLVE				118							//��ɢ�������� ����

#define SUB_CL_CLUB_ROOM_QUERY_SETTING			19							//���󷿷���������Ϣ
#define CMD_LC_CLUB_ROOM_QUERY_SETTING			119							//���󷿷���������Ϣ ����

/*************** ����Ȧ ������� **********/
#define SUB_CL_CLUB_NOTICE						21							//����Ȧ���� -- �ڲ���Ա�ɿ�
#define CMD_LC_CLUB_NOTICE						121							//����Ȧ���� ����
#define CMD_LC_CLUB_NOTICE_BDCAST				221							//����Ȧ���� �㲥

#define SUB_CL_CLUB_MESSAGE						22							//����Ȧ��� -- ���ⲿչʾ����Ϣ
#define CMD_LC_CLUB_MESSAGE						122							//����Ȧ��� ����  ע:����Ȧ��� ����㲥

#define SUB_CL_CLUB_CONTRIBUTE_FK				23							//���׷��� -- ���ⲿչʾ����Ϣ
#define CMD_LC_CLUB_CONTRIBUTE_FK				123							//���׷��� ����

#define SUB_CL_CLUB_AUTO_AGREE					24							//����Ȧ���� -- ��������Զ�ͬ��
#define CMD_LC_CLUB_AUTO_AGREE					124							//����Ȧ���� ����

#define SUB_CL_CLUB_CHAT						25							//����ALL 
#define CMD_LC_CLUB_CHAT						125							//����ALL ����
#define CMD_LC_CLUB_CHAT_BDCAST					225							//����ALL �㲥

#define SUB_CL_CLUB_STICKY_POST					26							//����Ȧ�����ö�
#define CMD_LC_CLUB_STICKY_POST					126							//����Ȧ�����ö�����

/*************** ����Ȧ ��Ա���� **********/
#define SUB_CL_CLUB_MEMBER_MANAGER				31							//�����Ա����
#define CMD_LC_CLUB_MEMBER_MANAGER				131							//�����Ա���� ����
#define CMD_LC_CLUB_MEMBER_MANAGER_FINISH		231							//�����Ա���� ����
#define CMD_LC_CLUB_DATA						331							//���������Ϣ

#define SUB_CL_CLUB_INVITE						32							//���������������Ȧ 
#define CMD_LC_CLUB_INVITE						132							//�������˼�������Ȧ ����
#define CMD_LC_CLUB_INVITE_REMIND				232							//�������˵�����

#define SUB_CL_CLUB_INVITE_RESULT				33							//�������˵Ļظ�
#define CMD_LC_CLUB_INVITE_RESULT				133							//�������˵Ļظ� ����

#define SUB_CL_CLUB_APPLICANT_LIST				34							//����Ȧ���������б�
#define CMD_LC_CLUB_APPLICANT_LIST				134							//����Ȧ���������б� ����
#define CMD_LC_CLUB_APPLICANT_LIST_FINISH		234							//����Ȧ���������б� ����

#define SUB_CL_CLUB_APPLICANT_RESULT			35							//Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�)
#define CMD_LC_CLUB_APPLICANT_RESULT			135							//Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�)

#define SUB_CL_CLUB_QUIT						36							//�û��˳�����Ȧ
#define CMD_LC_CLUB_QUIT						136							//�û����˳�����Ȧ ����

#define SUB_CL_CLUB_APPOINTMENT					37							//ְ������ 0��� 1��Ϊһ���Ա 2����Ϊ����Ա 
#define CMD_LC_CLUB_APPOINTMENT					137							//ְ������ ����
#define CMD_LC_CLUB_APPOINTMENT_NOTE			237							//ְ������ ����

#define SUB_CL_CLUB_INQUERY_LIST				38							//�������˲鿴�Լ��������б�
#define CMD_LC_CLUB_INQUERY_LIST				138							//�������˲鿴�Լ��������б� ����
#define CMD_LC_CLUB_INQUERY_LIST_FINISH			238							//�������˲鿴�Լ��������б� ����


/*************** ����Ȧ ս���鿴 **********/
#define SUB_CL_CLUB_RECORD_LIST					41							//����ս��ͳ�� ��ѯ
#define CMD_LC_CLUB_RECORD_LIST					141							//����ս��ͳ�� ����
#define CMD_LC_CLUB_RECORD_FINISH				241							//����ս��ͳ�� ����

#pragma endregion 

#pragma region MDM_SHOP �̳ǵ���
/* *******************************************************************************
**          MAIN:7      MDM_SHOP   �̳ǵ���
** ******************************************************************************/
//������Ϣ�š�
#define MDM_SHOP								7							//�̳ǵ���

//������Ϣ�š�
#define SUB_CL_SHOP_QUERY						1							//�̳ǹ���
#define CMD_LC_SHOP_QUERY_RESULT				101							//�̳ǹ���ʧ�ܷ���

#define SUB_CL_BAG_QUERY						2							//������Ʒ��ѯ
#define CMD_LC_BAG_RESULT						102							//������Ʒ����
#define CMD_LC_BAG_FINISH						202							//������Ʒ��ѯ����

#pragma endregion 

#pragma region MDM_GIFT �������

/* *******************************************************************************
**          MAIN:5      MDM_GIFT   �������
** ******************************************************************************/
//������Ϣ�š�
#define MDM_GIFT								5							//�������

//������Ϣ�š�
#define SUB_CL_GIFT_GIVE_PROPS					1							//���͵���
#define CMD_LC_GIFT_GIVE_PROPS					101							//���͵��߷���
#define CMD_LC_GIFT_GIVE_PROPS_SHOW				201							//�������˵�����

#define SUB_CL_OTHERS_RECHARGE_INFO				2							//��ֵ��Ϣ
#define CMD_LC_OTHERS_RECHARGE_INFO				102							//���͵��߷���

#define SUB_CL_OTHERS_EXCHANGE_INFO				3							//�һ���Ϣ��Ǯ->���ߣ�
#define CMD_LC_OTHERS_EXCHANGE_INFO				3							//�һ���Ϣ����


//��ֵ��Ϣ
struct STR_SUB_CL_OTHER_RECHARGE_INFO
{
	DWORD								dwUserID;							//�û� I D
};
//��ֵ��Ϣ����
struct STR_CMD_LC_OTHER_RECHARGE_INFO
{
	SCORE								dwMinMoney;		//���ٳ�ֵǮ  
	UINT32								dwChangeScale;  //�һ�����
};

//�һ�����
struct STR_SUB_CL_OTHER_EXCHANGE_INFO
{
	DWORD								dwUserID;							//�û� I D
};
//�һ����߷���
struct STR_CMD_LC_OTHER_EXCHANGE_INFO
{
	SCORE								dwMinMoney;		///���ٳ�ֵǮ  
	UINT32								dwChangeScale;  ///�һ�����
	SCORE								dwWithdrawals;  ///������RMB
	SCORE								dwBankMoney;	///���н��
};

//���ò������
struct STR_CMD_LC_OTHER_OPERATE_RESULT
{
	LONG							lResultCode;						//�������
};
#pragma endregion

#pragma region MDM_GAME ��Ϸ�����
#define MDM_GAME								8							//��Ϸ�����

//client->logon, logon->cpd, cpd->logon, logon->client
#define SUB_CL_GAME_QUERY_GAMEID				1							//��������֮ǰ, �Ȳ�ѯ���õ�GameID
#define CMD_LC_GAME_QUERY_GAMEID				101							//��������֮ǰ, �Ȳ�ѯ���õ�GameID

#pragma endregion

#pragma pack()

#endif