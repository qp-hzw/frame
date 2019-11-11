#ifndef GAME_SERVICE_HEAD_HEAD_FILE
#define GAME_SERVICE_HEAD_HEAD_FILE

//桌子接口
interface ITableFrame : public IUnknownEx
{
	//配置参数
public:
	//读取通用房间规则
	virtual VOID* GetCustomRule() =NULL;
	//读取子游戏特有房间规则
	virtual VOID * GetSubGameRule()=NULL;

	//写分接口
public:
	//写入录像记录 参数 小局数,数据和长度
	virtual bool WriteRecordInfo(WORD wXJCount,TCHAR strScore[],  VOID* pData, DWORD dwDataSize) = NULL;
	//处理小局结束
	virtual bool HandleXJGameEnd(BYTE cbCurGameCount, BYTE cbGameMode, SCORE *lGameScore) = NULL;

	//时间接口
public:
	//设置时间
	virtual bool SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)=NULL;
	//删除时间
	virtual bool KillGameTimer(DWORD dwTimerID)=NULL;
	//获取定时器剩余毫秒数
	virtual DWORD GetTimerLeftTickCount(DWORD dwTimerID)=NULL;

	//功能接口
public:
	//发送场景
	virtual bool SendGameScene(WORD  pIServerUserItem_chair, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////
//回调接口
interface ITableFrameSink : public IUnknownEx
{
	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart()=NULL;
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, WORD pIServerUserItem_chair, BYTE cbReason)=NULL;
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChairID, BYTE cbGameStatus, bool bSendSecret)=NULL;

	//事件接口
public:
	//时间事件
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)=NULL;

	//网络接口
public:
	//游戏消息
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD  pIServerUserItem_chair)=NULL;
	//框架消息
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD  pIServerUserItem_chair)=NULL;

};

#endif