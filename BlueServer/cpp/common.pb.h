// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: common.proto

#ifndef PROTOBUF_common_2eproto__INCLUDED
#define PROTOBUF_common_2eproto__INCLUDED

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

namespace MSG {

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_common_2eproto();
void protobuf_AssignDesc_common_2eproto();
void protobuf_ShutdownFile_common_2eproto();

class UserData_;

enum MsgId {
  CLOSED = 10000,
  LOGIN_REQ = 10101,
  LOGIN_ANS = 10102,
  PING_REQ = 10103,
  PONG_ANS = 10104,
  REGIST_REQ = 10105,
  REGIST_ANS = 10106
};
bool MsgId_IsValid(int value);
const MsgId MsgId_MIN = CLOSED;
const MsgId MsgId_MAX = REGIST_ANS;
const int MsgId_ARRAYSIZE = MsgId_MAX + 1;

const ::google::protobuf::EnumDescriptor* MsgId_descriptor();
inline const ::std::string& MsgId_Name(MsgId value) {
  return ::google::protobuf::internal::NameOfEnum(
    MsgId_descriptor(), value);
}
inline bool MsgId_Parse(
    const ::std::string& name, MsgId* value) {
  return ::google::protobuf::internal::ParseNamedEnum<MsgId>(
    MsgId_descriptor(), name, value);
}
enum ErrorCode {
  ERR_SUCCESS = 0,
  ERR_LOGIN_FAIL = 1,
  ERR_ARGUMENT_FAIL = 2,
  ERR_AUTHORITY_FAIL = 3,
  ERR_SESSIONKEY_FAIL = 4
};
bool ErrorCode_IsValid(int value);
const ErrorCode ErrorCode_MIN = ERR_SUCCESS;
const ErrorCode ErrorCode_MAX = ERR_SESSIONKEY_FAIL;
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
enum PlatForm {
  IOS = 0,
  ANDROID = 1
};
bool PlatForm_IsValid(int value);
const PlatForm PlatForm_MIN = IOS;
const PlatForm PlatForm_MAX = ANDROID;
const int PlatForm_ARRAYSIZE = PlatForm_MAX + 1;

const ::google::protobuf::EnumDescriptor* PlatForm_descriptor();
inline const ::std::string& PlatForm_Name(PlatForm value) {
  return ::google::protobuf::internal::NameOfEnum(
    PlatForm_descriptor(), value);
}
inline bool PlatForm_Parse(
    const ::std::string& name, PlatForm* value) {
  return ::google::protobuf::internal::ParseNamedEnum<PlatForm>(
    PlatForm_descriptor(), name, value);
}
// ===================================================================

class UserData_ : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:MSG.UserData_) */ {
 public:
  UserData_();
  virtual ~UserData_();

  UserData_(const UserData_& from);

  inline UserData_& operator=(const UserData_& from) {
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
  static const UserData_& default_instance();

  void Swap(UserData_* other);

  // implements Message ----------------------------------------------

  inline UserData_* New() const { return New(NULL); }

  UserData_* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const UserData_& from);
  void MergeFrom(const UserData_& from);
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
  void InternalSwap(UserData_* other);
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

  // required uint64 uid = 1;
  bool has_uid() const;
  void clear_uid();
  static const int kUidFieldNumber = 1;
  ::google::protobuf::uint64 uid() const;
  void set_uid(::google::protobuf::uint64 value);

  // required string name = 2;
  bool has_name() const;
  void clear_name();
  static const int kNameFieldNumber = 2;
  const ::std::string& name() const;
  void set_name(const ::std::string& value);
  void set_name(const char* value);
  void set_name(const char* value, size_t size);
  ::std::string* mutable_name();
  ::std::string* release_name();
  void set_allocated_name(::std::string* name);

  // optional string did = 3;
  bool has_did() const;
  void clear_did();
  static const int kDidFieldNumber = 3;
  const ::std::string& did() const;
  void set_did(const ::std::string& value);
  void set_did(const char* value);
  void set_did(const char* value, size_t size);
  ::std::string* mutable_did();
  ::std::string* release_did();
  void set_allocated_did(::std::string* did);

