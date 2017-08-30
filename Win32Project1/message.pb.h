// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: message.proto

#ifndef PROTOBUF_message_2eproto__INCLUDED
#define PROTOBUF_message_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3003000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3003000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
class FrameMsg;
class FrameMsgDefaultTypeInternal;
extern FrameMsgDefaultTypeInternal _FrameMsg_default_instance_;
class SceneMsg;
class SceneMsgDefaultTypeInternal;
extern SceneMsgDefaultTypeInternal _SceneMsg_default_instance_;
class VObject;
class VObjectDefaultTypeInternal;
extern VObjectDefaultTypeInternal _VObject_default_instance_;

namespace protobuf_message_2eproto {
// Internal implementation detail -- do not call these.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[];
  static const ::google::protobuf::uint32 offsets[];
  static void InitDefaultsImpl();
  static void Shutdown();
};
void AddDescriptors();
void InitDefaults();
}  // namespace protobuf_message_2eproto

enum VObject_VObjectType {
  VObject_VObjectType_Unknown = 0,
  VObject_VObjectType_Finger = 1,
  VObject_VObjectType_Finger_tip = 2,
  VObject_VObjectType_Palm = 3,
  VObject_VObjectType_rFinger = 8,
  VObject_VObjectType_rFinger_tip = 9,
  VObject_VObjectType_rPalm = 10,
  VObject_VObjectType_Sphere = 4,
  VObject_VObjectType_TextureSphere = 5,
  VObject_VObjectType_Box = 6,
  VObject_VObjectType_TextureBox = 7,
  VObject_VObjectType_VObject_VObjectType_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  VObject_VObjectType_VObject_VObjectType_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
bool VObject_VObjectType_IsValid(int value);
const VObject_VObjectType VObject_VObjectType_VObjectType_MIN = VObject_VObjectType_Unknown;
const VObject_VObjectType VObject_VObjectType_VObjectType_MAX = VObject_VObjectType_rPalm;
const int VObject_VObjectType_VObjectType_ARRAYSIZE = VObject_VObjectType_VObjectType_MAX + 1;

const ::google::protobuf::EnumDescriptor* VObject_VObjectType_descriptor();
inline const ::std::string& VObject_VObjectType_Name(VObject_VObjectType value) {
  return ::google::protobuf::internal::NameOfEnum(
    VObject_VObjectType_descriptor(), value);
}
inline bool VObject_VObjectType_Parse(
    const ::std::string& name, VObject_VObjectType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<VObject_VObjectType>(
    VObject_VObjectType_descriptor(), name, value);
}
// ===================================================================

class FrameMsg : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:FrameMsg) */ {
 public:
  FrameMsg();
  virtual ~FrameMsg();

  FrameMsg(const FrameMsg& from);

