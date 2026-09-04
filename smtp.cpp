#include "smtp.h"

Smtp::Smtp(const QString &user, const QString &pass, const QString &host, int port)
{
    socket = new QSslSocket(this);
    t = new QTextStream(socket);

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorReceived(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    this->user = user;
    this->pass = pass;
    this->host = host;
    this->port = port;
}

void Smtp::sendMail(const QString &from, const QString &to, const QString &subject, const QString &body)
{
    message = "To: " + to + "\n";
    message.append("From: " + from + "\n");
    message.append("Subject: " + subject + "\n");
    message.append("Content-Type: text/plain; charset=\"utf-8\"\n\n");
    message.append(body);
    message.replace("\n.", "\n..");

    rcpt = to;
    state = Init;

    socket->connectToHostEncrypted(host, port);
    if (!socket->waitForEncrypted(5000)) {
        emit status("Erreur de connexion SSL: " + socket->errorString());
    }
}

void Smtp::connected() {
    emit status("Connexion au serveur de messagerie réussie...");
}

void Smtp::readyRead()
{
    QString responseLine;
    do {
        responseLine = socket->readLine();
    } while (socket->canReadLine());

    if (state == Init && responseLine.startsWith("220")) {
        *t << "EHLO localhost" << "\r\n";
        t->flush();
        state = Handshake;
    } else if (state == Handshake && responseLine.startsWith("250")) {
        *t << "AUTH LOGIN" << "\r\n";
        t->flush();
        state = Auth;
    } else if (state == Auth && responseLine.startsWith("334")) {
        *t << QByteArray(user.toUtf8()).toBase64() << "\r\n";
        t->flush();
        state = User;
    } else if (state == User && responseLine.startsWith("334")) {
        *t << QByteArray(pass.toUtf8()).toBase64() << "\r\n";
        t->flush();
        state = Pass;
    } else if (state == Pass && responseLine.startsWith("235")) {
        *t << "MAIL FROM:<" << user << ">" << "\r\n";
        t->flush();
        state = Rcpt;
    } else if (state == Rcpt && responseLine.startsWith("250")) {
        *t << "RCPT TO:<" << rcpt << ">" << "\r\n";
        t->flush();
        state = Data;
    } else if (state == Data && responseLine.startsWith("250")) {
        *t << "DATA" << "\r\n";
        t->flush();
        state = Body;
    } else if (state == Body && responseLine.startsWith("354")) {
        *t << message << "\r\n.\r\n";
        t->flush();
        state = Quit;
    } else if (state == Quit && responseLine.startsWith("250")) {
        *t << "QUIT" << "\r\n";
        t->flush();
        state = Close;
        emit status("E-mail envoyé avec succès !");
    }
}

void Smtp::errorReceived(QAbstractSocket::SocketError socketError) {
    emit status("Erreur d'envoi : " + socket->errorString());
}

void Smtp::disconnected() {
    socket->deleteLater();
}

Smtp::~Smtp() {
    delete t;
}