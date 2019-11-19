import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3

ApplicationWindow {

    id: window

    visible: true

    width: 640
    height: 480

    Material.theme: Material.Light
    Material.accent: Material.Blue

    title: qsTr("MicroFlip")

    property variant paneWidthMultiplier: 0.95

    Column {
        id: column

        width: parent.width
        height: parent.height
        spacing: 20

        Pane {
            id: header
            height: 50
            width: parent.width
            Material.foreground: Material.Orange
//            color: Material.color(Material.Orange)
            Material.elevation: 7

            Row {

                spacing: 20

                Rectangle {
                    height: header.height
                    width: 22
                    color: Material.color(Material.Blue)
                }

                Label {
                    id: labelSymbol
                    text: exchangeInfo.symbol
                }

                Label {
                    id: labelLastPrice
                    text: qsTr("Last Price: ")
                }

                Label {
                    id: labelLastPriceValue
                    text: exchangeInfo.price
                }

                Label {
                    id: labelAvgPrice
                    text: qsTr("Avg. Price: ")
                }

                Label {
                    id: labelAvgPriceValue
                    text: exchangeInfo.avgPrice
                }
                Label {
                    id: labelRemoteConnectionState
                    text: remoteControl.remoteConnectionState
                }
            }
        }

        Pane {
            id: workOrderPane
            width: parent.width * paneWidthMultiplier
            height: window.height / 2
            anchors.horizontalCenter: parent.horizontalCenter
            Layout.alignment: Qt.AlignCenter

            Material.elevation: 6

            ListView {
                id: workOrderView
                model: workOrderModel
                width: workOrderPane.width
                height: workOrderPane.height

                delegate: Rectangle {
                    id: wordOrderRectangle
                    height: 25
                    width: workOrderView.width

                    color: workerMouseArea.containsMouse ? Material.color(Material.Blue) : Material.color(Material.DeepOrange)

                    MouseArea {
                        id: workerMouseArea
                        anchors {
                            left: wordOrderRectangle.left; top: wordOrderRectangle.top;
                            right: wordOrderRectangle.right; bottom: wordOrderRectangle.bottom
                        }
                        hoverEnabled: true
                    }

                    Row {
                        spacing: 10
                        Text {
                            text: Number(workId)
                            font.pixelSize: 15
                        }
                        Text {
                            text: workState
                            font.pixelSize: 15
                        }
                    }

                }
            }
        }

        Pane {
            id: serverLogPane
            width: parent.width * paneWidthMultiplier
            height: window.height / 2
            anchors.horizontalCenter: parent.horizontalCenter
            Layout.alignment: Qt.AlignCenter

            Material.elevation: 6

            ListView {
                id: logView
                width: 120
                height: 100

                model: logItemModel

                delegate: Rectangle {
                    width: 55
                    height: 25
                    color: "yellow"
                    Text { text: "[" + reportTime + "]" + "[ID " + workId + "]" + "[" + className + "] " + log
                    color: Material.color(Material.Blue)}
                }
            }
        }
    }

    RoundButton {

        height: 65
        width: 65
        z: 100

        x: parent.width - (75)
        y: parent.height - 75

        radius: width

        text: "\u002B" // Unicode Character 'Plus Sign'
        onClicked: textArea.readOnly = true
        highlighted: true
        Material.accent: Material.Orange
    }
}
