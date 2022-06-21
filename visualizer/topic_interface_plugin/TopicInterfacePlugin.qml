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
/*
This file has been inspired in https://github.com/ignitionrobotics/ign-gui/blob/ign-gui3/src/plugins/topic_viewer/TopicViewer.qml
See https://github.com/ignitionrobotics/ign-gui/blob/ign-gui3/LICENSE for its license.
*/

import QtQml.Models 2.2
import QtQuick 2.0
import QtQuick.Controls 1.4
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
