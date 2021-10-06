import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3

Rectangle {
  id: teleop
  width: panel.implicitWidth + 20
  height: panel.implicitHeight + 10
  color: "transparent"
  Layout.minimumWidth: 290
  Layout.minimumHeight: 200

  Keys.onPressed: {
    if (TeleopPlugin.isDriving) {
      if (event.key == Qt.Key_Left) {
        TeleopPlugin.UpdateSteeringAngle(1.0);
        TeleopPlugin.newSteeringAngle = true;
      } else if (event.key == Qt.Key_Right) {
        TeleopPlugin.UpdateSteeringAngle(-1.0);
        TeleopPlugin.newSteeringAngle = true;
      } else if (event.key == Qt.Key_Up) {
        TeleopPlugin.throttleKeyPressed = true;
        TeleopPlugin.keepCurrentThrottle = false;
      } else if (event.key == Qt.Key_Down) {
        TeleopPlugin.brakeKeyPressed = true;
        TeleopPlugin.keepCurrentBrake = false;
      }
    }
  }

  Keys.onReleased: {
    if (!event.isAutoRepeat) {
      if (event.key == Qt.Key_Space) {
        TeleopPlugin.keepCurrentThrottle = true;
        TeleopPlugin.keepCurrentBrake = true;
      } else if (event.key == Qt.Key_Up) {
        TeleopPlugin.throttleKeyPressed = false;
      } else if (event.key == Qt.Key_Down) {
        TeleopPlugin.brakeKeyPressed = false;
      } else if (event.key == Qt.Key_Left || event.key == Qt.Key_Right) {
        // do nothing
        // this avoids the accel/brake values of not going down
        // to zero after release when steering at the same time
      }
    }
  }

  Rectangle {
    id: panel
    state: "hide"
    default property alias data: grid.data
    anchors.bottom: teleop.bottom
    anchors.fill: parent
    implicitWidth: grid.implicitWidth + 10
    implicitHeight: grid.implicitHeight + 10
    // color: "#22000000"
    color: "transparent"

    GridLayout {
      id: grid
      columns: 2
      anchors.fill: parent
      anchors.margins: 5

      // Topic selection and start driving
      TextField {
        id: drivingTopic
        text: TeleopPlugin.carNumber
        selectByMouse: true
        enabled: !TeleopPlugin.isDriving
      }
      Button {
        id: startStopButton
        text: "Start Driving"
        font.capitalization: Font.MixedCase
        onClicked: {
          if (TeleopPlugin.isDriving) {
            startStopButton.text = "Start Driving"
            TeleopPlugin.isDriving = false
          } else {
            startStopButton.text = "Stop Driving"
            TeleopPlugin.isDriving = true
            // Hey! there are no bi-directional bindings.
            TeleopPlugin.carNumber = drivingTopic.text
            TeleopPlugin.OnStartDriving()
          }
          drivingTopic.enabled = !TeleopPlugin.isDriving
          teleop.focus = true
        }
      }
      // Steering angle
      Label {
        text: "Steering Angle:"
        visible: true
        font.weight: Font.DemiBold
        Layout.alignment: Qt.AlignLeft
      }
      Label {
        id: steeringAngleLabel
        text: TeleopPlugin.steeringAngleValue
        visible: true
        font.weight: Font.DemiBold
        Layout.alignment: Qt.AlignRight
      }
      // Throttle value
      Label {
        text: "Throttle Value:"
        visible: true
        font.weight: Font.DemiBold
        Layout.alignment: Qt.AlignLeft
      }
      Label {
        id: throttleValueLabel
        text: TeleopPlugin.throttleValue
        visible: true
        font.weight: Font.DemiBold
        Layout.alignment: Qt.AlignRight
      }
      // Brake value
      Label {
        text: "Brake Value:"
        visible: true
        font.weight: Font.DemiBold
        Layout.alignment: Qt.AlignLeft
      }
      Label {
        id: brakeValueLabel
        text: TeleopPlugin.brakeValue
        visible: true
        font.weight: Font.DemiBold
        Layout.alignment: Qt.AlignRight
      }
    }
  }
}
