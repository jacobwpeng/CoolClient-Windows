// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: client.proto

#ifndef PROTOBUF_client_2eproto__INCLUDED
#define PROTOBUF_client_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace ClientProto {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_client_2eproto();
void protobuf_AssignDesc_client_2eproto();
void protobuf_ShutdownFile_client_2eproto();

class FileInfo;
class ShakeHand;
class UploadRequest;
class UploadReply;

// ===================================================================

class FileInfo : public ::google::protobuf::Message {
 public:
  FileInfo();
  virtual ~FileInfo();

  FileInfo(const FileInfo& from);

  inline FileInfo& operator=(const FileInfo& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const FileInfo& default_instance();

  void Swap(FileInfo* other);

  // implements Message ----------------------------------------------

  FileInfo* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const FileInfo& from);
  void MergeFrom(const FileInfo& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required string FileId = 1;
  inline bool has_fileid() const;
  inline void clear_fileid();
  static const int kFileIdFieldNumber = 1;
  inline const ::std::string& fileid() const;
  inline void set_fileid(const ::std::string& value);
  inline void set_fileid(const char* value);
  inline void set_fileid(const char* value, size_t size);
  inline ::std::string* mutable_fileid();
  inline ::std::string* release_fileid();
  inline void set_allocated_fileid(::std::string* fileid);

  // required int32 HasFile = 2;
  inline bool has_hasfile() const;
  inline void clear_hasfile();
  static const int kHasFileFieldNumber = 2;
  inline ::google::protobuf::int32 hasfile() const;
  inline void set_hasfile(::google::protobuf::int32 value);

  // required int32 Percentage = 3;
  inline bool has_percentage() const;
  inline void clear_percentage();
  static const int kPercentageFieldNumber = 3;
  inline ::google::protobuf::int32 percentage() const;
  inline void set_percentage(::google::protobuf::int32 value);

  // repeated uint64 FileBit = 4;
  inline int filebit_size() const;
  inline void clear_filebit();
  static const int kFileBitFieldNumber = 4;
  inline ::google::protobuf::uint64 filebit(int index) const;
  inline void set_filebit(int index, ::google::protobuf::uint64 value);
  inline void add_filebit(::google::protobuf::uint64 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::uint64 >&
      filebit() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::uint64 >*
      mutable_filebit();

  // required int32 FileBitCount = 5;
  inline bool has_filebitcount() const;
  inline void clear_filebitcount();
  static const int kFileBitCountFieldNumber = 5;
  inline ::google::protobuf::int32 filebitcount() const;
  inline void set_filebitcount(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:ClientProto.FileInfo)
 private:
  inline void set_has_fileid();
  inline void clear_has_fileid();
  inline void set_has_hasfile();
  inline void clear_has_hasfile();
  inline void set_has_percentage();
  inline void clear_has_percentage();
  inline void set_has_filebitcount();
  inline void clear_has_filebitcount();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* fileid_;
  ::google::protobuf::int32 hasfile_;
  ::google::protobuf::int32 percentage_;
  ::google::protobuf::RepeatedField< ::google::protobuf::uint64 > filebit_;
  ::google::protobuf::int32 filebitcount_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(5 + 31) / 32];

  friend void  protobuf_AddDesc_client_2eproto();
  friend void protobuf_AssignDesc_client_2eproto();
  friend void protobuf_ShutdownFile_client_2eproto();

  void InitAsDefaultInstance();
  static FileInfo* default_instance_;
};
// -------------------------------------------------------------------

class ShakeHand : public ::google::protobuf::Message {
 public:
  ShakeHand();
  virtual ~ShakeHand();

  ShakeHand(const ShakeHand& from);

