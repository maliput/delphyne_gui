// Copyright 2021 Toyota Research Institute
/*
This file has been inspired in https://github.com/ignitionrobotics/ign-gui/blob/ign-gui3/src/plugins/topic_viewer/TopicViewer.qml
See https://github.com/ignitionrobotics/ign-gui/blob/ign-gui3/LICENSE for its license.
*/

import QtQml.Models 2.2
import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3

TreeView {
  objectName: "treeView"
  id: tree
  model: MessageModel

  Layout.minimumWidth: 400
  Layout.minimumHeight: 300
  anchors.fill: parent

  property int itemHeight: 30;

  // @{ Color properties.
  property color oddColor: (Material.theme == Material.Light) ?
                            Material.color(Material.Grey, Material.Shade100):
                            Material.color(Material.Grey, Material.Shade800);

  property color evenColor: (Material.theme == Material.Light) ?
                             Material.color(Material.Grey, Material.Shade200):
                             Material.color(Material.Grey, Material.Shade900);

  property color highlightColor: Material.accentColor;
  // @}

  // @{ Bar policies.
  verticalScrollBarPolicy: Qt.ScrollBarAsNeeded
  horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
  // }


  // @{ Header properties.
  headerVisible: false
  headerDelegate: Rectangle {
      visible: false
  }
  TableViewColumn {
    title: "Name"
    role: "name"
  }
  // @}

  // @{ Selection properties.
  selection: ItemSelectionModel {
    model: tree.model
  }
  selectionMode: SelectionMode.SingleSelection
  // @}

  // @{ Delegates
  // Builds a row of the tree.
  rowDelegate: Rectangle {
    id: row
    color: (styleData.selected)? highlightColor : (styleData.row % 2 == 0) ? evenColor : oddColor
    height: itemHeight;
  }

  // Builds the data of the row.
  itemDelegate: Item {
    Text {
      anchors.verticalCenter: parent.verticalCenter
      color: styleData.textColor
      elide: styleData.elideMode
      text: model === null ? "" : model.name + ": " + model.data
      font.pointSize: 12
      y: itemHeight * 0.2
    }
  }
  // @}

  // @{ Style
  style: TreeViewStyle {
    branchDelegate: Rectangle {
      height: itemHeight
      width: itemHeight
      color: "transparent"
      // Adds a + or a - sign depending on the expanded state of the node.
      Image {
        id: branchImage
        fillMode: Image.Pad
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        sourceSize.height: itemHeight * 0.4
        sourceSize.width: itemHeight * 0.4
        source: styleData.isExpanded ? "minus.png" : "plus.png"
      }
    }
  }
  // @}
}
