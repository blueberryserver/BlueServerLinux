#pragma once
namespace BLUE_BERRY
{
#define SEND_BUFFER_SIZE 65535
struct Header
{

	short _len;
	short _id;
};

class Packet : public Header
{
private:
	char* _recvBuff;
	char* _curPacketBuff;
	size_t _restBuffLen;
	size_t _curPacketLen;
	short _curId;

public:
	explicit Packet(char* recvBuff_, size_t restBuffLen_)
		: _recvBuff(recvBuff_), _curPacketBuff(recvBuff_), _restBuffLen(restBuffLen_), _curPacketLen(0), _curId(-1)
	{

	}
	//explicit Packet(int id_, char* sendDataBuff_, size_t sendDataBuffLen_);
	~Packet() = default;

	bool next()
	{
		_curPacketBuff += _curPacketLen;

		auto header = reinterpret_cast<Header*>(_curPacketBuff);
		if (header)
		{
			_restBuffLen -= _curPacketLen;
			_curPacketLen = header->_len;
			_curId = header->_id;
		}
		else
		{
			_curPacketBuff = nullptr;
			_restBuffLen = 0;
			_curId = -1;
		}

		// wrong packet length
		if (_curPacketLen < sizeof(Header) || _curPacketLen > SEND_BUFFER_SIZE)
		{
			//___Log->write(Log::CONS_FILE_LOG, "packet header length error type: ", header->_id, " len: ", header->_len);
			//disconnect();
			return false;
		}

		if (_curPacketLen > _restBuffLen)
		{
			return false;
		}

		return (_curPacketLen >= sizeof(Header));
	}


	short getId() const
	{
		return _curId;
	}

	char* getData() const
	{
		auto data = _curPacketBuff + sizeof(Header);
		return data;
	}

	unsigned short getDataLength() const
	{
		return static_cast<unsigned short>(_curPacketLen - sizeof(Header));
	}
	unsigned short getPacketLength() const
	{
		return static_cast<unsigned short>(_curPacketLen);
	}
};

}