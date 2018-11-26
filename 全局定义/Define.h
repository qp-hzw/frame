#ifndef DEFINE_HEAD_FILE_FRAME
#define DEFINE_HEAD_FILE_FRAME

#pragma pack(1)

/***********************************************000000000000********************************************/
//test服务器 -- 本地测试服务器

const  BYTE		_MYSTERY				=		 0;				          //test服务器	
const  TCHAR    _CPD_SERVER_ADDR[32]    =        TEXT("192.168.5.94");	  //协调服
const  TCHAR	_LOGON_SERVER_ADDR[32]	=		 TEXT("192.168.5.94");	  //登录服 node = 1
const  TCHAR	_GAME_SERVER_ADDR[32]	=		 TEXT("192.168.5.94");	  //游戏服 node = 1


/***********************************************11111111111111********************************************/
//游戏服3 -- 远端测试服务器
/*
const  BYTE		_MYSTERY				=		 1;							 //远端测试服务器
const  TCHAR	_LOGON_SERVER_ADDR[32]	=		 TEXT("47.99.46.57");		 //登录服 node = 1
const  TCHAR	_GAME_SERVER_ADDR[32]	=		 TEXT("47.99.46.57");		 //游戏服 node = 1
*/

/***********************************************22222222222222********************************************/

/*
//聚贤庄服务器
const  BYTE		_MYSTERY				=		 2;				   //聚贤庄
const  TCHAR    _CPD_SERVER_ADDR[32]    =        TEXT("120.79.45.100");		 //协调服
const  TCHAR	_LOGON_SERVER_ADDR[32]	=		 TEXT("116.62.149.123");	//登录服 node = 1
const  TCHAR	_GAME_SERVER_ADDR[32]	=		 TEXT("121.43.164.10");		//游戏服 node = 1
//const  TCHAR	_GAME_SERVER_ADDR[32]	=		 TEXT("120.27.238.146");	//游戏服 node = 2
*/

/***********************************************协调服 登陆服  游戏服 端口********************************************/
/* 服务器端口配置 */
const WORD _MIN_PORT		=           8600;                    			//服务器开放的最小端口 -- [min， min+9]为进程预留
const WORD _MAX_PORT		=           8900;                         	    //服务器开放的最大端口

const WORD  PORT_LOGON		=			_MIN_PORT;							//登陆服务器端口
const WORD  PORT_CENTER		=			_MIN_PORT+1;						//协调服务器端口
const WORD  PORT_MANAGER	=			_MIN_PORT+2;						//管理端口 (未使用)

const WORD _MIN_SERVER_PORT =           _MIN_PORT+10;                       //最小的ServerPort
const WORD _MAX_SERVER_PORT =           _MAX_PORT;                          //最大的ServerPort




//以后扔到define。h中
#define		MAX_TABLE_EX				 200;	//最大桌子数目

#define LEN_MAX_RECORD_SIZE				 4096								//录像大小
#define LEN_MAX_SERVICE_MEG_SIZE		 50									//客服消息大小

#pragma pack()

#endif