// Copyright 2019 Toyota Research Institute

#ifndef DELPHYNE_GUI_RULES_VISUALIZER_WIDGET_HH
#define DELPHYNE_GUI_RULES_VISUALIZER_WIDGET_HH

#include <string>

#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

namespace delphyne {
namespace gui {

/// \brief Struct containing the phase ring id and a phase id that belongs to the phase ring.
/// An empty QString represents no item selection.
struct PhaseRingPhaseIds {
  QString phase_ring_id;
  QString phase_id;
};

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
  /// \brief Construct the tree for the given phase_ring.
  /// \param[in] phases Dictionary containing all the phases for all the possible phase rings of a road geometry.
  /// \warning phases will be moved.
  void ConstructPhaseRingTree(std::unordered_map<std::string, std::vector<QString>>&& phases);

  /// \brief Get the select lane id from the list widget.
  /// \returns QString containing the selected lane id
  QString GetSelectedLaneId() const;

  /// \brief Get the selected phase ring id and phase id from the TreeWidget.
  /// \returns PhaseRingPhaseIds containing selected phase ring id and phase id (if any)
  PhaseRingPhaseIds GetSelectedPhaseRingAndPhaseId() const;

 signals:

  /// \brief Signal used to request rules for a given lane id
  void RequestRules();
  /// \brief Signal connected internally to handle rules for a given lane id
  void ReceiveRules(QString lane_id, QString rules);

 private slots:

  /// \brief Slot connected when the user clicks a lane item from the ListWidget.
  /// Emits RequestRules signal
  void OnLaneItemClicked(QListWidgetItem* item);
  /// \brief Slot connected to ReceiveRules signal. Clears the text browser
  /// and populates it with the rules for the requested lane id.
  void OnRulesReceived(QString lane_id, QString rules);

 private:
  QLabel* lanes_label{nullptr};
  QLabel* rules_label{nullptr};
  QListWidget* lanes_list{nullptr};
  QTextBrowser* rules_log_text_browser{nullptr};
  QTreeWidget* phase_tree{nullptr};
};
}  // namespace gui
}  // namespace delphyne

#endif  // DELPHYNE_GUI_RULES_VISUALIZER_WIDGET_HH
