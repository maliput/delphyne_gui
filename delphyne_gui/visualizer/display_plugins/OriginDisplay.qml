import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Rectangle {
  id: originDisplay
  color: "transparent"
  Layout.minimumWidth: 290
  Layout.minimumHeight: 110
  Layout.fillWidth: true

  // Checkbox to toggle axes visibility.
  RowLayout {
    CheckBox {
      id: visibilityCheckbox
      text: qsTr("Visible")
      checked: OriginDisplay.isVisible
      onClicked : {
        visibilityCheckbox.checked = !OriginDisplay.isVisible;
        OriginDisplay.isVisible = !OriginDisplay.isVisible;
      }
    }
  }
}
