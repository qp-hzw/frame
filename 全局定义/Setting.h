#ifndef DEFINE_HEAD_FILE_FRAME
#define DEFINE_HEAD_FILE_FRAME

//登陆服地址(外网ip, 供协调服记录使用) 会自动获取
//游戏服地址(外网ip; 与数据库校验使用) 会自动获取
//协调服 必须手动指定
/***********************************************22222222222222********************************************/
//测试服务器
const  BYTE		_MYSTERY				=		 2;				   
const  TCHAR    _CPD_SERVER_ADDR[32]    =        TEXT("127.0.0.1");			//协调服 -- 必须手动指定


/***********************************************22222222222222********************************************
//聚贤庄服务器
const  BYTE		_MYSTERY				=		 2;				   //聚贤庄
const  TCHAR    _CPD_SERVER_ADDR[32]    =        TEXT("120.27.235.52");		//协调服
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

#endif