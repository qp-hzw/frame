#ifndef CMD_GAME_SERVER_HEAD_FILE
#define CMD_GAME_SERVER_HEAD_FILE

#include "STR_CMD_GameServer.h"

/*
** ��Ҫ����:  ��Ϸ�� �� �������̵� ��Ϣ
** ��ϸ����:  1. ��Ϣ��(�� + ��)     2. �ṹ��
** ��ע˵��:  ����Ϣ�ŵĺ���: 
**            1. ��һ���ֶ�: ������־,        SUB(���ܵ�����Ϣ)�� CMD(���ͳ�ȥ����Ϣ), DBR(�����ݿ�Request)��DBO(���ݿⷵ�ص���Ϣ)
**            2. �ڶ����ֶ�: ��Ϣ����         L: logon;  G:gameserver;  C:client; CP:correspond;  W:web  S:���������Ϸ R:�ͻ�������Ϸ
**            3. �������ֶ�: ����Ϣ�ŵı�־   ����LOGON��Ϊ��¼ģ�����Ϣ
**            4. ֮����ֶ�: ����Ϣ�ŵĺ���   ����Accounts��ʾ�˺ŵ�¼
**           
**            �����淶
**            1. �ṹ�������:  ����Ϣ�ŵ�ǰ���STR, ����SUB_CG_LOGON_ACCOUNTS ��Ϊ STR_SUB_CG_LOGON_ACCOUNTS
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
**						MAIN:1   MDM_LOGON    ��¼ģ��
** *********************************************************************************/
#define MDM_GR_LOGON				1										//��¼��Ϣ

//��¼
#define SUB_CG_LOGON_USERID			1										//ID ��¼
#define CMD_GC_LOGON_USERID			101										//ID ��¼����

#define CMD_GC_LOGON_GET_USER_INFO	102										//����û�������Ϣ	USERID��¼��Ϸ���ɹ�֮��ᷢ�͸��ͻ���

#pragma endregion

#pragma region MDM_USER �û�����
/* *********************************************************************************
**							MAIN:2   MDM_USER    �û�����
** *********************************************************************************/
#define MDM_USER						2									//�û���Ϣ

//����Ϣ  *��ʾ�ͻ���δ�õ�		#��������		
#define SUB_CG_USER_RULE				1									//*�û�����		TODO ò���������û�IP��ʤ�ʡ������ʡ����֣�Ӧ����Ҫ����
#define SUB_CG_USER_LOOKON				2									//*�Թ�����		TODO �ɹ��޷��أ�ʧ�ܷ���103����ʧ��
#define SUB_CG_USER_SITDOWN				3									//*���������û�����/��������ɹ��󣬷�����������������£�����Ҫ����
#define SUB_CG_USER_STANDUP				4									//#��������		TODO �Ƿ�����ɢ���䣬���͵��Ǹ���Ϣ������15�Ĵ���������
#define SUB_CG_USER_INVITE_USER			5									//*�����û���������
#define CMD_GC_USER_INVITE_USER			6									//*�����û��������ӷ���
#define SUB_CG_USER_REFUSE_SIT  		7									//*�ܾ��������	TODO ���������������ϵͳ��Ϣ�������û�����
#define SUB_CG_USER_KICK_USER			8                                   //*�߳��û�		TODO ���������������ϵͳ��Ϣ���߳��û�

#define SUB_GR_USER_CHAIR_REQ			10                                  //*�������λ��
#define SUB_GR_USER_WAIT_DISTRIBUTE		12									//�ȴ�����		TODO ���û�׼���󣬷��͵Ŀհ����ͻ�����δ����ˣ�

#pragma region ���� && ����
#define SUB_CG_USER_CREATE_ROOM			13									//#��������
#define CMD_GC_USER_CREATE_ROOM_SUCCESS	121									//#�����ɹ�		TODO �����������������Ȧ����ķ��أ���ʵ��������ĳɹ�����Ӧ�ö��ø���Ϣ

