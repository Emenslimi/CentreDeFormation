#ifndef SMTP_H
#define SMTP_H

#include <QtNetwork/QSslSocket>
#include <QString>
#include <QTextStream>

class Smtp : public QObject
{
    Q_OBJECT

public:
    Smtp(const QString &user, const QString &pass, const QString &host, int port = 465);
    ~Smtp();

    void sendMail(const QString &from, const QString &to, const QString &subject, const QString &body);

signals:
    void status(const QString &message);

private slots:
    void readyRead();
    void connected();
    void errorReceived(QAbstractSocket::SocketError socketError);
    void disconnected();

private:
    QString user;
    QString pass;
    QString host;
    int port;
    QSslSocket *socket;
    QTextStream *t;
    QString message;
    QString rcpt;

    enum states { Init, Handshake, Auth, User, Pass, Rcpt, Data, Body, Quit, Close };
    int state;
};

#endif // SMTP_H