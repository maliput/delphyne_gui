// Copyright 2019 Toyota Research Institute

#include "rules_visualizer_widget.hh"

#include <delphyne/macros.h>

#include <ignition/gui/qt.h>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>

namespace delphyne {
namespace gui {

RulesVisualizerWidget::RulesVisualizerWidget(QWidget* parent) :
    QWidget(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);
  this->rules_log_text_browser = new QTextBrowser(this);
  this->rules_label = new QLabel("Rules", this);
  this->lanes_label = new QLabel("Lanes", this);
  this->lanes_list = new QListWidget(this);

  QObject::connect(
    this->lanes_list,
    SIGNAL(itemClicked(QListWidgetItem*)),
    this,
    SLOT(OnItemClicked(QListWidgetItem*))
    );

  QObject::connect(
    this,
    SIGNAL(ReceiveRules(QString, QString)),
    this,
    SLOT(OnRulesReceived(QString, QString))
    );

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

void RulesVisualizerWidget::OnItemClicked(QListWidgetItem* item) {
    emit RequestRulesForLaneId(item->text());
}

void RulesVisualizerWidget::OnRulesReceived(QString lane_id, QString rules) {
    this->ClearText();
    this->AddText(rules);
}

} // namespace gui
} // namespace delphyne
