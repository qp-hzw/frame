#ifndef DEFINE_HEAD_FILE_FRAME
#define DEFINE_HEAD_FILE_FRAME

#pragma pack(1)

/***********************************************000000000000********************************************/
//test������ -- ���ز��Է�����

const  BYTE		_MYSTERY				=		 0;				          //test������	
const  TCHAR    _CPD_SERVER_ADDR[32]    =        TEXT("192.168.5.94");	  //Э����
const  TCHAR	_LOGON_SERVER_ADDR[32]	=		 TEXT("192.168.5.94");	  //��¼�� node = 1
const  TCHAR	_GAME_SERVER_ADDR[32]	=		 TEXT("192.168.5.94");	  //��Ϸ�� node = 1


/***********************************************11111111111111********************************************/
//��Ϸ��3 -- Զ�˲��Է�����
/*
const  BYTE		_MYSTERY				=		 1;							 //Զ�˲��Է�����
const  TCHAR	_LOGON_SERVER_ADDR[32]	=		 TEXT("47.99.46.57");		 //��¼�� node = 1
const  TCHAR	_GAME_SERVER_ADDR[32]	=		 TEXT("47.99.46.57");		 //��Ϸ�� node = 1
*/

/***********************************************22222222222222********************************************/

/*
//����ׯ������
const  BYTE		_MYSTERY				=		 2;				   //����ׯ
const  TCHAR    _CPD_SERVER_ADDR[32]    =        TEXT("120.79.45.100");		 //Э����
const  TCHAR	_LOGON_SERVER_ADDR[32]	=		 TEXT("116.62.149.123");	//��¼�� node = 1
const  TCHAR	_GAME_SERVER_ADDR[32]	=		 TEXT("121.43.164.10");		//��Ϸ�� node = 1
//const  TCHAR	_GAME_SERVER_ADDR[32]	=		 TEXT("120.27.238.146");	//��Ϸ�� node = 2
*/

/***********************************************Э���� ��½��  ��Ϸ�� �˿�********************************************/
/* �������˿����� */
const WORD _MIN_PORT		=           8600;                    			//���������ŵ���С�˿� -- [min�� min+9]Ϊ����Ԥ��
const WORD _MAX_PORT		=           8900;                         	    //���������ŵ����˿�

const WORD  PORT_LOGON		=			_MIN_PORT;							//��½�������˿�
const WORD  PORT_CENTER		=			_MIN_PORT+1;						//Э���������˿�
const WORD  PORT_MANAGER	=			_MIN_PORT+2;						//����˿� (δʹ��)

const WORD _MIN_SERVER_PORT =           _MIN_PORT+10;                       //��С��ServerPort
const WORD _MAX_SERVER_PORT =           _MAX_PORT;                          //����ServerPort




//�Ժ��ӵ�define��h��
#define		MAX_TABLE_EX				 200;	//���������Ŀ

#define LEN_MAX_RECORD_SIZE				 4096								//¼���С
#define LEN_MAX_SERVICE_MEG_SIZE		 50									//�ͷ���Ϣ��С

#pragma pack()

#endif