#define SUB_CG_CLUB_CREATE_TABLE		15									//#�������� ����Ȧ����
#define CMD_GC_CLUB_CREATE_TABKE		115									//#�������� ����

#define SUB_CG_USER_JOIN_FK_ROOM		14									//#�������� ��Ҫ����
#define CMD_GC_USER_JOIN_ROOM_SUCCESS	122									//#����ɹ�		���뷿��/���/����Ȧ����ķ���

#define SUB_CG_USER_JOIN_TABLE_NO_PASS	16									//#��������,����Ҫ����, �����ٿ�ʼ
//���ص�Ϊ 2, CMD_GC_USER_JOIN_ROOM_SUCCESS;								����Ҫ�������ӷ���

#define SUB_CG_USER_JOIN_GOLD_HALL_ROOM	17									//���������ҳ�����
//���û�п���, ���ص��� 2, CMD_GC_CLUB_CREATE_TABKE
//����п�����, ���ص�Ϊ 2, CMD_GC_USER_JOIN_ROOM_SUCCESS

#pragma endregion

#define SUB_CG_USER_JOIN_GROUP_ROOM		18									//(��ɾ��)��������Ȧ���� ���ؼ��뷿��ɹ� ���ϴ�����
#define SUB_CG_USER_JOIN_GOLD_ROOM		19									//(��ɾ��)�����ҷ���û�п�λ�ý�ҷ������ȴ����� TODO Ϊ�˴���ͨ���ԣ���������ɿͻ�����Ϸ����

#define SUB_GR_GET_TABLELIST			21									//��ȡ�����б�		TODO ָ���Ǿ��������ѿ������б�
#define SUB_GR_GET_TABLELIST_RESULT		131									//�����б���		

#define SUB_GR_LOTTERY_NOTIFY			22									//�齱֪ͨ CMD TODO ���͵Ŀհ�
#define SUB_GR_QUERY_LOTTERY			23									//����齱 SUB
#define SUB_GR_LOTTERY_RESULT			132									//�齱��� CMD

#define SUB_CG_USER_RECONNECT_ROOM		24									//#��������
#define CMD_GC_USER_RECONNECT_ROOM		134									//#������������

#define SUB_CG_USER_CREATE_GROUP_ROOM	25									//(��ɾ��)#��������Ȧ���䣨ֻ��Ȧ�����ܴ������䣩	TODO ����Ȧ���ڵ�������Ϣ����
#define CMD_GC_USER_CREATE_GROUP_ROOM	205									//(��ɾ��)#��������Ȧ���䷵��

#define SUB_GR_USER_SCORE				101									//�û�����		TODO ����Ϸ����д�ֺ������������͸��ͻ���
#define SUB_GR_USER_STATUS				102									//�û�״̬		TODO �����û�״̬ʱ�������OnEventUserItemStatus�����ᷢ�͸���Ϣ���ͻ���
#define SUB_GR_REQUEST_FAILURE			103									//����ʧ��		�󲿷�ʧ����Ϣ�����������Ϣ

#define CMD_GC_USER_MODIFY_TREASURE		106									//�޸��û��Ƹ���Ϣ����

#define SUB_GR_CREATE_OTHER_SUCCESS		129									//*�����˿����ɹ�		TODO ɾ���������˿����ɹ����ڿ����ɹ��У�����Ҫ��������
#define SUB_GR_JOIN_DWTABLE_SUCCESS		133									//*������淿�ɹ�		�߼�������������뷿�䲻ͬ��ò���Ǽ���ɹ��Ϳ�ʼ��Ϸ��

#define SUB_GF_ONLINE_PLAYERS			313									//*��������

#pragma endregion

#pragma region MDM_GR_STATUS ״̬����
/* *********************************************************************************
**						 MAIN:3   MDM_GR_STATUS    ״̬����
** *********************************************************************************/
#define MDM_GR_STATUS				3									//״̬��Ϣ

