// BSD 3-Clause License
//
// Copyright (c) 2022, Woven Planet. All rights reserved.
// Copyright (c) 2021-2022, Toyota Research Institute. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#include "message.h"

namespace delphyne {
namespace gui {
namespace internal {

void Message::Parse(const std::string& _scopedName, google::protobuf::Message* _msg) {
  constexpr bool kIsRepeated{true};
  constexpr bool kIsNotRepeated{!kIsRepeated};

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

    const std::string scopedName =
        _scopedName.empty() ? fieldDescriptor->name() : _scopedName + "::" + fieldDescriptor->name();

    if (fieldDescriptor->is_repeated()) {
      // Parse all fields of the repeated message.
      for (int count = 0; count < reflection->FieldSize(*_msg, fieldDescriptor); ++count) {
        // Append number to name to differentiate between repeated variables.
        // To make it more easy to visualize, they are 1-indexed.
        const std::string itemName = scopedName + "::" + std::to_string(count + 1);
        // Evaluate the type.
        if (fieldType == google::protobuf::FieldDescriptor::TYPE_MESSAGE) {
          auto& value = reflection->GetRepeatedMessage(*_msg, fieldDescriptor, count);
          children[itemName] = std::make_unique<Message>(itemName, &value, kIsRepeated);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_DOUBLE) {
          const double value = reflection->GetRepeatedDouble(*_msg, fieldDescriptor, count);
          children[itemName] = std::make_unique<Message>(itemName, value, kIsRepeated);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_FLOAT) {
          const float value = reflection->GetRepeatedFloat(*_msg, fieldDescriptor, count);
          children[itemName] = std::make_unique<Message>(itemName, value, kIsRepeated);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT64) {
          const int64_t value = reflection->GetRepeatedInt64(*_msg, fieldDescriptor, count);
          children[itemName] = std::make_unique<Message>(itemName, value, kIsRepeated);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT32) {
          const int32_t value = reflection->GetRepeatedInt32(*_msg, fieldDescriptor, count);
          children[itemName] = std::make_unique<Message>(itemName, value, kIsRepeated);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT64) {
          const uint64_t value = reflection->GetRepeatedUInt64(*_msg, fieldDescriptor, count);
          children[itemName] = std::make_unique<Message>(itemName, value, kIsRepeated);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT32) {
          const uint32_t value = reflection->GetRepeatedUInt32(*_msg, fieldDescriptor, count);
          children[itemName] = std::make_unique<Message>(itemName, value, kIsRepeated);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_BOOL) {
          const bool value = reflection->GetRepeatedBool(*_msg, fieldDescriptor, count);
          children[itemName] = std::make_unique<Message>(itemName, value, kIsRepeated);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_STRING) {
          const std::string value = reflection->GetRepeatedString(*_msg, fieldDescriptor, count);
          children[itemName] = std::make_unique<Message>(itemName, value, kIsRepeated);
        } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_ENUM) {
          auto enumValueDescriptor = reflection->GetRepeatedEnum(*_msg, fieldDescriptor, count);
          const int enumValue = enumValueDescriptor->number();
          const std::string enumName = enumValueDescriptor->name();
          children[itemName] = std::make_unique<Message>(itemName, EnumValue{enumValue, enumName}, kIsRepeated);
        } else {
          // Unhandled message type.
        }
      }
    } else {  // It's not a repeated message, then we just need to parse them.
      if (fieldType == google::protobuf::FieldDescriptor::TYPE_MESSAGE) {
        auto& value = reflection->GetMessage(*_msg, fieldDescriptor);
        children[scopedName] = std::make_unique<Message>(scopedName, &value, kIsNotRepeated);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_DOUBLE) {
        const double value = reflection->GetDouble(*_msg, fieldDescriptor);
        children[scopedName] = std::make_unique<Message>(scopedName, value, kIsNotRepeated);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_FLOAT) {
        const float value = reflection->GetFloat(*_msg, fieldDescriptor);
        children[scopedName] = std::make_unique<Message>(scopedName, value, kIsNotRepeated);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT64) {
        const int64_t value = reflection->GetInt64(*_msg, fieldDescriptor);
        children[scopedName] = std::make_unique<Message>(scopedName, value, kIsNotRepeated);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT32) {
        const int32_t value = reflection->GetInt32(*_msg, fieldDescriptor);
        children[scopedName] = std::make_unique<Message>(scopedName, value, kIsNotRepeated);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT64) {
        const uint64_t value = reflection->GetUInt64(*_msg, fieldDescriptor);
        children[scopedName] = std::make_unique<Message>(scopedName, value, kIsNotRepeated);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT32) {
        const uint32_t value = reflection->GetUInt32(*_msg, fieldDescriptor);
        children[scopedName] = std::make_unique<Message>(scopedName, value, kIsNotRepeated);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_BOOL) {
        const bool value = reflection->GetBool(*_msg, fieldDescriptor);
        children[scopedName] = std::make_unique<Message>(scopedName, value, kIsNotRepeated);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_STRING) {
        const std::string value = reflection->GetString(*_msg, fieldDescriptor);
        children[scopedName] = std::make_unique<Message>(scopedName, value, kIsNotRepeated);
      } else if (fieldType == google::protobuf::FieldDescriptor::TYPE_ENUM) {
        auto enumValueDescriptor = reflection->GetEnum(*_msg, fieldDescriptor);
        const int enumValue = enumValueDescriptor->number();
        const std::string enumName = enumValueDescriptor->name();
        children[scopedName] = std::make_unique<Message>(scopedName, EnumValue{enumValue, enumName}, kIsNotRepeated);
      } else {
        // Unhandled message type.
      }
    }
  }
}

}  // namespace internal
}  // namespace gui
}  // namespace delphyne