  // optional .MSG.PlatForm platform = 4;
  bool has_platform() const;
  void clear_platform();
  static const int kPlatformFieldNumber = 4;
  ::MSG::PlatForm platform() const;
  void set_platform(::MSG::PlatForm value);

  // optional string login_date = 5;
  bool has_login_date() const;
  void clear_login_date();
  static const int kLoginDateFieldNumber = 5;
  const ::std::string& login_date() const;
  void set_login_date(const ::std::string& value);
  void set_login_date(const char* value);
  void set_login_date(const char* value, size_t size);
  ::std::string* mutable_login_date();
  ::std::string* release_login_date();
  void set_allocated_login_date(::std::string* login_date);

  // optional string logout_date = 6;
  bool has_logout_date() const;
  void clear_logout_date();
  static const int kLogoutDateFieldNumber = 6;
  const ::std::string& logout_date() const;
  void set_logout_date(const ::std::string& value);
  void set_logout_date(const char* value);
  void set_logout_date(const char* value, size_t size);
  ::std::string* mutable_logout_date();
  ::std::string* release_logout_date();
  void set_allocated_logout_date(::std::string* logout_date);

  // optional string reg_date = 7;
  bool has_reg_date() const;
  void clear_reg_date();
  static const int kRegDateFieldNumber = 7;
  const ::std::string& reg_date() const;
  void set_reg_date(const ::std::string& value);
  void set_reg_date(const char* value);
  void set_reg_date(const char* value, size_t size);
  ::std::string* mutable_reg_date();
  ::std::string* release_reg_date();
  void set_allocated_reg_date(::std::string* reg_date);

  // optional uint32 vc1 = 8;
  bool has_vc1() const;
  void clear_vc1();
  static const int kVc1FieldNumber = 8;
  ::google::protobuf::uint32 vc1() const;
  void set_vc1(::google::protobuf::uint32 value);

  // optional uint32 vc2 = 9;
  bool has_vc2() const;
  void clear_vc2();
  static const int kVc2FieldNumber = 9;
  ::google::protobuf::uint32 vc2() const;
  void set_vc2(::google::protobuf::uint32 value);

  // optional uint32 vc3 = 10;
  bool has_vc3() const;
  void clear_vc3();
  static const int kVc3FieldNumber = 10;
  ::google::protobuf::uint32 vc3() const;
  void set_vc3(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:MSG.UserData_)
 private:
  inline void set_has_uid();
  inline void clear_has_uid();
  inline void set_has_name();
  inline void clear_has_name();
  inline void set_has_did();
  inline void clear_has_did();
  inline void set_has_platform();
  inline void clear_has_platform();
  inline void set_has_login_date();
  inline void clear_has_login_date();
  inline void set_has_logout_date();
  inline void clear_has_logout_date();
  inline void set_has_reg_date();
  inline void clear_has_reg_date();
  inline void set_has_vc1();
  inline void clear_has_vc1();
  inline void set_has_vc2();
  inline void clear_has_vc2();
  inline void set_has_vc3();
  inline void clear_has_vc3();

  // helper for ByteSize()
  int RequiredFieldsByteSizeFallback() const;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::uint64 uid_;
  ::google::protobuf::internal::ArenaStringPtr name_;
  ::google::protobuf::internal::ArenaStringPtr did_;
  ::google::protobuf::internal::ArenaStringPtr login_date_;
  ::google::protobuf::internal::ArenaStringPtr logout_date_;
  int platform_;
  ::google::protobuf::uint32 vc1_;
  ::google::protobuf::internal::ArenaStringPtr reg_date_;
  ::google::protobuf::uint32 vc2_;
  ::google::protobuf::uint32 vc3_;
  friend void  protobuf_AddDesc_common_2eproto();
  friend void protobuf_AssignDesc_common_2eproto();
  friend void protobuf_ShutdownFile_common_2eproto();

