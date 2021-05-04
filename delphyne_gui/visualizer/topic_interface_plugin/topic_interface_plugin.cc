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
  for (auto it = result.begin() ; it != result.end(); ++it) {
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

}  // namespace

TopicInterfacePlugin::TopicInterfacePlugin() : ignition::gui::Plugin() {
  messageModel = new MessageModel;
  ignition::gui::App()->Engine()->rootContext()->setContextProperty("MessageModel", messageModel);
}

QStandardItemModel* TopicInterfacePlugin::Model() {
  std::lock_guard<std::mutex> lock(mutex);
  ignerr << *messageWidget << std::endl;
  return reinterpret_cast<QStandardItemModel *>(messageModel);
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

  // @{ Configures the timer that will be used to update the view.
  timer = new QTimer();
  connect(timer, SIGNAL(timeout()), this, SLOT(UpdateView()));
  timer->start(uiTimerPeriodMs);
  // @}
}

void TopicInterfacePlugin::UpdateView() {
  auto root = messageModel->invisibleRootItem();

  // TODO(#332): Change this to update the nodes instead of removing and adding
  //             them on every update.
  // @{ Remove everything.
  int rows = root->rowCount();
  while (rows != 0) {
    root->removeRow(0);
    rows--;
  }
  // @}

  // @{ Load the message values.
  std::lock_guard<std::mutex> lock(mutex);
  VisitMessageWidgets("root", root, messageWidget.get());
  // @}
}

void TopicInterfacePlugin::VisitMessageWidgets(const std::string& _name, QStandardItem* _parent, MessageWidget* _messageWidget) {

  // Does not visit blacklisted items.
  if (std::find_if(hideWidgets.begin(), hideWidgets.end(),
                   // amendedName is the name of the field but it applies a lower case transformation
                   // and removes the "::X::" of the name when it represents a repeated field.
                   [amendedName = RemoveNumberingField(StringToLowerCase(_name))](const std::string& hideTopic) {
                     return amendedName == StringToLowerCase(hideTopic);
                   }) != hideWidgets.end()) {
    return;
  }

  if (_messageWidget->IsCompound()) {
    const QString name = QString::fromStdString(GetSimpleName(_name));
    const QString type = QString::fromStdString(_messageWidget->TypeName());
    const QString data("");

    QStandardItem *item = new QStandardItem(name);
    item->setData(QVariant(name), MessageModel::kNameRole);
    item->setData(QVariant(type), MessageModel::kTypeRole);
    item->setData(QVariant(data), MessageModel::kDataRole);
    _parent->appendRow(item);

    for (const auto& name_child : _messageWidget->Children()) {
      VisitMessageWidgets(name_child.first, item, name_child.second.get());
    }
  } else {
    const QString name = QString::fromStdString(GetSimpleName(_name));
    const QString type = QString::fromStdString(_messageWidget->TypeName());

    std::stringstream ss;
    ss << _messageWidget->Value();
    const QString data = QString::fromStdString(ss.str());

    QStandardItem *item = new QStandardItem(name);
    item->setData(QVariant(name), MessageModel::kNameRole);
    item->setData(QVariant(type), MessageModel::kTypeRole);
    item->setData(QVariant(data), MessageModel::kDataRole);
    _parent->appendRow(item);
  }
}

void TopicInterfacePlugin::OnMessage(const google::protobuf::Message& _msg) {
  std::lock_guard<std::mutex> lock(mutex);
  messageWidget = std::make_unique<MessageWidget>("", &_msg);
}

}  // namespace gui
}  // namespace delphyne

// Register this plugin
IGNITION_ADD_PLUGIN(delphyne::gui::TopicInterfacePlugin, ignition::gui::Plugin)
