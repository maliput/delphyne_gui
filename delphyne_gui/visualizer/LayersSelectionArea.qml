// Copyright 2021 Toyota Research Institute

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Material 2.1
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.3

// Panel that contains checkboxs to enable or disable layers.
GridLayout {
  id: layersVisualizationPanel
  columns: 3
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
    Layout.columnSpan: 3
    anchors.horizontalCenter: parent.horizontalCenter
    Layout.alignment: Qt.AlignVTop | Qt.AlignHCenter
    font.pointSize: 10
    text: "MESH LAYERS"
  }

  /**
  * Ashpalt checkbox
  */
  CheckBox {
    id: layerAsphalt
    text: qsTr("Asphalt")
    checked: true
    onClicked : {
      MaliputViewerPlugin.OnNewMeshLayerSelection("asphalt", checked);
    }
  }

  /**
  * Lane checkbox
  */
  CheckBox {
    id: layerLane
    text: qsTr("Lane")
    checked: true
    onClicked : {
      MaliputViewerPlugin.OnNewMeshLayerSelection("lane_all", checked);
    }
  }

  /**
  * Marker checkbox
  */
  CheckBox {
    id: layerMarker
    text: qsTr("Marker")
    checked: true
    onClicked : {
      MaliputViewerPlugin.OnNewMeshLayerSelection("marker_all", checked);
    }
  }

  /**
  * HBounds checkbox
  */
  CheckBox {
    id: layerHBounds
    text: qsTr("HBounds")
    checked: true
    onClicked : {
      MaliputViewerPlugin.OnNewMeshLayerSelection("h_bounds", checked);
    }
  }

  /**
  * BranchPoint checkbox
  */
  CheckBox {
    id: layerBranchPoint
    text: qsTr("Branch Point")
    checked: true
    onClicked : {
      MaliputViewerPlugin.OnNewMeshLayerSelection("branch_point_all", checked);
    }
  }

  /**
  * GrayedAsphalt checkbox
  */
  CheckBox {
    id: layerGrayedAsphalt
    text: qsTr("Grayed asphalt")
    checked: false
    onClicked : {
      MaliputViewerPlugin.OnNewMeshLayerSelection("grayed_asphalt", checked);
    }
  }

  /**
  * GrayedLane checkbox
  */
  CheckBox {
    id: layerGrayedLane
    text: qsTr("Grayed lane")
    checked: false
    onClicked : {
      MaliputViewerPlugin.OnNewMeshLayerSelection("grayed_lane_all", checked);
    }
  }

  /**
  * GrayedMarker checkbox
  */
  CheckBox {
    id: layerGrayedMarker
    text: qsTr("Grayed marker")
    checked: false
    onClicked : {
      MaliputViewerPlugin.OnNewMeshLayerSelection("grayed_marker_all", checked);
    }
  }

  /*
  * Update checkboxes' state.
  */
  Connections {
    target: MaliputViewerPlugin
    onLayerCheckboxesChanged: {
      layerAsphalt.checked = MaliputViewerPlugin.layerCheckboxes[0]
      layerLane.checked = MaliputViewerPlugin.layerCheckboxes[1]
      layerMarker.checked = MaliputViewerPlugin.layerCheckboxes[2]
      layerHBounds.checked = MaliputViewerPlugin.layerCheckboxes[3]
      layerBranchPoint.checked = MaliputViewerPlugin.layerCheckboxes[4]
      layerGrayedAsphalt.checked = MaliputViewerPlugin.layerCheckboxes[5]
      layerGrayedLane.checked = MaliputViewerPlugin.layerCheckboxes[6]
      layerGrayedMarker.checked = MaliputViewerPlugin.layerCheckboxes[7]
    }
  }
}
