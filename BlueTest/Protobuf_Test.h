#pragma once
#include <gtest/gtest.h>
#include <iostream>

#include "../cpp/test.pb.h"

using namespace BLUE_BERRY::Test;

TEST(ProtocolBuff, Function)
{
	TestMessage msg;
	msg.set_teststring("TestString");
	msg.set_testuint32(32);
	msg.set_testsmalltype(TestMessage::NORMAL);
	msg.set_testbool(true);
	msg.set_testint32(-32);
	msg.set_testuint64(64);
	msg.set_testfloat(12.345f);


	ASSERT_EQ(msg.testint32(), -32);
	ASSERT_EQ(msg.testuint64(), 64);
	ASSERT_EQ(msg.testfloat(), 12.345f);
	ASSERT_STRCASEEQ(msg.teststring().c_str(), "TestString");

	char buffer[1024] = { 0, };
	msg.SerializeToArray(buffer, sizeof(buffer));


	TestMessage msg2;
	msg2.ParseFromArray(buffer, msg.ByteSize());

	ASSERT_EQ(msg.testint32(), msg2.testint32());
	ASSERT_EQ(msg.testuint64(), msg2.testuint64());
	ASSERT_EQ(msg.testfloat(), msg2.testfloat());
	ASSERT_STRCASEEQ(msg.teststring().c_str(), msg2.teststring().c_str());

	std::cout << msg2.DebugString() << std::endl;

	LoginReq* req = new LoginReq();
	req->set_id("test_id");
	ASSERT_STRCASEEQ(req->id().c_str(), "test_id");

	delete req;
}