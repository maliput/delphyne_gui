// Copyright 2021 Toyota Research Institute

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.3

Rectangle {
  id: maliputViewerPlugin
  Layout.minimumWidth: 400
  Layout.minimumHeight: 175
  anchors.fill: parent

  // Files Selection Panel
  Loader {
    id: filesLoader
    width: parent.width
    source: "FileSelectionArea.qml"
  }
  ToolSeparator {
    id: fileSectionSeparator
    orientation: Qt.Horizontal
    anchors.top: filesLoader.bottom
    anchors.topMargin: 15
    anchors.left: parent.left
    anchors.leftMargin: 10
    anchors.right: parent.right
    anchors.rightMargin: 10
  }
  // Layers Selection Panel
  Loader {
    id: layersLoader
    width: parent.width
    source: "LayersSelectionArea.qml"
    anchors.top: fileSectionSeparator.bottom
    anchors.left: parent.left
    anchors.right: parent.right
  }
  ToolSeparator {
    id: layersVisualizationSeparator
    orientation: Qt.Horizontal
    anchors.top: layersLoader.bottom
    anchors.topMargin: 15
    anchors.left: parent.left
    anchors.leftMargin: 10
    anchors.right: parent.right
    anchors.rightMargin: 10
  }
  // Label Selection Panel
  Loader {
    id: labelsLoader
    width: parent.width
    source: "LabelsSelectionArea.qml"
    anchors.top: layersVisualizationSeparator.bottom
    anchors.left: parent.left
    anchors.right: parent.right
  }
  ToolSeparator {
    id: selectedLanesSeparator
    orientation: Qt.Horizontal
    anchors.top: labelsLoader.bottom
    anchors.topMargin: 15
    anchors.left: parent.left
    anchors.leftMargin: 10
    anchors.right: parent.right
    anchors.rightMargin: 10
  }
  // Selected Lanes Panel
  Loader {
    id: selectedLanesLoader
    width: parent.width
    source: "SelectedLanesArea.qml"
    anchors.top: selectedLanesSeparator.bottom
    anchors.left: parent.left
    anchors.right: parent.right
  }
}