  inline ShakeHand& operator=(const ShakeHand& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const ShakeHand& default_instance();

  void Swap(ShakeHand* other);

  // implements Message ----------------------------------------------

  ShakeHand* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ShakeHand& from);
  void MergeFrom(const ShakeHand& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required string ClientId = 1;
  inline bool has_clientid() const;
  inline void clear_clientid();
  static const int kClientIdFieldNumber = 1;
  inline const ::std::string& clientid() const;
  inline void set_clientid(const ::std::string& value);
  inline void set_clientid(const char* value);
  inline void set_clientid(const char* value, size_t size);
  inline ::std::string* mutable_clientid();
  inline ::std::string* release_clientid();
  inline void set_allocated_clientid(::std::string* clientid);

  // required .ClientProto.FileInfo Info = 2;
  inline bool has_info() const;
  inline void clear_info();
  static const int kInfoFieldNumber = 2;
  inline const ::ClientProto::FileInfo& info() const;
  inline ::ClientProto::FileInfo* mutable_info();
  inline ::ClientProto::FileInfo* release_info();
  inline void set_allocated_info(::ClientProto::FileInfo* info);

  // @@protoc_insertion_point(class_scope:ClientProto.ShakeHand)
 private:
  inline void set_has_clientid();
  inline void clear_has_clientid();
  inline void set_has_info();
  inline void clear_has_info();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* clientid_;
  ::ClientProto::FileInfo* info_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_client_2eproto();
  friend void protobuf_AssignDesc_client_2eproto();
  friend void protobuf_ShutdownFile_client_2eproto();

  void InitAsDefaultInstance();
  static ShakeHand* default_instance_;
};
// -------------------------------------------------------------------

class UploadRequest : public ::google::protobuf::Message {
 public:
  UploadRequest();
  virtual ~UploadRequest();

  UploadRequest(const UploadRequest& from);

  inline UploadRequest& operator=(const UploadRequest& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const UploadRequest& default_instance();

  void Swap(UploadRequest* other);

  // implements Message ----------------------------------------------

  UploadRequest* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const UploadRequest& from);
  void MergeFrom(const UploadRequest& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required string ClientId = 1;
  inline bool has_clientid() const;
  inline void clear_clientid();
  static const int kClientIdFieldNumber = 1;
  inline const ::std::string& clientid() const;
  inline void set_clientid(const ::std::string& value);
  inline void set_clientid(const char* value);
  inline void set_clientid(const char* value, size_t size);
  inline ::std::string* mutable_clientid();
  inline ::std::string* release_clientid();
  inline void set_allocated_clientid(::std::string* clientid);

  // required string FileId = 2;
  inline bool has_fileid() const;
  inline void clear_fileid();
  static const int kFileIdFieldNumber = 2;
  inline const ::std::string& fileid() const;
  inline void set_fileid(const ::std::string& value);
  inline void set_fileid(const char* value);
  inline void set_fileid(const char* value, size_t size);
  inline ::std::string* mutable_fileid();
  inline ::std::string* release_fileid();
  inline void set_allocated_fileid(::std::string* fileid);

  // required int64 ChunkNumber = 3;
  inline bool has_chunknumber() const;
  inline void clear_chunknumber();
  static const int kChunkNumberFieldNumber = 3;
  inline ::google::protobuf::int64 chunknumber() const;
  inline void set_chunknumber(::google::protobuf::int64 value);

  // @@protoc_insertion_point(class_scope:ClientProto.UploadRequest)
 private:
  inline void set_has_clientid();
  inline void clear_has_clientid();
  inline void set_has_fileid();
  inline void clear_has_fileid();
  inline void set_has_chunknumber();
  inline void clear_has_chunknumber();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* clientid_;
  ::std::string* fileid_;
  ::google::protobuf::int64 chunknumber_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(3 + 31) / 32];

  friend void  protobuf_AddDesc_client_2eproto();
  friend void protobuf_AssignDesc_client_2eproto();
  friend void protobuf_ShutdownFile_client_2eproto();

  void InitAsDefaultInstance();
  static UploadRequest* default_instance_;
};
// -------------------------------------------------------------------

class UploadReply : public ::google::protobuf::Message {
 public:
  UploadReply();
  virtual ~UploadReply();

  UploadReply(const UploadReply& from);

