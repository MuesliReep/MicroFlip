#ifndef TCPAUTHSOCKET_H
#define TCPAUTHSOCKET_H

#include <QTcpSocket>

class TcpAuthSocket : public QTcpSocket
{
public:
    TcpAuthSocket();

    void setAuthenticated(bool value);
    bool isAuthenticated;

private:
    bool authenticated;
};

#endif // TCPAUTHSOCKET_H
