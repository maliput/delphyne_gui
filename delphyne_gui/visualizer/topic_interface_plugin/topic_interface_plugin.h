// Copyright 2021 Toyota Research Institute
#pragma once

#include <cmath>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include <ignition/gui/Plugin.hh>
#include <ignition/transport.hh>

#include "message_widget.h"

namespace delphyne {
namespace gui {

/// \brief Model implementation to visualize the data in a TreeView.
class MessageModel : public QStandardItemModel {
 public:
  static constexpr int kNameRole{101};
  static constexpr int kTypeRole{102};
  static constexpr int kDataRole{103};

  /// \brief roles and names of the model
  QHash<int, QByteArray> roleNames() const override {
    return {
      {kNameRole, "name"},
      {kTypeRole, "type"},
      {kDataRole, "data"},
    };
  }
};

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
  TopicInterfacePlugin();

  /// @brief Loads the plugin configuration.
  /// @details See class description for the expected configuration.
  /// @param _pluginElem The XML configuration of this plugin.
  void LoadConfig(const tinyxml2::XMLElement* _pluginElem) override;

  /// \brief Get the model of msgs & fields
  /// \return Pointer to the model of msgs & fields
  QStandardItemModel* Model();

  /// @brief Callback executed when there is a new message from the topic.
  /// @details @p _msg is converted into a MessageWidget and set to be
  ///          displayed.
  /// @param _msg The received message.
  void OnMessage(const google::protobuf::Message& _msg);

 public slots:

  /// \brief update the model according to the changes of the topics
  void UpdateModel();

 private:

  /// \brief Visits nodes in @p _messageWidget and adds them as new rows of a
  ///        @p _parent item. Node name is @p _name.
  void VisitMessageWidgets(const std::string& _name, QStandardItem* _parent, MessageWidget* _messageWidget);

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

  // ComponentsModel componentsModel;
  MessageModel* messageModel{nullptr};

  QTimer *timer{nullptr};
};

}  // namespace gui
}  // namespace delphyne
