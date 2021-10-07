// Copyright 2021 Toyota Research Institute

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3

GridLayout {
  id: laneInfoPanel
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
    font.family: "Helvetica"
    font.pixelSize: 14
    text: "INFO"
  }

  // Text are used to show rules of lanes.
  TextArea {
    id: laneInfo
    width: parent.width
    Layout.alignment: Qt.AlignVTop | Qt.AlignHLeft
    Layout.minimumHeight: 100
    Layout.preferredHeight: 140
    Layout.maximumHeight: 150
    Layout.fillWidth: true
    font.pixelSize: 12
    font.family: "Helvetica"
  }

  // When new data arrives the table is updated.
  Connections {
    target: MaliputViewerPlugin
    onLaneInfoChanged: {
      laneInfo.text = MaliputViewerPlugin.laneInfo
    }
  }
}