  void InitAsDefaultInstance();
  static UserData_* default_instance_;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// UserData_

// required uint64 uid = 1;
inline bool UserData_::has_uid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void UserData_::set_has_uid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void UserData_::clear_has_uid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void UserData_::clear_uid() {
  uid_ = GOOGLE_ULONGLONG(0);
  clear_has_uid();
}
inline ::google::protobuf::uint64 UserData_::uid() const {
  // @@protoc_insertion_point(field_get:MSG.UserData_.uid)
  return uid_;
}
inline void UserData_::set_uid(::google::protobuf::uint64 value) {
  set_has_uid();
  uid_ = value;
  // @@protoc_insertion_point(field_set:MSG.UserData_.uid)
}

// required string name = 2;
inline bool UserData_::has_name() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void UserData_::set_has_name() {
  _has_bits_[0] |= 0x00000002u;
}
inline void UserData_::clear_has_name() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void UserData_::clear_name() {
  name_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_name();
}
inline const ::std::string& UserData_::name() const {
  // @@protoc_insertion_point(field_get:MSG.UserData_.name)
  return name_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void UserData_::set_name(const ::std::string& value) {
  set_has_name();
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:MSG.UserData_.name)
}
inline void UserData_::set_name(const char* value) {
  set_has_name();
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:MSG.UserData_.name)
}
inline void UserData_::set_name(const char* value, size_t size) {
  set_has_name();
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:MSG.UserData_.name)
}
inline ::std::string* UserData_::mutable_name() {
  set_has_name();
  // @@protoc_insertion_point(field_mutable:MSG.UserData_.name)
  return name_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* UserData_::release_name() {
  // @@protoc_insertion_point(field_release:MSG.UserData_.name)
  clear_has_name();
  return name_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void UserData_::set_allocated_name(::std::string* name) {
  if (name != NULL) {
    set_has_name();
  } else {
    clear_has_name();
  }
  name_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), name);
  // @@protoc_insertion_point(field_set_allocated:MSG.UserData_.name)
}

// optional string did = 3;
inline bool UserData_::has_did() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void UserData_::set_has_did() {
  _has_bits_[0] |= 0x00000004u;
}
inline void UserData_::clear_has_did() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void UserData_::clear_did() {
  did_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_did();
}
inline const ::std::string& UserData_::did() const {
  // @@protoc_insertion_point(field_get:MSG.UserData_.did)
  return did_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void UserData_::set_did(const ::std::string& value) {
  set_has_did();
  did_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:MSG.UserData_.did)
}
inline void UserData_::set_did(const char* value) {
  set_has_did();
  did_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:MSG.UserData_.did)
}
inline void UserData_::set_did(const char* value, size_t size) {
  set_has_did();
  did_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:MSG.UserData_.did)
}
inline ::std::string* UserData_::mutable_did() {
  set_has_did();
  // @@protoc_insertion_point(field_mutable:MSG.UserData_.did)
  return did_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* UserData_::release_did() {
  // @@protoc_insertion_point(field_release:MSG.UserData_.did)
  clear_has_did();
  return did_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void UserData_::set_allocated_did(::std::string* did) {
  if (did != NULL) {
    set_has_did();
  } else {
    clear_has_did();
  }
  did_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), did);
  // @@protoc_insertion_point(field_set_allocated:MSG.UserData_.did)
}

// optional .MSG.PlatForm platform = 4;
inline bool UserData_::has_platform() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void UserData_::set_has_platform() {
  _has_bits_[0] |= 0x00000008u;
}
inline void UserData_::clear_has_platform() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void UserData_::clear_platform() {
  platform_ = 0;
  clear_has_platform();
}
inline ::MSG::PlatForm UserData_::platform() const {
  // @@protoc_insertion_point(field_get:MSG.UserData_.platform)
  return static_cast< ::MSG::PlatForm >(platform_);
}
inline void UserData_::set_platform(::MSG::PlatForm value) {
  assert(::MSG::PlatForm_IsValid(value));
  set_has_platform();
  platform_ = value;
  // @@protoc_insertion_point(field_set:MSG.UserData_.platform)
}

