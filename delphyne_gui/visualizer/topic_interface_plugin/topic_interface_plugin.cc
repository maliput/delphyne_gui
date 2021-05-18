// Copyright 2021 Toyota Research Institute
#include "topic_interface_plugin.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <utility>

#include <ignition/common/Console.hh>
#include <ignition/gui/Application.hh>
#include <ignition/plugin/Register.hh>

namespace delphyne {
namespace gui {
namespace {

// Serializes a @p _value into @p _os. Provides a valid operator overload for
// internal::Message::EnumValue so the following function's lambda can be
// resolved.
std::ostream& operator<<(std::ostream& _os, const internal::Message::EnumValue& _value) {
  _os << _value.name;
  return _os;
}

// Serializes a @p _value into @p _os.
std::ostream& operator<<(std::ostream& _os, const internal::Message::Variant& _value) {
  std::visit([&_os](auto&& arg) { _os << arg; }, _value);
  return _os;
}

// @returns A lower case string with the contents of @p _str.
std::string StringToLowerCase(const std::string& _str) {
  std::string result(_str);
  for (auto it = result.begin(); it != result.end(); ++it) {
    *it = std::tolower(*it);
  }
  return result;
}

// Field names are called: `attribute0::attribute1::N::attribute2::M::attribute3`
// where N and M are non-negative numbers. This function returns
// `attribute0::attribute1::attribute2::attribute3` when @p _fieldName contains
// the aforementioned contents.
std::string RemoveNumberingField(const std::string& _fieldName) {
  static const std::regex kNumberFieldRegex("(::([0-9]))+");
  static const std::string kRegexReplacement("");
  return std::regex_replace(_fieldName, kNumberFieldRegex, kRegexReplacement);
}

// @returns A string with the name of the field without the chain of attribute
// names from the root message. "::" is used to split field names.
std::string GetSimpleName(const std::string _fullName) {
  const auto pos = _fullName.rfind("::");
  return pos == std::string::npos ? _fullName : _fullName.substr(pos + strlen("::"));
}

// @returns Given @p _fullName that has the following structure:
// "a::b::x::c::y::d::z" where "{a, b, c, d}" are types names and "{x, y, z}"
// are numbers because they name repeated fields, this function returns "d: z".
// When there are none occurrences of "::", this function returns @p _fullName.
std::string GetRepeatedName(const std::string _fullName) {
  auto pos = _fullName.rfind("::");
  if (pos == std::string::npos) {
    return _fullName;
  }
  const std::string lastName = _fullName.substr(pos + strlen("::"));
  const std::string scopedName = _fullName.substr(0, pos);
  pos = scopedName.rfind("::");

  return pos == std::string::npos ? (scopedName + ": " + lastName)
                                  : (scopedName.substr(pos + strlen("::")) + ": " + lastName);
}

}  // namespace

TopicInterfacePlugin::TopicInterfacePlugin() : ignition::gui::Plugin() {
  messageModel = new MessageModel;
  ignition::gui::App()->Engine()->rootContext()->setContextProperty("MessageModel", messageModel);
}

QStandardItemModel* TopicInterfacePlugin::Model() {
  std::lock_guard<std::mutex> lock(mutex);
  return reinterpret_cast<QStandardItemModel*>(messageModel);
}

void TopicInterfacePlugin::LoadConfig(const tinyxml2::XMLElement* _pluginElem) {
  if (title.empty()) {
    title = "Topic interface";
  }

  if (_pluginElem) {
    // Widget UI title.
    if (auto xmlTitle = _pluginElem->FirstChildElement("title")) {
      title = xmlTitle->GetText();
    }
    // Transport configuration.
    if (auto xmlTopicName = _pluginElem->FirstChildElement("topic")) {
      topicName = xmlTopicName->GetText();
    }
    // Visibility per widget.
    for (auto xmlHideWidgetElement = _pluginElem->FirstChildElement("hide"); xmlHideWidgetElement != nullptr;
         xmlHideWidgetElement = xmlHideWidgetElement->NextSiblingElement("hide")) {
      hideWidgets.push_back(xmlHideWidgetElement->GetText());
    }
  }

  // Subscribe
  if (!node.Subscribe(topicName, &TopicInterfacePlugin::OnMessage, this)) {
    ignerr << "Failed to subscribe to topic [" << topicName << "]" << std::endl;
  }

  // Configures the callback that will be used to update the view.
  connect(this, SIGNAL(MessageReceived()), this, SLOT(OnMessageReceived()));
}

void TopicInterfacePlugin::OnMessageReceived() {
  auto root = messageModel->invisibleRootItem();

  // @{ Load the message values.
  std::lock_guard<std::mutex> lock(mutex);
  VisitMessages("", root, message.get(), true /* top level item */);
  // @}
}

void TopicInterfacePlugin::VisitMessages(const std::string& _name, QStandardItem* _parent, internal::Message* _message,
                                         bool _isTopLevel) {
  // Does not visit blacklisted items.
  // amendedName is the name of the field but it applies a lower case transformation
  // and removes the "::X::" of the name when it represents a repeated field.
  const std::string amendedName = RemoveNumberingField(StringToLowerCase(_name));
  if (std::find_if(hideWidgets.begin(), hideWidgets.end(), [amendedName](const std::string& hideTopic) {
        return amendedName == StringToLowerCase(hideTopic);
      }) != hideWidgets.end()) {
    return;
  }

  const QString uniqueName = QString::fromStdString(_name);
  const QString name = QString::fromStdString(_message->IsRepeated() ? GetRepeatedName(_name) : GetSimpleName(_name));

  QStandardItem* item{nullptr};
  bool shouldAppendToParent{false};
  if (items.find(_name) != items.end()) {
    item = items.at(_name);
  } else {
    item = new QStandardItem(name);
    items.emplace(_name, item);
    shouldAppendToParent = true;
  }

  QString data("");
  if (_message->IsCompound()) {
    for (const auto& name_child : _message->Children()) {
      VisitMessages(name_child.first, _isTopLevel ? _parent : item, name_child.second.get(),
                    false /* not a top level element */);
    }
  } else {
    std::stringstream ss;
    ss << _message->Value();
    data = QString::fromStdString(ss.str());
  }
  // When the item is a top level message, it is not added to the UI but its
  // children are.
  if (!_isTopLevel) {
    item->setData(QVariant(name), MessageModel::kNameRole);
    item->setData(QVariant(QString::fromStdString(_message->TypeName())), MessageModel::kTypeRole);
    item->setData(QVariant(data), MessageModel::kDataRole);
    if (shouldAppendToParent) {
      _parent->appendRow(item);
    }
  }
}

void TopicInterfacePlugin::OnMessage(const google::protobuf::Message& _msg) {
  std::lock_guard<std::mutex> lock(mutex);
  message = std::make_unique<internal::Message>("", &_msg, false /* is not repeated */);
  emit MessageReceived();
}

}  // namespace gui
}  // namespace delphyne

// Register this plugin
IGNITION_ADD_PLUGIN(delphyne::gui::TopicInterfacePlugin, ignition::gui::Plugin)