#define CMD_GR_TABLE_INFO			100									//������Ϣ
#define CMD_GR_TABLE_STATUS			101									//����״̬

#pragma endregion

#pragma region  MDM_GR_MATCH ��������
/* *********************************************************************************
**						MAIN:7   MDM_GR_MATCH    ��������
** *********************************************************************************/
#define MDM_GR_MATCH				7									//��������

#define SUB_GR_MATCH_FEE			400									//��������
#define SUB_GR_MATCH_NUM			401									//�ȴ�����
#define SUB_GR_LEAVE_MATCH			402									//�˳�����
#define SUB_GR_MATCH_INFO			403									//������Ϣ
#define SUB_GR_MATCH_WAIT_TIP		404									//�ȴ���ʾ
#define SUB_GR_MATCH_RESULT			405									//�������
#define SUB_GR_MATCH_STATUS			406									//����״̬
#define SUB_GR_MATCH_USER_COUNT		407									//��������
#define SUB_GR_MATCH_DESC			408									//��������

#pragma endregion

#pragma region MDM_G_FRAME �������
/* *********************************************************************************
**							MAIN:100   MDM_G_FRAME    �������
** *********************************************************************************/
#define MDM_G_FRAME					100									//�������

//������Ϣ
#define SUB_RG_FRAME_OPTION					1								//��Ϸ����
#define CMD_GR_FRAME_GAME_OPTION			101								//��Ϸ����
#define CMD_GR_FRAME_GAME_STATUS			102								//��Ϸ״̬
#define CMD_GR_FRAME_GAME_DISSMISS			301							    //��ɢ���״̬

//��ɢ����
#define SUB_RG_FRAME_ASK_DISMISS			2								//���������ɢ����
#define CMD_GR_FRMAE_VOTE_DISMISS			103								//�㲥���������ɢ
#define CMD_GR_FRMAE_ASK_DISMISS_RESULT     202								//���������ɢ���� ���

#define	SUB_RG_FRAME_VOTE_DISMISS			3								//�����ɢ����
#define	CMD_GR_FRAME_DISMISS_RESULT			104								//�㲥�����ɢ������

//GPS���
#define SUB_CG_COM_CHECK_USER_GPS			5								//����У���û�GPSλ����Ϣ
#define CMD_GC_COM_CHECK_USER_GPS			105								//У���û�GPSλ����Ϣ����

//IPУ��
#define SUB_CG_COM_CHECK_USER_IP			8								//����У���û�IP��Ϣ
#define CMD_GC_COM_CHECK_USER_IP			108								//У���û�IP��Ϣ����

//�û�׼��
#define SUB_GF_USER_READY					4								//�û�׼��

//�û�����
#define SUB_RG_FRAME_CHAT					7								//�û�����
#define CMD_GR_FRAME_CHAT					107								//�û�����


//����clientû��
#define SUB_GF_LOOKON_CONFIG			803									//�Թ�����

#define SUB_GF_XUFEI					805									//*������Ϣ

#define SUB_GF_USER_EXPRESSION			811									//*�û�����
#define SUB_GF_SOUND					812									//*��������

#define CMD_GR_FRAME_TREASURE_NO		105									//*�û��Ƹ������Լ����¾���Ϸʱ,���͸�client 


//��Ϸ��Ϣ
#define CMD_GF_LOOKON_STATUS			803									//*�Թ�״̬

//ϵͳ��Ϣ
#define CMD_GF_SYSTEM_MESSAGE			806									//ϵͳ��Ϣ

#pragma endregion

#pragma region MDM_GF_GAME ����Ϸ����
/* *********************************************************************************
**							MAIN:200   MDM_GF_GAME    ����Ϸ����
** *********************************************************************************/
#define MDM_GF_GAME					200									//��Ϸ����

#pragma endregion

#pragma pack()

#endif