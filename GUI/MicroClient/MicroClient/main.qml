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

        Rectangle {
            id: header
            height: 50
            width: parent.width
            Material.foreground: Material.Pink
            color: Material.color(Material.Orange)

            Row {
                id: headerRow
                height: parent.height
                width: parent.width
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: column.horizontalCenter

                Item {
                    id: name

                    Rectangle {

                        id: exchangeNameRectangle
                        height: parent.height
                        width: exchangeName.width + 15

                        color: Material.color(Material.DeepOrange)

                        Label {
                            id: exchangeName
                            text: qsTr("Exchange Name")
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            color: "white"
                        }
                    }
                }

                Item {

                    Rectangle {

                        id: lastPriceRectangle
                        height: parent.height
                        width: (parent.width - exchangeNameRectangle) / 2
    //                    anchors.left: exchangeNameRectangle.anchors.right

                        Label {
                            id: labelLastPrice
                            text: qsTr("Last Price: ")
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Label {
                            id: labelLastPriceValue
                            text: qsTr("0.00")
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                }



//                Rectangle {

//                    id: avgPriceRectangle
//                    height: parent.height
//                    width: (parent.width - exchangeNameRectangle) / 2
//                    anchors.left: lastPriceRectangle.anchors.right

//                    Label {
//                        id: labelAvgPrice
//                        text: qsTr("Avg. Price: ")
//                        anchors.verticalCenter: parent.verticalCenter
//                    }

//                    Label {
//                        id: labelAvgPriceValue
//                        text: qsTr("0.00")
//                        anchors.verticalCenter: parent.verticalCenter
//                    }
//                }
            }
        }

        Pane {
            id: workerPane
            width: parent.width * paneWidthMultiplier
            height: 150
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
            height: 50
            anchors.horizontalCenter: parent.horizontalCenter
            Layout.alignment: Qt.AlignCenter

            Material.elevation: 6

            ListModel {
                id: logModel
            }

            Component {
                id: logDelegate

                Item {
                    id: logDelegateItem
                    width: logView.width; height: 80
                }
            }

            ListView {
                id: logView
                model: logModel
                delegate: logDelegate
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
