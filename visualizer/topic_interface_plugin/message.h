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
#pragma once

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <variant>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <ignition/common/Console.hh>

namespace delphyne {
namespace gui {
namespace internal {

/// @brief Holds the information of a google::protobuf::Message to be consumed
///        by a Qt widget.
/// @details Holds a variant when it is a leaf node in the message tree
///          ( @see Message::Variant ). Otherwise, when it is a compound type,
///          it populates a dictionary of Messages.
///          See Parse() implementation for a full description of how it uses
///          the reflection API in Google Protobuf Message class to get the
///          information of each field.
class Message {
 public:
  /// Wraps an enumeration field in Google Protobuf.
  struct EnumValue {
    int value;         ///< The integer value of the enum.
    std::string name;  ///< The string label the enum has.
  };

  /// It simplifies the operation by consumers when dealing with the type.
  /// An instance is well-constructed iff none or just one field has a value.
  using Variant = std::variant<double, float, int64_t, int32_t, uint32_t, uint64_t, bool, std::string, EnumValue>;

  /// @brief Constructs a Message.
  ///
  /// @details It is a template constuctor that behaves differently based on T.
  ///          When T is one of the possible Variant's types, it just stores the
  ///          value in `variantValue`. When it is a pointer (it'll compile
  ///          only for google::protobuf::Message*) it'll forward a copy of
  ///          @p _value to Parse() nodes.
  /// @param _name The message name. The fully qualified attribute name will be
  ///        used when it is recursively built.
  /// @param _value The value to hold. When it is a pointer, it is assumed to be
  ///        a google::protobuf::Message* which will request a recursive
  ///        introspection via Parse() to populate children.
  /// @param _isRepeated Whether or not this message is a repeated field in a
  ///        google::protobuf::Message definition.
  template <typename T>
  Message(const std::string& _name, T _value, bool _isRepeated) {
    name = _name;
    isRepeated = _isRepeated;
    if constexpr (std::is_pointer<T>::value) {
      auto msg = _value->New();
      msg->CopyFrom(*_value);
      Parse(name, msg);
    } else {
      typeName = typeid(_value).name();
      variantValue = _value;
    }
  }

  /// @return The full name of this item in the proto message hierarchy. It
  ///         uses "::" to separate field names and injects "::X::" where X is
  ///         a non-negative number to differentiate repeated fields.
  std::string Name() const { return name; }

  /// @return The type name of the message.
  std::string TypeName() const { return typeName; }

  /// @return Whether this type is compound or not.
  bool IsCompound() const { return !children.empty(); }

  /// @return The value this message holds.
  Variant Value() const { return variantValue; }

  /// @return Whether the message is a repeated value of a type at the certain
  ///         level in the hierarchy.
  bool IsRepeated() const { return isRepeated; }

  /// @return The children dictionary.
  const std::map<std::string, std::unique_ptr<Message>>& Children() const { return children; }

 private:
  /// @brief Parses @p _msg and stores children Message into
  ///       `variantValue` using @p _scopedName as `name`.
  void Parse(const std::string& _scopedName, google::protobuf::Message* _msg);

  /// @brief This message full name.
  std::string name{""};

  /// @brief Holds the type name of the data.
  std::string typeName{""};

  /// @brief Holds the data of a terminal node.
  Variant variantValue;

  /// @brief Whether or not this is a repeated field.
  bool isRepeated{false};

  /// @brief Holds the children, nested values.
  std::map<std::string, std::unique_ptr<Message>> children;
};

}  // namespace internal
}  // namespace gui
}  // namespace delphyne
