# ifndef _MATCH_MANAGER_H_
# define _MATCH_MANAGER_H_

# include "stdafx.h"
# include "DataBaseEngineSink.h"

class CMatchManager
{
public:
	//读取比赛的配置信息
	static bool ReadMatchConfig(STR_DBR_GR_MATCH_CONFIG config);
};

# endif