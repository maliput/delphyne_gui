// Copyright 2021 Toyota Research Institute

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3

Rectangle {
  property var isPlaying: true

  id: playbackWidget

  Layout.minimumWidth: 460
  Layout.minimumHeight: 175

  Button {
    id: rewindButton
    x: 10
    y: 10
    Image {
      anchors.centerIn: rewindButton
      sourceSize.height: rewindButton.background.height - 6
      height: sourceSize.height
      source: "icons/rewind.svg"
    }
    background: Rectangle {
        implicitWidth: 40
        implicitHeight: 40
        opacity: enabled ? 1 : 0.3
        border.width: 2
        radius: 5
        color: rewindButton.pressed ? "grey" : "transparent"
    }
    onClicked: { PlaybackPlugin.OnRewindButtonPush(); }
  }

  Button {
    id: pauseButton
    Image {
      anchors.centerIn: pauseButton
      sourceSize.height: pauseButton.background.height - 6
      height: sourceSize.height
      source: "icons/pause.svg"
    }
    background: Rectangle {
        implicitWidth: 40
        implicitHeight: 40
        opacity: enabled ? 1 : 0.3
        border.width: 2
        radius: 5
        color: isPlaying ? "transparent" : "grey"
    }
    anchors.left : rewindButton.right
    anchors.leftMargin : 10
    anchors.verticalCenter : rewindButton.verticalCenter
    onClicked: { PlaybackPlugin.OnPauseButtonPush();
                isPlaying=false; }
  }
  Button {
    id: playButton
    Image {
      anchors.centerIn: playButton
      sourceSize.height: playButton.background.height - 6
      height: sourceSize.height
      source: "icons/play.svg"
    }
    background: Rectangle {
        implicitWidth: 40
        implicitHeight: 40
        opacity: enabled ? 1 : 0.3
        border.width: 2
        radius: 5
        color: !isPlaying ? "transparent" : "grey"
    }
    anchors.left : pauseButton.right
    anchors.leftMargin : 10
    anchors.verticalCenter : pauseButton.verticalCenter
    onClicked: { PlaybackPlugin.OnPlayButtonPush(); isPlaying=true;}
  }
  Button {
    id: stepButton
    Image {
      anchors.centerIn: stepButton
      sourceSize.height: stepButton.background.height - 6
      height: sourceSize.height
      source: "icons/step.svg"
    }
    background: Rectangle {
        implicitWidth: 40
        implicitHeight: 40
        opacity: enabled ? 1 : 0.3
        border.width: 2
        radius: 5
        color: !isPlaying ? stepButton.pressed ? "grey" : "transparent" : "grey"
    }
    enabled: isPlaying ? false : true
    anchors.left : playButton.right
    anchors.leftMargin : 10
    anchors.verticalCenter : playButton.verticalCenter
    onClicked: { PlaybackPlugin.OnStepButtonPush(spinBox.value); }
  }

  SpinBox {
    id: spinBox
    value: 50
    stepSize: 1
    anchors.left : stepButton.right
    anchors.verticalCenter : stepButton.verticalCenter
  }

  Text {
    anchors.left : spinBox.right
    anchors.verticalCenter : spinBox.verticalCenter
    font.pointSize: 11
    text: "ms"
    styleColor: "gray"
  }

  Text {
    id: currentTimeText
    anchors.left : rewindButton.left
    anchors.top : rewindButton.bottom
    anchors.topMargin : 20
    font.pointSize: 10; text: PlaybackPlugin.currentTime ; styleColor: "gray"
  }

  Slider {
    id: slider
    height: 50
    width: 250
    anchors.verticalCenter : currentTimeText.verticalCenter
    anchors.left : currentTimeText.right
    anchors.leftMargin: 20
    handle: Rectangle {
        x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
        y: slider.topPadding + slider.availableHeight / 2 - height / 2
        implicitWidth: 10
        implicitHeight: 25
        color: slider.pressed ? "#f0f0f0" : "#f6f6f6"
        border.color: "black"
    }

    from: 0
    value: !pressed ? PlaybackPlugin.sliderValue : PlaybackPlugin.OnSliderDrop(slider.value)
    to: 100
    stepSize: 0.01

  }
  Text {
    id: currentEndTimeText
    anchors.left : slider.right
    anchors.verticalCenter : slider.verticalCenter
    font.pointSize: 10; text: PlaybackPlugin.simTime ; styleColor: "gray"
  }
}
