// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: test.proto

#ifndef PROTOBUF_test_2eproto__INCLUDED
#define PROTOBUF_test_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3000000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3000000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace BLUE_BERRY {
namespace Test {

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_test_2eproto();
void protobuf_AssignDesc_test_2eproto();
void protobuf_ShutdownFile_test_2eproto();

class LoginAns;
class LoginReq;
class TestMessage;

enum TestMessage_SmallType {
  TestMessage_SmallType_NORMAL = 0,
  TestMessage_SmallType_HARD = 1,
  TestMessage_SmallType_HELL = 2
};
bool TestMessage_SmallType_IsValid(int value);
const TestMessage_SmallType TestMessage_SmallType_SmallType_MIN = TestMessage_SmallType_NORMAL;
const TestMessage_SmallType TestMessage_SmallType_SmallType_MAX = TestMessage_SmallType_HELL;
const int TestMessage_SmallType_SmallType_ARRAYSIZE = TestMessage_SmallType_SmallType_MAX + 1;

const ::google::protobuf::EnumDescriptor* TestMessage_SmallType_descriptor();
inline const ::std::string& TestMessage_SmallType_Name(TestMessage_SmallType value) {
  return ::google::protobuf::internal::NameOfEnum(
    TestMessage_SmallType_descriptor(), value);
}
inline bool TestMessage_SmallType_Parse(
    const ::std::string& name, TestMessage_SmallType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<TestMessage_SmallType>(
    TestMessage_SmallType_descriptor(), name, value);
}
enum TestType {
  TYPE_1 = 0,
  TYPE_2 = 1,
  TYPE_3 = 2
};
bool TestType_IsValid(int value);
const TestType TestType_MIN = TYPE_1;
const TestType TestType_MAX = TYPE_3;
const int TestType_ARRAYSIZE = TestType_MAX + 1;

const ::google::protobuf::EnumDescriptor* TestType_descriptor();
inline const ::std::string& TestType_Name(TestType value) {
  return ::google::protobuf::internal::NameOfEnum(
    TestType_descriptor(), value);
}
inline bool TestType_Parse(
    const ::std::string& name, TestType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<TestType>(
    TestType_descriptor(), name, value);
}
enum ErrorCode {
  ERR_SUCCESS = 0,
  ERR_LOGIN_FAIL = 1,
  ERR_ARGUMENT_FAIL = 2
};
bool ErrorCode_IsValid(int value);
const ErrorCode ErrorCode_MIN = ERR_SUCCESS;
const ErrorCode ErrorCode_MAX = ERR_ARGUMENT_FAIL;
const int ErrorCode_ARRAYSIZE = ErrorCode_MAX + 1;

const ::google::protobuf::EnumDescriptor* ErrorCode_descriptor();
inline const ::std::string& ErrorCode_Name(ErrorCode value) {
  return ::google::protobuf::internal::NameOfEnum(
    ErrorCode_descriptor(), value);
}
inline bool ErrorCode_Parse(
    const ::std::string& name, ErrorCode* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ErrorCode>(
    ErrorCode_descriptor(), name, value);
}
// ===================================================================

class TestMessage : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:BLUE_BERRY.Test.TestMessage) */ {
 public:
  TestMessage();
  virtual ~TestMessage();

  TestMessage(const TestMessage& from);

