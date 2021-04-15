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
}
