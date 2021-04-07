import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3

Rectangle {
  ListModel {
    id: tableModel
    ListElement {
        topic: "/topic/example1"
        messages: "245"
        frequency: "15"
        bandwidth: "20.15"
    }
    ListElement {
        topic: "/topic/example2"
        messages: "245"
        frequency: "15"
        bandwidth: "20.15"
    }
    ListElement {
        topic: "/topic/example3"
        messages: "245"
        frequency: "15"
        bandwidth: "20.15"
    }
  }

  id: topicStats
  color: "transparent"
  anchors.fill: parent
  Layout.minimumWidth: 300
  Layout.minimumHeight: 250
  TableView {
    width: parent.width
    height: parent.height
    TableViewColumn {
        role: "topic"
        title: "Topic"
        width: parent.width/4
    }
    TableViewColumn {
        role: "messages"
        title: "Messages"
        width: parent.width/4
    }
    TableViewColumn {
        role: "frequency"
        title: "Frequency [Hz]"
        width: parent.width/4
    }
    TableViewColumn {
        role: "bandwidth"
        title: "Bandwidth [B/s]"
        width: parent.width/4
    }
    model: tableModel
  }
}
