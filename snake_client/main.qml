import QtQuick 2.12
import QtQuick.Controls 2.12
import SnakeModel 0.1

ApplicationWindow {
    property var buttonSize: 60
    height: 420
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

    function set_wait_circle_state(bool){
        wait_circle.visible = bool;
        wait_circle.running = bool;
    }

    TextField {
        id: text_ip
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 100
        width: parent.width - 50
        text: "Server ip address"
    }

    Button {
        id: connect_button
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: text_ip.bottom
        anchors.topMargin: 20
        width: 100
        text: "Connect"
        
        onClicked: {
            snake_model.set_ip(text_ip.text)
        }
    }

    Dialog {
        id: wrong_ip_dialog
        width: parent.width
        title: "Invalid ip, try again"
        standardButtons: Dialog.Ok
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

            onIpSet: {
                connect_button.visible = false;
                text_ip.visible = false;
                set_wait_circle_state(true);
                label_connection.text = "Connecting...";
                label_connection.visible = true;
            }

            onIpNotSet: {
                wrong_ip_dialog.open();
            }

            onSendingEnabled: {
                set_buttons_visibility(true);
                label_connection.visible = false;
                set_wait_circle_state(false);
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
                set_wait_circle_state(true);
            }

            onConnectionEstablished: {
                set_buttons_visibility(false);
                label_connection.text = "Connected, wait for your turn..."
                label_connection.visible = true;
                set_wait_circle_state(false);
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
        visible: false
        text: "Connecting..."
    }

    BusyIndicator {
        id: wait_circle
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: label_connection.bottom
        anchors.topMargin: 20
        visible: false
        running: true
    }

    Label {
        id: label_score
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 100
        visible: false
    }
}
