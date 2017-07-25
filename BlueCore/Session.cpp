#include "Session.h"
#include <iostream>
#include "Logger.h"
namespace BLUE_BERRY
{

Session::Session(boost::asio::io_service& io_)
	: _socket(io_)
{
	_recvBuff = new CircularBuffer();
	_sendBuff = new CircularBuffer();
}

Session::~Session()
{
	delete _recvBuff;
	delete _sendBuff;
}

void Session::onRecvComplete(boost::system::error_code errCode_, std::size_t length_)
{
	if (length_ == 0)
	{
		LOG(L_INFO_, "Disconnect", "socket", (int)_socket.native(), "length", (int)length_);
		return;
	}

	LOG(L_INFO_, "Recv Complete", "socket", (int)_socket.native(), "length", (int)length_);

	// recv packet proc

	// call async recv
	postRecv();
}

void Session::onSendComplete(boost::system::error_code errCode_, std::size_t length_)
{
	LOG(L_INFO_, "Send Complete", "socket", (int)_socket.native());
	_sending.store(false);

	_sendBuff->remove(length_);

	postSend();
}

void Session::onAcceptComplete()
{
	LOG(L_INFO_, "Accept Complete", "socket", (int)_socket.native());
	_connected.store(true);
	postRecv();
}

void Session::onConnectComplete()
{
	LOG(L_INFO_, "Connect Complete", "socket", (int)_socket.native());
	_connected.store(true);
}

void Session::send(BufferHelperPtr sendBuff_)
{
	if (_connected.load() == false)
	{
		return;
	}

	if (_reservedSendBuffCount.load() >= SHRT_MAX)
	{
		disconnect();
		return;
	}

	_reservedSendBuffs.push(sendBuff_);
	_reservedSendBuffCount.fetch_add(1);

	if (_sending.load() == false)
	{
		postSend();
	}
}

void Session::disconnect()
{
	_socket.close();
	_connected.store(false);
}

void Session::postRecv()
{
	auto bufferSize = _recvBuff->getFreeSize();
	auto buff = _recvBuff->getWritableBuffer();

	_socket.async_receive(
		boost::asio::buffer(buff, bufferSize),
		std::bind(&Session::onRecvComplete, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Session::postSend()
{
	_sending.store(true);

	if(_connected.load() == false)
	{
		return;
	}

	if (_reservedSendBuffCount.load() == 0)
	{
		return;
	}

	std::vector<BufferHelperPtr> tempBuffers; tempBuffers.reserve(1024);
	if (_reservedSendBuffs.popAll(tempBuffers) == true)
	{
		long tempBufferCount = 0;
		for (auto it = tempBuffers.begin(); it != tempBuffers.end(); ++it)
		{
			if (_sendBuff->getFreeSize() > (*it)->_len)
			{
				// 전송 버퍼에 넣기
				memcpy((void*)_sendBuff->getWritableBuffer(), (*it)->_start, (*it)->_len);
				_sendBuff->commit((*it)->_len);
				tempBufferCount++;
			}
			else
			{
				// 전송 버퍼가 부족할 경우 다시 예약 전송 규에 규잉
				_reservedSendBuffs.push(*it);
			}
		}
		tempBuffers.clear();

		_reservedSendBuffCount.fetch_sub(tempBufferCount);
	}

	auto buffSize = _sendBuff->getContiguiousBytes();
	auto buff = _sendBuff->getReadableBuffer();

	if (buffSize == 0)
	{
		_sending.store(false);
		return;
	}

	_socket.async_send(
		boost::asio::buffer(buff, buffSize),
		std::bind(&Session::onSendComplete, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

}
