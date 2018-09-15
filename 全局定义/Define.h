#ifndef DEFINE_HEAD_FILE_FRAME
#define DEFINE_HEAD_FILE_FRAME

#pragma pack(1)

#define		MAX_TABLE_EX				 200;	//最大桌子数目



const  BYTE		_MYSTERY				=		 2;				   //公司ID	
const  TCHAR	_GAME_SERVER_ADDR[32]	=		 TEXT("121.43.164.10");		//游戏服地址 node = 1


/* 服务器端口配置 */
const WORD _MIN_PORT		=           8600;                    			//服务器开放的最小端口 -- [min， min+9]为进程预留
const WORD _MAX_PORT		=           8900;                         	    //服务器开放的最大端口

const WORD  PORT_LOGON		=			_MIN_PORT;							//登陆服务器端口
const WORD  PORT_CENTER		=			_MIN_PORT+1;						//协调服务器端口
const WORD  PORT_MANAGER	=			_MIN_PORT+2;						//管理端口 (未使用)

const WORD _MIN_SERVER_PORT =           _MIN_PORT+10;                       //最小的ServerPort
const WORD _MAX_SERVER_PORT =           _MAX_PORT;                          //最大的ServerPort
#pragma pack()

#endif