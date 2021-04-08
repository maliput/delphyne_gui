import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3

Rectangle {
  id: topicStats
  color: "transparent"
  anchors.fill: parent
  Layout.minimumWidth: 300
  Layout.minimumHeight: 250

  // Search bar.
  RowLayout {
      id: searchBar
      width: parent.width
      height: 50

      Image {
        id: searchImg
        source: "icons/search.svg"
        anchors.left: parent.left
        anchors.leftMargin: 5
      }

      TextField {
          id: searchText
          anchors.left: searchImg.right
          anchors.leftMargin: 5
          anchors.right: parent.right
          anchors.rightMargin: 5
          Layout.fillWidth: true

          placeholderText: qsTr("Topic to be searched...")
          onTextChanged: {
            TopicsStats.SearchTopic(text)
          }
      }
  }

  // Table of stats.
  TableView {
    id: tableView
    objectName: "tableView"
    anchors.top: searchBar.bottom
    anchors.left: parent.left
    width: parent.width
    height: parent.height
    TableViewColumn {
        role: "topic"
        title: "Topic"
        width: tableView.width/4
    }
    TableViewColumn {
        role: "messages"
        title: "Messages"
        width: tableView.width/4
    }
    TableViewColumn {
        role: "frequency"
        title: "Frequency [Hz]"
        width: tableView.width/4
    }
    TableViewColumn {
        role: "bandwidth"
        title: "Bandwidth [B/s]"
        width: tableView.width/4
    }
    model: ListModel {
      id: tableModel
      // This list will be updated dynamically when
      // there is new data to show
    }
  }

  // When new data arrives the table is updated.
  Connections {
      target: TopicsStats
      onDataChanged: {
        tableModel.clear()
        for (var i = 0; i < TopicsStats.data.length; i=i+4)  {
          tableModel.append({"topic": TopicsStats.data[i], "messages": TopicsStats.data[i+1], "frequency" : TopicsStats.data[i+2] , "bandwidth" : TopicsStats.data[i+3]})
        }
      }
  }
}