// optional string login_date = 5;
inline bool UserData_::has_login_date() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void UserData_::set_has_login_date() {
  _has_bits_[0] |= 0x00000010u;
}
inline void UserData_::clear_has_login_date() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void UserData_::clear_login_date() {
  login_date_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_login_date();
}
inline const ::std::string& UserData_::login_date() const {
  // @@protoc_insertion_point(field_get:MSG.UserData_.login_date)
  return login_date_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void UserData_::set_login_date(const ::std::string& value) {
  set_has_login_date();
  login_date_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:MSG.UserData_.login_date)
}
inline void UserData_::set_login_date(const char* value) {
  set_has_login_date();
  login_date_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:MSG.UserData_.login_date)
}
inline void UserData_::set_login_date(const char* value, size_t size) {
  set_has_login_date();
  login_date_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:MSG.UserData_.login_date)
}
inline ::std::string* UserData_::mutable_login_date() {
  set_has_login_date();
  // @@protoc_insertion_point(field_mutable:MSG.UserData_.login_date)
  return login_date_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* UserData_::release_login_date() {
  // @@protoc_insertion_point(field_release:MSG.UserData_.login_date)
  clear_has_login_date();
  return login_date_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void UserData_::set_allocated_login_date(::std::string* login_date) {
  if (login_date != NULL) {
    set_has_login_date();
  } else {
    clear_has_login_date();
  }
  login_date_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), login_date);
  // @@protoc_insertion_point(field_set_allocated:MSG.UserData_.login_date)
}

// optional string logout_date = 6;
inline bool UserData_::has_logout_date() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void UserData_::set_has_logout_date() {
  _has_bits_[0] |= 0x00000020u;
}
inline void UserData_::clear_has_logout_date() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void UserData_::clear_logout_date() {
  logout_date_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_logout_date();
}
inline const ::std::string& UserData_::logout_date() const {
  // @@protoc_insertion_point(field_get:MSG.UserData_.logout_date)
  return logout_date_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void UserData_::set_logout_date(const ::std::string& value) {
  set_has_logout_date();
  logout_date_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:MSG.UserData_.logout_date)
}
inline void UserData_::set_logout_date(const char* value) {
  set_has_logout_date();
  logout_date_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:MSG.UserData_.logout_date)
}
inline void UserData_::set_logout_date(const char* value, size_t size) {
  set_has_logout_date();
  logout_date_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:MSG.UserData_.logout_date)
}
inline ::std::string* UserData_::mutable_logout_date() {
  set_has_logout_date();
  // @@protoc_insertion_point(field_mutable:MSG.UserData_.logout_date)
  return logout_date_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* UserData_::release_logout_date() {
  // @@protoc_insertion_point(field_release:MSG.UserData_.logout_date)
  clear_has_logout_date();
  return logout_date_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void UserData_::set_allocated_logout_date(::std::string* logout_date) {
  if (logout_date != NULL) {
    set_has_logout_date();
  } else {
    clear_has_logout_date();
  }
  logout_date_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), logout_date);
  // @@protoc_insertion_point(field_set_allocated:MSG.UserData_.logout_date)
}

