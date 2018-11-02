#ifndef DEFINE_HEAD_FILE_FRAME
#define DEFINE_HEAD_FILE_FRAME

#pragma pack(1)

#define		MAX_TABLE_EX				 200;	//���������Ŀ

#define LEN_MAX_RECORD_SIZE				 4096								//¼���С
#define LEN_MAX_SERVICE_MEG_SIZE		 50									//�ͷ���Ϣ��С


/*
//test������
const  BYTE		_MYSTERY				=		 0;				   //��˾ID	
const  TCHAR	_GAME_SERVER_ADDR[32]	=		 TEXT("192.168.5.94");		//��Ϸ����ַ node = 1
*/



//����ׯ������
const  BYTE		_MYSTERY				=		 2;				   //��˾ID	
const  TCHAR	_GAME_SERVER_ADDR[32]	=		 TEXT("121.43.164.10");		//��Ϸ����ַ node = 1
//const  TCHAR	_GAME_SERVER_ADDR[32]	=		 TEXT("120.27.238.146");	//��Ϸ����ַ node = 2



/* �������˿����� */
const WORD _MIN_PORT		=           8600;                    			//���������ŵ���С�˿� -- [min�� min+9]Ϊ����Ԥ��
const WORD _MAX_PORT		=           8900;                         	    //���������ŵ����˿�

const WORD  PORT_LOGON		=			_MIN_PORT;							//��½�������˿�
const WORD  PORT_CENTER		=			_MIN_PORT+1;						//Э���������˿�
const WORD  PORT_MANAGER	=			_MIN_PORT+2;						//����˿� (δʹ��)

const WORD _MIN_SERVER_PORT =           _MIN_PORT+10;                       //��С��ServerPort
const WORD _MAX_SERVER_PORT =           _MAX_PORT;                          //����ServerPort
#pragma pack()

#endif