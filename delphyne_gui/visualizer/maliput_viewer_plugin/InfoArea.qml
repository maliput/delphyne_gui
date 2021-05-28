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
    anchors.horizontalCenter: parent.horizontalCenter
    Layout.alignment: Qt.AlignVTop | Qt.AlignHCenter
    font.pixelSize: 14
    text: "INFO"
  }

  // Text are used to show rules of lanes.
  TextArea {
    id: laneInfo
    anchors.top: titleText.bottom
    anchors.left: parent.left
    width: parent.width
    Layout.minimumHeight: 80
    Layout.preferredHeight: 100
    Layout.maximumHeight: 100
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