// optional string reg_date = 7;
inline bool UserData_::has_reg_date() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void UserData_::set_has_reg_date() {
  _has_bits_[0] |= 0x00000040u;
}
inline void UserData_::clear_has_reg_date() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void UserData_::clear_reg_date() {
  reg_date_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_reg_date();
}
inline const ::std::string& UserData_::reg_date() const {
  // @@protoc_insertion_point(field_get:MSG.UserData_.reg_date)
  return reg_date_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void UserData_::set_reg_date(const ::std::string& value) {
  set_has_reg_date();
  reg_date_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:MSG.UserData_.reg_date)
}
inline void UserData_::set_reg_date(const char* value) {
  set_has_reg_date();
  reg_date_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:MSG.UserData_.reg_date)
}
inline void UserData_::set_reg_date(const char* value, size_t size) {
  set_has_reg_date();
  reg_date_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:MSG.UserData_.reg_date)
}
inline ::std::string* UserData_::mutable_reg_date() {
  set_has_reg_date();
  // @@protoc_insertion_point(field_mutable:MSG.UserData_.reg_date)
  return reg_date_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* UserData_::release_reg_date() {
  // @@protoc_insertion_point(field_release:MSG.UserData_.reg_date)
  clear_has_reg_date();
  return reg_date_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void UserData_::set_allocated_reg_date(::std::string* reg_date) {
  if (reg_date != NULL) {
    set_has_reg_date();
  } else {
    clear_has_reg_date();
  }
  reg_date_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), reg_date);
  // @@protoc_insertion_point(field_set_allocated:MSG.UserData_.reg_date)
}

// optional uint32 vc1 = 8;
inline bool UserData_::has_vc1() const {
  return (_has_bits_[0] & 0x00000080u) != 0;
}
inline void UserData_::set_has_vc1() {
  _has_bits_[0] |= 0x00000080u;
}
inline void UserData_::clear_has_vc1() {
  _has_bits_[0] &= ~0x00000080u;
}
inline void UserData_::clear_vc1() {
  vc1_ = 0u;
  clear_has_vc1();
}
inline ::google::protobuf::uint32 UserData_::vc1() const {
  // @@protoc_insertion_point(field_get:MSG.UserData_.vc1)
  return vc1_;
}
inline void UserData_::set_vc1(::google::protobuf::uint32 value) {
  set_has_vc1();
  vc1_ = value;
  // @@protoc_insertion_point(field_set:MSG.UserData_.vc1)
}

// optional uint32 vc2 = 9;
inline bool UserData_::has_vc2() const {
  return (_has_bits_[0] & 0x00000100u) != 0;
}
inline void UserData_::set_has_vc2() {
  _has_bits_[0] |= 0x00000100u;
}
inline void UserData_::clear_has_vc2() {
  _has_bits_[0] &= ~0x00000100u;
}
inline void UserData_::clear_vc2() {
  vc2_ = 0u;
  clear_has_vc2();
}
inline ::google::protobuf::uint32 UserData_::vc2() const {
  // @@protoc_insertion_point(field_get:MSG.UserData_.vc2)
  return vc2_;
}
inline void UserData_::set_vc2(::google::protobuf::uint32 value) {
  set_has_vc2();
  vc2_ = value;
  // @@protoc_insertion_point(field_set:MSG.UserData_.vc2)
}

// optional uint32 vc3 = 10;
inline bool UserData_::has_vc3() const {
  return (_has_bits_[0] & 0x00000200u) != 0;
}
inline void UserData_::set_has_vc3() {
  _has_bits_[0] |= 0x00000200u;
}
inline void UserData_::clear_has_vc3() {
  _has_bits_[0] &= ~0x00000200u;
}
inline void UserData_::clear_vc3() {
  vc3_ = 0u;
  clear_has_vc3();
}
inline ::google::protobuf::uint32 UserData_::vc3() const {
  // @@protoc_insertion_point(field_get:MSG.UserData_.vc3)
  return vc3_;
}
inline void UserData_::set_vc3(::google::protobuf::uint32 value) {
  set_has_vc3();
  vc3_ = value;
  // @@protoc_insertion_point(field_set:MSG.UserData_.vc3)
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace MSG

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::MSG::MsgId> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::MSG::MsgId>() {
  return ::MSG::MsgId_descriptor();
}
template <> struct is_proto_enum< ::MSG::ErrorCode> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::MSG::ErrorCode>() {
  return ::MSG::ErrorCode_descriptor();
}
template <> struct is_proto_enum< ::MSG::PlatForm> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::MSG::PlatForm>() {
  return ::MSG::PlatForm_descriptor();
}

}  // namespace protobuf
}  // namespace google
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_common_2eproto__INCLUDED
