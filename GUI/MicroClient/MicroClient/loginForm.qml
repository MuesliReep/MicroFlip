import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

Item {
    width: 505

    RowLayout {
        id: rowLayout
        x: 0
        y: 0
        width: parent.width
        height: parent.height

        Text {
            id: element
            text: qsTr("Text")
            font.pixelSize: 12
        }

        TextField {
            id: textField
            text: qsTr("Text Field")
        }

        Label {
            id: label
            text: qsTr("Label")
        }

        TextField {
            id: textField1
            text: qsTr("Text Field")
        }

    }

}
