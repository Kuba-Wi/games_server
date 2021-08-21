#pragma once

#include <QObject>

class qt_ui_iface : public QObject {
    Q_OBJECT
public:
    void refresh_model() {
        emit this->refreshModel();
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

    void connection_established() {
        emit this->connectionEstablished();
    }

    void debug() {
        emit this->debu();
    }

signals:
    void refreshModel();
    void enableSending();
    void stopSending();
    void setBoardDimensions();
    void waitForConnection();
    void connectionEstablished();
    void debu();
} inline qt_ui_if;
