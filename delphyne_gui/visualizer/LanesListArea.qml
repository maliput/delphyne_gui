// Copyright 2021 Toyota Research Institute

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Material 2.1
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.3

// Table to list the lanes of the road network.
GridLayout {
  id: lanesListPanel
  columns: 1
  anchors.left: parent.left
  anchors.leftMargin: 10
  anchors.right: parent.right
  anchors.rightMargin: 10
  anchors.top: parent.top
  Layout.fillWidth: true

  /**
  * Title text
  */
  Text {
    id: titleText
    Layout.columnSpan: 1
    anchors.horizontalCenter: parent.horizontalCenter
    Layout.alignment: Qt.AlignVTop | Qt.AlignHCenter
    font.pointSize: 10
    text: "LANES"
  }

  // Table of stats.
  TableView {

    id: tableView
    anchors.top: titleText.bottom
    anchors.left: parent.left
    width: parent.width
    Layout.preferredHeight: 100
    Layout.fillWidth: true
    selectionMode: 1 /* Single Selection */
    TableViewColumn {
        role: "lane_id"
        title: "Lanes ID"
    }
    model: ListModel {
      id: tableModel
      // This list will be updated dynamically when
      // new data is presented.
    }
    onClicked: {
      MaliputViewerPlugin.OnTableLaneIdSelection(row)
    }
  }

  // When new data arrives the table is updated.
  Connections {
    target: MaliputViewerPlugin
    onListLanesChanged: {
      tableModel.clear()
      for (var i = 0; i < MaliputViewerPlugin.listLanes.length; ++i)  {
        tableModel.append({"lane_id": MaliputViewerPlugin.listLanes[i]})
      }
    }
    onTableLaneIdSelection: {
      tableView.selection.clear();
      tableView.selection.select(_index);
    }
  }
}
