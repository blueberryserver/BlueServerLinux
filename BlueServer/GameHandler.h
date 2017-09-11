#pragma once
#include "ChatHandler.h"

namespace BLUE_BERRY
{

class GameHandler: public ChatHandler
{
public:
	GameHandler();

public:
	DECLARE_HANDLER(SessionPtr, CreateCharReq);
	DECLARE_HANDLER(SessionPtr, PlayDungeonReq);
	DECLARE_HANDLER(SessionPtr, LevelupCharReq);
	DECLARE_HANDLER(SessionPtr, TierupCharReq);

public:
	DECLARE_MGR(GameHandler)
};

EXTERN_MGR(GameHandler)

}