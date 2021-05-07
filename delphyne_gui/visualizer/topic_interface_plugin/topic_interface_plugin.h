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

/// @brief Model implementation to visualize the data in a TreeView.
class MessageModel : public QStandardItemModel {
 public:
  static constexpr int kNameRole{101};
  static constexpr int kTypeRole{102};
  static constexpr int kDataRole{103};

  /// @brief roles and names of the model
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
///            `<hide>attribute0::attribute1::attribute2</hide>` to omit
///            displaying that specific element and all its descendants.
///          - Use `<title>My fancy title</title>` to select the widget display
///            title.
///          - Use `<ui_update_period_ms>NumberInMs</ui_update_period_ms>` to
///            define the update period of the UI.
class TopicInterfacePlugin : public ignition::gui::Plugin {
  Q_OBJECT

 public:
  /// @brief Constructor.
  TopicInterfacePlugin();

  /// @brief Loads the plugin configuration.
  /// @details See class description for the expected configuration.
  /// @param _pluginElem The XML configuration of this plugin.
  void LoadConfig(const tinyxml2::XMLElement* _pluginElem) override;

  /// @brief Get the model of msgs & fields
  /// @return Pointer to the model of msgs & fields
  QStandardItemModel* Model();

  /// @brief Callback executed when there is a new message from the topic.
  /// @details Update the message.
  /// @details @p _msg is converted into a MessageWidget and set to be
  ///          displayed.
  /// @param _msg The received message.
  void OnMessage(const google::protobuf::Message& _msg);

 public slots:

  /// @brief Updates the UI with the values of messageModel.
  void UpdateView();

 private:
  /// @brief Default UI update period in milliseconds.
  static constexpr int kUiTimerPerdiodMs{10000};

  /// @brief Visits nodes in @p _messageWidget and adds them as new rows of a
  ///        @p _parent item when they are not there.
  /// @details This function implements a visitor pattern and is called
  ///          recursively over the children nodes of @p _messageWidget. For
  ///          each node in @p _messageWidget, a new MessageModel is added with
  ///          the its contents.
  ///          When @p _messageWidget is a top level message item, its children
  ///          will be added to the parent UI element but not itself.
  /// @param _name The name of @p _messageWidget node.
  /// @param _parent The parent of @p _messageWidget node.
  /// @param _messageWidget The message to fill in an UI item.
  /// @param _isTopLevel Whether @p _messageWidget is a top level item in the
  ///        tree hierarchy.
  void VisitMessageWidgets(const std::string& _name, QStandardItem* _parent, MessageWidget* _messageWidget,
                           bool _isTopLevel);

  /// @brief The type of the message to receive.
  std::string msgType{};

  /// @brief The topic name to listen to.
  std::string topicName{"/echo"};

  /// @brief List of message types to hide.
  std::vector<std::string> hideWidgets;

  /// @brief Latest received message
  std::unique_ptr<MessageWidget> messageWidget;

  /// @brief Transport node.
  ignition::transport::Node node;

  /// @brief Mutex to protect message changes between threads.
  std::mutex mutex;

  /// @brief ComponentsModel componentsModel;
  MessageModel* messageModel{nullptr};

  /// @brief Handles the update in the UI.
  QTimer* timer{nullptr};
};

}  // namespace gui
}  // namespace delphyne
