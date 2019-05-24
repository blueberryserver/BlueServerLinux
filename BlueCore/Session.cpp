#include "Session.h"
#include <iostream>
#include "Logger.h"
#include "Packet.h"
#include "MsgHandler.h"


namespace BLUE_BERRY
{
Session::Session(tcp::socket socket_)
	: _socket(std::move(socket_)), _packetProc(nullptr)
{
	_recvBuff = new CircularBuffer();
	_sendBuff = new CircularBuffer();
	_reservedSendBuffCount.store(0);
	_sending.store(false);
	_connected.store(false);
}

Session::Session(boost::asio::io_context& io_)
	:_socket(io_), _packetProc(nullptr)
{
	_recvBuff = new CircularBuffer();
	_sendBuff = new CircularBuffer();
	_reservedSendBuffCount.store(0);
	_sending.store(false);
	_connected.store(false);
}

Session::~Session()
{
	//std::cout << "destory session" << std::endl;
	delete _recvBuff;
	delete _sendBuff;
	
	std::vector<BufferHelperPtr> tempBuffers; tempBuffers.reserve(1024);
	if (_reservedSendBuffs.popAll(tempBuffers) == true)
	{		
		tempBuffers.clear();
		_reservedSendBuffCount.store(0);
	}	
	_packetProc = nullptr;
	_socket.close();
}

void Session::onRecvComplete(boost::system::error_code errCode_, std::size_t length_)
{
	if (errCode_ == boost::asio::error::operation_aborted )
	{
		LOG(L_DEBUG_, "Disconnect", "socket", (int)_socket.native_handle(), "length", (int)length_);
		_connected.store(false);
		onClose();
		return;
	}

	LOG(L_DEBUG_, "Recv Complete", "socket", (int)_socket.native_handle(), "length", (int)length_);

	_recvBuff->commit(length_);


	// recv packet proc
	if (_packetProc != nullptr)
	{
		_packetProc(_recvBuff);
	}
	else
	{
		recvPacketProc();
	}

	// call async recv
	postRecv();
}

void Session::onSendComplete(boost::system::error_code errCode_, std::size_t length_)
{
	LOG(L_DEBUG_, "Send Complete", "socket", (int)_socket.native_handle(), "length", (int)length_);
	_sending.store(false);

	_sendBuff->remove(length_);

	postSend();
}

void Session::onAcceptComplete()
{
	LOG(L_DEBUG_, "Accept Complete", "socket", (int)_socket.native_handle());
	_connected.store(true);

	// call async recv
	postRecv();
}

void Session::onConnectComplete(boost::system::error_code errCode_)
{
	LOG(L_DEBUG_, "Connect Complete", "socket", (int)_socket.native_handle());
	_connected.store(true);

	// call async recv
	postRecv();
}

void Session::send(BufferHelperPtr sendBuff_)
{
	if (_connected.load() == false)
	{
		return;
	}

	if (_reservedSendBuffCount.load() >= SHRT_MAX)
	{
		LOG(L_DEBUG_, "disconnect", "socket", (int)_socket.native_handle());
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

bool Session::isConnected()
{
	return _connected.load() == true;
}

void Session::connect(const std::string& addr_, const std::string& port_)
{
	//boost::asio::ip::tcp::endpoint endPoint(boost::asio::ip::address::from_string(addr_), port_);
	//_socket.async_connect(endPoint, std::bind(&Session::onConnectComplete, shared_from_this(), std::placeholders::_1));

	tcp::resolver resolver(_socket.get_executor());
	auto endpoints = resolver.resolve(tcp::resolver::query(addr_, port_));

	boost::asio::async_connect(_socket, endpoints, [this](boost::system::error_code ec_, tcp::endpoint endpoint_) {
		if (!ec_)
		{
			this->onConnectComplete(ec_);
		}
	});		
}

void Session::postRecv()
{
	auto bufferSize = _recvBuff->getFreeSize();
	auto buff = _recvBuff->getWritableBuffer();

	_socket.async_receive(
		boost::asio::buffer(buff, bufferSize),
		[sharedThis = shared_from_this()](auto ec, auto len) { sharedThis->onRecvComplete(ec, len); }
		);
}

void Session::postSend()
{
	//LOG(L_INFO_, "post Send", "reservedCount", (int)_reservedSendBuffCount.load());

	_sending.store(true);
	if(_connected.load() == false)
	{
		_sending.store(false);
		return;
	}

	if (_reservedSendBuffCount.load() == 0)
	{
		_sending.store(false);
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
		[sharedThis = shared_from_this()](auto ec, auto len) { sharedThis->onSendComplete(ec, len); }
		);
}


void Session::recvPacketProc()
{
	auto rBufferPoint = _recvBuff->getReadableBuffer();
	auto recvBuffSize = _recvBuff->getContiguiousBytes();

	Packet packet(rBufferPoint, recvBuffSize);
	while (packet.next())
	{
		/*char* packet =
		size_t restBuffSize = packet.getPacket();*/

		//__msgHandler->execute(shared_from_this(), packet.getId(), packet.getData(), packet.getDataLength());

		//recvBuffSize -= header->_len;
		_recvBuff->remove(packet.getPacketLength());
	}


	char buff[1024] = { 0, };
	memcpy(buff, rBufferPoint, recvBuffSize);

	LOG(L_DEBUG_, "Recv Complete", "buff size", (int)recvBuffSize, "data", buff);

	_recvBuff->remove(recvBuffSize);
}

}


