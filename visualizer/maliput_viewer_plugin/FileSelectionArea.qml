// BSD 3-Clause License
//
// Copyright (c) 2022, Woven Planet. All rights reserved.
// Copyright (c) 2021-2022, Toyota Research Institute. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.3

// Panel that contains four buttons for selecting:
// - Map file.
// - RuleRegistry file.
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

  // Path to the RuleRegistry file.
  property string ruleRegistryPath: ""

  // Path to the RoadRulebook file.
  property string roadRulebookPath: ""

  // Path to the TrafficLightBook file.
  property string trafficLightBookPath: ""

  // Path to the PhaseRingBook file.
  property string phaseRingBookPath: ""

  // Path to the IntersectionBook file.
  property string intersectionBookPath: ""

  // Path to the map file.
  property string mapFilePath: ""

  /**
  * Title text
  */
  Text {
    id: titleText
    Layout.columnSpan: 2
    Layout.alignment: Qt.AlignHCenter
    font.pixelSize: 14
    text: "FILES SELECTION"
  }

  /**
  * RuleRegistry selecting button
  */
  FileDialog {
    id: ruleRegistryDialog
    title: "Please choose a RuleRegistry"
    nameFilters: [ "YAML files (*.yaml)", "All files (*)" ]
    selectExisting : true
    selectFolder : false
    selectMultiple : false
    sidebarVisible : true
    onAccepted: {
      console.log("RuleRegistry selection: You chose: " + ruleRegistryDialog.fileUrl)
      ruleRegistryPath = ruleRegistryDialog.fileUrl
    }
    onRejected: {
      console.log("RuleRegistry selection: Canceled")
    }
    visible: false
  }
  TextField {
    id: ruleRegistryPathTextField
    Layout.fillWidth: true
    readOnly: true
    text: ruleRegistryPath
    placeholderText: qsTr("Optional: Select a RuleRegistry file...")
    font.pixelSize: 12
    font.family: "Helvetica"
  }
  Button {
    id: ruleRegistryButton
    text: "RuleRegistry"
    checkable: false
    Layout.preferredWidth: parent.width * 0.3
    onClicked: {
      ruleRegistryDialog.visible = true
    }
    Material.background: Material.primary
    style: ButtonStyle {
      label: Text {
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "Helvetica"
        font.pointSize: 10
        color: "black"
        text: ruleRegistryButton.text
      }
    }
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
    font.pixelSize: 12
    font.family: "Helvetica"
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
    style: ButtonStyle {
      label: Text {
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "Helvetica"
        font.pointSize: 10
        color: "black"
        text: roadRulebookButton.text
      }
    }
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
    font.pixelSize: 12
    font.family: "Helvetica"
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
    style: ButtonStyle {
      label: Text {
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "Helvetica"
        font.pointSize: 10
        color: "black"
        text: trafficLightBookButton.text
      }
    }
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
    font.pixelSize: 12
    font.family: "Helvetica"
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
    style: ButtonStyle {
      label: Text {
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "Helvetica"
        font.pointSize: 10
        color: "black"
        text: phaseRingBookButton.text
      }
    }
  }

  /**
  * Intersection book selecting button
  */
  FileDialog {
    id: intersectionBookDialog
    title: "Please choose a IntersectionBook"
    nameFilters: [ "YAML files (*.yaml)", "All files (*)" ]
    selectExisting : true
    selectFolder : false
    selectMultiple : false
    sidebarVisible : true
    onAccepted: {
      console.log("IntersectionBook selection: You chose: " + intersectionBookDialog.fileUrl)
      intersectionBookPath = intersectionBookDialog.fileUrl
    }
    onRejected: {
      console.log("IntersectionBook selection: Canceled")
    }
    visible: false
  }
  TextField {
    id: intersectionBookPathTextField
    Layout.fillWidth: true
    readOnly: true
    text: intersectionBookPath
    placeholderText: qsTr("Optional: Select a IntersectionBook file...")
    font.pixelSize: 12
    font.family: "Helvetica"
  }
  Button {
    id: intersectionBookButton
    text: "IntersectionBook"
    checkable: false
    Layout.preferredWidth: parent.width * 0.3
    onClicked: {
      intersectionBookDialog.visible = true
    }
    Material.background: Material.primary
    style: ButtonStyle {
      label: Text {
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "Helvetica"
        font.pointSize: 10
        color: "black"
        text: intersectionBookButton.text
      }
    }
  }

  /**
  * Mapfile selecting button
  */
  FileDialog {
    id: mapFileDialog
    title: "Please choose a map to load"
    nameFilters: [ "XODR files (*.xodr)", "OSM files (*.osm)", "YAML files (*.yaml)", "All files (*)" ]
    selectExisting : true
    selectFolder : false
    selectMultiple : false
    sidebarVisible : true
    onAccepted: {
      console.log("Map file selection: You chose: " + mapFileDialog.fileUrl)
      mapFilePath = mapFileDialog.fileUrl
    }
    onRejected: {
      console.log("Map file selection: Canceled")
    }
    visible: false
  }

  TextField {
    id: mapFilePathTextField
    Layout.fillWidth: true
    readOnly: true
    text: mapFilePath
    placeholderText: qsTr("Select an XODR, OSM, or YAML map file...")
    font.pixelSize: 12
    font.family: "Helvetica"
  }
  Button {
    id: mapFileButton
    text: "Map file"
    checkable: false
    Layout.preferredWidth: parent.width * 0.3
    onClicked: {
      mapFileDialog.visible = true
    }
    Material.background: Material.primary
    style: ButtonStyle {
      label: Text {
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "Helvetica"
        font.pointSize: 10
        color: "black"
        text: mapFileButton.text
      }
    }
  }

  /*
  * Load button
  */
  Button {
    id: loadButton
    text: "LOAD"
    checkable: false
    Layout.columnSpan: 2
    Layout.fillWidth: true
    onClicked: {
      MaliputViewerPlugin.OnNewRoadNetwork(mapFilePath, ruleRegistryPath, roadRulebookPath, trafficLightBookPath, phaseRingBookPath, intersectionBookPath)
    }
    Material.background: Material.primary
    style: ButtonStyle {
      label: Text {
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "Helvetica"
        font.pointSize: 10
        color: "black"
        text: loadButton.text
      }
    }
  }
}
