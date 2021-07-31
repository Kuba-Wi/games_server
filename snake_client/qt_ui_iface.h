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

    void set_board_dimensions() {
        emit this->setBoardDimensions();
    }

signals:
    void refreshClient();
    void enableSending();
    void setBoardDimensions();
} inline qt_ui_if;
