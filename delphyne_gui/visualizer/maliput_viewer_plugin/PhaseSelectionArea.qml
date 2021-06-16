// Copyright 2021 Toyota Research Institute

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Material 2.1
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.3

GridLayout {
  id: phaseSelectionPanel
  columns: 1
  anchors.left: parent.left
  anchors.leftMargin: 10
  anchors.right: parent.right
  anchors.rightMargin: 10
  anchors.top: parent.top
  Layout.fillWidth: true

  // Title text
  Text {
    id: titleText
    Layout.columnSpan: 1
    anchors.horizontalCenter: parent.horizontalCenter
    Layout.alignment: Qt.AlignVTop | Qt.AlignHCenter
    font.family: "Helvetica"
    font.pixelSize: 14
    text: "PHASE"
  }

  TreeView {
    id: phaseRingTree
    model: PhaseTreeModel
    anchors.top: titleText.bottom
    anchors.left: parent.left
    width: parent.width
    Layout.preferredHeight: 100
    Layout.fillWidth: true
    TableViewColumn {
        // 'display' role is the default role when using QStandardItem::setText method.
        role: "display"
        title: "Phase Ring"
    }
    itemDelegate: Item {
      Text {
          anchors.verticalCenter: parent.verticalCenter
          color: styleData.textColor
          elide: styleData.elideMode
          font.family: "Helvetica"
          font.pixelSize: 12
          text: styleData.value
      }
    }

    onClicked: {
      MaliputViewerPlugin.OnPhaseSelection(index)
    }
  }
}
