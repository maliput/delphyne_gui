// Copyright 2021 Toyota Research Institute

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Material 2.1
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.3

GridLayout {
  id: rulesListPanel
  columns: 1
  anchors.left: parent.left
  anchors.leftMargin: 10
  anchors.right: parent.right
  anchors.rightMargin: 10
  anchors.top: parent.top
  Layout.fillWidth: true

  // Title text.
  Text {
    id: titleText
    Layout.columnSpan: 1
    Layout.alignment: Qt.AlignVTop | Qt.AlignHCenter
    font.pixelSize: 14
    text: "RULES"
  }

  // Text are used to show rules of lanes.
  TextArea {
    id: rules
    Layout.alignment: Qt.AlignVTop | Qt.AlignHLeft
    width: parent.width
    Layout.minimumHeight: 100
    Layout.preferredHeight: 175
    Layout.maximumHeight: 175
    Layout.fillWidth: true
    font.pixelSize: 12
    font.family: "Helvetica"
  }

  // When new data arrives the table is updated.
  Connections {
    target: MaliputViewerPlugin
    onRulesListChanged: {
      rules.text = MaliputViewerPlugin.rulesList
    }
  }
}
