// Copyright 2021 Toyota Research Institute

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Material 2.1
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.3

// Panel that contains checkboxs to enable or disable labels.
GridLayout {
  id: labelsVisualizationPanel
  columns: 2
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
    Layout.columnSpan: 2
    anchors.horizontalCenter: parent.horizontalCenter
    Layout.alignment: Qt.AlignVTop | Qt.AlignHCenter
    font.pointSize: 10
    text: "LABELS"
  }

  /**
  * Lane checkbox
  */
  CheckBox {
    id: labelLane
    text: qsTr("Lane")
    checked: true
    onClicked : {
      MaliputViewerPlugin.OnNewTextLabelSelection("lane_labels", checked);
    }
  }

  /**
  * BranchPoint checkbox
  */
  CheckBox {
    id: labelBranchPoint
    text: qsTr("BranchPoint")
    checked: true
    onClicked : {
      MaliputViewerPlugin.OnNewTextLabelSelection("branch_point_labels", checked);
    }
  }

  /*
  * Update checkboxes' state.
  */
  Connections {
    target: MaliputViewerPlugin
    onLabelCheckboxesChanged: {
      labelLane.checked = MaliputViewerPlugin.labelCheckboxes[0]
      labelBranchPoint.checked = MaliputViewerPlugin.labelCheckboxes[1]
    }
  }
}
