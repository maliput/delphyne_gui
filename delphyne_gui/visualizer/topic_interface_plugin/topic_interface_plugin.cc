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

// Serializes a @p value into @p os.
std::ostream& operator<<(std::ostream& os, const MessageWidget::Variant& value) {
  if (value.doubleVal.has_value()) {
    os << value.doubleVal.value();
  } else if (value.floatVal.has_value()) {
    os << value.floatVal.value();
  } else if (value.int64Val.has_value()) {
    os << value.int64Val.value();
  } else if (value.int32Val.has_value()) {
    os << value.int32Val.value();
  } else if (value.uInt64Val.has_value()) {
    os << value.uInt64Val.value();
  } else if (value.uInt32Val.has_value()) {
    os << value.uInt32Val.value();
  } else if (value.boolVal.has_value()) {
    os << std::boolalpha << value.boolVal.value();
  } else if (value.stringVal.has_value()) {
    os << value.stringVal.value();
  } else if (value.enumVal.has_value()) {
    os << value.enumVal.value().name;
  }
  return os;
}

// Serializes a @p message into @p os.
std::ostream& operator<<(std::ostream& os, const MessageWidget& message) {
  os << "{ type: " << message.TypeName();
  os << ", is_compound: " << std::boolalpha << message.IsCompound();
  if (!message.IsCompound()) {
    os << ", value: " << message.Value();
  } else {
    os << ", children: { ";
    for (const auto& key_message : message.Children()) {
      os << "{ ";
      os << key_message.first << ": ";
      os << *key_message.second;
      os << " }, ";
    }
    os << "}, ";
  }
  os << " }";
  return os;
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
  // TODO(#332): separator was not found, this is an error when calling this
  //             function as it assumes the field is repeated.
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

  int uiTimerPeriodMs = kUiTimerPerdiodMs;

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
    // UI update period.
    if (auto xmlUiUpdatePeriodMs = _pluginElem->FirstChildElement("ui_update_period_ms")) {
      xmlUiUpdatePeriodMs->QueryIntText(&uiTimerPeriodMs);
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
  VisitMessageWidgets("", root, messageWidget.get(), true /* top level item */);
  // @}
}

void TopicInterfacePlugin::VisitMessageWidgets(const std::string& _name, QStandardItem* _parent,
                                               MessageWidget* _messageWidget, bool _isTopLevel) {
  // Does not visit blacklisted items.
  if (std::find_if(hideWidgets.begin(), hideWidgets.end(),
                   // amendedName is the name of the field but it applies a lower case transformation
                   // and removes the "::X::" of the name when it represents a repeated field.
                   [amendedName = RemoveNumberingField(StringToLowerCase(_name))](const std::string& hideTopic) {
                     return amendedName == StringToLowerCase(hideTopic);
                   }) != hideWidgets.end()) {
    return;
  }

  const QString uniqueName = QString::fromStdString(_name);
  const QString name =
      QString::fromStdString(_messageWidget->IsRepeated() ? GetRepeatedName(_name) : GetSimpleName(_name));

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
  if (_messageWidget->IsCompound()) {
    for (const auto& name_child : _messageWidget->Children()) {
      VisitMessageWidgets(name_child.first, _isTopLevel ? _parent : item, name_child.second.get(),
                          false /* not a top level element */);
    }
  } else {
    std::stringstream ss;
    ss << _messageWidget->Value();
    data = QString::fromStdString(ss.str());
  }
  // When the item is a top level message, it is not added to the UI but its
  // children are.
  if (!_isTopLevel) {
    item->setData(QVariant(name), MessageModel::kNameRole);
    item->setData(QVariant(QString::fromStdString(_messageWidget->TypeName())), MessageModel::kTypeRole);
    item->setData(QVariant(data), MessageModel::kDataRole);
    if (shouldAppendToParent) {
      _parent->appendRow(item);
    }
  }
}

void TopicInterfacePlugin::OnMessage(const google::protobuf::Message& _msg) {
  std::lock_guard<std::mutex> lock(mutex);
  messageWidget = std::make_unique<MessageWidget>("", &_msg, false /* is not repeated */);
  emit MessageReceived();
}

}  // namespace gui
}  // namespace delphyne

// Register this plugin
IGNITION_ADD_PLUGIN(delphyne::gui::TopicInterfacePlugin, ignition::gui::Plugin)
