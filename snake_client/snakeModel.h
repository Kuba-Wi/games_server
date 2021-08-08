#pragma once

#include <qqml.h>
#include <QAbstractTableModel>
#include <QColor>
#include <QString>

#include <algorithm>
#include <memory>
#include <string>
#include <thread>

#include "snake_client.h"
#include "qt_ui_iface.h"

class SnakeModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    SnakeModel() :_snake_client(std::make_unique<snake_client>(std::make_unique<network>())) {
        connect(&qt_ui_if, &qt_ui_iface::refreshClient, this, &SnakeModel::refresh);
        connect(&qt_ui_if, &qt_ui_iface::enableSending, this, [&](){
            emit this->sendingEnabled();
        });
        connect(&qt_ui_if, &qt_ui_iface::stopSending, this, [&](){
            emit this->sendingStopped();
        });
        connect(&qt_ui_if, &qt_ui_iface::waitForConnection, this, [&](){
            emit this->waitForConnection();
        });
        connect(&qt_ui_if, &qt_ui_iface::connectionEstablished, this, [&](){
            emit this->connectionEstablished();
        });
        connect(&qt_ui_if, &qt_ui_iface::setBoardDimensions, this, [&](){
            emit this->boardDimensionsSet();
        });
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        if (parent.isValid())
            return 0;

        return _snake_client->get_board_height();
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const override
    {
        if (parent.isValid())
            return 0;

        return _snake_client->get_board_width();
    }

    QVariant data(const QModelIndex &index, [[maybe_unused]] int role) const override
    {
        if (_snake_client->check_index_present(index.row(), index.column())) {
            return QColor{"yellow"};
        }
        return QColor{"black"};
    }

    QHash<int, QByteArray> roleNames() const override
    {
        return {{Qt::DisplayRole, "display"}};
    }

    bool
    setData([[maybe_unused]] const QModelIndex &index,
            [[maybe_unused]] const QVariant &value,
            [[maybe_unused]] int role) override
    {
        // Currently not needed
        return false;
    }

    Qt::ItemFlags
    flags(const QModelIndex &index) const override
    {
        if (!index.isValid())
            return Qt::NoItemFlags;

        return Qt::ItemIsEditable;
    }

public slots:
    void send_data(int data) {
        _snake_client->send_data(data);
    }

    void refresh() {
        this->beginResetModel();
        this->endResetModel();
    }

    int get_board_height() {
        return _snake_client->get_board_height();
    }

    int get_board_width() {
        return _snake_client->get_board_width();
    }

    bool set_ip(const QString& ip) {
        return _snake_client->set_server_address(ip.toStdString());
    }

    void connect_network() {
        _snake_client->connect_network();
    }

signals:
    void gameFinished();
    void sendingEnabled();
    void sendingStopped();
    void waitForConnection();
    void connectionEstablished();
    void boardDimensionsSet();

private:
    std::unique_ptr<snake_client> _snake_client;
};
