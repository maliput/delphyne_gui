// Copyright 2021 Toyota Research Institute
#include "message_widget.h"

namespace delphyne {
namespace gui {

void MessageWidget::Parse(const std::string& _scopedName, google::protobuf::Message* _msg) {
  typeName = _msg->GetTypeName();
  auto reflection = _msg->GetReflection();
  auto descriptor = _msg->GetDescriptor();

  if (!descriptor) {
    // TODO Should throw!
    return;
  }

  const int fieldCount = descriptor->field_count();

  for (int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex) {
    auto fieldDescriptor = descriptor->field(fieldIndex);

    const auto fieldType = fieldDescriptor->type();

    const std::string scopedName = _scopedName.empty() ? fieldDescriptor->name() : _scopedName + "::" + fieldDescriptor->name();

    if (fieldDescriptor->is_repeated()) {
      // Parse all fields of the repeated message.
      for (int count = 0; count < reflection->FieldSize(*_msg, fieldDescriptor); ++count) {
        // Append number to name to differentiate between repeated variables.
        const std::string itemName = scopedName + "::" + std::to_string(count);
        // Evaluate the type.
        if (fieldType == google::protobuf::FieldDescriptor::TYPE_MESSAGE) {
          auto& value = reflection->GetRepeatedMessage(*_msg, fieldDescriptor, count);
          children[itemName] = std::make_unique<MessageWidget>(itemName, &value);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_DOUBLE) {
          const double value = reflection->GetRepeatedDouble(*_msg, fieldDescriptor, count);
          children[itemName] = std::make_unique<MessageWidget>(itemName, value);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_FLOAT) {
          const float value = reflection->GetRepeatedFloat(*_msg, fieldDescriptor, count);
          children[itemName] = std::make_unique<MessageWidget>(itemName, value);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT64) {
          const int64_t value = reflection->GetRepeatedInt64(*_msg, fieldDescriptor, count);
          children[itemName] = std::make_unique<MessageWidget>(itemName, value);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT32) {
          const int32_t value = reflection->GetRepeatedInt32(*_msg, fieldDescriptor, count);
          children[itemName] = std::make_unique<MessageWidget>(itemName, value);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT64) {
          const uint64_t value = reflection->GetRepeatedUInt64(*_msg, fieldDescriptor, count);
          children[itemName] = std::make_unique<MessageWidget>(itemName, value);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT32) {
          const uint32_t value = reflection->GetRepeatedUInt32(*_msg, fieldDescriptor, count);
          children[itemName] = std::make_unique<MessageWidget>(itemName, value);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_BOOL) {
          const bool value = reflection->GetRepeatedBool(*_msg, fieldDescriptor, count);
          children[itemName] = std::make_unique<MessageWidget>(itemName, value);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_STRING) {
          const std::string value = reflection->GetRepeatedString(*_msg, fieldDescriptor, count);
          children[itemName] = std::make_unique<MessageWidget>(itemName, value);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_ENUM) {
          auto enumValueDescriptor = reflection->GetRepeatedEnum(*_msg, fieldDescriptor, count);
          const int enumValue = enumValueDescriptor->number();
          const std::string enumName = enumValueDescriptor->name();
          children[itemName] = std::make_unique<MessageWidget>(itemName, EnumValue{enumValue, enumName});
        } else {
          // ignwarn << "Unhandled message type [" << fieldDescriptor->type_name() << "]" << std::endl;
        }
      }
    } else {  // It's not a repeated message, then we just need to parse them.
      if (fieldType == google::protobuf::FieldDescriptor::TYPE_MESSAGE) {
        auto& value = reflection->GetMessage(*_msg, fieldDescriptor);
        children[scopedName] = std::make_unique<MessageWidget>(scopedName, &value);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_DOUBLE) {
        const double value = reflection->GetDouble(*_msg, fieldDescriptor);
        children[scopedName] = std::make_unique<MessageWidget>(scopedName, value);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_FLOAT) {
        const float value = reflection->GetFloat(*_msg, fieldDescriptor);
        children[scopedName] = std::make_unique<MessageWidget>(scopedName, value);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT64) {
        const int64_t value = reflection->GetInt64(*_msg, fieldDescriptor);
        children[scopedName] = std::make_unique<MessageWidget>(scopedName, value);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT32) {
        const int32_t value = reflection->GetInt32(*_msg, fieldDescriptor);
        children[scopedName] = std::make_unique<MessageWidget>(scopedName, value);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT64) {
        const uint64_t value = reflection->GetUInt64(*_msg, fieldDescriptor);
        children[scopedName] = std::make_unique<MessageWidget>(scopedName, value);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT32) {
        const uint32_t value = reflection->GetUInt32(*_msg, fieldDescriptor);
        children[scopedName] = std::make_unique<MessageWidget>(scopedName, value);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_BOOL) {
        const bool value = reflection->GetBool(*_msg, fieldDescriptor);
        children[scopedName] = std::make_unique<MessageWidget>(scopedName, value);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_STRING) {
        const std::string value = reflection->GetString(*_msg, fieldDescriptor);
        children[scopedName] = std::make_unique<MessageWidget>(scopedName, value);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_ENUM) {
        auto enumValueDescriptor = reflection->GetEnum(*_msg, fieldDescriptor);
        const int enumValue = enumValueDescriptor->number();
        const std::string enumName = enumValueDescriptor->name();
        children[scopedName] = std::make_unique<MessageWidget>(scopedName, EnumValue{enumValue, enumName});
      } else {
        // ignwarn << "Unhandled message type [" << fieldDescriptor->type_name() << "]" << std::endl;
      }
    }
  }
}

}  // namespace gui
}  // namespace delphyne
