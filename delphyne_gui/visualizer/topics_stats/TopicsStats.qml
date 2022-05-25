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
        anchors.leftMargin: 5
      }

      TextField {
          id: searchText
          anchors.leftMargin: 5
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
    anchors.top: searchBar.bottom
    anchors.left: parent.left
    width: parent.width
    height: parent.height
    TableViewColumn {
        role: "topic"
        title: "Topic"
    }
    TableViewColumn {
        role: "messages"
        title: "Messages"
    }
    TableViewColumn {
        role: "frequency"
        title: "Frequency"
    }
    TableViewColumn {
        role: "bandwidth"
        title: "Bandwidth"
    }
    model: ListModel {
      id: tableModel
      // This list will be updated dynamically when
      // new data is presented.
    }

    itemDelegate: Item {
      Text {
          anchors.verticalCenter: parent.verticalCenter
          color: styleData.textColor
          elide: styleData.elideMode
          font.family: "Helvetica"
          font.pixelSize: 12
          text: styleData.value
      }
    }
  }

  // When new data arrives the table is updated.
  Connections {
      target: TopicsStats
      onDisplayedTopicDataChanged: {
        tableModel.clear()
        for (var i = 0; i < TopicsStats.displayedTopicData.length; i = i + 4)  {
          tableModel.append({"topic": TopicsStats.displayedTopicData[i],
                             "messages": TopicsStats.displayedTopicData[i+1],
                             "frequency" : TopicsStats.displayedTopicData[i+2],
                             "bandwidth" : TopicsStats.displayedTopicData[i+3]})
        }
      }
  }
}
