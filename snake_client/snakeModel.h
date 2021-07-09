#pragma once

#include <qqml.h>
#include <QAbstractTableModel>
#include <QColor>

#include <algorithm>
#include <memory>
#include <thread>

#include "client_connection.h"

class SnakeModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    SnakeModel() {
        _connection = std::make_unique<client_connection>();
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        if (parent.isValid())
            return 0;

        return 10;
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const override
    {
        if (parent.isValid())
            return 0;

        return 12;
    }

    QVariant data(const QModelIndex &index, [[maybe_unused]] int role) const override
    {
        if (_connection->check_index_present(index.row(), index.column())) {
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
        _connection->send_data(data);
    }

    void refresh() {
        this->beginResetModel();
        _connection->refresh_client();
        this->endResetModel();
    }

signals:
    void gameFinished();

private:
    std::unique_ptr<client_connection> _connection;
};
