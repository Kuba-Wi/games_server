import QtQuick 2.12
import QtQuick.Controls 2.12
import SnakeModel 0.1

ApplicationWindow {
    property var currentDirection: Enum.Direction.Up
    property var buttonSize: 50

    visible: true
    title: qsTr("Snake")

    TableView {
        id: table_view
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        width: parent.width
        height: parent.width
        columnSpacing: 1
        rowSpacing: 1
        clip: true

        model: SnakeModel {
            id: snake_model
            onGameFinished: {
                timer.running = false;
                table_view.visible = false;
                label_score.visible = true;
                label_score.text = "Score: "
            }
        }

        delegate: Rectangle {
            implicitWidth: (table_view.width - table_view.columnSpacing * 11) / 12
            implicitHeight: (table_view.height - table_view.rowSpacing * 9) / 10
            color: display
        }
    }

    Button {
        id: up_button
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: down_button.top
        anchors.bottomMargin: buttonSize
        width: buttonSize
        height: buttonSize
        onClicked: {
            snake_model.send_data(currentDirection);
            if (currentDirection != Enum.Direction.Down)
                currentDirection = Enum.Direction.Up
        }
        text: "up"
    }

    Button {
        id: down_button
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        width: buttonSize
        height: buttonSize
        onClicked: {
            if (currentDirection != Enum.Direction.Up)
                currentDirection = Enum.Direction.Down
        }
        text: "down"
    }

    Button {
        anchors.right: up_button.left
        anchors.top: up_button.bottom
        width: buttonSize
        height: buttonSize
        onClicked: {
            if (currentDirection != Enum.Direction.Right)
                currentDirection = Enum.Direction.Left
        }
        text: "left"
    }

    Button {
        anchors.left: up_button.right
        anchors.top: up_button.bottom
        width: buttonSize
        height: buttonSize
        onClicked: {
            if (currentDirection != Enum.Direction.Left)
                currentDirection = Enum.Direction.Right
        }
        text: "right"
    }

    Timer {
        id: timer
        interval: 1
        running: true
        repeat: true
        onTriggered: {
            snake_model.send_data(currentDirection);
            snake_model.refresh();
        }
    }

    Label {
        id: label_score
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 100
        visible: false
    }
}
