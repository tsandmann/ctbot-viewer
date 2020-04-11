#pragma once

#include <QQmlApplicationEngine>
#include <QList>
#include <QString>

#include "remotecall_model.h"
#include "remotecall_list.h"
#include "connect_button.h"


class QTcpSocket;
class QQuickItem;
class CommandEvaluator;

class RemotecallViewer {
    RCList* p_rcList_;
    RCModel rc_model_;
    QQmlApplicationEngine* p_engine_;
    QTcpSocket* p_socket_;
    ConnectButton* p_fetch_button_;
    ConnectButton* p_clear_button_;
    ConnectButton* p_abort_button_;
    ConnectButton* p_rc_button_;

    static QQuickItem* find_item(const QList<QObject*>& nodes, const QString& name);

public:
    RemotecallViewer(QQmlApplicationEngine* p_engine, CommandEvaluator& command_eval, QTcpSocket* p_socket);

    ~RemotecallViewer();

    void register_buttons();
};
