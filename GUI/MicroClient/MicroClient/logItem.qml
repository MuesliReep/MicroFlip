import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3

Item {

    Label {
        id: timeValue
        text: qsTr("[TIME]")
    }
    Label {
        id: classNameValue
        text: qsTr("[CLASS]")
    }
    Label {
        id: messageValue
        text: qsTr("[MESSAGE]")
    }
}
