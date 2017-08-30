#pragma once
#include <unordered_map>
#include "ChatChannel.h"

namespace BLUE_BERRY
{

class ChatChannelManager
{
public:
	ChatChannelManager();
	virtual ~ChatChannelManager();

	ChatChannelPtr createChannel(const std::string& country_);
	ChatChannelPtr findChannel(const std::string& country_);

private:
	std::unordered_map< std::string, ChatChannelPtr > _channels;

public:
	DECLARE_MGR(ChatChannelManager)
};

EXTERN_MGR(ChatChannelManager)
}