// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: cmd_acc2d.proto

#ifndef PROTOBUF_cmd_5facc2d_2eproto__INCLUDED
#define PROTOBUF_cmd_5facc2d_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
#include "ignition/msgs/header.pb.h"
// @@protoc_insertion_point(includes)

namespace ignition {
namespace msgs {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_cmd_5facc2d_2eproto();
void protobuf_AssignDesc_cmd_5facc2d_2eproto();
void protobuf_ShutdownFile_cmd_5facc2d_2eproto();

class CmdAcc2D;

// ===================================================================

class CmdAcc2D : public ::google::protobuf::Message {
 public:
  CmdAcc2D();
  virtual ~CmdAcc2D();

  CmdAcc2D(const CmdAcc2D& from);

  inline CmdAcc2D& operator=(const CmdAcc2D& from) {
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
  static const CmdAcc2D& default_instance();

  void Swap(CmdAcc2D* other);

  // implements Message ----------------------------------------------

  CmdAcc2D* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const CmdAcc2D& from);
  void MergeFrom(const CmdAcc2D& from);
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

  // optional .ignition.msgs.Header header = 1;
  inline bool has_header() const;
  inline void clear_header();
  static const int kHeaderFieldNumber = 1;
  inline const ::ignition::msgs::Header& header() const;
  inline ::ignition::msgs::Header* mutable_header();
  inline ::ignition::msgs::Header* release_header();
  inline void set_allocated_header(::ignition::msgs::Header* header);

  // optional double acceleration = 2 [default = 0];
  inline bool has_acceleration() const;
  inline void clear_acceleration();
  static const int kAccelerationFieldNumber = 2;
  inline double acceleration() const;
  inline void set_acceleration(double value);

  // optional double theta = 3 [default = 0];
  inline bool has_theta() const;
  inline void clear_theta();
  static const int kThetaFieldNumber = 3;
  inline double theta() const;
  inline void set_theta(double value);

  // @@protoc_insertion_point(class_scope:ignition.msgs.CmdAcc2D)
 private:
  inline void set_has_header();
  inline void clear_has_header();
  inline void set_has_acceleration();
  inline void clear_has_acceleration();
  inline void set_has_theta();
  inline void clear_has_theta();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::ignition::msgs::Header* header_;
  double acceleration_;
  double theta_;
  friend void  protobuf_AddDesc_cmd_5facc2d_2eproto();
  friend void protobuf_AssignDesc_cmd_5facc2d_2eproto();
  friend void protobuf_ShutdownFile_cmd_5facc2d_2eproto();

  void InitAsDefaultInstance();
  static CmdAcc2D* default_instance_;
};
// ===================================================================


// ===================================================================

// CmdAcc2D

// optional .ignition.msgs.Header header = 1;
inline bool CmdAcc2D::has_header() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void CmdAcc2D::set_has_header() {
  _has_bits_[0] |= 0x00000001u;
}
inline void CmdAcc2D::clear_has_header() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void CmdAcc2D::clear_header() {
  if (header_ != NULL) header_->::ignition::msgs::Header::Clear();
  clear_has_header();
}
inline const ::ignition::msgs::Header& CmdAcc2D::header() const {
  // @@protoc_insertion_point(field_get:ignition.msgs.CmdAcc2D.header)
  return header_ != NULL ? *header_ : *default_instance_->header_;
}
inline ::ignition::msgs::Header* CmdAcc2D::mutable_header() {
  set_has_header();
  if (header_ == NULL) header_ = new ::ignition::msgs::Header;
  // @@protoc_insertion_point(field_mutable:ignition.msgs.CmdAcc2D.header)
  return header_;
}
inline ::ignition::msgs::Header* CmdAcc2D::release_header() {
  clear_has_header();
  ::ignition::msgs::Header* temp = header_;
  header_ = NULL;
  return temp;
}
inline void CmdAcc2D::set_allocated_header(::ignition::msgs::Header* header) {
  delete header_;
  header_ = header;
  if (header) {
    set_has_header();
  } else {
    clear_has_header();
  }
  // @@protoc_insertion_point(field_set_allocated:ignition.msgs.CmdAcc2D.header)
}

// optional double acceleration = 2 [default = 0];
inline bool CmdAcc2D::has_acceleration() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void CmdAcc2D::set_has_acceleration() {
  _has_bits_[0] |= 0x00000002u;
}
inline void CmdAcc2D::clear_has_acceleration() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void CmdAcc2D::clear_acceleration() {
  acceleration_ = 0;
  clear_has_acceleration();
}
inline double CmdAcc2D::acceleration() const {
  // @@protoc_insertion_point(field_get:ignition.msgs.CmdAcc2D.acceleration)
  return acceleration_;
}
inline void CmdAcc2D::set_acceleration(double value) {
  set_has_acceleration();
  acceleration_ = value;
  // @@protoc_insertion_point(field_set:ignition.msgs.CmdAcc2D.acceleration)
}

// optional double theta = 3 [default = 0];
inline bool CmdAcc2D::has_theta() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void CmdAcc2D::set_has_theta() {
  _has_bits_[0] |= 0x00000004u;
}
inline void CmdAcc2D::clear_has_theta() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void CmdAcc2D::clear_theta() {
  theta_ = 0;
  clear_has_theta();
}
inline double CmdAcc2D::theta() const {
  // @@protoc_insertion_point(field_get:ignition.msgs.CmdAcc2D.theta)
  return theta_;
}
inline void CmdAcc2D::set_theta(double value) {
  set_has_theta();
  theta_ = value;
  // @@protoc_insertion_point(field_set:ignition.msgs.CmdAcc2D.theta)
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace msgs
}  // namespace ignition

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_cmd_5facc2d_2eproto__INCLUDED
