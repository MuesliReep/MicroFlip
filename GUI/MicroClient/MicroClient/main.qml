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
//                Layout.fillWidth: true
//                Layout.fillHeight: true

                delegate: Rectangle {
                    id: workOrderRectangle
                    height: 25
                    width: workOrderView.width

                    color: workerMouseArea.containsMouse ? Material.color(Material.Blue) : Material.color(Material.DeepOrange)

                    MouseArea {
                        id: workerMouseArea
                        anchors {
                            left: workOrderRectangle.left; top: workOrderRectangle.top;
                            right: workOrderRectangle.right; bottom: workOrderRectangle.bottom
                        }
                        hoverEnabled: true
                        onClicked: {

                            workOrderRectangle.height = 50
                            buttonRemoveWorkOrder.visible = true
                        }

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
                        Rectangle {
//                            Layout.fillWidth: true
                            width: 55
                        }

                        Button {
                            id: buttonRemoveWorkOrder
                            text: "Remove"
                            visible: false
                            onClicked: remoteControl.onRemoveWorker(workId, false);
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
        onClicked: popup.open()
        highlighted: true
        Material.accent: Material.Orange
    }

    Popup {
        id: popup
        width: 400
        height: 300
        modal: true
        focus: true
        anchors.centerIn: Overlay.overlay
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        Material.elevation: 8

        spacing: 0

        ColumnLayout {
            anchors.fill: parent

            Item {
                id: popupHeader

                Rectangle {

                    Text {
                        id: name
                        text: qsTr("Create Workorder")
                    }
                }
            }

            Item {
                id: popupContent
            }

            Item {
                id: popupFooter

                height: 50

            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "orange"

                ScrollView {
                    anchors.fill: parent

                    Column {
                        SpinBox {
                            id: spinBoxNumWorkers
                            editable: true
                            from: 1
                            value: 1
                        }

                        TextField {
                            id: textFieldPair
                            text: qsTr("ETHUSDT")
                        }

                        TextField {
                            id: textFieldAmount
                            text: "0.1"
                        }

                        TextField {
                            id: textFieldProfitTarget
                            text: "0.00001"
                        }

                        TextField {
                            id: textFieldMinSellPrice
                            text: "0.0"
                        }

                        CheckBox {
                            id: checkBoxSingleShot
                            text: "Single Shot"
                            checked: true
                        }

                        SpinBox {
                            id: spinBoxShortInterval
                            editable: true
                            from: 0
                            stepSize: 1
                            value: 5
                        }
                        SpinBox {
                            id: spinBoxLongInterval
                            editable: true
                            from: 0
                            stepSize: 1
                            value: 10
                        }

                        ComboBox {
                            id: comboBoxMode
                            currentIndex: 0

                            model: ListModel {
                                ListElement { text: "MINSELL"   }
                                ListElement { text: "TICKERAVG" }
                            }
                        }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                height: 50
                color: "blue"

                Row {
                    Button {
                        text: "Cancel"
                        onClicked: popup.close()
                    }
                    Button {

                        text: "Create"
                        onClicked: {
                            remoteControl.onAddNewWorker(spinBoxNumWorkers.value,
                                                         textFieldPair.text,
                                                         textFieldAmount.text,
                                                         textFieldProfitTarget.text,
                                                         spinBoxShortInterval.value,
                                                         spinBoxLongInterval.value,
                                                         comboBoxMode.currentText,
                                                         checkBoxSingleShot.checked,
                                                         textFieldMinSellPrice.text)
                            popup.close()
                        }
                    }
                }
            }
        }

    }

} // ApplicationWindow
