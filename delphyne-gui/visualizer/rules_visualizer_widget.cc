// Copyright 2019 Toyota Research Institute

#include "rules_visualizer_widget.hh"

#include <QtWidgets/QListWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <delphyne/macros.h>
#include <ignition/common/Console.hh>
#include <ignition/gui/qt.h>

namespace delphyne {
namespace gui {

RulesVisualizerWidget::RulesVisualizerWidget(QWidget* parent) : QWidget(parent) {
  QVBoxLayout* layout = new QVBoxLayout(this);
  this->rules_log_text_browser = new QTextBrowser(this);
  this->rules_label = new QLabel("Rules", this);
  this->lanes_label = new QLabel("Lanes", this);
  this->lanes_list = new QListWidget(this);
  this->phase_tree = new QTreeWidget(this);
  this->phase_tree->setColumnCount(1);
  this->phase_tree->setHeaderLabel("Phase ring");

  QObject::connect(this->lanes_list, SIGNAL(itemClicked(QListWidgetItem*)), this,
                   SLOT(OnLaneItemClicked(QListWidgetItem*)));

  QObject::connect(this, SIGNAL(ReceiveRules(QString, QString)), this, SLOT(OnRulesReceived(QString, QString)));

  layout->addWidget(this->phase_tree);
  layout->addWidget(this->lanes_label);
  layout->addWidget(this->lanes_list);
  layout->addWidget(this->rules_label);
  layout->addWidget(this->rules_log_text_browser);

  this->setLayout(layout);
}

void RulesVisualizerWidget::AddLaneId(const QString& lane_id) {
  DELPHYNE_DEMAND(this->lanes_list != nullptr);
  this->lanes_list->addItem(lane_id);
}

void RulesVisualizerWidget::AddText(const QString& text) {
  DELPHYNE_DEMAND(this->rules_log_text_browser != nullptr);
  this->rules_log_text_browser->append(text + "\n");
}

void RulesVisualizerWidget::ClearLaneList() {
  DELPHYNE_DEMAND(this->lanes_list != nullptr);
  this->lanes_list->clear();
}

void RulesVisualizerWidget::ClearText() {
  DELPHYNE_DEMAND(this->rules_log_text_browser != nullptr);
  this->rules_log_text_browser->clear();
}

void RulesVisualizerWidget::ConstructPhaseRingTree(
    std::unordered_map<std::string, std::vector<QString>>&& phases_per_ring) {
  DELPHYNE_DEMAND(this->phase_tree != nullptr);
  this->phase_tree->clear();
  for (auto& phase_ring_n_phases : phases_per_ring) {
    QTreeWidgetItem* phase_ring_item = new QTreeWidgetItem(this->phase_tree);
    phase_ring_item->setText(0, QString::fromStdString(phase_ring_n_phases.first));
    phase_ring_item->setFlags(phase_ring_item->flags() & (~Qt::ItemIsSelectable));
    this->phase_tree->addTopLevelItem(phase_ring_item);
    std::vector<QString>& phases = phase_ring_n_phases.second;
    for (size_t i = 0; i < phases.size(); ++i) {
      QTreeWidgetItem* phase_item = new QTreeWidgetItem();
      phase_item->setText(0, std::move(phases[i]));
      phase_ring_item->addChild(phase_item);
    }
  }
}

QString RulesVisualizerWidget::GetSelectedLaneId() const {
  QListWidgetItem* selected_item = this->lanes_list->currentItem();
  if (selected_item) {
    return selected_item->text();
  }
  return QString();
}

PhaseRingPhaseIds RulesVisualizerWidget::GetSelectedPhaseRingAndPhaseId() const {
  PhaseRingPhaseIds phase_ring_phase_ids;
  QList<QTreeWidgetItem*> selected_items = this->phase_tree->selectedItems();
  if (selected_items.size() > 0) {
    // We asume that we only have 1 level of depth in the tree. Example:
    // |_PhaseRing1
    // |__Phase1
    // |__Phase2
    // |_PhaseRing2
    // |__Phase1
    QTreeWidgetItem* parent = selected_items[0]->parent();
    phase_ring_phase_ids.phase_ring_id = parent->text(0);
    phase_ring_phase_ids.phase_id = selected_items[0]->text(0);
  }
  return phase_ring_phase_ids;
}

void RulesVisualizerWidget::OnLaneItemClicked(QListWidgetItem* item) { emit RequestRules(); }

void RulesVisualizerWidget::OnRulesReceived(QString lane_id, QString rules) {
  QList<QListWidgetItem*> items = this->lanes_list->findItems(lane_id, Qt::MatchExactly);
  DELPHYNE_DEMAND(items.size() == 1);
  items[0]->setSelected(true);
  this->lanes_list->scrollToItem(items[0]);
  this->ClearText();
  this->AddText(rules);
  QTextCursor cursor = this->rules_log_text_browser->textCursor();
  cursor.setPosition(0);
  this->rules_log_text_browser->setTextCursor(cursor);
}

}  // namespace gui
}  // namespace delphyne
