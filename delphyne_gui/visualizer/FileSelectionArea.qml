// Copyright 2021 Toyota Research Institute

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Material 2.1
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.3

// Panel that contains four buttons for selecting:
// - Map file.
// - RoadRulebook file.
// - TrafficLightBook file.
// - PhaseRingBook file.
GridLayout {
  id: filesSelectionArea
  columns: 2
  anchors.left: parent.left
  anchors.leftMargin: 10
  anchors.right: parent.right
  anchors.rightMargin: 10
  anchors.top: parent.top
  anchors.topMargin: 10
  Layout.fillWidth: true

  // Path to the RoadRulebook file.
  property string roadRulebookPath: ""

  // Path to the TrafficLightBook file.
  property string trafficLightBookPath: ""

  // Path to the PhaseRingBook file.
  property string phaseRingBookPath: ""

  // Path to the map file.
  property string mapFilePath: ""

  /**
  * Mapfile selecting button
  */
  FileDialog {
    id: mapFileDialog
    title: "Please choose a map to load"
    nameFilters: [ "XODR files (*.xodr)", "YAML files (*.yaml)", "All files (*)" ]
    selectExisting : true
    selectFolder : false
    selectMultiple : false
    sidebarVisible : true
    onAccepted: {
        console.log("Map file selection: You chose: " + mapFileDialog.fileUrl)
        mapFilePath = mapFileDialog.fileUrl
        MaliputViewerPlugin.OnNewRoadNetwork(mapFilePath)
    }
    onRejected: {
        console.log("Map file selection: Canceled")
    }
    visible: false
  }
  TextField {
    id: mapFilePathTextField
    Layout.fillWidth: true
    Layout.columnSpan: 2
    readOnly: true
    text: mapFilePath
    placeholderText: qsTr("Load an .xodr or yaml map...")
  }
  Button {
    id: loadFileButton
    text: "LOAD"
    checkable: false
    Layout.columnSpan: 2
    Layout.fillWidth: true
    onClicked: {
      mapFileDialog.visible = true
    }
    Material.background: Material.primary
  }


  /**
  * RoadRulebook selecting button
  */
  FileDialog {
    id: roadRulebookDialog
    title: "Please choose a RoadRulebook"
    nameFilters: [ "YAML files (*.yaml)", "All files (*)" ]
    selectExisting : true
    selectFolder : false
    selectMultiple : false
    sidebarVisible : true
    onAccepted: {
        console.log("RoadRuleBook selection: You chose: " + roadRulebookDialog.fileUrl)
        roadRulebookPath = roadRulebookDialog.fileUrl
        MaliputViewerPlugin.OnNewRoadRulebook(roadRulebookPath)
    }
    onRejected: {
        console.log("RoadRuleBook selection: Canceled")
    }
    visible: false
  }
  TextField {
    id: roadRulebookPathTextField
    Layout.fillWidth: true
    readOnly: true
    text: roadRulebookPath
    placeholderText: qsTr("Optional: Select a RoadRuleBook file...")
  }
  Button {
    id: roadRulebookButton
    text: "RoadRulebook"
    checkable: false
    Layout.preferredWidth: parent.width * 0.3
    onClicked: {
      roadRulebookDialog.visible = true
    }
    Material.background: Material.primary
  }

  /**
  * Traffic light book selecting button
  */
  FileDialog {
    id: trafficLightBookDialog
    title: "Please choose a TrafficLightBook"
    nameFilters: [ "YAML files (*.yaml)", "All files (*)" ]
    selectExisting : true
    selectFolder : false
    selectMultiple : false
    sidebarVisible : true
    onAccepted: {
        console.log("TrafficLightBook selection: You chose: " + trafficLightBookDialog.fileUrl)
        trafficLightBookPath = trafficLightBookDialog.fileUrl
        MaliputViewerPlugin.OnNewTrafficLightBook(trafficLightBookPath)
    }
    onRejected: {
        console.log("TrafficLightBook selection: Canceled")
    }
    visible: false
  }
  TextField {
    id: trafficLightBookPathTextField
    Layout.fillWidth: true
    readOnly: true
    text: trafficLightBookPath
    placeholderText: qsTr("Optional: Select a TrafficLightBook file...")
  }
  Button {
    id: trafficLightBookButton
    text: "TrafficLightBook"
    checkable: false
    Layout.preferredWidth: parent.width * 0.3
    onClicked: {
      trafficLightBookDialog.visible = true
    }
    Material.background: Material.primary
  }

  /**
  * Phase ring book selecting button
  */
  FileDialog {
    id: phaseRingBookDialog
    title: "Please choose a PhaseRingBook"
    nameFilters: [ "YAML files (*.yaml)", "All files (*)" ]
    selectExisting : true
    selectFolder : false
    selectMultiple : false
    sidebarVisible : true
    onAccepted: {
        console.log("PhaseRingBook selection: You chose: " + phaseRingBookDialog.fileUrl)
        phaseRingBookPath = phaseRingBookDialog.fileUrl
        MaliputViewerPlugin.OnNewPhaseRingBook(phaseRingBookPath)
    }
    onRejected: {
        console.log("PhaseRingBook selection: Canceled")
    }
    visible: false
  }
  TextField {
    id: phaseRingBookPathTextField
    Layout.fillWidth: true
    readOnly: true
    text: phaseRingBookPath
    placeholderText: qsTr("Optional: Select a PhaseRingBook file...")
  }
  Button {
    id: phaseRingBookButton
    text: "PhaseRingBook"
    checkable: false
    Layout.preferredWidth: parent.width * 0.3
    onClicked: {
      phaseRingBookDialog.visible = true
    }
    Material.background: Material.primary
  }
}