  inline TestMessage& operator=(const TestMessage& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const TestMessage& default_instance();

  void Swap(TestMessage* other);

  // implements Message ----------------------------------------------

  inline TestMessage* New() const { return New(NULL); }

  TestMessage* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const TestMessage& from);
  void MergeFrom(const TestMessage& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(TestMessage* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  typedef TestMessage_SmallType SmallType;
  static const SmallType NORMAL =
    TestMessage_SmallType_NORMAL;
  static const SmallType HARD =
    TestMessage_SmallType_HARD;
  static const SmallType HELL =
    TestMessage_SmallType_HELL;
  static inline bool SmallType_IsValid(int value) {
    return TestMessage_SmallType_IsValid(value);
  }
  static const SmallType SmallType_MIN =
    TestMessage_SmallType_SmallType_MIN;
  static const SmallType SmallType_MAX =
    TestMessage_SmallType_SmallType_MAX;
  static const int SmallType_ARRAYSIZE =
    TestMessage_SmallType_SmallType_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  SmallType_descriptor() {
    return TestMessage_SmallType_descriptor();
  }
  static inline const ::std::string& SmallType_Name(SmallType value) {
    return TestMessage_SmallType_Name(value);
  }
  static inline bool SmallType_Parse(const ::std::string& name,
      SmallType* value) {
    return TestMessage_SmallType_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  // required string testString = 1;
  bool has_teststring() const;
  void clear_teststring();
  static const int kTestStringFieldNumber = 1;
  const ::std::string& teststring() const;
  void set_teststring(const ::std::string& value);
  void set_teststring(const char* value);
  void set_teststring(const char* value, size_t size);
  ::std::string* mutable_teststring();
  ::std::string* release_teststring();
  void set_allocated_teststring(::std::string* teststring);

  // required uint32 testUint32 = 2;
  bool has_testuint32() const;
  void clear_testuint32();
  static const int kTestUint32FieldNumber = 2;
  ::google::protobuf::uint32 testuint32() const;
  void set_testuint32(::google::protobuf::uint32 value);

  // required .BLUE_BERRY.Test.TestMessage.SmallType testSmallType = 3;
  bool has_testsmalltype() const;
  void clear_testsmalltype();
  static const int kTestSmallTypeFieldNumber = 3;
  ::BLUE_BERRY::Test::TestMessage_SmallType testsmalltype() const;
  void set_testsmalltype(::BLUE_BERRY::Test::TestMessage_SmallType value);

  // optional .BLUE_BERRY.Test.TestType testTestType = 4 [default = TYPE_1];
  bool has_testtesttype() const;
  void clear_testtesttype();
  static const int kTestTestTypeFieldNumber = 4;
  ::BLUE_BERRY::Test::TestType testtesttype() const;
  void set_testtesttype(::BLUE_BERRY::Test::TestType value);

  // required bool testBool = 5;
  bool has_testbool() const;
  void clear_testbool();
  static const int kTestBoolFieldNumber = 5;
  bool testbool() const;
  void set_testbool(bool value);

  // required int32 testInt32 = 6;
  bool has_testint32() const;
  void clear_testint32();
  static const int kTestInt32FieldNumber = 6;
  ::google::protobuf::int32 testint32() const;
  void set_testint32(::google::protobuf::int32 value);

  // optional uint64 testUint64 = 7;
  bool has_testuint64() const;
  void clear_testuint64();
  static const int kTestUint64FieldNumber = 7;
  ::google::protobuf::uint64 testuint64() const;
  void set_testuint64(::google::protobuf::uint64 value);

  // optional float testFloat = 8;
  bool has_testfloat() const;
  void clear_testfloat();
  static const int kTestFloatFieldNumber = 8;
  float testfloat() const;
  void set_testfloat(float value);

  // @@protoc_insertion_point(class_scope:BLUE_BERRY.Test.TestMessage)
 private:
  inline void set_has_teststring();
  inline void clear_has_teststring();
  inline void set_has_testuint32();
  inline void clear_has_testuint32();
  inline void set_has_testsmalltype();
  inline void clear_has_testsmalltype();
  inline void set_has_testtesttype();
  inline void clear_has_testtesttype();
  inline void set_has_testbool();
  inline void clear_has_testbool();
  inline void set_has_testint32();
  inline void clear_has_testint32();
  inline void set_has_testuint64();
  inline void clear_has_testuint64();
  inline void set_has_testfloat();
  inline void clear_has_testfloat();

  // helper for ByteSize()
  int RequiredFieldsByteSizeFallback() const;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::internal::ArenaStringPtr teststring_;
  ::google::protobuf::uint32 testuint32_;
  int testsmalltype_;
  int testtesttype_;
  bool testbool_;
  ::google::protobuf::uint64 testuint64_;
  ::google::protobuf::int32 testint32_;
  float testfloat_;
  friend void  protobuf_AddDesc_test_2eproto();
  friend void protobuf_AssignDesc_test_2eproto();
  friend void protobuf_ShutdownFile_test_2eproto();

  void InitAsDefaultInstance();
  static TestMessage* default_instance_;
};
// -------------------------------------------------------------------

class LoginReq : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:BLUE_BERRY.Test.LoginReq) */ {
 public:
  LoginReq();
  virtual ~LoginReq();

  LoginReq(const LoginReq& from);

  inline LoginReq& operator=(const LoginReq& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const LoginReq& default_instance();

  void Swap(LoginReq* other);

  // implements Message ----------------------------------------------

  inline LoginReq* New() const { return New(NULL); }

  LoginReq* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const LoginReq& from);
  void MergeFrom(const LoginReq& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(LoginReq* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required string id = 1;
  bool has_id() const;
  void clear_id();
  static const int kIdFieldNumber = 1;
  const ::std::string& id() const;
  void set_id(const ::std::string& value);
  void set_id(const char* value);
  void set_id(const char* value, size_t size);
  ::std::string* mutable_id();
  ::std::string* release_id();
  void set_allocated_id(::std::string* id);

  // @@protoc_insertion_point(class_scope:BLUE_BERRY.Test.LoginReq)
 private:
  inline void set_has_id();
  inline void clear_has_id();

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::internal::ArenaStringPtr id_;
  friend void  protobuf_AddDesc_test_2eproto();
  friend void protobuf_AssignDesc_test_2eproto();
  friend void protobuf_ShutdownFile_test_2eproto();

  void InitAsDefaultInstance();
  static LoginReq* default_instance_;
};
// -------------------------------------------------------------------

class LoginAns : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:BLUE_BERRY.Test.LoginAns) */ {
 public:
  LoginAns();
  virtual ~LoginAns();

  LoginAns(const LoginAns& from);

  inline LoginAns& operator=(const LoginAns& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const LoginAns& default_instance();

  void Swap(LoginAns* other);

  // implements Message ----------------------------------------------

  inline LoginAns* New() const { return New(NULL); }

  LoginAns* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const LoginAns& from);
  void MergeFrom(const LoginAns& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(LoginAns* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required .BLUE_BERRY.Test.ErrorCode err = 1;
  bool has_err() const;
  void clear_err();
  static const int kErrFieldNumber = 1;
  ::BLUE_BERRY::Test::ErrorCode err() const;
  void set_err(::BLUE_BERRY::Test::ErrorCode value);

  // @@protoc_insertion_point(class_scope:BLUE_BERRY.Test.LoginAns)
 private:
  inline void set_has_err();
  inline void clear_has_err();

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  int err_;
  friend void  protobuf_AddDesc_test_2eproto();
  friend void protobuf_AssignDesc_test_2eproto();
  friend void protobuf_ShutdownFile_test_2eproto();

  void InitAsDefaultInstance();
  static LoginAns* default_instance_;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// TestMessage

// required string testString = 1;
inline bool TestMessage::has_teststring() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void TestMessage::set_has_teststring() {
  _has_bits_[0] |= 0x00000001u;
}
inline void TestMessage::clear_has_teststring() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void TestMessage::clear_teststring() {
  teststring_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_teststring();
}
inline const ::std::string& TestMessage::teststring() const {
  // @@protoc_insertion_point(field_get:BLUE_BERRY.Test.TestMessage.testString)
  return teststring_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void TestMessage::set_teststring(const ::std::string& value) {
  set_has_teststring();
  teststring_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:BLUE_BERRY.Test.TestMessage.testString)
}
inline void TestMessage::set_teststring(const char* value) {
  set_has_teststring();
  teststring_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:BLUE_BERRY.Test.TestMessage.testString)
}
inline void TestMessage::set_teststring(const char* value, size_t size) {
  set_has_teststring();
  teststring_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:BLUE_BERRY.Test.TestMessage.testString)
}
inline ::std::string* TestMessage::mutable_teststring() {
  set_has_teststring();
  // @@protoc_insertion_point(field_mutable:BLUE_BERRY.Test.TestMessage.testString)
  return teststring_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* TestMessage::release_teststring() {
  // @@protoc_insertion_point(field_release:BLUE_BERRY.Test.TestMessage.testString)
  clear_has_teststring();
  return teststring_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void TestMessage::set_allocated_teststring(::std::string* teststring) {
  if (teststring != NULL) {
    set_has_teststring();
  } else {
    clear_has_teststring();
  }
  teststring_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), teststring);
  // @@protoc_insertion_point(field_set_allocated:BLUE_BERRY.Test.TestMessage.testString)
}

// required uint32 testUint32 = 2;
inline bool TestMessage::has_testuint32() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void TestMessage::set_has_testuint32() {
  _has_bits_[0] |= 0x00000002u;
}
inline void TestMessage::clear_has_testuint32() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void TestMessage::clear_testuint32() {
  testuint32_ = 0u;
  clear_has_testuint32();
}
inline ::google::protobuf::uint32 TestMessage::testuint32() const {
  // @@protoc_insertion_point(field_get:BLUE_BERRY.Test.TestMessage.testUint32)
  return testuint32_;
}
inline void TestMessage::set_testuint32(::google::protobuf::uint32 value) {
  set_has_testuint32();
  testuint32_ = value;
  // @@protoc_insertion_point(field_set:BLUE_BERRY.Test.TestMessage.testUint32)
}

// required .BLUE_BERRY.Test.TestMessage.SmallType testSmallType = 3;
inline bool TestMessage::has_testsmalltype() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void TestMessage::set_has_testsmalltype() {
  _has_bits_[0] |= 0x00000004u;
}
inline void TestMessage::clear_has_testsmalltype() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void TestMessage::clear_testsmalltype() {
  testsmalltype_ = 0;
  clear_has_testsmalltype();
}
inline ::BLUE_BERRY::Test::TestMessage_SmallType TestMessage::testsmalltype() const {
  // @@protoc_insertion_point(field_get:BLUE_BERRY.Test.TestMessage.testSmallType)
  return static_cast< ::BLUE_BERRY::Test::TestMessage_SmallType >(testsmalltype_);
}
inline void TestMessage::set_testsmalltype(::BLUE_BERRY::Test::TestMessage_SmallType value) {
  assert(::BLUE_BERRY::Test::TestMessage_SmallType_IsValid(value));
  set_has_testsmalltype();
  testsmalltype_ = value;
  // @@protoc_insertion_point(field_set:BLUE_BERRY.Test.TestMessage.testSmallType)
}

// optional .BLUE_BERRY.Test.TestType testTestType = 4 [default = TYPE_1];
inline bool TestMessage::has_testtesttype() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void TestMessage::set_has_testtesttype() {
  _has_bits_[0] |= 0x00000008u;
}
inline void TestMessage::clear_has_testtesttype() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void TestMessage::clear_testtesttype() {
  testtesttype_ = 0;
  clear_has_testtesttype();
}
inline ::BLUE_BERRY::Test::TestType TestMessage::testtesttype() const {
  // @@protoc_insertion_point(field_get:BLUE_BERRY.Test.TestMessage.testTestType)
  return static_cast< ::BLUE_BERRY::Test::TestType >(testtesttype_);
}
inline void TestMessage::set_testtesttype(::BLUE_BERRY::Test::TestType value) {
  assert(::BLUE_BERRY::Test::TestType_IsValid(value));
  set_has_testtesttype();
  testtesttype_ = value;
  // @@protoc_insertion_point(field_set:BLUE_BERRY.Test.TestMessage.testTestType)
}

// required bool testBool = 5;
inline bool TestMessage::has_testbool() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void TestMessage::set_has_testbool() {
  _has_bits_[0] |= 0x00000010u;
}
inline void TestMessage::clear_has_testbool() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void TestMessage::clear_testbool() {
  testbool_ = false;
  clear_has_testbool();
}
inline bool TestMessage::testbool() const {
  // @@protoc_insertion_point(field_get:BLUE_BERRY.Test.TestMessage.testBool)
  return testbool_;
}
inline void TestMessage::set_testbool(bool value) {
  set_has_testbool();
  testbool_ = value;
  // @@protoc_insertion_point(field_set:BLUE_BERRY.Test.TestMessage.testBool)
}

// required int32 testInt32 = 6;
inline bool TestMessage::has_testint32() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void TestMessage::set_has_testint32() {
  _has_bits_[0] |= 0x00000020u;
}
inline void TestMessage::clear_has_testint32() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void TestMessage::clear_testint32() {
  testint32_ = 0;
  clear_has_testint32();
}
inline ::google::protobuf::int32 TestMessage::testint32() const {
  // @@protoc_insertion_point(field_get:BLUE_BERRY.Test.TestMessage.testInt32)
  return testint32_;
}
inline void TestMessage::set_testint32(::google::protobuf::int32 value) {
  set_has_testint32();
  testint32_ = value;
  // @@protoc_insertion_point(field_set:BLUE_BERRY.Test.TestMessage.testInt32)
}

// optional uint64 testUint64 = 7;
inline bool TestMessage::has_testuint64() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void TestMessage::set_has_testuint64() {
  _has_bits_[0] |= 0x00000040u;
}
inline void TestMessage::clear_has_testuint64() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void TestMessage::clear_testuint64() {
  testuint64_ = GOOGLE_ULONGLONG(0);
  clear_has_testuint64();
}
inline ::google::protobuf::uint64 TestMessage::testuint64() const {
  // @@protoc_insertion_point(field_get:BLUE_BERRY.Test.TestMessage.testUint64)
  return testuint64_;
}
inline void TestMessage::set_testuint64(::google::protobuf::uint64 value) {
  set_has_testuint64();
  testuint64_ = value;
  // @@protoc_insertion_point(field_set:BLUE_BERRY.Test.TestMessage.testUint64)
}

// optional float testFloat = 8;
inline bool TestMessage::has_testfloat() const {
  return (_has_bits_[0] & 0x00000080u) != 0;
}
inline void TestMessage::set_has_testfloat() {
  _has_bits_[0] |= 0x00000080u;
}
inline void TestMessage::clear_has_testfloat() {
  _has_bits_[0] &= ~0x00000080u;
}
inline void TestMessage::clear_testfloat() {
  testfloat_ = 0;
  clear_has_testfloat();
}
inline float TestMessage::testfloat() const {
  // @@protoc_insertion_point(field_get:BLUE_BERRY.Test.TestMessage.testFloat)
  return testfloat_;
}
inline void TestMessage::set_testfloat(float value) {
  set_has_testfloat();
  testfloat_ = value;
  // @@protoc_insertion_point(field_set:BLUE_BERRY.Test.TestMessage.testFloat)
}

// -------------------------------------------------------------------

// LoginReq

// required string id = 1;
inline bool LoginReq::has_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void LoginReq::set_has_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void LoginReq::clear_has_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void LoginReq::clear_id() {
  id_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_id();
}
inline const ::std::string& LoginReq::id() const {
  // @@protoc_insertion_point(field_get:BLUE_BERRY.Test.LoginReq.id)
  return id_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void LoginReq::set_id(const ::std::string& value) {
  set_has_id();
  id_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:BLUE_BERRY.Test.LoginReq.id)
}
inline void LoginReq::set_id(const char* value) {
  set_has_id();
  id_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:BLUE_BERRY.Test.LoginReq.id)
}
inline void LoginReq::set_id(const char* value, size_t size) {
  set_has_id();
  id_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:BLUE_BERRY.Test.LoginReq.id)
}
inline ::std::string* LoginReq::mutable_id() {
  set_has_id();
  // @@protoc_insertion_point(field_mutable:BLUE_BERRY.Test.LoginReq.id)
  return id_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* LoginReq::release_id() {
  // @@protoc_insertion_point(field_release:BLUE_BERRY.Test.LoginReq.id)
  clear_has_id();
  return id_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void LoginReq::set_allocated_id(::std::string* id) {
  if (id != NULL) {
    set_has_id();
  } else {
    clear_has_id();
  }
  id_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), id);
  // @@protoc_insertion_point(field_set_allocated:BLUE_BERRY.Test.LoginReq.id)
}

// -------------------------------------------------------------------

// LoginAns

// required .BLUE_BERRY.Test.ErrorCode err = 1;
inline bool LoginAns::has_err() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void LoginAns::set_has_err() {
  _has_bits_[0] |= 0x00000001u;
}
inline void LoginAns::clear_has_err() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void LoginAns::clear_err() {
  err_ = 0;
  clear_has_err();
}
inline ::BLUE_BERRY::Test::ErrorCode LoginAns::err() const {
  // @@protoc_insertion_point(field_get:BLUE_BERRY.Test.LoginAns.err)
  return static_cast< ::BLUE_BERRY::Test::ErrorCode >(err_);
}
inline void LoginAns::set_err(::BLUE_BERRY::Test::ErrorCode value) {
  assert(::BLUE_BERRY::Test::ErrorCode_IsValid(value));
  set_has_err();
  err_ = value;
  // @@protoc_insertion_point(field_set:BLUE_BERRY.Test.LoginAns.err)
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace Test
}  // namespace BLUE_BERRY

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::BLUE_BERRY::Test::TestMessage_SmallType> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::BLUE_BERRY::Test::TestMessage_SmallType>() {
  return ::BLUE_BERRY::Test::TestMessage_SmallType_descriptor();
}
template <> struct is_proto_enum< ::BLUE_BERRY::Test::TestType> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::BLUE_BERRY::Test::TestType>() {
  return ::BLUE_BERRY::Test::TestType_descriptor();
}
template <> struct is_proto_enum< ::BLUE_BERRY::Test::ErrorCode> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::BLUE_BERRY::Test::ErrorCode>() {
  return ::BLUE_BERRY::Test::ErrorCode_descriptor();
}

}  // namespace protobuf
}  // namespace google
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_test_2eproto__INCLUDED