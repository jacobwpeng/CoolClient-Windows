// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: netpack_header.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "netpack_header.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace {

const ::google::protobuf::Descriptor* NetPackHeader_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  NetPackHeader_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_netpack_5fheader_2eproto() {
  protobuf_AddDesc_netpack_5fheader_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "netpack_header.proto");
  GOOGLE_CHECK(file != NULL);
  NetPackHeader_descriptor_ = file->message_type(0);
  static const int NetPackHeader_offsets_[3] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(NetPackHeader, payloadtype_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(NetPackHeader, payloadlength_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(NetPackHeader, messagename_),
  };
  NetPackHeader_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      NetPackHeader_descriptor_,
      NetPackHeader::default_instance_,
      NetPackHeader_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(NetPackHeader, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(NetPackHeader, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(NetPackHeader));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_netpack_5fheader_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    NetPackHeader_descriptor_, &NetPackHeader::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_netpack_5fheader_2eproto() {
  delete NetPackHeader::default_instance_;
  delete NetPackHeader_reflection_;
}

void protobuf_AddDesc_netpack_5fheader_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\024netpack_header.proto\"P\n\rNetPackHeader\022"
    "\023\n\013payloadType\030\001 \001(\005\022\025\n\rpayloadLength\030\002 "
    "\001(\005\022\023\n\013messageName\030\003 \001(\t", 104);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "netpack_header.proto", &protobuf_RegisterTypes);
  NetPackHeader::default_instance_ = new NetPackHeader();
  NetPackHeader::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_netpack_5fheader_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_netpack_5fheader_2eproto {
  StaticDescriptorInitializer_netpack_5fheader_2eproto() {
    protobuf_AddDesc_netpack_5fheader_2eproto();
  }
} static_descriptor_initializer_netpack_5fheader_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int NetPackHeader::kPayloadTypeFieldNumber;
const int NetPackHeader::kPayloadLengthFieldNumber;
const int NetPackHeader::kMessageNameFieldNumber;
#endif  // !_MSC_VER

NetPackHeader::NetPackHeader()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void NetPackHeader::InitAsDefaultInstance() {
}

NetPackHeader::NetPackHeader(const NetPackHeader& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void NetPackHeader::SharedCtor() {
  _cached_size_ = 0;
  payloadtype_ = 0;
  payloadlength_ = 0;
  messagename_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

NetPackHeader::~NetPackHeader() {
  SharedDtor();
}

void NetPackHeader::SharedDtor() {
  if (messagename_ != &::google::protobuf::internal::kEmptyString) {
    delete messagename_;
  }
  if (this != default_instance_) {
  }
}

void NetPackHeader::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* NetPackHeader::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return NetPackHeader_descriptor_;
}

const NetPackHeader& NetPackHeader::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_netpack_5fheader_2eproto();
  return *default_instance_;
}

NetPackHeader* NetPackHeader::default_instance_ = NULL;

NetPackHeader* NetPackHeader::New() const {
  return new NetPackHeader;
}

void NetPackHeader::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    payloadtype_ = 0;
    payloadlength_ = 0;
    if (has_messagename()) {
      if (messagename_ != &::google::protobuf::internal::kEmptyString) {
        messagename_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool NetPackHeader::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional int32 payloadType = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &payloadtype_)));
          set_has_payloadtype();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_payloadLength;
        break;
      }

      // optional int32 payloadLength = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_payloadLength:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &payloadlength_)));
          set_has_payloadlength();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(26)) goto parse_messageName;
        break;
      }

      // optional string messageName = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_messageName:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_messagename()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->messagename().data(), this->messagename().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void NetPackHeader::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional int32 payloadType = 1;
  if (has_payloadtype()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->payloadtype(), output);
  }

  // optional int32 payloadLength = 2;
  if (has_payloadlength()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->payloadlength(), output);
  }

  // optional string messageName = 3;
  if (has_messagename()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->messagename().data(), this->messagename().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      3, this->messagename(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* NetPackHeader::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional int32 payloadType = 1;
  if (has_payloadtype()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->payloadtype(), target);
  }

  // optional int32 payloadLength = 2;
  if (has_payloadlength()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->payloadlength(), target);
  }

  // optional string messageName = 3;
  if (has_messagename()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->messagename().data(), this->messagename().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        3, this->messagename(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int NetPackHeader::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional int32 payloadType = 1;
    if (has_payloadtype()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->payloadtype());
    }

    // optional int32 payloadLength = 2;
    if (has_payloadlength()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->payloadlength());
    }

    // optional string messageName = 3;
    if (has_messagename()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->messagename());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void NetPackHeader::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const NetPackHeader* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const NetPackHeader*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void NetPackHeader::MergeFrom(const NetPackHeader& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_payloadtype()) {
      set_payloadtype(from.payloadtype());
    }
    if (from.has_payloadlength()) {
      set_payloadlength(from.payloadlength());
    }
    if (from.has_messagename()) {
      set_messagename(from.messagename());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void NetPackHeader::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void NetPackHeader::CopyFrom(const NetPackHeader& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool NetPackHeader::IsInitialized() const {

  return true;
}

void NetPackHeader::Swap(NetPackHeader* other) {
  if (other != this) {
    std::swap(payloadtype_, other->payloadtype_);
    std::swap(payloadlength_, other->payloadlength_);
    std::swap(messagename_, other->messagename_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata NetPackHeader::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = NetPackHeader_descriptor_;
  metadata.reflection = NetPackHeader_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)