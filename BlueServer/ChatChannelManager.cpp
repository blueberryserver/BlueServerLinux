#include "ChatChannelManager.h"

namespace BLUE_BERRY
{

DEFINE_MGR(ChatChannelManager)

ChatChannelManager::ChatChannelManager()
{
}


ChatChannelManager::~ChatChannelManager()
{
	_channels.clear();
}

ChatChannelPtr ChatChannelManager::createChannel(const std::string & country_)
{
	auto channel = std::make_shared<ChatChannel>(country_.c_str());
	_channels.insert(std::make_pair( country_, channel));
	return channel;
}

ChatChannelPtr ChatChannelManager::findChannel(const std::string& country_)
{
	auto it = _channels.find(country_);
	if (it != _channels.end())
	{
		return it->second;
	}
	return createChannel(country_);
}


}