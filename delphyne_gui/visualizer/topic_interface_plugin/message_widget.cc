// Copyright 2021 Toyota Research Institute
#include "message_widget.h"

namespace delphyne {
namespace gui {

void MessageWidget::Parse(google::protobuf::Message* msg) {
  typeName = msg->GetTypeName();
  auto reflection = msg->GetReflection();
  auto descriptor = msg->GetDescriptor();

  if (!descriptor) {
    // TODO Should throw!
    return;
  }

  const int fieldCount = descriptor->field_count();

  for (int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex) {
    auto fieldDescriptor = descriptor->field(fieldIndex);

    const auto fieldType = fieldDescriptor->type();

    const std::string fieldName = fieldDescriptor->name();

    const std::string scopedName = fieldName;

    if (fieldDescriptor->is_repeated()) {
      // Parse all fields of the repeated message.
      for (int count = 0; count < reflection->FieldSize(*msg, fieldDescriptor); ++count) {
        // Append number to name to differentiate between repeated variables.
        const std::string name = scopedName + "::" + std::to_string(count);
        // Evaluate the type.
        if (fieldType == google::protobuf::FieldDescriptor::TYPE_MESSAGE) {
          auto& value = reflection->GetRepeatedMessage(*msg, fieldDescriptor, count);
          children[scopedName] = std::make_unique<MessageWidget>(&value);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_DOUBLE) {
          const double value = reflection->GetRepeatedDouble(*msg, fieldDescriptor, count);
          children[name] = std::make_unique<MessageWidget>(value);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_FLOAT) {
          const float value = reflection->GetRepeatedFloat(*msg, fieldDescriptor, count);
          children[name] = std::make_unique<MessageWidget>(value);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT64) {
          const int64_t value = reflection->GetRepeatedInt64(*msg, fieldDescriptor, count);
          children[name] = std::make_unique<MessageWidget>(value);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT32) {
          const int32_t value = reflection->GetRepeatedInt32(*msg, fieldDescriptor, count);
          children[name] = std::make_unique<MessageWidget>(value);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT64) {
          const uint64_t value = reflection->GetRepeatedUInt64(*msg, fieldDescriptor, count);
          children[name] = std::make_unique<MessageWidget>(value);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT32) {
          const uint32_t value = reflection->GetRepeatedUInt32(*msg, fieldDescriptor, count);
          children[name] = std::make_unique<MessageWidget>(value);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_BOOL) {
          const bool value = reflection->GetRepeatedBool(*msg, fieldDescriptor, count);
          children[name] = std::make_unique<MessageWidget>(value);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_STRING) {
          const std::string value = reflection->GetRepeatedString(*msg, fieldDescriptor, count);
          children[name] = std::make_unique<MessageWidget>(value);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_ENUM) {
          auto enumValueDescriptor = reflection->GetRepeatedEnum(*msg, fieldDescriptor, count);
          const int enumValue = enumValueDescriptor->number();
          const std::string enumName = enumValueDescriptor->name();
          children[name] = std::make_unique<MessageWidget>(EnumValue{enumValue, enumName});
        } else {
          ignwarn << "Unhandled message type [" << fieldType << "]" << std::endl;
        }
      }
    } else {  // It's not a repeated message, then we just need to parse them.
      if (fieldType == google::protobuf::FieldDescriptor::TYPE_MESSAGE) {
        auto& value = reflection->GetMessage(*msg, fieldDescriptor);
        children[scopedName] = std::make_unique<MessageWidget>(&value);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_DOUBLE) {
        const double value = reflection->GetDouble(*msg, fieldDescriptor);
        children[scopedName] = std::make_unique<MessageWidget>(value);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_FLOAT) {
        const float value = reflection->GetFloat(*msg, fieldDescriptor);
        children[scopedName] = std::make_unique<MessageWidget>(value);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT64) {
        const int64_t value = reflection->GetInt64(*msg, fieldDescriptor);
        children[scopedName] = std::make_unique<MessageWidget>(value);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT32) {
        const int32_t value = reflection->GetInt32(*msg, fieldDescriptor);
        children[scopedName] = std::make_unique<MessageWidget>(value);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT64) {
        const uint64_t value = reflection->GetUInt64(*msg, fieldDescriptor);
        children[scopedName] = std::make_unique<MessageWidget>(value);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT32) {
        const uint32_t value = reflection->GetUInt32(*msg, fieldDescriptor);
        children[scopedName] = std::make_unique<MessageWidget>(value);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_BOOL) {
        const bool value = reflection->GetBool(*msg, fieldDescriptor);
        children[scopedName] = std::make_unique<MessageWidget>(value);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_STRING) {
        const std::string value = reflection->GetString(*msg, fieldDescriptor);
        children[scopedName] = std::make_unique<MessageWidget>(value);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_ENUM) {
        auto enumValueDescriptor = reflection->GetEnum(*msg, fieldDescriptor);
        const int enumValue = enumValueDescriptor->number();
        const std::string enumName = enumValueDescriptor->name();
        children[scopedName] = std::make_unique<MessageWidget>(EnumValue{enumValue, enumName});
      } else {
        ignwarn << "Unhandled message type [" << fieldType << "]" << std::endl;
      }
    }
  }
}

}  // namespace gui
}  // namespace delphyne
