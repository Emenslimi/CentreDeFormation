#include "connection.h"

Connection::Connection() {}

bool Connection::createconnect() {
    db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("Source_Project");
    db.setUserName("system");
    db.setPassword("1234");

    if (!db.open()) {
        qDebug() << "Erreur de connexion Oracle:" << db.lastError().text();
        return false;
    }
    qDebug() << "Connexion Oracle réussie !";
    return true;
}

void Connection::closeConnection() {
    db.close();
}