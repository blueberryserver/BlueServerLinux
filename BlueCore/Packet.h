#pragma once
namespace BLUE_BERRY
{

#define SEND_BUFFER_SIZE 65535

class Header
{
public:
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
	~Packet() {}

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

		// �߸��� ��Ŷ ���� Ȯ��
		if (_curPacketLen < sizeof(Header) || _curPacketLen > SEND_BUFFER_SIZE)
		{
			// �ַ� ��Ŷ ũ�� �ַ�
			//___Log->write(Log::CONS_FILE_LOG, "packet header length error type: ", header->_id, " len: ", header->_len);
			//���� ����
			//disconnect();
			return false;
		}

		// ��Ŷ ���̿� ���� ���� ���� Ȯ��
		if (_curPacketLen > static_cast<short>(_restBuffLen))
		{
			return false;
		}

		return (_curPacketLen >= sizeof(Header));
	}


	short getId()
	{
		return _curId;
	}

	char* getData()
	{
		// ��Ŷ = ��� + ������
		auto data = _curPacketBuff + sizeof(Header);

		// ������
		return data;
	}

	unsigned short getDataLength()
	{
		return static_cast<unsigned short>(_curPacketLen - sizeof(Header));
	}
	unsigned short getPacketLength()
	{
		return static_cast<unsigned short>(_curPacketLen);
	}
};

}