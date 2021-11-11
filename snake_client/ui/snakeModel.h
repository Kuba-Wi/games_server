#pragma once

#include <qqml.h>
#include <QAbstractTableModel>
#include <QColor>
#include <QString>

#include <memory>
#include <string>

#include "snake_client.h"
#include "qt_ui_iface.h"

class SnakeModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    SnakeModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, [[maybe_unused]] int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool setData([[maybe_unused]] const QModelIndex &index,
                 [[maybe_unused]] const QVariant &value,
                 [[maybe_unused]] int role) override;

public slots:
    void send_data(int data) { _snake_client->send_data(data); }
    void refresh();
    void connect_to_ip(const QString& ip);

    int get_board_height() { return _snake_client->get_board_height(); }
    int get_board_width() { return _snake_client->get_board_width(); }

signals:
    void ipSet();
    void ipNotSet();
    void sendingEnabled();
    void sendingStopped();
    void connectionEstablished();
    void boardDimensionsSet();
    void connectionError(QString message);

private:
    std::unique_ptr<snake_client> _snake_client;
};