  inline UploadReply& operator=(const UploadReply& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const UploadReply& default_instance();

  void Swap(UploadReply* other);

  // implements Message ----------------------------------------------

  UploadReply* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const UploadReply& from);
  void MergeFrom(const UploadReply& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required int32 ReturnCode = 1;
  inline bool has_returncode() const;
  inline void clear_returncode();
  static const int kReturnCodeFieldNumber = 1;
  inline ::google::protobuf::int32 returncode() const;
  inline void set_returncode(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:ClientProto.UploadReply)
 private:
  inline void set_has_returncode();
  inline void clear_has_returncode();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 returncode_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_client_2eproto();
  friend void protobuf_AssignDesc_client_2eproto();
  friend void protobuf_ShutdownFile_client_2eproto();

  void InitAsDefaultInstance();
  static UploadReply* default_instance_;
};
// ===================================================================


// ===================================================================

// FileInfo

// required string FileId = 1;
inline bool FileInfo::has_fileid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void FileInfo::set_has_fileid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void FileInfo::clear_has_fileid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void FileInfo::clear_fileid() {
  if (fileid_ != &::google::protobuf::internal::kEmptyString) {
    fileid_->clear();
  }
  clear_has_fileid();
}
inline const ::std::string& FileInfo::fileid() const {
  return *fileid_;
}
inline void FileInfo::set_fileid(const ::std::string& value) {
  set_has_fileid();
  if (fileid_ == &::google::protobuf::internal::kEmptyString) {
    fileid_ = new ::std::string;
  }
  fileid_->assign(value);
}
inline void FileInfo::set_fileid(const char* value) {
  set_has_fileid();
  if (fileid_ == &::google::protobuf::internal::kEmptyString) {
    fileid_ = new ::std::string;
  }
  fileid_->assign(value);
}
inline void FileInfo::set_fileid(const char* value, size_t size) {
  set_has_fileid();
  if (fileid_ == &::google::protobuf::internal::kEmptyString) {
    fileid_ = new ::std::string;
  }
  fileid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* FileInfo::mutable_fileid() {
  set_has_fileid();
  if (fileid_ == &::google::protobuf::internal::kEmptyString) {
    fileid_ = new ::std::string;
  }
  return fileid_;
}
inline ::std::string* FileInfo::release_fileid() {
  clear_has_fileid();
  if (fileid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = fileid_;
    fileid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void FileInfo::set_allocated_fileid(::std::string* fileid) {
  if (fileid_ != &::google::protobuf::internal::kEmptyString) {
    delete fileid_;
  }
  if (fileid) {
    set_has_fileid();
    fileid_ = fileid;
  } else {
    clear_has_fileid();
    fileid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// required int32 HasFile = 2;
inline bool FileInfo::has_hasfile() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void FileInfo::set_has_hasfile() {
  _has_bits_[0] |= 0x00000002u;
}
inline void FileInfo::clear_has_hasfile() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void FileInfo::clear_hasfile() {
  hasfile_ = 0;
  clear_has_hasfile();
}
inline ::google::protobuf::int32 FileInfo::hasfile() const {
  return hasfile_;
}
inline void FileInfo::set_hasfile(::google::protobuf::int32 value) {
  set_has_hasfile();
  hasfile_ = value;
}

// required int32 Percentage = 3;
inline bool FileInfo::has_percentage() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void FileInfo::set_has_percentage() {
  _has_bits_[0] |= 0x00000004u;
}
inline void FileInfo::clear_has_percentage() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void FileInfo::clear_percentage() {
  percentage_ = 0;
  clear_has_percentage();
}
inline ::google::protobuf::int32 FileInfo::percentage() const {
  return percentage_;
}
inline void FileInfo::set_percentage(::google::protobuf::int32 value) {
  set_has_percentage();
  percentage_ = value;
}

// repeated uint64 FileBit = 4;
inline int FileInfo::filebit_size() const {
  return filebit_.size();
}
inline void FileInfo::clear_filebit() {
  filebit_.Clear();
}
inline ::google::protobuf::uint64 FileInfo::filebit(int index) const {
  return filebit_.Get(index);
}
inline void FileInfo::set_filebit(int index, ::google::protobuf::uint64 value) {
  filebit_.Set(index, value);
}
inline void FileInfo::add_filebit(::google::protobuf::uint64 value) {
  filebit_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::uint64 >&
FileInfo::filebit() const {
  return filebit_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::uint64 >*
FileInfo::mutable_filebit() {
  return &filebit_;
}

// required int32 FileBitCount = 5;
inline bool FileInfo::has_filebitcount() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void FileInfo::set_has_filebitcount() {
  _has_bits_[0] |= 0x00000010u;
}
inline void FileInfo::clear_has_filebitcount() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void FileInfo::clear_filebitcount() {
  filebitcount_ = 0;
  clear_has_filebitcount();
}
inline ::google::protobuf::int32 FileInfo::filebitcount() const {
  return filebitcount_;
}
inline void FileInfo::set_filebitcount(::google::protobuf::int32 value) {
  set_has_filebitcount();
  filebitcount_ = value;
}

// -------------------------------------------------------------------

// ShakeHand

// required string ClientId = 1;
inline bool ShakeHand::has_clientid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ShakeHand::set_has_clientid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ShakeHand::clear_has_clientid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ShakeHand::clear_clientid() {
  if (clientid_ != &::google::protobuf::internal::kEmptyString) {
    clientid_->clear();
  }
  clear_has_clientid();
}
inline const ::std::string& ShakeHand::clientid() const {
  return *clientid_;
}
inline void ShakeHand::set_clientid(const ::std::string& value) {
  set_has_clientid();
  if (clientid_ == &::google::protobuf::internal::kEmptyString) {
    clientid_ = new ::std::string;
  }
  clientid_->assign(value);
}
inline void ShakeHand::set_clientid(const char* value) {
  set_has_clientid();
  if (clientid_ == &::google::protobuf::internal::kEmptyString) {
    clientid_ = new ::std::string;
  }
  clientid_->assign(value);
}
inline void ShakeHand::set_clientid(const char* value, size_t size) {
  set_has_clientid();
  if (clientid_ == &::google::protobuf::internal::kEmptyString) {
    clientid_ = new ::std::string;
  }
  clientid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* ShakeHand::mutable_clientid() {
  set_has_clientid();
  if (clientid_ == &::google::protobuf::internal::kEmptyString) {
    clientid_ = new ::std::string;
  }
  return clientid_;
}
inline ::std::string* ShakeHand::release_clientid() {
  clear_has_clientid();
  if (clientid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = clientid_;
    clientid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void ShakeHand::set_allocated_clientid(::std::string* clientid) {
  if (clientid_ != &::google::protobuf::internal::kEmptyString) {
    delete clientid_;
  }
  if (clientid) {
    set_has_clientid();
    clientid_ = clientid;
  } else {
    clear_has_clientid();
    clientid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// required .ClientProto.FileInfo Info = 2;
inline bool ShakeHand::has_info() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ShakeHand::set_has_info() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ShakeHand::clear_has_info() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ShakeHand::clear_info() {
  if (info_ != NULL) info_->::ClientProto::FileInfo::Clear();
  clear_has_info();
}
inline const ::ClientProto::FileInfo& ShakeHand::info() const {
  return info_ != NULL ? *info_ : *default_instance_->info_;
}
inline ::ClientProto::FileInfo* ShakeHand::mutable_info() {
  set_has_info();
  if (info_ == NULL) info_ = new ::ClientProto::FileInfo;
  return info_;
}
inline ::ClientProto::FileInfo* ShakeHand::release_info() {
  clear_has_info();
  ::ClientProto::FileInfo* temp = info_;
  info_ = NULL;
  return temp;
}
inline void ShakeHand::set_allocated_info(::ClientProto::FileInfo* info) {
  delete info_;
  info_ = info;
  if (info) {
    set_has_info();
  } else {
    clear_has_info();
  }
}

// -------------------------------------------------------------------

// UploadRequest

// required string ClientId = 1;
inline bool UploadRequest::has_clientid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void UploadRequest::set_has_clientid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void UploadRequest::clear_has_clientid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void UploadRequest::clear_clientid() {
  if (clientid_ != &::google::protobuf::internal::kEmptyString) {
    clientid_->clear();
  }
  clear_has_clientid();
}
inline const ::std::string& UploadRequest::clientid() const {
  return *clientid_;
}
inline void UploadRequest::set_clientid(const ::std::string& value) {
  set_has_clientid();
  if (clientid_ == &::google::protobuf::internal::kEmptyString) {
    clientid_ = new ::std::string;
  }
  clientid_->assign(value);
}
inline void UploadRequest::set_clientid(const char* value) {
  set_has_clientid();
  if (clientid_ == &::google::protobuf::internal::kEmptyString) {
    clientid_ = new ::std::string;
  }
  clientid_->assign(value);
}
inline void UploadRequest::set_clientid(const char* value, size_t size) {
  set_has_clientid();
  if (clientid_ == &::google::protobuf::internal::kEmptyString) {
    clientid_ = new ::std::string;
  }
  clientid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* UploadRequest::mutable_clientid() {
  set_has_clientid();
  if (clientid_ == &::google::protobuf::internal::kEmptyString) {
    clientid_ = new ::std::string;
  }
  return clientid_;
}
inline ::std::string* UploadRequest::release_clientid() {
  clear_has_clientid();
  if (clientid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = clientid_;
    clientid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void UploadRequest::set_allocated_clientid(::std::string* clientid) {
  if (clientid_ != &::google::protobuf::internal::kEmptyString) {
    delete clientid_;
  }
  if (clientid) {
    set_has_clientid();
    clientid_ = clientid;
  } else {
    clear_has_clientid();
    clientid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// required string FileId = 2;
inline bool UploadRequest::has_fileid() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void UploadRequest::set_has_fileid() {
  _has_bits_[0] |= 0x00000002u;
}
inline void UploadRequest::clear_has_fileid() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void UploadRequest::clear_fileid() {
  if (fileid_ != &::google::protobuf::internal::kEmptyString) {
    fileid_->clear();
  }
  clear_has_fileid();
}
inline const ::std::string& UploadRequest::fileid() const {
  return *fileid_;
}
inline void UploadRequest::set_fileid(const ::std::string& value) {
  set_has_fileid();
  if (fileid_ == &::google::protobuf::internal::kEmptyString) {
    fileid_ = new ::std::string;
  }
  fileid_->assign(value);
}
inline void UploadRequest::set_fileid(const char* value) {
  set_has_fileid();
  if (fileid_ == &::google::protobuf::internal::kEmptyString) {
    fileid_ = new ::std::string;
  }
  fileid_->assign(value);
}
inline void UploadRequest::set_fileid(const char* value, size_t size) {
  set_has_fileid();
  if (fileid_ == &::google::protobuf::internal::kEmptyString) {
    fileid_ = new ::std::string;
  }
  fileid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* UploadRequest::mutable_fileid() {
  set_has_fileid();
  if (fileid_ == &::google::protobuf::internal::kEmptyString) {
    fileid_ = new ::std::string;
  }
  return fileid_;
}
inline ::std::string* UploadRequest::release_fileid() {
  clear_has_fileid();
  if (fileid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = fileid_;
    fileid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void UploadRequest::set_allocated_fileid(::std::string* fileid) {
  if (fileid_ != &::google::protobuf::internal::kEmptyString) {
    delete fileid_;
  }
  if (fileid) {
    set_has_fileid();
    fileid_ = fileid;
  } else {
    clear_has_fileid();
    fileid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// required int64 ChunkNumber = 3;
inline bool UploadRequest::has_chunknumber() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void UploadRequest::set_has_chunknumber() {
  _has_bits_[0] |= 0x00000004u;
}
inline void UploadRequest::clear_has_chunknumber() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void UploadRequest::clear_chunknumber() {
  chunknumber_ = GOOGLE_LONGLONG(0);
  clear_has_chunknumber();
}
inline ::google::protobuf::int64 UploadRequest::chunknumber() const {
  return chunknumber_;
}
inline void UploadRequest::set_chunknumber(::google::protobuf::int64 value) {
  set_has_chunknumber();
  chunknumber_ = value;
}

// -------------------------------------------------------------------

// UploadReply

// required int32 ReturnCode = 1;
inline bool UploadReply::has_returncode() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void UploadReply::set_has_returncode() {
  _has_bits_[0] |= 0x00000001u;
}
inline void UploadReply::clear_has_returncode() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void UploadReply::clear_returncode() {
  returncode_ = 0;
  clear_has_returncode();
}
inline ::google::protobuf::int32 UploadReply::returncode() const {
  return returncode_;
}
inline void UploadReply::set_returncode(::google::protobuf::int32 value) {
  set_has_returncode();
  returncode_ = value;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace ClientProto

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_client_2eproto__INCLUDED