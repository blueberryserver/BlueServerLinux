#include "ChatRoomManager.h"

namespace BLUE_BERRY
{
DEFINE_MGR(ChatRoomManager)

ChatRoomManager::ChatRoomManager()
{
}


ChatRoomManager::~ChatRoomManager()
{
	_rooms.clear();
}

ChatRoomPtr ChatRoomManager::createRoom(const std::string & name_)
{
	auto room = std::make_shared<ChatRoom>(name_.c_str());

	std::lock_guard<std::recursive_mutex> guard(_mtx);
	_rooms.insert(std::make_pair(name_, room));
	return room;

}

ChatRoomPtr ChatRoomManager::findRoom(const std::string & name_)
{
	std::lock_guard<std::recursive_mutex> guard(_mtx);
	auto it = _rooms.find(name_);
	if (it != _rooms.end())
	{
		return it->second;
	}
	return createRoom(name_);
}


void ChatRoomManager::deleteRoom(const std::string& name_)
{
	std::lock_guard<std::recursive_mutex> guard(_mtx);
	auto it = _rooms.find(name_);
	if (it != _rooms.end())
	{
		_rooms.erase(it);
	}
}

}