  inline FrameMsg& operator=(const FrameMsg& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const FrameMsg& default_instance();

  static inline const FrameMsg* internal_default_instance() {
    return reinterpret_cast<const FrameMsg*>(
               &_FrameMsg_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    0;

  void Swap(FrameMsg* other);

  // implements Message ----------------------------------------------

  inline FrameMsg* New() const PROTOBUF_FINAL { return New(NULL); }

  FrameMsg* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const FrameMsg& from);
  void MergeFrom(const FrameMsg& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(FrameMsg* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // bytes frame = 37;
  void clear_frame();
  static const int kFrameFieldNumber = 37;
  const ::std::string& frame() const;
  void set_frame(const ::std::string& value);
  #if LANG_CXX11
  void set_frame(::std::string&& value);
  #endif
  void set_frame(const char* value);
  void set_frame(const void* value, size_t size);
  ::std::string* mutable_frame();
  ::std::string* release_frame();
  void set_allocated_frame(::std::string* frame);

  // int32 command = 1;
  void clear_command();
  static const int kCommandFieldNumber = 1;
  ::google::protobuf::int32 command() const;
  void set_command(::google::protobuf::int32 value);

  // float headPos_x = 2;
  void clear_headpos_x();
  static const int kHeadPosXFieldNumber = 2;
  float headpos_x() const;
  void set_headpos_x(float value);

  // float headPos_y = 3;
  void clear_headpos_y();
  static const int kHeadPosYFieldNumber = 3;
  float headpos_y() const;
  void set_headpos_y(float value);

  // float headPos_z = 4;
  void clear_headpos_z();
  static const int kHeadPosZFieldNumber = 4;
  float headpos_z() const;
  void set_headpos_z(float value);

  // @@protoc_insertion_point(class_scope:FrameMsg)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr frame_;
  ::google::protobuf::int32 command_;
  float headpos_x_;
  float headpos_y_;
  float headpos_z_;
  mutable int _cached_size_;
  friend struct protobuf_message_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class SceneMsg : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:SceneMsg) */ {
 public:
  SceneMsg();
  virtual ~SceneMsg();

  SceneMsg(const SceneMsg& from);

  inline SceneMsg& operator=(const SceneMsg& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const SceneMsg& default_instance();

  static inline const SceneMsg* internal_default_instance() {
    return reinterpret_cast<const SceneMsg*>(
               &_SceneMsg_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    1;

  void Swap(SceneMsg* other);

  // implements Message ----------------------------------------------

  inline SceneMsg* New() const PROTOBUF_FINAL { return New(NULL); }

  SceneMsg* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const SceneMsg& from);
  void MergeFrom(const SceneMsg& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(SceneMsg* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated .VObject scene_objects = 1;
  int scene_objects_size() const;
  void clear_scene_objects();
  static const int kSceneObjectsFieldNumber = 1;
  const ::VObject& scene_objects(int index) const;
  ::VObject* mutable_scene_objects(int index);
  ::VObject* add_scene_objects();
  ::google::protobuf::RepeatedPtrField< ::VObject >*
      mutable_scene_objects();
  const ::google::protobuf::RepeatedPtrField< ::VObject >&
      scene_objects() const;

  // int32 objects_count = 2;
  void clear_objects_count();
  static const int kObjectsCountFieldNumber = 2;
  ::google::protobuf::int32 objects_count() const;
  void set_objects_count(::google::protobuf::int32 value);

  // float headPos_x = 3;
  void clear_headpos_x();
  static const int kHeadPosXFieldNumber = 3;
  float headpos_x() const;
  void set_headpos_x(float value);

  // float headPos_y = 4;
  void clear_headpos_y();
  static const int kHeadPosYFieldNumber = 4;
  float headpos_y() const;
  void set_headpos_y(float value);

  // float headPos_z = 5;
  void clear_headpos_z();
  static const int kHeadPosZFieldNumber = 5;
  float headpos_z() const;
  void set_headpos_z(float value);

  // @@protoc_insertion_point(class_scope:SceneMsg)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::RepeatedPtrField< ::VObject > scene_objects_;
  ::google::protobuf::int32 objects_count_;
  float headpos_x_;
  float headpos_y_;
  float headpos_z_;
  mutable int _cached_size_;
  friend struct protobuf_message_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class VObject : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:VObject) */ {
 public:
  VObject();
  virtual ~VObject();

  VObject(const VObject& from);

  inline VObject& operator=(const VObject& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const VObject& default_instance();

  static inline const VObject* internal_default_instance() {
    return reinterpret_cast<const VObject*>(
               &_VObject_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    2;

  void Swap(VObject* other);

  // implements Message ----------------------------------------------

  inline VObject* New() const PROTOBUF_FINAL { return New(NULL); }

  VObject* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const VObject& from);
  void MergeFrom(const VObject& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(VObject* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  typedef VObject_VObjectType VObjectType;
  static const VObjectType Unknown =
    VObject_VObjectType_Unknown;
  static const VObjectType Finger =
    VObject_VObjectType_Finger;
  static const VObjectType Finger_tip =
    VObject_VObjectType_Finger_tip;
  static const VObjectType Palm =
    VObject_VObjectType_Palm;
  static const VObjectType rFinger =
    VObject_VObjectType_rFinger;
  static const VObjectType rFinger_tip =
    VObject_VObjectType_rFinger_tip;
  static const VObjectType rPalm =
    VObject_VObjectType_rPalm;
  static const VObjectType Sphere =
    VObject_VObjectType_Sphere;
  static const VObjectType TextureSphere =
    VObject_VObjectType_TextureSphere;
  static const VObjectType Box =
    VObject_VObjectType_Box;
  static const VObjectType TextureBox =
    VObject_VObjectType_TextureBox;
  static inline bool VObjectType_IsValid(int value) {
    return VObject_VObjectType_IsValid(value);
  }
  static const VObjectType VObjectType_MIN =
    VObject_VObjectType_VObjectType_MIN;
  static const VObjectType VObjectType_MAX =
    VObject_VObjectType_VObjectType_MAX;
  static const int VObjectType_ARRAYSIZE =
    VObject_VObjectType_VObjectType_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  VObjectType_descriptor() {
    return VObject_VObjectType_descriptor();
  }
  static inline const ::std::string& VObjectType_Name(VObjectType value) {
    return VObject_VObjectType_Name(value);
  }
  static inline bool VObjectType_Parse(const ::std::string& name,
      VObjectType* value) {
    return VObject_VObjectType_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  // .VObject.VObjectType type = 1;
  void clear_type();
  static const int kTypeFieldNumber = 1;
  ::VObject_VObjectType type() const;
  void set_type(::VObject_VObjectType value);

  // float size_x = 2;
  void clear_size_x();
  static const int kSizeXFieldNumber = 2;
  float size_x() const;
  void set_size_x(float value);

  // float size_y = 3;
  void clear_size_y();
  static const int kSizeYFieldNumber = 3;
  float size_y() const;
  void set_size_y(float value);

  // float size_z = 4;
  void clear_size_z();
  static const int kSizeZFieldNumber = 4;
  float size_z() const;
  void set_size_z(float value);

  // float p_x = 5;
  void clear_p_x();
  static const int kPXFieldNumber = 5;
  float p_x() const;
  void set_p_x(float value);

  // float p_y = 6;
  void clear_p_y();
  static const int kPYFieldNumber = 6;
  float p_y() const;
  void set_p_y(float value);

  // float p_z = 7;
  void clear_p_z();
  static const int kPZFieldNumber = 7;
  float p_z() const;
  void set_p_z(float value);

  // float q_x = 8;
  void clear_q_x();
  static const int kQXFieldNumber = 8;
  float q_x() const;
  void set_q_x(float value);

  // float q_y = 9;
  void clear_q_y();
  static const int kQYFieldNumber = 9;
  float q_y() const;
  void set_q_y(float value);

  // float q_z = 10;
  void clear_q_z();
  static const int kQZFieldNumber = 10;
  float q_z() const;
  void set_q_z(float value);

  // float q_w = 11;
  void clear_q_w();
  static const int kQWFieldNumber = 11;
  float q_w() const;
  void set_q_w(float value);

  // float color_x = 12;
  void clear_color_x();
  static const int kColorXFieldNumber = 12;
  float color_x() const;
  void set_color_x(float value);

  // float color_y = 13;
  void clear_color_y();
  static const int kColorYFieldNumber = 13;
  float color_y() const;
  void set_color_y(float value);

  // float color_z = 14;
  void clear_color_z();
  static const int kColorZFieldNumber = 14;
  float color_z() const;
  void set_color_z(float value);

  // float color_w = 15;
  void clear_color_w();
  static const int kColorWFieldNumber = 15;
  float color_w() const;
  void set_color_w(float value);

  // @@protoc_insertion_point(class_scope:VObject)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  int type_;
  float size_x_;
  float size_y_;
  float size_z_;
  float p_x_;
  float p_y_;
  float p_z_;
  float q_x_;
  float q_y_;
  float q_z_;
  float q_w_;
  float color_x_;
  float color_y_;
  float color_z_;
  float color_w_;
  mutable int _cached_size_;
  friend struct protobuf_message_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// FrameMsg

// int32 command = 1;
inline void FrameMsg::clear_command() {
  command_ = 0;
}
inline ::google::protobuf::int32 FrameMsg::command() const {
  // @@protoc_insertion_point(field_get:FrameMsg.command)
  return command_;
}
inline void FrameMsg::set_command(::google::protobuf::int32 value) {
  
  command_ = value;
  // @@protoc_insertion_point(field_set:FrameMsg.command)
}

// float headPos_x = 2;
inline void FrameMsg::clear_headpos_x() {
  headpos_x_ = 0;
}
inline float FrameMsg::headpos_x() const {
  // @@protoc_insertion_point(field_get:FrameMsg.headPos_x)
  return headpos_x_;
}
inline void FrameMsg::set_headpos_x(float value) {
  
  headpos_x_ = value;
  // @@protoc_insertion_point(field_set:FrameMsg.headPos_x)
}

// float headPos_y = 3;
inline void FrameMsg::clear_headpos_y() {
  headpos_y_ = 0;
}
inline float FrameMsg::headpos_y() const {
  // @@protoc_insertion_point(field_get:FrameMsg.headPos_y)
  return headpos_y_;
}
inline void FrameMsg::set_headpos_y(float value) {
  
  headpos_y_ = value;
  // @@protoc_insertion_point(field_set:FrameMsg.headPos_y)
}

// float headPos_z = 4;
inline void FrameMsg::clear_headpos_z() {
  headpos_z_ = 0;
}
inline float FrameMsg::headpos_z() const {
  // @@protoc_insertion_point(field_get:FrameMsg.headPos_z)
  return headpos_z_;
}
inline void FrameMsg::set_headpos_z(float value) {
  
  headpos_z_ = value;
  // @@protoc_insertion_point(field_set:FrameMsg.headPos_z)
}

// bytes frame = 37;
inline void FrameMsg::clear_frame() {
  frame_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& FrameMsg::frame() const {
  // @@protoc_insertion_point(field_get:FrameMsg.frame)
  return frame_.GetNoArena();
}
inline void FrameMsg::set_frame(const ::std::string& value) {
  
  frame_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:FrameMsg.frame)
}
#if LANG_CXX11
inline void FrameMsg::set_frame(::std::string&& value) {
  
  frame_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:FrameMsg.frame)
}
#endif
inline void FrameMsg::set_frame(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  frame_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:FrameMsg.frame)
}
inline void FrameMsg::set_frame(const void* value, size_t size) {
  
  frame_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:FrameMsg.frame)
}
inline ::std::string* FrameMsg::mutable_frame() {
  
  // @@protoc_insertion_point(field_mutable:FrameMsg.frame)
  return frame_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* FrameMsg::release_frame() {
  // @@protoc_insertion_point(field_release:FrameMsg.frame)
  
  return frame_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void FrameMsg::set_allocated_frame(::std::string* frame) {
  if (frame != NULL) {
    
  } else {
    
  }
  frame_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), frame);
  // @@protoc_insertion_point(field_set_allocated:FrameMsg.frame)
}

// -------------------------------------------------------------------

// SceneMsg

// int32 objects_count = 2;
inline void SceneMsg::clear_objects_count() {
  objects_count_ = 0;
}
inline ::google::protobuf::int32 SceneMsg::objects_count() const {
  // @@protoc_insertion_point(field_get:SceneMsg.objects_count)
  return objects_count_;
}
inline void SceneMsg::set_objects_count(::google::protobuf::int32 value) {
  
  objects_count_ = value;
  // @@protoc_insertion_point(field_set:SceneMsg.objects_count)
}

// float headPos_x = 3;
inline void SceneMsg::clear_headpos_x() {
  headpos_x_ = 0;
}
inline float SceneMsg::headpos_x() const {
  // @@protoc_insertion_point(field_get:SceneMsg.headPos_x)
  return headpos_x_;
}
inline void SceneMsg::set_headpos_x(float value) {
  
  headpos_x_ = value;
  // @@protoc_insertion_point(field_set:SceneMsg.headPos_x)
}

// float headPos_y = 4;
inline void SceneMsg::clear_headpos_y() {
  headpos_y_ = 0;
}
inline float SceneMsg::headpos_y() const {
  // @@protoc_insertion_point(field_get:SceneMsg.headPos_y)
  return headpos_y_;
}
inline void SceneMsg::set_headpos_y(float value) {
  
  headpos_y_ = value;
  // @@protoc_insertion_point(field_set:SceneMsg.headPos_y)
}

// float headPos_z = 5;
inline void SceneMsg::clear_headpos_z() {
  headpos_z_ = 0;
}
inline float SceneMsg::headpos_z() const {
  // @@protoc_insertion_point(field_get:SceneMsg.headPos_z)
  return headpos_z_;
}
inline void SceneMsg::set_headpos_z(float value) {
  
  headpos_z_ = value;
  // @@protoc_insertion_point(field_set:SceneMsg.headPos_z)
}

// repeated .VObject scene_objects = 1;
inline int SceneMsg::scene_objects_size() const {
  return scene_objects_.size();
}
inline void SceneMsg::clear_scene_objects() {
  scene_objects_.Clear();
}
inline const ::VObject& SceneMsg::scene_objects(int index) const {
  // @@protoc_insertion_point(field_get:SceneMsg.scene_objects)
  return scene_objects_.Get(index);
}
inline ::VObject* SceneMsg::mutable_scene_objects(int index) {
  // @@protoc_insertion_point(field_mutable:SceneMsg.scene_objects)
  return scene_objects_.Mutable(index);
}
inline ::VObject* SceneMsg::add_scene_objects() {
  // @@protoc_insertion_point(field_add:SceneMsg.scene_objects)
  return scene_objects_.Add();
}
inline ::google::protobuf::RepeatedPtrField< ::VObject >*
SceneMsg::mutable_scene_objects() {
  // @@protoc_insertion_point(field_mutable_list:SceneMsg.scene_objects)
  return &scene_objects_;
}
inline const ::google::protobuf::RepeatedPtrField< ::VObject >&
SceneMsg::scene_objects() const {
  // @@protoc_insertion_point(field_list:SceneMsg.scene_objects)
  return scene_objects_;
}

// -------------------------------------------------------------------

// VObject

// .VObject.VObjectType type = 1;
inline void VObject::clear_type() {
  type_ = 0;
}
inline ::VObject_VObjectType VObject::type() const {
  // @@protoc_insertion_point(field_get:VObject.type)
  return static_cast< ::VObject_VObjectType >(type_);
}
inline void VObject::set_type(::VObject_VObjectType value) {
  
  type_ = value;
  // @@protoc_insertion_point(field_set:VObject.type)
}

// float size_x = 2;
inline void VObject::clear_size_x() {
  size_x_ = 0;
}
inline float VObject::size_x() const {
  // @@protoc_insertion_point(field_get:VObject.size_x)
  return size_x_;
}
inline void VObject::set_size_x(float value) {
  
  size_x_ = value;
  // @@protoc_insertion_point(field_set:VObject.size_x)
}

// float size_y = 3;
inline void VObject::clear_size_y() {
  size_y_ = 0;
}
inline float VObject::size_y() const {
  // @@protoc_insertion_point(field_get:VObject.size_y)
  return size_y_;
}
inline void VObject::set_size_y(float value) {
  
  size_y_ = value;
  // @@protoc_insertion_point(field_set:VObject.size_y)
}

// float size_z = 4;
inline void VObject::clear_size_z() {
  size_z_ = 0;
}
inline float VObject::size_z() const {
  // @@protoc_insertion_point(field_get:VObject.size_z)
  return size_z_;
}
inline void VObject::set_size_z(float value) {
  
  size_z_ = value;
  // @@protoc_insertion_point(field_set:VObject.size_z)
}

// float p_x = 5;
inline void VObject::clear_p_x() {
  p_x_ = 0;
}
inline float VObject::p_x() const {
  // @@protoc_insertion_point(field_get:VObject.p_x)
  return p_x_;
}
inline void VObject::set_p_x(float value) {
  
  p_x_ = value;
  // @@protoc_insertion_point(field_set:VObject.p_x)
}

// float p_y = 6;
inline void VObject::clear_p_y() {
  p_y_ = 0;
}
inline float VObject::p_y() const {
  // @@protoc_insertion_point(field_get:VObject.p_y)
  return p_y_;
}
inline void VObject::set_p_y(float value) {
  
  p_y_ = value;
  // @@protoc_insertion_point(field_set:VObject.p_y)
}

// float p_z = 7;
inline void VObject::clear_p_z() {
  p_z_ = 0;
}
inline float VObject::p_z() const {
  // @@protoc_insertion_point(field_get:VObject.p_z)
  return p_z_;
}
inline void VObject::set_p_z(float value) {
  
  p_z_ = value;
  // @@protoc_insertion_point(field_set:VObject.p_z)
}

// float q_x = 8;
inline void VObject::clear_q_x() {
  q_x_ = 0;
}
inline float VObject::q_x() const {
  // @@protoc_insertion_point(field_get:VObject.q_x)
  return q_x_;
}
inline void VObject::set_q_x(float value) {
  
  q_x_ = value;
  // @@protoc_insertion_point(field_set:VObject.q_x)
}

// float q_y = 9;
inline void VObject::clear_q_y() {
  q_y_ = 0;
}
inline float VObject::q_y() const {
  // @@protoc_insertion_point(field_get:VObject.q_y)
  return q_y_;
}
inline void VObject::set_q_y(float value) {
  
  q_y_ = value;
  // @@protoc_insertion_point(field_set:VObject.q_y)
}

// float q_z = 10;
inline void VObject::clear_q_z() {
  q_z_ = 0;
}
inline float VObject::q_z() const {
  // @@protoc_insertion_point(field_get:VObject.q_z)
  return q_z_;
}
inline void VObject::set_q_z(float value) {
  
  q_z_ = value;
  // @@protoc_insertion_point(field_set:VObject.q_z)
}

// float q_w = 11;
inline void VObject::clear_q_w() {
  q_w_ = 0;
}
inline float VObject::q_w() const {
  // @@protoc_insertion_point(field_get:VObject.q_w)
  return q_w_;
}
inline void VObject::set_q_w(float value) {
  
  q_w_ = value;
  // @@protoc_insertion_point(field_set:VObject.q_w)
}

// float color_x = 12;
inline void VObject::clear_color_x() {
  color_x_ = 0;
}
inline float VObject::color_x() const {
  // @@protoc_insertion_point(field_get:VObject.color_x)
  return color_x_;
}
inline void VObject::set_color_x(float value) {
  
  color_x_ = value;
  // @@protoc_insertion_point(field_set:VObject.color_x)
}

// float color_y = 13;
inline void VObject::clear_color_y() {
  color_y_ = 0;
}
inline float VObject::color_y() const {
  // @@protoc_insertion_point(field_get:VObject.color_y)
  return color_y_;
}
inline void VObject::set_color_y(float value) {
  
  color_y_ = value;
  // @@protoc_insertion_point(field_set:VObject.color_y)
}

// float color_z = 14;
inline void VObject::clear_color_z() {
  color_z_ = 0;
}
inline float VObject::color_z() const {
  // @@protoc_insertion_point(field_get:VObject.color_z)
  return color_z_;
}
inline void VObject::set_color_z(float value) {
  
  color_z_ = value;
  // @@protoc_insertion_point(field_set:VObject.color_z)
}

// float color_w = 15;
inline void VObject::clear_color_w() {
  color_w_ = 0;
}
inline float VObject::color_w() const {
  // @@protoc_insertion_point(field_get:VObject.color_w)
  return color_w_;
}
inline void VObject::set_color_w(float value) {
  
  color_w_ = value;
  // @@protoc_insertion_point(field_set:VObject.color_w)
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)


#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::VObject_VObjectType> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::VObject_VObjectType>() {
  return ::VObject_VObjectType_descriptor();
}

}  // namespace protobuf
}  // namespace google
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_message_2eproto__INCLUDED
