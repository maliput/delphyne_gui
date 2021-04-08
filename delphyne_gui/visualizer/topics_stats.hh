// Copyright 2021 Toyota Research Institute
#pragma once

#include "ignition/gui/Plugin.hh"

namespace delphyne {
namespace gui {

/// \brief Show stats of all the topics.
class TopicsStats : public ignition::gui::Plugin {
  Q_OBJECT

  // TODO(francocipollone): Use QHash<QString,QString> when using a new version of QT instead of QStringList.
  Q_PROPERTY(QStringList data READ Data WRITE SetData NOTIFY DataChanged)
public:
  Q_INVOKABLE QStringList Data() const;

  Q_INVOKABLE void SetData(const QStringList& _Data);

 signals:
  /// @{ Signals to notify that properties changed.
  void DataChanged();

  /// \brief Constructor
 public:
  TopicsStats();

  /// \brief Destructor
  virtual ~TopicsStats() = default;

  // Documentation inherited
  virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem);

 protected slots:

  void SearchTopic(const QString& _topic);

 protected:
  /// @brief Timer event callback which handles the publication of new data in the table.
  void timerEvent(QTimerEvent* _event) override;

private:
  static constexpr int kTimerPeriodInMs{1000};

  /// @brief Triggers an event every `kTimerPeriodInMs`.
  QBasicTimer timer;

  /// \brief Table data.
  ///  The list is expected to be comformed using blocks of
  ///  [`topic`, `messages`, `frequency`, `bandwidth`]. In the QML file
  ///   this is parsed to get the four values for each row.
  ///
  QStringList data;

  /// Contains the stats of the topics.
  /// The key of `rawData` is the topic name, and value is a map containing the value of
  /// `messages`, `frequency` and `bandwidth`.
  /// This way the table will be automatically ordered by topic name.
  std::map<std::string, std::map<std::string, std::string>> rawData;
  std::string topicFilter{""};

  int counter{0};
};

}  // namespace gui
}  // namespace delphyne
