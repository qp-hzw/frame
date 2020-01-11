#ifndef DEFINE_HEAD_FILE
#define DEFINE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//��������
#define MAX_CHAIR					6									//�������
#define MAX_ANDROID					256									//������

//��������
#define INVALID_CHAIR				0xFFFF								//��Ч����
#define INVALID_TABLE				0xFFFF								//��Ч����

//////////////////////////////////////////////////////////////////////////////////
//��������
#define SCORE						LONGLONG							//��������
//ϵͳ����
#define LEN_USER_CHAT				128									//���쳤��

/* ************************************************************************************
**                               ���ݳ���
** ************************************************************************************/
//�����Ϣ
#define LEN_MD5						33									//��������
#define LEN_USERNOTE				32									//��ע����
#define LEN_ACCOUNTS				32									//�ʺų���
#define LEN_NICKNAME				32									//�ǳƳ���
#define LEN_PASSWORD				33									//���볤��
#define LEN_GROUP_NAME				32									//��������
#define LEN_MY_SIGNATURE            128                                 //����ǩ��
#define LEN_HEAD_URL				256									//ͷ���ַ����
#define LEN_USER_NOTE				256									//�û���ע
#define LEN_MOBILE_PHONE			12									//�ƶ��绰
#define LEN_IDENTITY_NUM			19									//֤������
#define LEN_IDENTITY_NAME			16									//��ʵ����
#define LEN_IP_ADDR                 32                                  //IP��ַ����

//������ʶ
#define LEN_MACHINE_ID				33									//���г���

//�б�����
#define LEN_KIND					32									//���ͳ���
#define LEN_SERVER					32									//���䳤��

//////////////////////////////////////////////////////////////////////////////////
//�б�����

//��Ч����
#define UD_NULL						0									//��Ч����
#define UD_IMAGE					100									//ͼ������
#define UD_CUSTOM					200									//�Զ�����

//��������
#define UD_GAME_ID					1									//��Ϸ��ʶ
#define UD_USER_ID					2									//�û���ʶ
#define	UD_NICKNAME					3									//�û��ǳ�

//��չ����
#define UD_GENDER					10									//�û��Ա�
#define UD_GROUP_NAME				11									//��������
#define UD_UNDER_WRITE				12									//����ǩ��

//״̬��Ϣ
#define UD_TABLE					20									//��Ϸ����
#define UD_CHAIR					21									//���Ӻ���

//������Ϣ
#define UD_SCORE					30									//�û�����
#define UD_GRADE					31									//�û��ɼ�
#define UD_USER_MEDAL				32									//�û�����
#define UD_EXPERIENCE				33									//�û�����
#define UD_LOVELINESS				34									//�û�����
#define UD_WIN_COUNT				35									//ʤ������
#define UD_LOST_COUNT				36									//�������
#define UD_DRAW_COUNT				37									//�;�����
#define UD_FLEE_COUNT				38									//�Ӿ�����
#define UD_PLAY_COUNT				39									//�ܾ�����

//���ֱ���
#define UD_WIN_RATE					40									//�û�ʤ��
#define UD_LOST_RATE				41									//�û�����
#define UD_DRAW_RATE				42									//�û�����
#define UD_FLEE_RATE				43									//�û�����
#define UD_GAME_LEVEL				44									//��Ϸ�ȼ�

//��չ��Ϣ
#define UD_NOTE_INFO				50									//�û���ע
#define UD_LOOKON_USER				51									//�Թ��û�

//ͼ���б�
#define UD_IMAGE_FLAG				(UD_IMAGE+1)						//�û���־
#define UD_IMAGE_GENDER				(UD_IMAGE+2)						//�û��Ա�
#define UD_IMAGE_STATUS				(UD_IMAGE+3)						//�û�״̬

//////////////////////////////////////////////////////////////////////////////////
//���ݿⶨ��

#define DB_ERROR 					-1  								//����ʧ��
#define DB_SUCCESS 					0  									//����ɹ�
#define DB_NEEDMB 					18 									//����ʧ��

//////////////////////////////////////////////////////////////////////////////////
//���߱�ʾ
#define PT_USE_MARK_DOUBLE_SCORE    0x0001								//˫������
#define PT_USE_MARK_FOURE_SCORE     0x0002								//�ı�����
#define PT_USE_MARK_GUARDKICK_CARD  0x0010								//���ߵ���
#define PT_USE_MARK_POSSESS         0x0020								//������� 

#define MAX_PT_MARK                 4                                   //��ʶ��Ŀ

//��Ч��Χ
#define VALID_TIME_DOUBLE_SCORE     3600                                //��Чʱ��
#define VALID_TIME_FOUR_SCORE       3600                                //��Чʱ��
#define VALID_TIME_GUARDKICK_CARD   3600                                //����ʱ��
#define VALID_TIME_POSSESS          3600                                //����ʱ��
#define VALID_TIME_KICK_BY_MANAGER  3600                                //��Ϸʱ�� 

//////////////////////////////////////////////////////////////////////////////////
//�豸����
#define DEVICE_TYPE_PC              0x00                                //PC
#define DEVICE_TYPE_ANDROID         0x10                                //Android
#define DEVICE_TYPE_ITOUCH          0x20                                //iTouch
#define DEVICE_TYPE_IPHONE          0x40                                //iPhone
#define DEVICE_TYPE_IPAD            0x80                                //iPad

/////////////////////////////////////////////////////////////////////////////////
//�ֻ�����

//��ͼģʽ
#define	VIEW_MODE_ALL				0x0001								//ȫ������
#define	VIEW_MODE_PART				0x0002								//���ֿ���

//��Ϣģʽ
#define VIEW_INFO_LEVEL_1			0x0010								//������Ϣ
#define VIEW_INFO_LEVEL_2			0x0020								//������Ϣ
#define VIEW_INFO_LEVEL_3			0x0040								//������Ϣ
#define VIEW_INFO_LEVEL_4			0x0080								//������Ϣ

//��������
#define RECVICE_GAME_CHAT			0x0100								//��������
#define RECVICE_ROOM_CHAT			0x0200								//��������
#define RECVICE_ROOM_WHISPER		0x0400								//����˽��

//��Ϊ��ʶ
#define BEHAVIOR_LOGON_NORMAL       0x0000                              //��ͨ��¼
#define BEHAVIOR_LOGON_IMMEDIATELY  0x1000                              //������¼

/////////////////////////////////////////////////////////////////////////////////
//������
#define RESULT_ERROR 					-1  								//�������
#define RESULT_SUCCESS 					0  									//����ɹ�
#define RESULT_FAIL 					1  									//����ʧ��

/////////////////////////////////////////////////////////////////////////////////
//�仯ԭ��
#define SCORE_REASON_WRITE              0                                   //д�ֱ仯
#define SCORE_REASON_INSURE             1                                   //���б仯
#define SCORE_REASON_PROPERTY           2                                   //���߱仯

/////////////////////////////////////////////////////////////////////////////////
//��¼����ʧ��ԭ��
#define LOGON_FAIL_SERVER_INVALIDATION  200                                 //����ʧЧ

//������Ϣ
#define LEN_FEEDBACK_TITLE					32									//��������
#define LEN_FEEDBACK_CONTENT				256									//��������

//��Ϣ����
#define LEN_MESSAGE_DESCRIBE				128									//��Ϣ����

//����Ȧ����
#define LEN_GROUP_NAME						32									//����Ȧ����
#define LEN_GROUP_NOTICE					256									//����Ȧ����
#define LEN_GROUP_MESSAGE					256									//����Ȧ������Ϣ����

#define LEN_MAX_RECORD_SIZE				 4096								//¼���С
#define LEN_MAX_SERVICE_MEG_SIZE		 50									//�ͷ���Ϣ��С


#define SOCKET_TCP_PACKET			1024*4                              //���绺��
#endif