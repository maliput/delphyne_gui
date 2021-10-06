// Copyright 2021 Toyota Research Institute

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.3

Rectangle {
  id: maliputViewerPlugin
  Layout.minimumWidth: 400
  Layout.preferredWidth: 450
  Layout.minimumHeight: 1150
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
    id: labelsLoaderSeparator
    orientation: Qt.Horizontal
    anchors.top: labelsLoader.bottom
    anchors.topMargin: 15
    anchors.left: parent.left
    anchors.leftMargin: 10
    anchors.right: parent.right
    anchors.rightMargin: 10
  }
  // Mouse Info Selection Panel
  Loader {
    id: mouseInfo
    width: parent.width
    source: "InfoArea.qml"
    anchors.top: labelsLoaderSeparator.bottom
    anchors.left: parent.left
    anchors.right: parent.right
  }
  ToolSeparator {
    id: mouseInfoSeparator
    orientation: Qt.Horizontal
    anchors.top: mouseInfo.bottom
    anchors.topMargin: 15
    anchors.left: parent.left
    anchors.leftMargin: 10
    anchors.right: parent.right
    anchors.rightMargin: 10
  }
  // Phase selection Panel
  Loader {
    id: phaseSelectionLoader
    width: parent.width
    source: "PhaseSelectionArea.qml"
    anchors.top: mouseInfoSeparator.bottom
    anchors.left: parent.left
    anchors.right: parent.right
  }
  ToolSeparator {
    id: phaseSelectionLoaderSeparator
    orientation: Qt.Horizontal
    anchors.top: phaseSelectionLoader.bottom
    anchors.topMargin: 0
    anchors.left: parent.left
    anchors.leftMargin: 10
    anchors.right: parent.right
    anchors.rightMargin: 10
  }
  // Lanes List Panel
  Loader {
    id: lanesListLoader
    width: parent.width
    source: "LanesListArea.qml"
    anchors.top: phaseSelectionLoaderSeparator.bottom
    anchors.left: parent.left
    anchors.right: parent.right
  }
  ToolSeparator {
    id: lanesListLoaderSeparator
    orientation: Qt.Horizontal
    anchors.top: lanesListLoader.bottom
    anchors.topMargin: 0
    anchors.left: parent.left
    anchors.leftMargin: 10
    anchors.right: parent.right
    anchors.rightMargin: 10
  }
  // Rules List Panel
  Loader {
    id: rulesListLoader
    width: parent.width
    source: "RulesListArea.qml"
    anchors.top: lanesListLoaderSeparator.bottom
    anchors.left: parent.left
    anchors.right: parent.right
  }
}
