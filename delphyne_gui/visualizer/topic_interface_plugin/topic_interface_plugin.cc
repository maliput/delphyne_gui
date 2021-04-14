// Copyright 2021 Toyota Research Institute
#include "topic_interface_plugin.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <sstream>

#include <ignition/common/Console.hh>
#include <ignition/plugin/Register.hh>

namespace delphyne {
namespace gui {
namespace {

// @{ TODO(#332): Remove this once we have multiple widgets that we can display
//                each type.
std::ostream& operator<<(std::ostream& os, const MessageWidget::Variant& value) {
  os << "{ ";
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
    os << " value: " << value.enumVal.value().value << ", name: " << value.enumVal.value().name;
  }
  os << " }";
  return os;
}

std::ostream& operator<<(std::ostream& os, const MessageWidget& message) {
  os << "{ type: " << message.TypeName() << ", is_compound: " << std::boolalpha << message.IsCompound();
  if (!message.IsCompound()) {
    os << ", value: " << message.Value();
  } else {
    os << "{ ";
    for (const auto& key_message : message.Children()) {
      os << "{ ";
      os << key_message.first << ": ";
      os << *key_message.second;
      os << " }, ";
    }
  }
  os << " }";
  return os;
}
// @}

}  // namespace

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
    if (auto xmlMessageType = _pluginElem->FirstChildElement("message_type")) {
      msgType = xmlMessageType->GetText();
    }
    if (msgType.empty()) {
      ignwarn << "Message type not specified, widget will be constructed "
              << "according to the first message received on topic [" << topicName << "]." << std::endl;
    }
    // Visibility per widget.
    for (auto xmlHideWidgetElement = _pluginElem->FirstChildElement("hide"); xmlHideWidgetElement != nullptr;
         xmlHideWidgetElement = xmlHideWidgetElement->NextSiblingElement("hide")) {
      hideWidgets.push_back(xmlHideWidgetElement->GetText());
    }
  }

  // Build the message widget.
  if (!msgType.empty()) {
    auto newMsg = ignition::msgs::Factory::New(msgType, "");
    if (!newMsg) {
      ignerr << "Unable to create message of type[" << msgType << "] "
             << "widget will be initialized when a message is received." << std::endl;
    }
  }

  // Subscribe
  if (!node.Subscribe(topicName, &TopicInterfacePlugin::OnMessage, this)) {
    ignerr << "Failed to subscribe to topic [" << topicName << "]" << std::endl;
  }
}

void TopicInterfacePlugin::OnMessage(const google::protobuf::Message& _msg) {
  std::lock_guard<std::mutex> lock(mutex);
  messageWidget = std::make_unique<MessageWidget>(&_msg);
  // TODO(#332): Instead of serializing to string. Create multiple widgets that
  //             display this information in the UI.
  ignerr << *messageWidget << std::endl;
}

}  // namespace gui
}  // namespace delphyne

// Register this plugin
IGNITION_ADD_PLUGIN(delphyne::gui::TopicInterfacePlugin, ignition::gui::Plugin)
