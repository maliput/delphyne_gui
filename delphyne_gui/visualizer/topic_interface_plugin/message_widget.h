// Copyright 2021 Toyota Research Institute
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <map>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include <ignition/common/Console.hh>

namespace delphyne {
namespace gui {

/// @brief Holds the information of a google::protobuf::Message to be consumed
///        by a Qt widget.
/// @details Holds a variant like-struct when it is a leaf node in the message
///          tree ( @see MessageWidget::Variant ). Otherwise, when it is a
///          compound type, it populates a dictionary of MessageWidgets.
///          See Parse() implementation for a full description of how it uses
///          the reflection API in Google Protobuf Message class to get the
///          information of each field.
class MessageWidget {
 public:
  /// Wraps an enumeration field in Google Protobuf.
  struct EnumValue {
    int value;         ///< The integer value of the enum.
    std::string name;  ///< The string label the enum has.
  };

  /// Operates as a std::variant from optionals. It simplifies the operation by
  /// consumers when dealing with the type differences.
  /// An instance is well-constructed iff none or just one field has a value.
  struct Variant {
    std::optional<double> doubleVal{std::nullopt};
    std::optional<float> floatVal{std::nullopt};
    std::optional<int64_t> int64Val{std::nullopt};
    std::optional<int32_t> int32Val{std::nullopt};
    std::optional<uint64_t> uInt64Val{std::nullopt};
    std::optional<uint32_t> uInt32Val{std::nullopt};
    std::optional<bool> boolVal{std::nullopt};
    std::optional<std::string> stringVal{std::nullopt};
    std::optional<EnumValue> enumVal{std::nullopt};
  };

  /// @{ Multiple constructors for the different types.
  ///    TODO(#332): Improve this using template constructors. Probably, Variant
  ///                needs to change to std::variant.
  explicit MessageWidget(const std::string& _name, double _value) {
    name = _name;
    typeName = typeid(_value).name();
    variantValue.doubleVal = _value;
  }
  explicit MessageWidget(const std::string& _name, float _value) {
    name = _name;
    typeName = typeid(_value).name();
    variantValue.floatVal = _value;
  }
  explicit MessageWidget(const std::string& _name, int64_t _value) {
    name = _name;
    typeName = typeid(_value).name();
    variantValue.int64Val = _value;
  }
  explicit MessageWidget(const std::string& _name, int32_t _value) {
    name = _name;
    typeName = typeid(_value).name();
    variantValue.int32Val = _value;
  }
  explicit MessageWidget(const std::string& _name, uint64_t _value) {
    name = _name;
    typeName = typeid(_value).name();
    variantValue.uInt64Val = _value;
  }
  explicit MessageWidget(const std::string& _name, uint32_t _value) {
    name = _name;
    typeName = typeid(_value).name();
    variantValue.uInt32Val = _value;
  }
  explicit MessageWidget(const std::string& _name, bool _value) {
    name = _name;
    typeName = typeid(_value).name();
    variantValue.boolVal = _value;
  }
  explicit MessageWidget(const std::string& _name, const std::string& _value) {
    name = _name;
    typeName = typeid(_value).name();
    variantValue.stringVal = _value;
  }
  explicit MessageWidget(const std::string& _name, EnumValue _value) {
    name = _name;
    typeName = typeid(_value).name();
    variantValue.enumVal = _value;
  }
  explicit MessageWidget(const std::string& _name, const google::protobuf::Message* _msg) {
    name = _name;
    auto msg = _msg->New();
    msg->CopyFrom(*_msg);
    Parse(name, msg);
  }
  /// @}

  std::string Name() const { return name; }

  /// @return The type name of the message.
  std::string TypeName() const { return typeName; }

  /// @return Whether this type is compound or not.
  bool IsCompound() const { return !children.empty(); }

  /// @return The value this message holds.
  Variant Value() const { return variantValue; }

  /// @return The children dictionary.
  const std::map<std::string, std::unique_ptr<MessageWidget>>& Children() const { return children; }

 private:
  /// @brief Parses @p _msg and stores children MessageWidget into
  ////       `variantValue`.
  void Parse(const std::string& _scopedName, google::protobuf::Message* _msg);

  std::string name{""};

  /// @brief Holds the type name of the data.
  std::string typeName{""};

  /// @brief Holds the data of a terminal node.
  Variant variantValue;

  /// @brief Holds the children, nested values.
  std::map<std::string, std::unique_ptr<MessageWidget>> children;
};

}  // namespace gui
}  // namespace delphyne
