#include "snakeModel.h"

SnakeModel::SnakeModel() :_snake_client(std::make_unique<snake_client>(std::make_unique<network>())) {
    connect(&qt_ui_if, &qt_ui_iface::refreshModel, this, &SnakeModel::refresh);
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

int SnakeModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return _snake_client->get_board_height();
}

int SnakeModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return _snake_client->get_board_width();
}

QVariant SnakeModel::data(const QModelIndex &index, [[maybe_unused]] int role) const {
    if (_snake_client->check_index_present(index.row(), index.column())) {
        return QColor{"yellow"};
    }
    return QColor{"black"};
}

QHash<int, QByteArray> SnakeModel::roleNames() const {
    return {{Qt::DisplayRole, "display"}};
}

Qt::ItemFlags SnakeModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsEditable;
}

bool SnakeModel::setData([[maybe_unused]] const QModelIndex &index,
                         [[maybe_unused]] const QVariant &value,
                         [[maybe_unused]] int role) {

    // Currently not needed
    return false;
}

void SnakeModel::refresh() {
    this->beginResetModel();
    this->endResetModel();
}

void SnakeModel::connect_to_ip(const QString& ip) {
    if (_snake_client->set_server_address(ip.toStdString())) {
        _snake_client->connect_network();
        emit this->ipSet();
    } else {
        emit this->ipNotSet();
    }
}
