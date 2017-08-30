#pragma once
#include <unordered_map>
#include "ChatRoom.h"

namespace BLUE_BERRY
{
class ChatRoomManager
{
public:
	ChatRoomManager();
	virtual ~ChatRoomManager();

	ChatRoomPtr createRoom(const std::string& name_);
	ChatRoomPtr findRoom(const std::string& name_);
	void deleteRoom(const std::string& name_);

private:
	std::recursive_mutex _mtx;
	std::unordered_map< std::string, ChatRoomPtr > _rooms;

public:
	DECLARE_MGR(ChatRoomManager)
};

EXTERN_MGR(ChatRoomManager)

}