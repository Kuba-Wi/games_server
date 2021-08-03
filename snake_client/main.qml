import QtQuick 2.12
import QtQuick.Controls 2.12
import SnakeModel 0.1

ApplicationWindow {
    property var buttonSize: 50
    height: 400
    width: 220

    visible: true
    title: qsTr("Snake")

    function set_buttons_visibility(bool){
        up_button.enabled = bool;
        down_button.enabled = bool;
        right_button.enabled = bool;
        left_button.enabled = bool;
        table_view.visible = bool;
    }

    TableView {

        id: table_view
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        width: parent.width
        height: parent.width
        columnSpacing: 1
        rowSpacing: 1
        clip: true
        visible: false

        model: SnakeModel {
            id: snake_model
            property var boardHeight: 0
            property var boardWidth: 0
            onGameFinished: {
                table_view.visible = false;
                label_score.visible = true;
                label_score.text = "Score: "
            }

            onSendingEnabled: {
                set_buttons_visibility(true);
                label_connection.visible = false;
            }

            onSendingStopped: {
                set_buttons_visibility(false);
                label_connection.text = "Connected, wait for your turn..."
                label_connection.visible = true;
            }

            onWaitForConnection: {
                set_buttons_visibility(false);
                label_connection.text = "Connecting...";
                label_connection.visible = true;
            }

            onEstablishConnection: {
                set_buttons_visibility(false);
                label_connection.text = "Connected, wait for your turn..."
                label_connection.visible = true;
            }

            onBoardDimensionsSet: {
                boardHeight = snake_model.get_board_height();
                boardWidth = snake_model.get_board_width();
            }
        }

        delegate: Rectangle {
            implicitWidth: (table_view.width - table_view.columnSpacing * (snake_model.boardWidth - 1)) / snake_model.boardWidth
            implicitHeight: (table_view.height - table_view.rowSpacing * (snake_model.boardHeight - 1)) / snake_model.boardHeight
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
        enabled: false
        onClicked: {
            snake_model.send_data(Enum.Direction.Up);
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
        enabled: false
        onClicked: {
            snake_model.send_data(Enum.Direction.Down);
        }
        text: "down"
    }

    Button {
        id: left_button
        anchors.right: up_button.left
        anchors.top: up_button.bottom
        width: buttonSize
        height: buttonSize
        enabled: false
        onClicked: {
            snake_model.send_data(Enum.Direction.Left);
        }
        text: "left"
    }

    Button {
        id: right_button
        anchors.left: up_button.right
        anchors.top: up_button.bottom
        width: buttonSize
        height: buttonSize
        enabled: false
        onClicked: {
            snake_model.send_data(Enum.Direction.Right);
        }
        text: "right"
    }

    Label {
        id: label_connection
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 100
        visible: true
        text: "Connecting..."
    }

    Label {
        id: label_score
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 100
        visible: false
    }
}
