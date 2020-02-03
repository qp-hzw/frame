#ifndef CMD_CORRESPOND_HEAD_FILE
#define CMD_CORRESPOND_HEAD_FILE

#include "STR_CMD_Correspond.h"

/*
** ��Ҫ����:  Э���� �� ��������(��¼��, ��Ϸ��, web��̨) ��Ϣ
** ��ϸ����:  1. ��Ϣ��(�� + ��)     2. �ṹ��
** ��ע˵��:  ����Ϣ�ŵĺ���: 
**            1. ��һ���ֶ�: ������־,
**				 CPR(��������->Э��������Ϣ)�� CPO(Э����->�������̵���Ϣ), 
**            2. �ڶ����ֶ�: ��Ϣ����         
**				 L: logon;  G:gameserver; W:web;  P:Э����
**            3. �������ֶ�: ����Ϣ�ŵı�־   ����REGISTER��Ϊע��ģ�����Ϣ
**            4. ֮����ֶ�: ����Ϣ�ŵĺ���   ����GameServer��ʾ��Ϸ��ע��
**           
**            �����淶
**            1. �ṹ�������:  ����Ϣ�ŵ�ǰ���STR, ����SUB_CL_LOGON_ACCOUNTS ��Ϊ STR_SUB_CL_LOGON_ACCOUNTS
**
**            2. �����������淶: 1) ָ���p  2)������־CPR  3)����Ϣ�ű�־  4)����Ϣ�ű�־
**
**            3. ����������:  On_������־_����Ϣ������Ϣ��
**
**
*/

#pragma region MDM_REGISTER ע��ģ��
//ע������
#define MDM_REGISTER_CPD			1									//ע��ģ��

//����ע��
#define CPR_REGISTER_SERVER  	    1									//����ע��

//ע����
#define CPO_REGISTER_SUCESS	        101									//ע�����
#define CPO_REGISTER_FAILURE	    102									//ע��ʧ��

#pragma endregion


#pragma region MDM_TRANSFER ��ת����
#define CPD_MDM_TRANSFER			2									//��ת����

#define CPR_GP_CLUB_TABLE_INFO		1									//�㲥������Ϣ
#define CPO_PL_CLUB_TABLE_INFO		101									//�㲥������Ϣ

#define CPR_GP_CLUB_PLAYER_INFO		2									//�㲥�û���Ϣ
#define CPO_PL_CLUB_PLAYER_INFO		102									//�㲥�û���Ϣ

#define CPR_LP_CLUB_TABLE_DISSOLVE	3									//��ɢ����
#define CPO_PG_CLUB_TABLE_DISSOLVE	103									//��ɢ����

#define CPR_LP_CREATE_TABLE			4									//�������� ��ѯ���õ�GameID
#define CPO_PL_CREATE_TABLE			104									//�������� ��ѯ���õ�GameID

#define CPR_LP_OFFLINE_PLAYERQUERY	5									//��ѯ������� logon->correspond
#define CPR_PG_OFFLINE_PLAYERQUERY	105									//��ѯ������� correspond ->game
#define CPO_GP_OFFLINE_FINISH		205									//��ѯ������ҷ���  game->correspond
#define CPO_PL_OFFLINE_FiNISH		305									//��ѯ������ҷ���  correspond->logon
#pragma endregion 

#endif