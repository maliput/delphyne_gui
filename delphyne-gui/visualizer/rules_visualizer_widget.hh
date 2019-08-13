// Copyright 2019 Toyota Research Institute

#ifndef DELPHYNE_GUI_RULES_VISUALIZER_WIDGET_HH
#define DELPHYNE_GUI_RULES_VISUALIZER_WIDGET_HH

#include <string>

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QTextBrowser>

namespace delphyne {
namespace gui {

/// \class RulesVisualizerWidget
/// \brief This is a class that implements a simple visualizer for loaded lanes
/// with their rules associated.
class RulesVisualizerWidget : public QWidget {
  Q_OBJECT

 public:
  /// \brief Default constructor.
  explicit RulesVisualizerWidget(QWidget* parent = 0);

  /// \brief Default Destructor.
  virtual ~RulesVisualizerWidget() = default;

  void AddLaneId(const QString& lane_id);
  void AddText(const QString& text);
  void ClearLaneList();
  void ClearText();

  signals:

  void RequestRulesForLane(QString lane_id);
  /* TODO: Add corresponding arguments */
  void ReceiveRules(QString lane_id, QString rules);

 private slots:

  void OnItemClicked(QListWidgetItem* item);
  /* TODO: Maybe use a signal between maliput viewer and this plugin for
  the mouse raycast and the road network? */
  //void OnMouseClicked();

  /* TODO: Add corresponding arguments */
  void OnRulesReceived(QString lane_id, QString rules);

 private:

  QLabel* lanes_label;
  QLabel* rules_label;
  QListWidget* lanes_list = nullptr;
  QTextBrowser* rules_log_text_browser = nullptr;


};
}
}

#endif // DELPHYNE_GUI_RULES_VISUALIZER_WIDGET_HH
