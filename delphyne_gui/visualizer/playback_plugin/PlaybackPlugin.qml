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
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3

Rectangle {
  id: playbackWidget
  Layout.minimumWidth: 460
  Layout.minimumHeight: 175

  /**
   * True when is playing.
   */
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
    text: rewindIcon
    checkable: true
    height: playButton.height * 0.8
    width: playButton.width * 0.8
    Layout.minimumWidth: width
    Layout.leftMargin: 10
    onClicked: {
      PlaybackPlugin.OnRewindButtonPush();
    }
    Material.background: Material.primary
  }

  /**
   * Pause/Play
   */
  RoundButton {
    id: playButton
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
    onClicked: {
      PlaybackPlugin.OnStepButtonPush(spinBox.value);
    }
    Material.background: Material.primary
  }

  /**
   * SpinBox
   */
  SpinBox {
    id: spinBox
    value: 50
    stepSize: 1
    anchors.left : stepButton.right
    anchors.verticalCenter : stepButton.verticalCenter
  }

  /**
   * ms text
   */
  Text {
    anchors.left : spinBox.right
    anchors.verticalCenter : spinBox.verticalCenter
    font.pointSize: 11
    text: "ms"
    styleColor: "gray"
  }

  /**
   * Current time text
   */
  Text {
    id: currentTimeText
    anchors.left : rewindButton.left
    anchors.top : rewindButton.bottom
    anchors.topMargin : 20
    font.pointSize: 10; text: PlaybackPlugin.currentTime ; styleColor: "gray"
  }

  /**
   * Timeline slider
   */
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

  /**
   * Current/End time text.
   */
  Text {
    id: currentEndTimeText
    anchors.left : slider.right
    anchors.verticalCenter : slider.verticalCenter
    font.pointSize: 10; text: PlaybackPlugin.simTime ; styleColor: "gray"
  }
}
