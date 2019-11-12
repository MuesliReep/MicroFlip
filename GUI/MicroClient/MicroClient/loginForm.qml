import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.0
import QtQuick.Templates 2.5



Item {
    width: 505

    RowLayout {
        id: rowLayout
        x: 0
        y: 0
        width: parent.width
        height: 25

        Label {
            id: labelExchangeName
            text: qsTr("Exchange Name")
        }

        Label {
            id: labelLastPrice
            text: qsTr("Last Price:")
        }

        Label {
            id: labelLastPriceValue
            text: qsTr("0.00")
        }

        Label {
            id: labelAvgPrice
            text: qsTr("Avg. Price:")
        }

        Label {
            id: labelAvgPriceValue
            text: qsTr("0.00")
        }

    }

}
