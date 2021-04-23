// Copyright 2021 Toyota Research Institute
#pragma once

#include <cmath>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include <ignition/gui/Plugin.hh>
#include <ignition/transport.hh>

#include "message_widget.h"

namespace delphyne {
namespace gui {

/// @brief Implements a topic interface plugin.
/// @details The plugin subscribes to an ignition topic and updates in the UI
///          the value of the message when it is received. Note that this plugin
///          would subscribe to a topic in particular but will use Google
///          protobuf's introspection and reflection API to parse the message
///          and get its fields.
///          - Use `<topic>ignition_topic_name</topic>` to configure the topic
///            name.
///          - Via the xml plugin configuration one can generate a blacklist of
///            types. Use multiple nodes like
///            `<hide>type::name::goes::here</hide>` to omit displaying that
///            specific type.
///          - Use `<title>My fancy title</title>` to select the widget display
///            title.
class TopicInterfacePlugin : public ignition::gui::Plugin {
  Q_OBJECT

 public:
  /// @brief Constructor.
  TopicInterfacePlugin() = default;

  /// @brief Loads the plugin configuration.
  /// @details See class description for the expected configuration.
  /// @param _pluginElem The XML configuration of this plugin.
  void LoadConfig(const tinyxml2::XMLElement* _pluginElem) override;

  /// @brief Callback executed when there is a new message from the topic.
  /// @details @p _msg is converted into a MessageWidget and set to be
  ///          displayed.
  /// @param _msg The received message.
  void OnMessage(const google::protobuf::Message& _msg);

 private:
  /// @brief The type of the message to receive.
  std::string msgType{};

  /// @brief The topic name to listen to.
  std::string topicName{"/echo"};

  /// @brief List of message types to hide.
  std::vector<std::string> hideWidgets;

  /// \brief Latest received message
  std::unique_ptr<MessageWidget> messageWidget;

  /// @brief Transport node.
  ignition::transport::Node node;

  /// \brief Mutex to protect message buffer.
  std::mutex mutex;

  /// @brief Transport publisher.
  std::unique_ptr<ignition::transport::Node::Publisher> publisher;
};

}  // namespace gui
}  // namespace delphyne
