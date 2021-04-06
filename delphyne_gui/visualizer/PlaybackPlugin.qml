// Copyright 2021 Toyota Research Institute

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3

Rectangle {
  id: playbackWidget
  Layout.minimumWidth: 460
  Layout.minimumHeight: 175

  property var isPlaying: true

  /**
   * Play icon
   */
  property string playIcon: "\u25B6"

    /**
   * Play icon
   */
  property string rewindIcon: "\u25C4"

  /**
   * Pause icon
   */
  property string pauseIcon: "\u275A\u275A"

  /**
   * Step icon
   */
  property string stepIcon: "\u25B8\u25B8"

  /**
   * Rewind
   */
  RoundButton {
    id: rewindButton
    x: 10
    y: 10
    visible: showPlay
    text: rewindIcon
    checkable: true
    height: playButton.height * 0.8
    width: playButton.width * 0.8
    Layout.minimumWidth: width
    Layout.leftMargin: 10
    onClicked: { PlaybackPlugin.OnRewindButtonPush(); }
    Material.background: Material.primary
  }

  /**
   * Pause/Play
   */
  RoundButton {
    id: playButton
    visible: showPlay
    text: isPlaying ? pauseIcon : playIcon
    checkable: true
    width: 60
    height: 60
    Layout.minimumWidth: width
    Layout.leftMargin: 10
    anchors.left : rewindButton.right
    anchors.leftMargin : 10
    anchors.verticalCenter : rewindButton.verticalCenter
    onClicked: {
          if (isPlaying){
            PlaybackPlugin.OnPauseButtonPush();
            isPlaying=false;
          } else {
            PlaybackPlugin.OnPlayButtonPush();
            isPlaying=true;
          }
    }
    Material.background: Material.primary
  }

  /**
   * Step
   */
  RoundButton {
    id: stepButton
    visible: showPlay
    text: stepIcon
    checkable: true
    height: playButton.height * 0.8
    width: playButton.width * 0.8
    Layout.minimumWidth: width
    Layout.leftMargin: 10
    enabled: isPlaying ? false : true
    anchors.left : playButton.right
    anchors.leftMargin : 10
    anchors.verticalCenter : playButton.verticalCenter
    onClicked: { PlaybackPlugin.OnStepButtonPush(spinBox.value); }
    Material.background: Material.primary
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
