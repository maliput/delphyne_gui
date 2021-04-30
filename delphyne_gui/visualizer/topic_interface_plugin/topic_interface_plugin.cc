// Copyright 2021 Toyota Research Institute
#include "topic_interface_plugin.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <utility>

#include <ignition/common/Console.hh>
#include <ignition/gui/Application.hh>
#include <ignition/plugin/Register.hh>

namespace delphyne {
namespace gui {
namespace {

// @{ TODO(#332): Remove this once we have multiple widgets that we can display
//                each type.
std::ostream& operator<<(std::ostream& os, const MessageWidget::Variant& value) {
  // os << "{ ";
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
    // os << " value: " << value.enumVal.value().value << ", name: " << value.enumVal.value().name;
    os << value.enumVal.value().name;
  }
  // os << " }";
  return os;
}

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
// @}

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

  timer = new QTimer();
  connect(timer, SIGNAL(timeout()), this, SLOT(UpdateModel()));
  timer->start(5000);
}

void TopicInterfacePlugin::UpdateModel() {
  // Remove everything.
  auto root = messageModel->invisibleRootItem();
  int rows = root->rowCount();
  while (rows != 0) {
    root->removeRow(0);
    rows--;
  }

  // Load the message values.
  std::lock_guard<std::mutex> lock(mutex);
  VisitMessageWidgets("root", root, messageWidget.get());
}

void TopicInterfacePlugin::VisitMessageWidgets(const std::string& _name, QStandardItem* _parent, MessageWidget* _messageWidget) {
  if (_messageWidget->IsCompound()) {
    const QString name = QString::fromStdString(_name);
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
    const QString name = QString::fromStdString(_name);
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
  messageWidget = std::make_unique<MessageWidget>(&_msg);
}

}  // namespace gui
}  // namespace delphyne

// Register this plugin
IGNITION_ADD_PLUGIN(delphyne::gui::TopicInterfacePlugin, ignition::gui::Plugin)
