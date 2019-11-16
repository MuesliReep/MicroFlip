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

    //property alias exchangeNameValue: labelExchangeName.text

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
//                text: qsTr("Exchange Name")
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

}

        }

        Pane {
            id: workerPane
            width: parent.width * paneWidthMultiplier
            height: window.height / 2
            anchors.horizontalCenter: parent.horizontalCenter
            Layout.alignment: Qt.AlignCenter

            Material.elevation: 6

            ListModel {
                id: workerModel

                ListElement {
                    workID: 11
                    workerStatus: "TEST"
                }
                ListElement {
                    workID: 12
                    workerStatus: "TEST2"
                }
                ListElement {
                    workID: 13
                    workerStatus: "TEST3"
                }
            }

            Component {
                id: workerDelegate

                Item {
                    id: workerDelegateItem
                    width: workerView.width; height: 50

                    Rectangle {

//                        anchors {
//                            left: parent.left; top: parent.top;
//                            right: parent.right; bottom: parent.bottom
//                        }

                        height: parent.height
                        width: parent.width

                        color: workerMouseArea.containsMouse ? Material.color(Material.Blue) : Material.color(Material.DeepOrange)

                        MouseArea {
                            id: workerMouseArea
                            anchors {
                                left: parent.left; top: parent.top;
                                right: parent.right; bottom: parent.bottom
                            }
                            hoverEnabled: true
                        }

                        Row {
                            spacing: 10
                            Text {
                                text: Number(workID)
                                font.pixelSize: 15
                            }
                            Text {
                                text: workerStatus
                                font.pixelSize: 15
                            }
                        }
                    }
                }
            }

            ListView {
                id: workerView
                model: workerModel
                delegate: workerDelegate
                anchors {
                    left: parent.left; top: parent.top;
                    right: parent.right; bottom: parent.bottom
                    margins: 5
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

//            Component {
//                id: logDelegate

//                Item {
//                    id: logDelegateItem
//                    width: logView.width; height: 25
//                    Text: log
//                }
//            }

            ListView {
                id: logView
                width: 120
                height: 100

                model: logItemModel
//                delegate: logDelegate

                delegate: Rectangle {
                    width: 55
                    height: 25
                    color: "yellow"
                    Text { text: log
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
