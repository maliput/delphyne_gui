// Copyright 2019 Toyota Research Institute

#ifndef DELPHYNE_GUI_RULES_VISUALIZER_WIDGET_HH
#define DELPHYNE_GUI_RULES_VISUALIZER_WIDGET_HH

#include <string>

#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

namespace delphyne {
namespace gui {

/// \class RulesVisualizerWidget
/// \brief A class that implements a simple visualizer for loaded lanes
/// with their rules associated.
class RulesVisualizerWidget : public QWidget {
  Q_OBJECT

 public:
  /// \brief Default constructor.
  explicit RulesVisualizerWidget(QWidget* parent = nullptr);

  /// \brief Default Destructor.
  virtual ~RulesVisualizerWidget() = default;

  /// \brief Add lane id to ListWidget.
  void AddLaneId(const QString& lane_id);
  /// \brief Append text to TextBrowser adding a newline at the end.
  void AddText(const QString& text);
  /// \brief Clear the ListWidget.
  void ClearLaneList();
  /// \brief Clear the text in TextBrowser.
  void ClearText();

 signals:

  /// \brief Signal used to request rules for a given lane id
  void RequestRulesForLaneId(QString lane_id);
  /// \brief Signal connected internally to handle rules for a given lane id
  void ReceiveRules(QString lane_id, QString rules);

 private slots:

  /// \brief Slot connected when the user clicks an item from the ListWidget.
  /// Emits RequestRulesForLaneId signal
  void OnItemClicked(QListWidgetItem* item);
  /// \brief Slot connected to ReceiveRules signal. Clears the text browser
  /// and populates it with the rules for the requested lane id.
  void OnRulesReceived(QString lane_id, QString rules);

 private:
  QLabel* lanes_label{nullptr};
  QLabel* rules_label{nullptr};
  QListWidget* lanes_list{nullptr};
  QTextBrowser* rules_log_text_browser{nullptr};
};
}
}

#endif  // DELPHYNE_GUI_RULES_VISUALIZER_WIDGET_HH
