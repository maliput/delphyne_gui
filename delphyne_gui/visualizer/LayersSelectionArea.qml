// Copyright 2021 Toyota Research Institute

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
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
    font.pixelSize: 14
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
    style: CheckBoxStyle {
      label: Text {
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "Helvetica"
        font.pixelSize: 12
        color: "black"
        text: layerAsphalt.text
      }
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
    style: CheckBoxStyle {
      label: Text {
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "Helvetica"
        font.pixelSize: 12
        color: "black"
        text: layerLane.text
      }
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
    style: CheckBoxStyle {
      label: Text {
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "Helvetica"
        font.pixelSize: 12
        color: "black"
        text: layerMarker.text
      }
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
    style: CheckBoxStyle {
      label: Text {
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "Helvetica"
        font.pixelSize: 12
        color: "black"
        text: layerHBounds.text
      }
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
    style: CheckBoxStyle {
      label: Text {
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "Helvetica"
        font.pixelSize: 12
        color: "black"
        text: layerBranchPoint.text
      }
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
    style: CheckBoxStyle {
      label: Text {
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "Helvetica"
        font.pixelSize: 12
        color: "black"
        text: layerGrayedAsphalt.text
      }
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
    style: CheckBoxStyle {
      label: Text {
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "Helvetica"
        font.pixelSize: 12
        color: "black"
        text: layerGrayedLane.text
      }
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
    style: CheckBoxStyle {
      label: Text {
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "Helvetica"
        font.pixelSize: 12
        color: "black"
        text: layerGrayedMarker.text
      }
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
