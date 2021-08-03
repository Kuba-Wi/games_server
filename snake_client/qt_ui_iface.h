#pragma once

#include <QObject>

class qt_ui_iface : public QObject {
    Q_OBJECT
public:
    void refresh_client() {
        emit this->refreshClient();
    }

    void enable_sending() {
        emit this->enableSending();
    }

    void stop_sending() {
        emit this->stopSending();
    }

    void set_board_dimensions() {
        emit this->setBoardDimensions();
    }

    void wait_for_connection() {
        emit this->waitForConnection();
    }

    void establish_connection() {
        emit this->establishConnection();
    }

signals:
    void refreshClient();
    void enableSending();
    void stopSending();
    void setBoardDimensions();
    void waitForConnection();
    void establishConnection();
} inline qt_ui_if;
